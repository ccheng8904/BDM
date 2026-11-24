"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : library for importing configuration from XML and JSON files
"""

import xml.dom.minidom as dom
import json
#import yaml    Preparation for YAML support
import sys
import pickle
import filecmp
import os
import shutil

loader_dict = {
    ".pkl":  "load_config_from_pkl",
    ".xml":  "load_config_from_xml"
}

# Try to import module for JSON files processing
try:
    import json
    loader_dict[".json"] = "load_config_from_json"
except ImportError:
    loader_dict[".json"] = "load_config_import_error"


# Try to import module for YAML files processing
try:
    import yaml
    loader_dict[".yaml"] = "load_config_from_yaml"
except ImportError:
    loader_dict[".yaml"] = "load_config_import_error"


def load_config_import_error(config_file):
    sys.exit("Module for processing configuration file {} is missing".format(config_file))


def load_config_from_pkl(pkl_file):
    with open(pkl_file, 'rb') as file:
        new_cfg_dict = pickle.load(file)
    return new_cfg_dict


def load_config_from_xml(xml_file):
    new_cfg_dict = dict()
    cfgs_xml = dom.parse(xml_file).firstChild

    for item in cfgs_xml.childNodes:
        if item.nodeType == 1:
            item_name = item.nodeName
            elements = item.getElementsByTagName('element')

            if len(elements) > 0:
                item_list = list()
                for element in elements:
                    if len(element.childNodes) > 1:
                        element_dict = dict()
                        for entry in element.childNodes:
                            if entry.nodeType == 1:
                                element_dict[entry.nodeName] = entry.firstChild.nodeValue
                        item_list.append(element_dict)
                    else:
                        item_list.append(element.firstChild.nodeValue)
                new_cfg_dict[item_name] = item_list
    return new_cfg_dict


def load_config_from_json(json_file):
    new_cfg_dict = dict()
    with open(json_file) as cfg_json:
        try:
            new_cfg_dict = json.load(cfg_json)
        except Exception as e:
            sys.exit("Configuration file \"" + json_file + "\" is not valid JSON file\n" + str(e))
    return new_cfg_dict


def load_config_from_yaml(yaml_file):
    new_cfg_dict = dict()
    with open(yaml_file) as cfg_yaml:
        try:
            new_cfg_dict = yaml.safe_load(cfg_yaml)
        except Exception as e:
            sys.exit("Configuration file \"" + yaml_file + "\" is not valid YAML file\n" + str(e))
    return new_cfg_dict


def join_config_dicts(source, destination):
    for key, value in source.items():
        if isinstance(value, dict):
            # get node or create one
            node = destination.setdefault(key, {})
            join_config_dicts(value, node)
        elif (key in destination) and isinstance(destination[key], list):
            destination[key] += value
        else:
            destination[key] = value

    return destination


def find_file(file, search_paths):
    if os.path.exists(file):
        return file
    elif search_paths is not None:
        for path in search_paths:
            full_path = os.path.normpath(os.path.join(path, file))
            if os.path.exists(full_path):
                return full_path
    return None


# Function loads one or more configuration files with different format
# and returns joined dictionary from all files.
# Supported formats are .pkl (python pickle format), .json and .xml
def load_config(config_files, search_paths):
    if config_files is None:
        return None

    config_dict = dict()
    for file in config_files:
        extension = os.path.splitext(file)[1]
        file = find_file(file, search_paths)
        if file is None:
            print("ERROR: Configuration file " + file + " not found")
        elif extension in loader_dict:
            new_config_dict = globals()[loader_dict[extension]](file)
            join_config_dicts(new_config_dict, config_dict)
        else:
            print("ERROR: Unsupported configuration file type: " + file)

    return config_dict


# This function saves configuration dictionary to file
# If file exists, existing file is compared with newly created and if
# they don't match, original file is replaced. If original and new
# file matches, original is preserved, thus keeping timestamp intact.
def save_config(config_dict, file):
    file_tmp = file + ".tmp"

    out_dir = os.path.dirname(file)
    if out_dir != "":
        try:
            os.makedirs(out_dir)
        except FileExistsError:
            # directory already exists
            pass

    with open(file_tmp, 'wb') as f:
        pickle.dump(config_dict, f, pickle.HIGHEST_PROTOCOL)

    if os.path.exists(file):
        if not filecmp.cmp(file, file_tmp):
            shutil.copy(file_tmp, file)
    else:
        shutil.copy(file_tmp, file)

    os.remove(file_tmp)
