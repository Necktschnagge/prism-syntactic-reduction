#!/usr/bin/python

__author__ = "Clemens Dubslaff"
__copyright__ = "Copyright 2020, Clemens Dubslaff"
__license__ = "GPL3"
__version__ = "0.1"
__maintainer__ = "Clemens Dubslaff"
__email__ = "clemens.dubslaff@tu-dresden.de"
__status__ = "Hack"

#####################
# iReduce
# Takes a prism and a control-flow variable, performs live range analysis, and applies 
# reset value and register allocation optimization. The model is assumed to have only
# one module and one command per line.
# Currently, everything happens syntactically, which means that if a variable is a
# substring of another variable, it can yield problems :)
#####################

# imports
import re
import sys

command_rex = re.compile('.*\[.*\].*->.*;')
locvar_rex = re.compile('.+:.*init')

# familize a file
def read_commands(inf, cf):
	commands = [] # list of read commands: [cf, read, write, succ_cf]
	rv = {} # dictionary of var_name -> (lower bound, upper bound, init value)
	for line in inf:
		# store commands
		iscommand = command_rex.match(line)
		if iscommand:
			r = re.findall('[\w\'=:\-\+\.]+', line)
			# go through the command and put infos
			succ_cf = set() # set of successors
			read_set = set() # read variables
			write_set = set() # set of write lists
			for i in range(len(r))[1:-1]:
				if (r[i] == "="):
					if (r[i-1][len(r[i-1])-1] == "'"):
						if (r[i-1] == cf+"'"):
							succ_cf.add(r[i+1])
						else:
							write_set.add(r[i-1][:-1])
					else:
						if (r[i-1] == cf):
							cf_val = r[i+1]
						else:
							read_set.add(r[i-1])
			commands.append((cf_val,read_set,write_set,succ_cf))
		# read variable domains, init value
		islocalvar = locvar_rex.match(line)
		if islocalvar:
			r = re.findall('\w+', line)
			if not r[0] == cf:
				rv[r[0]] = (r[1],r[2],r[4])
			
# 	for i in range(len(commands)):
#  		print(str(commands[i])+"\n\n")
 			
	return (commands, rv)
	
# computes the set of commands for each cf-value
# cf -> {ci1, ci2, ..., cin}
def compute_fcf(commands):
	fcf = {}
	for i in range(len(commands)):
		acf = commands[i][0]
		if fcf.get(acf) == None:
			fcf[acf] = set()
		fcf[acf].add(i)
	# print(fcf)
	return fcf

# computes the commands that could be predecessors of a command
# ci -> {ci1, ci2, ..., cin}
def compute_fpred(commands):
	pred = {}
	fcf = compute_fcf(commands)
	for i in range(len(commands)):
		for acf in commands[i][3]: # succ_cf of the command
			for c in fcf.get(acf): # transfer cfs to command-indices
				if pred.get(c) == None:
					pred[c] = set()
				pred[c].add(i)
	# print(pred)
	return pred
	
# computes the commands that are successors of a command
# ci -> {ci1, ci2, ..., cin}
def compute_fsucc(commands):
	succ = {}
	fcf = compute_fcf(commands)
	for i in range(len(commands)):
		for acf in commands[i][3]: # succ_cf of the command
			for c in fcf.get(acf): # transfer cfs to command-indices
				if succ.get(i) == None:
					succ[i] = set()
				succ[i].add(c)
	return succ
		
# performs live range analysis
# returns a list with indices as within commands
def live_range_analysis(commands):
	print("perform live range analysis")
	U = set(range(len(commands)))
	pred = compute_fpred(commands)
	live = []
	for i in range(len(commands)):
		live.append(commands[i][1]) # appends the set of read variables
	while len(U)>0:
		cp = U.pop()
		for c in pred[cp]:
			I = live[c].union(live[cp].difference(commands[c][2])) # 2 in commands ... write_set!
			if not I.issubset(live[c]):
				live[c] = I
				U.add(c)
	print(live)
	return live
	
# performs live range analysis using the somehow redundant textbook algorithm
# returns a list with indices as within commands
def live_range_analysis_inout(commands):
	print("perform live range analysis")
	succ = compute_fsucc(commands)
	change = True
	ins = []
	outs = []
	for i in range(len(commands)):
		ins.append(set())
		outs.append(set())
	while change:
		change = False
		for i in range(len(commands)):
			oouts = outs[i].copy()
			oins = ins[i].copy()
			for ii in succ[i]:
				outs[i].update(ins[ii])
			ins[i] = commands[i][1]
			ins[i].update(outs[i].difference(commands[i][2]))
			if not (outs[i].issubset(oouts)) or not (ins[i].issubset(oins)):
				change = True
	return ins
	
# performs reset value optimization
def reset_optimization(commands, live, cf, ex, rv, inf, outf):
	print("perform reset value optimization")
	c = 0 # command counter
	fcf = compute_fcf(commands) # cf -> cis
	for line in inf:
		iscommand = command_rex.match(line)
		if iscommand:
			r = line.split(cf+"'")
			os = r[0] # put string until first appearance of cf'
			for ui in range(len(r))[1:]:
				u = re.findall("\w+",r[ui])
				L = live[c].difference(ex)
				for s in fcf[u[0]]:
					L = L.difference(live[s])
				# print(L)
				for x in L:
					# TODO: implement replace of updates
# 					if len(re.findall(x+"'", r[ui])) > 0:
# 						os += "achtung"
# 					else:
						os += x+"' = "+rv[x][2]+") & (" # reset to reset value
				os += "cf'" + r[ui] # the splitted cf again added
			outf.write(os)
			c += 1 # increment the command counter
		else:
			outf.write(line)
			
# color graph
def color_nodes(graph):
	print("color the graph")
	color_map = {}
	# Consider nodes in descending degree 
	for node in sorted(graph, key=lambda x: len(graph[x]), reverse=True):
		neighbor_colors = set(color_map.get(neigh) for neigh in graph[node])
		color_map[node] = next(
			color for color in range(len(graph)) if color not in neighbor_colors
		)
	print(color_map)
	return color_map
    
# compute interference graph, over variable names
def compute_interference(live, rv, commands):
	print("compute interference graph")
	graph = {}
	for v in rv.keys(): # initialize the graph with all variables
		graph[v] = set()
	succ = compute_fsucc(commands)
	isets = []
	for i in range(len(live)):
		livei = set()
		for j in succ[i]: # indices
			livei.update(live[int(j)])
#		print(i, ": ", livei)
		for x in livei: # variable names
			for y in livei: # variable names
				graph[x].add(y)
		isets.append(livei)
		# after computing the set of live, build graph		
	print(graph)	
	return graph
	
# compute inverted coloring
def compute_inv_coloring(coloring):
	inv_coloring = {}
	for v in coloring.keys():
		if inv_coloring.get(coloring[v]) == None:
			inv_coloring[coloring[v]] = set()
		inv_coloring[coloring[v]].add(v)
	return inv_coloring

# performs register allocation optimization
def register_optimization(coloring, ex, rv, inf, outf):
	print("perform register allocation optimization")
	inv_coloring = compute_inv_coloring(coloring)

	# compute variable names that will be merged (excluding ex-variables!)
	mergeset = set()
	for k in inv_coloring.keys():
		if len(inv_coloring[k]) > 1:
			mergeset.update(inv_coloring[k])
	mergeset = mergeset.difference(ex) # exclude ex-variables
	print(mergeset)
	
	# update the variable bounds of these variables
	mergebounds = {}
	for subs in mergeset:
		new_var = "merged_"+str(coloring[subs])
		if mergebounds.get(new_var) == None:
			mergebounds[new_var] = (rv[subs][0],rv[subs][1])
		else:
			mergebounds[new_var] = (min(mergebounds[new_var][0],rv[subs][0]),max(mergebounds[new_var][1],rv[subs][1]))
	
	merge_written = set()
	for line in inf:
		os = line
		iscommand = command_rex.match(os)
		islocalvar = locvar_rex.match(os)
		if iscommand:
			for subs in mergeset:
				os = re.sub(subs+" ", "merged_"+str(coloring[subs])+" ", os)
				os = re.sub(subs+"\)", "merged_"+str(coloring[subs])+"\)", os)
				os = re.sub(subs+"'", "merged_"+str(coloring[subs])+"'", os)
		if islocalvar:
			r = re.findall('\w+', os)
			var_name = r[0]
			if var_name in mergeset:
				if (coloring[var_name] in merge_written):
					os = "// merged_"+str(coloring[var_name])+":"+os # if merged already written, then discard
				else:
					bounds = mergebounds["merged_"+str(coloring[var_name])]
					# TODO: when init values different, do not merge
					os = "merged_"+str(coloring[var_name])+" : ["+bounds[0]+" .. "+bounds[1]+"] init "+rv[var_name][2]+";\n"+\
						"// merged_"+str(coloring[var_name])+":"+os
					merge_written.add(coloring[var_name])
					# print(merge_written)
					
		outf.write(os)
		
# main program
if __name__ == '__main__':
	# simple sanity check whether input format is ok
	if len(sys.argv) > 2:
		# input parameters
		infile_name = sys.argv[1]
		outfile_name = sys.argv[2]
		cf_var = sys.argv[3]
		redmethod = sys.argv[4]
		ex = set()
		for i in range(len(sys.argv))[5:]:
			ex.add(sys.argv[i])
		print("variables to be excluded from optimization: "+str(ex))
		
		# start live range analysis
		input_file = open(infile_name,'r')
		(commands, resetv) = read_commands(input_file, cf_var)
		input_file.close()
		
		live = live_range_analysis(commands)
		
		input_file = open(infile_name,'r')
		output_file = open(outfile_name,'w')
		if redmethod == "1":
			graph = compute_interference(live, resetv, commands)
			coloring = color_nodes(graph)
			register_optimization(coloring, ex, resetv, input_file, output_file)
		else:
			reset_optimization(commands, live, cf_var, ex, resetv, input_file, output_file)
		# close files
		input_file.close()
		output_file.close()
		
	else:
		print('iReduce: call as ./ireduce.py in.prism out.prism cf param {var0,...,varn}')
		print('where')
		print('  in.prism	... input model')
		print('  out.prism	... output model')
		print('  cf       	... control-flow variable')
		print('  param	    ... 0:reset optimization; 1:register allocation')
		print('  vari      	... variables to be excluded from optimization')