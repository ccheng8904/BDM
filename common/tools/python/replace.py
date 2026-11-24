"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : replace.py
Date of creation : 7.8.2018
Author Name/Dept : 
Organization     : 
Description      : Executes the replacement.
"""

import re
from rplTmplt import TemplateFileReplace
from argparse import ArgumentParser


assignment_define_pattern = re.compile('([^=]+)=([^=]+)')

def build_dictionary_from_define_args(argDefines):
    # add all welled formed macro definitions to Replacements dictionary  
    # only assignment defines allowed
    
    if not argDefines:
        return  
        
    DefinesDict = {}

    for defi in argDefines:
        m = assignment_define_pattern.match(defi[0])
        if m != None:
            # if match: add replacement
            DefinesDict.setdefault(m.group(1),m.group(2))       
        else:
            # else: inform about bad argument
            print ("The following define option can not be processed because it is badly formed: ", defi, " Use option -help to learn about well formed options")
    
    return DefinesDict

def main():

    # ============== Argument parsing ======================================
    argP = ArgumentParser(description='Replaces tags from a template.')
    argP.add_argument("-o", "--outfile",  dest="outfile", default="", required=True, help="Path and name of output file")
    argP.add_argument("-t", "--template", dest="template", default="", required=True, help="Path and name of template file")
    argP.add_argument("-D", action='append', nargs='*', dest="define",  required=False, help="Replace definition <macro>=<value> i.e. the expression <$macro> in the template file is replaced by value in the output file")
    argP.add_argument("-i", "--ignore_unknown", dest="ignore", action="store_const", const=True, default=False, required=False, help="If this option is given then unknown macros are ignored otherwise they are replaced by their name")
    args = argP.parse_args()
    
    # create Replace instance with Replacements dictionary
    Replacements = build_dictionary_from_define_args(args.define)
    Replacer = TemplateFileReplace(Replacements)
    
    # execute replacing on template file and write to target file
    Replacer.replaceTemplate(args.template, args.outfile, args.ignore)

     
if __name__ == '__main__':
    main()

