# Phyton script for copying Cypress PSoC projects and editing references

import os, sys
import re
import shutil


# Function for adjusting relative path
def ajd_rel_pat(srcAbsPath, targetRelPath, dstAbsPath):
    targetAbsPath = os.path.abspath(os.path.join(srcAbsPath, targetRelPath))
    return os.path.relpath(targetAbsPath, dstAbsPath)


# Print error if target file exists
if len(sys.argv) != 4:
    print("Argument mismatch")
    exit(-1)

srcPrjDir = sys.argv[1]
dstPrjDir = sys.argv[2]
derivative = sys.argv[3]

# Local variables for project name and path
srcPrjDir = os.path.abspath(srcPrjDir)
dstPrjDir = os.path.abspath(dstPrjDir)
srcPrjName = os.path.basename(os.path.splitext(srcPrjDir)[0])
dstPrjName = os.path.basename(os.path.splitext(dstPrjDir)[0])
srcPrjFile = os.path.join(srcPrjDir, srcPrjName + ".cyprj")
dstPrjFile = os.path.join(dstPrjDir, dstPrjName + ".cyprj")

# Print error if target file exists
if not os.path.isfile(srcPrjFile):
    print("Source project not found")
    exit(-1)

# Print error if source file do not exists
if os.path.isdir(dstPrjDir):
    print("Target project already exists")
    exit(-1)

print("Copying project...")
shutil.copytree(srcPrjDir, dstPrjDir)
print("Renaming project files...")
for file in os.listdir(dstPrjDir):
    src = os.path.join(dstPrjDir, file)
    dst = os.path.join(dstPrjDir, file.replace(srcPrjName, dstPrjName))
    shutil.move(src, dst)


print("Retargeting relative dependencies...")
re_dep = re.compile(r'(<library_dep[^\"]*\")(\.\.[^\"]*)(.*)')
re_uc = re.compile(r'(<selected_device[^\"]*\")([^\"]*)(.*)')
outFileStr = ""

with open(dstPrjFile, "rt") as fin:
    for line in fin:
        # Search for dependency line
        match = re_dep.match(line)
        if match:
            # Get target path from line
            targetRelPath = match.group(2)
            # Adjust relative path
            targetRelPath = ajd_rel_pat(srcPrjDir, targetRelPath, dstPrjDir)
            # Substitute old path with new one
            line = match.group(1) + targetRelPath + match.group(3) + "\n"

        # Search for microcontroller line
        match = re_uc.match(line)
        if match:
            print("Changing project derivative to " + derivative)
            line = match.group(1) + derivative + match.group(3) + "\n"

        outFileStr += line
    fin.close()

print("Renaming project from " + srcPrjName + " to " + dstPrjName)
outFileStr = outFileStr.replace(srcPrjName, dstPrjName)

print("Saving changes...")
with open(dstPrjFile, "wt") as fout:
    fout.write(outFileStr)
    fout.close()
