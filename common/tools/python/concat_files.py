# Phyton script for concatenating files into one output file

import os, sys
from fnmatch import fnmatch

root    = sys.argv[1]
dstFile = sys.argv[2]
pattern = sys.argv[3]

outFileStr = ""
for path, subdirs, files in os.walk(root):
    for name in files:
        if fnmatch(name, pattern):
            srcFile = os.path.join(path, name)
            with open(srcFile, "rt") as fin:
                outFileStr += fin.read()

with open(dstFile, "wt") as fout:
    fout.write(outFileStr)
    fout.close()
