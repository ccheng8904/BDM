"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : Support routines for code generator
"""

import os
from collections import OrderedDict


class CgData:
    search_paths = ["."] * 3
    error_log = ""
    function_dict = dict()
    data_dict = dict()
    iterator_list = list()
    iterator_names = list()
    output_disabled = False


def is_output_disabled():
    return CgData.output_disabled


def disable_output(disable_state):
    CgData.output_disabled = disable_state


def set_variable_dictionary(dictionary):
    CgData.data_dict = dictionary.copy()


def search_in_dict(var, dict):
    for arg in var:
        if arg[0] == '$':
            arg = get_variable(arg[2:-1])
        if arg in dict:
            dict = dict[arg]
            ret_val = dict
        else:
            # report_warning("variable \"{}\" not defined".format(var_name))
            ret_val = None
    return ret_val


def split_var_name(var_name):
    ret_val = list()
    var_str = ""
    brace_count = 0
    for ch in var_name:
        if ch == '(':
            brace_count += 1
        elif ch == ')':
            brace_count -= 1
        elif ch == '.' and brace_count == 0:
            ret_val.append(var_str)
            var_str = ""
            continue
        var_str += ch
    ret_val.append(var_str)
    return ret_val


def get_variable(var_name):
    var_args = split_var_name(var_name)
    config_dict = CgData.data_dict
    iterator_dict = get_iterator()
    ret_val = None

    # Check if value is number
    try:
        ret_val = int(var_name, 0)
        return ret_val
    except ValueError:
        pass

    # Try to read out variable from system variables (make process)
    if var_args[0] == 'os':
        ret_val = os.getenv(var_args[1])

    # read variable from configuration dictionary
    if ret_val is None:
        ret_val = search_in_dict(var_args, config_dict)

    if ret_val is None and iterator_dict is not None:
        ret_val = search_in_dict(var_args, iterator_dict)

    return ret_val


def set_variable(name, value, config_dict=None):
    define_name = name.split('.')
    if config_dict is None:
        config_dict = CgData.data_dict

    for touple in define_name[:-1]:
        if touple not in config_dict:
            config_dict[touple] = dict()
        config_dict = config_dict[touple]

    config_dict[define_name[-1]] = value
    return ""


def push_iterator():
    CgData.iterator_list.append(None)
    CgData.iterator_names.append(None)


def pop_iterator():
    CgData.iterator_list.pop()
    CgData.iterator_names.pop()


def save_iterator(item, iter_var=None, key=None):
    CgData.iterator_list[-1] = item
    CgData.iterator_names[-1] = key
    if iter_var is not None:
        set_variable(iter_var, item)


def get_iterator():
    if len(CgData.iterator_list) > 0:
        return CgData.iterator_list[-1]
    else:
        return None


def get_iterator_name():
    if len(CgData.iterator_names) > 0:
        return CgData.iterator_names[-1]
    else:
        return None


def var_to_list(in_var):
    if isinstance(in_var, list):
        return in_var, None
    elif isinstance(in_var, dict) or isinstance(in_var, OrderedDict):
        ret_names = list()
        ret_list = list()
        for key in in_var:
            ret_names.append(key)
            ret_list.append(in_var[key])
        return ret_list, ret_names
    else:
        return [in_var], None


def var_to_str(in_var):
    if isinstance(in_var, str):
        return in_var
    elif isinstance(in_var, int):
        return str(in_var)
    else:
        report_error("Unable to convert {} to string".format(in_var))


def set_search_path(path, priority=3):
    if priority < 3:
        CgData.search_paths[priority] = os.path.dirname(os.path.realpath(path))
    else:
        CgData.search_paths.append(os.path.realpath(path))


def search_folder_for_file(folder, filename):
    depth = 1
    ret = ""
    for root, dirs, files in os.walk(folder):
        if root[len(folder):].count(os.sep) < depth:
            for f in files:
                if f == filename:
                    ret = os.path.join(root, f)
    return ret


def find_file(filename):
    absolute_path = ""
    for path in CgData.search_paths:
        absolute_path = search_folder_for_file(path, filename)
        if absolute_path != "":
            break

    if not os.path.exists(absolute_path):
        report_error("File \"{}\" not found".format(filename))
        return None

    return absolute_path


def report_error(message_str):
    CgData.error_log += "error: " + message_str + "\n"
    raise Exception(message_str)


def report_warning(message_str):
    CgData.error_log += "warning: " + message_str + "\n"


def is_error():
    return CgData.error_log != ""


def get_error_message():
    error_msg = CgData.error_log[:-1]
    CgData.error_log = ""
    return error_msg


def clear_error():
    CgData.error_log = ""


def register_function(name, param=None):
    def add_to_dict(_name, _param, _func):
        CgData.function_dict
        function_entry = dict()
        function_entry["name"] = _func.__name__
        function_entry["param"] = _param
        function_entry["function"] = _func
        CgData.function_dict[_name] = function_entry

    def decorate(func):
        if isinstance(name, list):
            for entry in name:
                add_to_dict(entry["name"], entry["param"], func)
        else:
            add_to_dict(name, param, func)
    return decorate


def get_function(function_name):
    if function_name in CgData.function_dict:
        return CgData.function_dict[function_name]
    else:
        return None


def get_functions():
    return CgData.function_dict
