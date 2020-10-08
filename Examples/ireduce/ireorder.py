#!/usr/bin/python

__author__ = "Clemens Dubslaff"
__copyright__ = "Copyright 2020, Clemens Dubslaff"
__license__ = "GPL3"
__version__ = "0.3"
__maintainer__ = "Clemens Dubslaff"
__email__ = "clemens.dubslaff@tu-dresden.de"
__status__ = "Hack"

#####################
# iReorder
# takes a family prism model where the family is described through an init block in conjunctive
# normal form and performs iterative variable reordering using the prism version with variable
# reordering (see https://wwwtcs.inf.tu-dresden.de/ALGI/PUB/STTT/)
#####################

## imports
import re
import sys
import subprocess

globalvar_rex = re.compile('.*global.*:.+;')
mtime_rex = re.compile('Time for model construction:.*')
states_rex = re.compile('States:.*')
rtime_rex = re.compile('Reordering.*done.*')
trans_rex = re.compile('MTBDD nodes of transition matrix:.*')

# generates a new variant out from the model plus new init block from state
def generate_variant(inf, outf, fvars, vlist, cstate):
	input_file = open(inf, 'r')
	output_file = open(outf, 'w')
	
	# delete init block from the model
	initblock = False
	for line in input_file:
		if ('init' in line):
			initblock = True
		if initblock:
			if 'endinit' in line:
				initblock = False
		else:
			output_file.write(line)

	# generate the init block corresponding to current state
	output_file.write('\ninit\n')
	outfamblock = ''
	for v in vlist:
		outfamblock += '\t( %s=%s ' % (v, fvars.get(v)[0])
		# only include those family options until state
		for val in fvars.get(v)[1:cstate.get(v)]:
			outfamblock += '| %s=%s ' % (v, val)
		outfamblock += ') & \n'
	output_file.write(outfamblock)
	output_file.write('true\nendinit\n')
	input_file.close()
	output_file.close()
	
# generate number of file
def genfn(i):
	return folder+"/"+file_name[0]+str(i)
	
# run prism
def run_prism(inf, outf, logf):
	log = open(logf, 'w')
	subprocess.Popen(["prism", inf, "-mtbdd", 
						"-reorder", "-reorderoptions", "norebuild", 
						"-exportreordered", outf], 
					stdout=log, stderr=log).wait()
	log.close()

# prints out log information
def generate_stats(log_name,stat_name):
	# read stats from log file
	log_file = open(log_name, 'r')
	for line in log_file:
		r = ""
		if (mtime_rex.match(line)) or rtime_rex.match(line) or states_rex.match(line) or trans_rex.match(line):
			r = re.findall('\w+', line)
		if mtime_rex.match(line):
			mtime = r[4]+"."+r[5]
		if rtime_rex.match(line):
			rtime = r[3]+"."+r[4]
		if states_rex.match(line):
			states = r[1]
			initstates = r[2]
		if trans_rex.match(line):
			rtrans = r[5]
			trans = r[6]
	log_file.close()
	# print out stats
	stat_print = format_str.format(str(round),initstates,states,trans,rtrans,mtime,rtime)
	print(stat_print)
	# write stats to stats file
	stat = str(round)+"\t"+initstates+"\t"+states+"\t"+trans+"\t"+rtrans+"\t"+mtime+"\t"+rtime+"\n"
	stat_file = open(stat_name, 'a')
	stat_file.write(stat)
	stat_file.close()
		
# main program
if __name__ == '__main__':
	# simple sanity check whether input format is ok
	if len(sys.argv) > 4:
		input_file = open(sys.argv[1], 'r')
		file_name = sys.argv[1].split('.')
		folder = sys.argv[4]
		subprocess.Popen(["mkdir", "-p", folder]).wait()

		# read init block from the model (simple but effective)
		# assumes global variables only
		# get variable order
		initblock = False
		flist = []
		vlist = []
		for line in input_file:
			isglobalvar = globalvar_rex.match(line)
			if isglobalvar:
				r = re.findall('\w+', line)
				vlist.append(r[1])
			if ('init' in line):
				initblock = True
			if initblock:
				if 'endinit' in line:
					initblock = False
				r = re.findall('\w+', line)
				flist += r
		input_file.close()

		# extract family parameters from the init block (assume CNF)
		fam_vars = {}  # family variables and options
		it_state = {}  # the evaluation domain during iterative variable reordering
		max_state = {} # the goal evaluation domain for iterative variable reordering
		for i in range(len(flist)-1):
			if i % 2 == 1:
				if fam_vars.get(flist[i]) == None:
					# then add family variable
					fam_vars[flist[i]] = [flist[i+1]]
					# also initialize iterative states
					it_state[flist[i]] = 1
					max_state[flist[i]] = 1
				else:
					fam_vars[flist[i]].append(flist[i+1])
					max_state[flist[i]] += 1

		print("generate initial model from "+sys.argv[1])
		# generate initial family model (input model, output model, family variables, variable order, state)
		generate_variant(sys.argv[1], genfn(0)+".prism", fam_vars, vlist, it_state)

		# iterative reordering
		round = 0
		adapt = int(sys.argv[2])
		step = int(sys.argv[3])
		if step < 1:
			step = 1
		inc = step
		print("start iterative variable reordering...")
		# stats formating
		format_str = '{:>3} {:>10} {:>20} {:>10} {:>10} {:>12} {:>10}'
		print(format_str.format("i", "size", "states", "#before", "#after", "mtime", "rtime"))
		# initialize stat file
		stat_file = open(genfn(str(adapt)+str(step))+".csv", 'w')
		stat_file.close()
		# start actual iterative variable reordering
		while (inc != 0):
			run_prism(genfn(round)+".prism", genfn(round)+".out", genfn(round)+".log")
			generate_stats(genfn(round)+".log", genfn(str(adapt)+str(step))+".csv")
			
			# possibly adapt iteration variable order (heuristics)
			if adapt > 0:
				vlist = [] # reinitialize vlist
				input_file = open(genfn(round)+".out", 'r')
				for line in input_file:
					if globalvar_rex.match(line):
						r = re.findall('\w+', line)
						vlist.append(r[1])
				input_file.close()
				if adapt == 2: # rho-max takes the minimum of the reverse order
					vlist.reverse()
			# increment the state according to vlist
			inc = 0
			for v in vlist:
				if (inc < step) and (it_state[v] < max_state[v]):
					val = min(step-inc, max_state[v]-it_state[v])
					inc += val
					it_state[v] += val
			if (inc != 0):
				generate_variant(genfn(round)+".out", genfn(round+1)+".prism", fam_vars, vlist, it_state)
				round += 1
			else:
				generate_variant(genfn(round)+".out", genfn("_final")+".prism", fam_vars, vlist, it_state)
	else:
		print('iReorder: call as ./ireorder.py family.prism adapt-order step-size out-dict')
		print('where')
		print('  family.prism        ... input family model')
		print('  adapt-order [0,1,2] ... variable order for selection: 0 (no, pi-min), 1 (yes, rho-min), 2 (yes, rho-max)')
		print('  step-size [int]     ... iteration step size (>0)')
		print('  out-dict     		 ... folder for the output')
