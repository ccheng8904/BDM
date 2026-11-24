"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Date of creation : 29.09.2018
Description      : Filter selected symbols from sym-file
"""


from argparse import ArgumentParser
import re


def main():

    # ============== Argument parsing ======================================
    argP = ArgumentParser(description='Filters symbols from sym-file.')
    argP.add_argument("-s", "--symfile", dest="symfile", default="", required=True, help="Path and name of symbole file")
    argP.add_argument("-n", action='append', nargs='*', dest="symbols",  required=False, help="name of symbole to find in sym-file")
    args = argP.parse_args()
    
    try:
        symbole_file = open(args.symfile, "r")
    except:
        print ("Symbol file not found: " + args.symfile)
        return
    
    allLines = symbole_file.readlines()
    
    for sym in args.symbols:
        #sym_regex = r"RegCmd\s+DATA\s+" + re.escape(sym)
        #FilterPattern = re.compile(sym_regex)
        addr = "---"
        sign = "---"
        for line in allLines:
            sym_txt = "".join(sym)
            sym_entry = re.match(r"(?:RegCmd\s+(?:PROC|DATA)\s+)" + sym_txt + "\s+(\w+)\s+(\w+)", line)
            if sym_entry != None:
                addr = sym_entry.group(1)
                sign = sym_entry.group(2)
        print("{0:<32s}  {1:>8s}    {2:<20s}".format(sym_txt, addr, sign))

     
if __name__ == '__main__':
    main()

