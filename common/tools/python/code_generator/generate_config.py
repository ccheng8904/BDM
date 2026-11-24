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
from config_loader import save_config

from argparse import ArgumentParser

argP = ArgumentParser(description='Script for joining multiple configuration xml/json/pkl files to one output file')

argP.add_argument("-i", action='append', dest="input_file", required=True, help="Input configuration file(s).")
argP.add_argument("-o", dest="output_file", required=True, help="Output configuration file.")
args = argP.parse_args()

if __name__ == "__main__":
    # load configuration from XML/JSON files
    cfg_dict = load_config(args.input_file, None)
    
    # save dictionary
    save_config(cfg_dict, args.output_file)
