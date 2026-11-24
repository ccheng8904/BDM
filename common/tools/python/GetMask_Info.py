#!/usr/bin/env python

#======================================================================================================================
# File Name       : getMask_Info.py
# Date of creation: 10th Nov 2017
# Author Name/Dept: Unit Test Automation Team, RBEI/EHT2.
# Organization    : RBEI
# Description     : The tool provides lint masked information. Give the details of line number, colunm number, 
#                   misra rule, and misra rule severity.
# Python Version  : 3.5.2
# File Version    : 1.5
# Modified on     : 2nd February 2017
# Modified by     : Pabboj Sandeep Kumar
# Modification Description: Development of tool to list masked information.
#======================================================================================================================

import os
import sys
import re
from optparse import OptionParser
from BSH_Misra_Config import bsh_misra_config

"""
This variables to handle the length of strings
"""
LENGTH_OF_FILENAME = 100
LENGTH_OF_LINENO = 16
LENGTH_OF_COLUMNNO = 16
LENGTH_OF_MISRA_RULE = 32
LENGTH_OF_MISRA_SEVERITY = 32

"""
class to get lint masked line details and misra rules
"""
class ListMaskInfo:

    def __init__(self):
        pass
    
    """ getmasklines to get line number of masked lint along with position of lint masking and its message_ids.
        It returns the "lineinfo" dictionary with line number, column number and message ids of given 
        ".c/.h" file."""     
    def getmasklines(self, file):
        lineinfo = {}
        """ Regular expression to identify lint comments from .c/.h file """
        pattern = re.compile(r'(?:/lint[^\n]*|\*lint(?:(?!\*/).)*\*/)')
        try:
            with open(file, 'r') as fhandle:
                file_data = fhandle.readlines()
                """ Identifying message ids and column number of masked location """
                for line in range(0, len(file_data)):
                    tabspace = 0
                    column_numbers = []
                    message_id = []
                    for match in re.finditer(pattern, file_data[line]):
                        col_number = match.start()
                        for ch in range(0, col_number):
                            if file_data[line][ch] == '\t':
                                tabspace += tab_length - 1
                        col_number += tabspace
                        column_numbers.append(col_number)
                        tabspace = 0
                    if pattern.findall(file_data[line]):
                        lintstr = pattern.findall(file_data[line])
                        for lint_comment in lintstr:
                            mid = re.findall(r'\d+', lint_comment)
                            message_id += mid
                        column_numbers = list(map(str, column_numbers))
                        lineinfo[line] = column_numbers, message_id
            fhandle.close()
        except:
            if os.path.isfile(file):
                sys.stdout.write("Couldn't able to read the %s file\n"%file)
                return None
            else:
                return False
        if lineinfo:
            return lineinfo
        else:
            return None
            
    """ getmaskrules to get misra rule and its severity for given message ids. Takes lineinfo of getmasklines and
        gives masked information on console or stores in file if filename is provided as a argument 
        to this script """
    def getmaskrules(self, outfile, file, lineinfo):
        if outfile:
            textfile = open(outfile, 'a')
            if os.stat(outfile).st_size == 0:
                textfile.write("[File Name]".ljust(LENGTH_OF_FILENAME) + "[Line No]".rjust(LENGTH_OF_LINENO) + "[Column No]".rjust(LENGTH_OF_COLUMNNO) + \
                        "[Masked Misra rules]".rjust(LENGTH_OF_MISRA_RULE) + "[BSH Misra severity]".rjust(LENGTH_OF_MISRA_SEVERITY) + "\n")   
            if lineinfo is False:
                textfile.write("Given file %s is not available\n"%file)
                sys.stdout.write("Given file %s is not available\n"%file)
                return True        
        if lineinfo:
            for line in lineinfo: 
                message_ids = list(lineinfo[line][1])
                masked_misra_rule = []
                masked_misra_severity = []
                for mID in message_ids:
                    tempID = int(mID)
                    for key in bsh_misra_config:
                        if (tempID in bsh_misra_config[key]) and (key not in masked_misra_rule):
                            masked_misra_rule.append(key)
                            masked_misra_severity.append(bsh_misra_config[key][0])
                for col_number in lineinfo[line][0]:
                    for rule_info in range(len(masked_misra_severity)):
                        if outfile:
                            textfile.write(options.file.ljust(LENGTH_OF_FILENAME) + str(line+1).rjust(LENGTH_OF_LINENO) + (col_number).rjust(LENGTH_OF_COLUMNNO) + \
                               (masked_misra_rule[rule_info]).rjust(LENGTH_OF_MISRA_RULE) + (masked_misra_severity[rule_info]).rjust(LENGTH_OF_MISRA_SEVERITY) + "\n")
                            sys.stdout.write(options.file.ljust(LENGTH_OF_FILENAME) + str(line+1).rjust(LENGTH_OF_LINENO) + (col_number).rjust(LENGTH_OF_COLUMNNO) + \
                               (masked_misra_rule[rule_info]).rjust(LENGTH_OF_MISRA_RULE) + (masked_misra_severity[rule_info]).rjust(LENGTH_OF_MISRA_SEVERITY) + "\n")
                        else:
                            sys.stdout.write(options.file.ljust(LENGTH_OF_FILENAME) + str(line+1).rjust(LENGTH_OF_LINENO) + (col_number).rjust(LENGTH_OF_COLUMNNO) + \
                               (masked_misra_rule[rule_info]).rjust(LENGTH_OF_MISRA_RULE) + (masked_misra_severity[rule_info]).rjust(LENGTH_OF_MISRA_SEVERITY) + "\n")        
        if outfile:
            textfile.close()
        if lineinfo is False:
            sys.stdout.write("Given file %s is not available\n"%file)
        return True           
        

"""
MAIN

"__main__" is the name scope in top level executes 
"""
if __name__ == "__main__":
    parser = OptionParser() 
	
    parser.add_option(
        "-f","--file",
        help = "Provides source file information.",
        action = "store",
        dest = "file",
        default = None
    )
    parser.add_option(
        "-o","--output",
        help = "Stores output into file.",
        action = "store",
        dest = "output",
        default = None
    )
    parser.add_option(
        "-t","--tabspace",
        help = "To decide length of tabs.",
        action = "store",
        dest = "tabspace",
        default = None
    )
    (options, args) = parser.parse_args(args = sys.argv)
    
    """ Python counts each tab as a one char, to handle this in identifing column number need to provide tab length 
        default it will be 4 spaces or provide tab length in '-t or '--tabspace' argument of this script  """
    if options.tabspace is not None:
        tab_length = options.tabspace
    else:
        tab_length = 4
    file = options.file
    header_file = file.replace('.c', '.h')               #To get header file name for given '.c' file
    lintmask = ListMaskInfo()
    if ".c" in options.file:        
        ret = lintmask.getmasklines(file)
        lintmask.getmaskrules(options.output, file, ret)
    if (os.path.isfile(header_file)) or (options.file.endswith('.h')):
        ret = lintmask.getmasklines(header_file)
        lintmask.getmaskrules(options.output, file, ret)
    if (options.file.endswith('.c')) or (options.file.endswith('.h')):
        pass
    else:
        sys.stdout.write("Given %s file format is not acceptable\n"%options.file)