#!/usr/bin/env python
"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : mk_folders.py
Date of creation : 25.11.2019
Author Name/Dept : Nikolai Ehrhardt
Organization     : 
Description      : Creates folders for you.
"""

import os

from argparse import ArgumentParser


def main():

    argP = ArgumentParser(description='Creates the stubs.')

    argP.add_argument("-folders", dest="folders", nargs="*", required=True, help="filepath of define file")
  
    args = argP.parse_args()
    
    for folder in args.folders:
        try:
            os.makedirs(folder)
        except:
            pass
                    

if __name__ == '__main__':
    main()
