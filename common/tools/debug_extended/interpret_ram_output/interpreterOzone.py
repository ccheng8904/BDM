"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : interpreter.py
Date of creation : 2022-11-14
Author Name/Dept : Fabian Deml (GDE-EDS6)
Organization     : GDE-EDS
Description      : Python script to generate binary dump with J-Link and convert it into ASCII string
"""

# This python script helps to interpret the debug data from the RAM interface of the debug extended component of the Framework.
# 
# It is used in additiion to the file "interpreter.py" and is only needed, if you use the Ozone debugger.

import subprocess

# Add a line to the generated hex file
def addTopLineToHexFile(File):
    with open(File, "r+") as f:
        Content = f.read()
        f.seek(0, 0)
        f.write('\n' + Content)

# Convert the .bin to .hex file
def prepHexFile(OutFile):
    String = "..\\..\\patchbin\\PatchBin.exe -ib()" + OutFile + ".bin" + " -oh()" + OutFile + ".hex"
    subprocess.call(String, shell=True)
    addTopLineToHexFile(OutFile +".hex")


if __name__ == '__main__':
    # Create hex files for the following variables
    prepHexFile(".\\DebugData_RingBuffer")
    prepHexFile(".\\DebugData_RingBufferIndex")

    # Convert the hex files to the output string
    subprocess.call("python interpreter.py", shell=True)
 
