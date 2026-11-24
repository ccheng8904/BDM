import os
import sys
import glob

path = sys.argv[1]
target = sys.argv[2]
os.chdir(path)

def fdisplay(filename):
    with open(filename, 'r') as f:     
       fcon = f.read()
       print(fcon)
	   
if target == "ncov2con":
    nfiles = glob.glob(path + '/**/*.nstc', recursive=True)
    for filename in nfiles:
        print(filename)
        fdisplay(filename)
		
if target == "axstyle2console":
    ufiles = glob.glob(path + '/**/*.[us]stc', recursive=True)
    for filename in ufiles:
        print(filename)
        fdisplay(filename)


