#!/usr/bin/python

__author__ = "Clemens Dubslaff"
__copyright__ = "Copyright 2020, Clemens Dubslaff"
__license__ = "GPL3"
__version__ = "0.1"
__maintainer__ = "Clemens Dubslaff"
__email__ = "clemens.dubslaff@tu-dresden.de"
__status__ = "Hack"

#####################
# iPreProcess
# takes a prism model and familize options, reorders the model, 
# and familizes the model
#####################

## imports
import re
import sys
import subprocess

locvar_rex = re.compile('.+:.*init')

# familize a file
def familize(inf, outf, fvars):
	vars = {} # dictionary of var_name -> (lower bound, upper bound, init value)
	# go through the lines, extract variable declarations
	# otherwise streamline to outfile
	for line in inf:
		islocalvar = locvar_rex.match(line)
		if islocalvar:
			r = re.findall('\w+', line)
			vars[r[0]] = (r[1],r[2],r[4])
		else:
			outf.write(line)
	# globalize variable declarations and write it to the output
	for v in vars.keys():
		line = 'global %s : [%s .. %s];\n' % (v,vars[v][0],vars[v][1])
		outf.write(line)
	# generate the input block
	outf.write('\ninit\n')
	outfamblock = ''
	# only take those family variables that have variable declarations
	for v in set(fvars.keys())&set(vars.keys()):
		outfamblock += '\t( %s=%s ' % (v, fvars.get(v)[0])
		for val in fvars.get(v)[1:]:
			outfamblock += '| %s=%s ' % (v, val)
		outfamblock += ') & \n'
	outf.write(outfamblock)
	# write all init values for the non-familized variables
	candidates = set(vars.keys())-set(fvars.keys())
	first = candidates.pop()
	outinitblock = '\t ( %s=%s ) ' % (first, vars.get(first)[2])
	for v in candidates:
		outinitblock += '& ( %s=%s ) ' % (v, vars.get(v)[2])
	outinitblock += '\n'
	outf.write(outinitblock)
	outf.write('endinit\n')

# run prism
def run_prism(inf, outf, logf):
	log = open(logf, 'w')
	subprocess.Popen(["/usr/bin/time", "-l", "prism", inf, "-mtbdd", 
						"-reorder", "-reorderoptions", "norebuild", 
						"-exportreordered", outf], 
					stdout=log, stderr=log).wait()
	log.close()
		
# main program
if __name__ == '__main__':
	# simple sanity check whether input format is ok
	if len(sys.argv) > 2:
		file_name = sys.argv[1].split('.')
				
		# perform reordering on the input model
		run_prism(sys.argv[1], file_name[0]+"_reorder.prism", 
			file_name[0]+"_reorder.log")
		
		# familize the model
		fam_vars = {}  # family variables and options
		input_file = open(file_name[0]+"_reorder.prism",'r')
		output_file = open(sys.argv[2],'w')
		for i in range(len(sys.argv))[3:]:
			var = sys.argv[i].split('=')
			if fam_vars.get(var[0]) == None:
				fam_vars[var[0]] = [var[1]]
			else:
				fam_vars[var[0]].append(var[1])
		familize(input_file, output_file, fam_vars)
		input_file.close()
		output_file.close()
		
	else:
		print('iPreProcess: call as ./iprep.py nofamily.prism family.prism var0={x00,x01,...,x0n} [...] varm={xm0,xm1,...,xmn}')
		print('where')
		print('  nofamily.prism ... input model without init block')
		print('  family.prism   ... output model with init block')
		print('  vari       	... i-th variable to be familized')
		print('  xij        	... possible values of vari in the family')
