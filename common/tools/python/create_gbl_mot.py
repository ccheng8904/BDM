#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  PROJECT          MCU Framework
#*******************************************************************************
#  Description      creates a mot file out of an GBL file
#                   As the size of GBL file is unknown PatchBin needs this
#                   little tool to correctly write the S0 Record
#*******************************************************************************

from argparse import ArgumentParser
from setup_gui.setup import * 
import os
import subprocess

def strHex(hex):
    return "0x%0.4X" % hex

def main():
    argP = ArgumentParser(description='creates a mot file out of an GBL file')
    argP.add_argument("-infile", dest="infile", required=True, help="name of the GBL input file")
    argP.add_argument("-offset", dest="offset", required=True, help="offset address of motorola file")
    args = argP.parse_args()
    
    image_size = os.path.getsize(args.infile)   # get the size of the binary GBL image
    start_addr = int(args.offset,16)            # converts the offset argument into a integer
    end_addr   = start_addr + image_size - 1    # calculate the end address of moved image
    log_name   = os.path.dirname(args.infile) + "/patchbin_gbl.log" # name and path of patchbin logfile

    patchBinOpt  = " -L" + log_name             # specify log file for patchbin call
    patchBinOpt += " -ib()" + args.infile       # specify the input file
    patchBinOpt += " -m()(abs=" + strHex(start_addr) + ")" # move the content to the specified offset address 
    patchBinOpt += " -CRC(" + strHex(start_addr) + "," + strHex(end_addr) + ")(v=CRCGBL):32" # calculate 32 bit CRC
    patchBinOpt += " -omS37()" + args.infile + ".mot#"   # specify the output file
    # comments for WinFlash printed into S0-Record
    patchBinOpt += "\\nCRCstart1=" + strHex(start_addr) + "\\nCRCend1=" + strHex(end_addr) + "\\n"
    patchBinOpt += "CRC1=%CRCGBL\\nCRCtmo=10\\n"

    FNULL = open(os.devnull, 'w')    #suppress output to stdout from the subprocess
    
    # calling patchbin with prepared parameters
    subprocess.call("./common/tools/patchbin/patchbin.exe" + patchBinOpt , stdout=FNULL, stderr=FNULL, shell=False)

if __name__ == '__main__':
    main()

