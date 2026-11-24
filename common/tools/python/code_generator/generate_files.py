"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : this module manages generation of files for MCU FW
"""

from config_loader import load_config

from argparse import ArgumentParser
from code_generator import CodeGenerator
from cg_tools import *

import os.path
import sys

argP = ArgumentParser(description='Source code generator for MCU Framework')

argP.add_argument("-i", dest="input_file", required=False, help="Input template file.")
argP.add_argument("-o", dest="output_file", required=False, help="Generated output file.")
argP.add_argument("-d", dest="dict_entry", required=False, help="Dictionary entry to search for output files")

argP.add_argument("-p", action='append', dest="parameter_list", required=False, help="Optional parameter(s) to be passed to code generator")
argP.add_argument("-c", action='append', dest="config_files", required=False, help="Configuration JSON or XML configuration file.")
argP.add_argument("-s", action='append', dest="search_path", required=False, help="Template file search path.")
argP.add_argument("-a", dest="app_dir", required=False, help="Application folder where to put configuration files.")
argP.add_argument("-v", dest="variant", default="", required=False, help="Variant name for the cases when output file is in folder named as variant.")
argP.add_argument("-overwrite", dest="force", default=False, action="store_true", required=False, help="Switch to force overwriting of files.")
args = argP.parse_args()


if __name__ == "__main__":
    # load configuration from XML/JSON files
    cfg_dict = load_config(args.config_files, args.search_path)

    # Create code generator instance
    cg = CodeGenerator(cfg_dict)

    # Store parameters to configuration dictionary
    if args.parameter_list is not None:
        for parameter in args.parameter_list:
            param_splitted = parameter.split('=')
            if len(param_splitted) == 2:
                param_name = param_splitted[0]
                param_value = param_splitted[1]
                try:
                    param_value = int(param_value, 0)
                except:
                    pass
                if isinstance(param_value, str):
                    param_value = param_value.strip('\'')
                set_variable(param_name, param_value, cfg_dict)
            else:
                print("invalid parameter option: \"" + parameter + "\"")

    if args.output_file is not None:
        output_file = args.output_file
        output_file_list = [output_file]
    elif args.dict_entry is not None and args.dict_entry in cfg_dict:
        output_file_list = cfg_dict[args.dict_entry]
    else:
        sys.exit("Error, unsupported parameter combination")

    # Add search paths
    if args.search_path is not None:
        for path in args.search_path:
            set_search_path(path)

    # Input file is directly set
    if args.input_file is not None:
        cg.generate_file(args.input_file, output_file, args.force)
    
    # Search input file based on arguments
    else:
        for output_file in output_file_list:
            out_file_short = output_file
            if args.app_dir is not None:
                cfg_prog_path = args.app_dir
                out_file_short = output_file.replace(cfg_prog_path,"")
                output_file = os.path.join(cfg_prog_path, out_file_short)
            input_file = cg.find_cfg_template(args.search_path, out_file_short, args.variant)
        
            # Generate file
            if input_file is not None:
                cg.generate_file(input_file, output_file, args.force)
            else:
                print("template for \"" + output_file + "\" not found")
