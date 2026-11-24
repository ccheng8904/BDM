#!/usr/bin/env python
"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : mkPSTD.py
Date of creation : 7.8.2018
Author Name/Dept : Ulrike Brueggemann, Nikolai Ehrhardt 
Organization     : 
Description      : Will generate the PSTD file.
"""


import sys
import os

from argparse import ArgumentParser

from pteng.soco.SourceCodeDescriptor import SourceCodeDescriptor, INDENTTATION
from rplTmplt import TemplateFileReplace

def main():

    global ausgabe
    ausgabe = 1
    
    # argument parsing
    argP = ArgumentParser(description='Creates the stubs.')
    
    argP.add_argument("-o",  dest="outfile", required=True, help="Out file path for the xml file")
    argP.add_argument("-g",  dest="graphFile", required=True, help="ir file")
    argP.add_argument("-p",  dest="project_name", required=True, help="project name")
    argP.add_argument("-f",  dest="folder_name", help="folder name")
    argP.add_argument("-y",  dest="type_system_ID", help="type_system identifier")

    args = argP.parse_args()
        
    print ("graph file: " + args.graphFile)
    
    source_description = SourceCodeDescriptor.CreateFrom_AxivionGraphFilepath(args.graphFile, args.project_name, args.folder_name, args.type_system_ID)
          
    source_description.writePSTD(args.outfile)

    # ---------- end of main ----------

if __name__ == '__main__':
    main()

    
    
    