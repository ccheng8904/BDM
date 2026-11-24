"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : built in functions for code generator
"""

import importlib.util
from collections import OrderedDict
import sys
import re
from cg_tools import *
import csv


@register_function("str")
def function_str(self, args, unused):
    var = self.get_arg(args, 0, type_check=[str, int])
    string = var_to_str(var)
    str_start = self.get_arg(args, 1, type_check=int)
    str_end = self.get_arg(args, 2, default=len(string), type_check=int)
    return string[str_start:str_end]


@register_function("int")
def function_int(self, args, unused):
    num = self.get_arg(args, 0, type_check=[str, int])

    if isinstance(num, str):
        try:
            num = int(num, 0)
            return num
        except:
            report_error("cannot convert \"{}\" to number".format(num))
    return num


@register_function([{"name": "dec", "param": 'd'}, {"name": "hex", "param": "x"}])
def function_conv(self, args, base):
    num = self.function_int(args, base)

    if base != 'd':
        base_str = '0' + base
    else:
        base_str = ""

    return base_str + format(num, base)


@register_function("calc")
def function_calc(self, args, unused):
    expression = self.get_arg(args, 0, process=False)

    # replace variables in expression by values
    for item in re.finditer(r'\$\(([\w|.]+)\)', expression):
        var_name = item.group(0)
        value = get_variable(var_name[2:-1])
        expression = expression.replace(var_name, str(value))

    try:
        return int(eval(expression))
    except:
        report_error("expression \"{}\" cannot be calculated".format(expression))
        return None


@register_function("inc", 1)
def function_inc(self, args, increment_def):
    var_name = args[0]
    increment = self.get_arg(args, 1, default=increment_def, type_check=int)
    value = get_variable(var_name)
    value = value + increment
    set_variable(var_name, value)
    return ""


@register_function([{"name": "enable", "param": False}, {"name": "disable", "param": True}])
def function_supress(self, args, disable):
    disable_output(disable)
    return ""


@register_function([{"name": "ifdef", "param": False}, {"name": "ifndef", "param": True}])
def function_ifdef(self, args, inverted):
    condition = self.get_arg(args, 0) is not None
    expression_true = self.get_arg(args, 1, process=False)
    expression_false = self.get_arg(args, 2, process=False, default="")

    # Clear warning: variable "XXX" not defined
    clear_error()

    if condition != inverted:
        return self.process(expression_true)
    else:
        return self.process(expression_false)


@register_function([{"name": "ifeq", "param": False}, {"name": "ifneq", "param": True}])
def function_ifeq(self, args, inverted):
    input_1 = self.get_arg(args, 0)
    input_2 = self.get_arg(args, 1)
    expression_true = self.get_arg(args, 2, process=False)
    expression_false = self.get_arg(args, 3, process=False, default="")

    condition = input_1 == input_2

    if condition != inverted:
        return self.process(expression_true)
    else:
        return self.process(expression_false)


@register_function([{"name": "sort", "param": False}, {"name": "rsort", "param": True}])
def function_sort(self, args, rev_sort):
    input_arg = self.get_arg(args, 0, type_check=[list, dict, OrderedDict])
    sort_key = self.get_arg(args, 1, default="", type_check=str)

    if isinstance(input_arg, dict) or isinstance(input_arg, OrderedDict):
        sorted_dict = sorted(input_arg.items(), key=lambda kv: kv[1][sort_key], reverse=rev_sort)
        output = OrderedDict(sorted_dict)
    else:
        if sort_key != "":
            output = sorted(input_arg, key=lambda i: i[sort_key], reverse=rev_sort)
        else:
            output = sorted(input_arg, reverse=rev_sort)
    return output


@register_function("$")
def function_variable(self, args, unused):
    var_value = self.get_arg(args, 0)

    if isinstance(var_value, list) and len(var_value) == 1:
        return var_value[0]
    if isinstance(var_value, str):
        return var_value
    elif isinstance(var_value, int):
        return str(var_value)
    return var_value


@register_function("macro_start")
def function_macro_start(self, args, unused):
    macro_name = self.get_arg(args, 0, process=False, type_check=str)
    self.macro_rec_start(macro_name)
    return ""


@register_function("define")
def function_define(self, args, unused):
    define_name = self.get_arg(args, 0, process=False)
    define_value = self.get_arg(args, 1)
    return set_variable(define_name, define_value)


@register_function("undef")
def function_undef(self, args, unused):
    define_name = self.get_arg(args, 0, process=False)
    return set_variable(define_name, None)


@register_function("foreach")
def function_foreach(self, args, unused):
    ret_str = ''
    iterator_name = self.get_arg(args, 0, process=False)
    foreach_tmp = self.get_arg(args, 1, type_check=[list, dict, str, int, OrderedDict])
    foreach_expr = self.get_arg(args, 2, process=False)

    foreach_list, foreach_names = var_to_list(foreach_tmp)
    push_iterator()
    for var in foreach_list:
        if foreach_names is not None:
            key = foreach_names.pop(0)
        else:
            key = None
        save_iterator(var, iterator_name, key)
        try:
            ret_str += self.process(foreach_expr)
        except:
            report_error("invalid expression: " + foreach_expr)
    pop_iterator()
    return ret_str


@register_function("macro")
def function_macro(self, args, unused):
    macro_str = self.get_arg(args, 0, type_check=[str])
    macro_args = args[1:]
    arg_num = 1
    macro_out = ""

    # replace $0 to be last iterator name
    iter_name = get_iterator_name()
    if iter_name is not None:
        macro_str = macro_str.replace('$0', iter_name)

    # replace arguments
    for arg in macro_args:
        replace_var = self.get_arg([arg], 0, type_check=[str, int])
        replace_str = var_to_str(replace_var)
        macro_str = macro_str.replace('$' + str(arg_num), replace_str)
        arg_num += 1

    for line in macro_str.splitlines(keepends=True):
        macro_out += self.process_line(line)

    # replace variables in expression by values
    for item in re.finditer(r'\$\(([\w|.]+)\)', macro_out):
        var_name = item.group(0)
        value = get_variable(var_name[2:-1])
        macro_out = macro_out.replace(var_name, str(value))

    return macro_out


@register_function("load_csv")
def function_load_csv(self, args, unused):
    var_name = self.get_arg(args, 0, process=False)
    filename = self.get_arg(args, 1, type_check=str)
    absolute_path = find_file(filename)

    with open(absolute_path, 'r') as csv_file:
        csv_res = list()
        d_reader = csv.DictReader(csv_file, skipinitialspace=True)
        
        for row in d_reader:
            csv_res.append(row)
    
    return set_variable(var_name, csv_res)


@register_function("table")
def function_table(self, args, unused):
    table = self.get_arg(args, 0, type_check=[dict, OrderedDict])
    table_item = self.get_arg(args, 1, type_check=str)

    if table_item in table:
        return table[table_item]
    report_error("unable to get item \"{}\" from table {}".format(table_item, table))


@register_function("subst")
def function_subst(self, args, unused):
    pattern = self.get_arg(args, 0, type_check=str)
    replacement = self.get_arg(args, 1, type_check=str)
    ret_str = self.get_arg(args, 2, type_check=str)
    return ret_str.replace(pattern, replacement)


@register_function([{"name": "toupper", "param": True}, {"name": "tolower", "param": False}])
def function_str_case(self, args, case_type):
    string = self.get_arg(args, 0, type_check=str)
    if case_type:
        return string.upper()
    else:
        return string.lower()


@register_function("capitalize")
def function_capitalize(self, args, unused):
    input_arg = self.get_arg(args, 0, type_check=[str, list])
    ret_str = ""
    if isinstance(input_arg, str):
        ret_str = input_arg.capitalize()
    else:
        for var in input_arg:
            if not isinstance(var, str):
                report_error("Parameter to capitalize must be string, but is \"{}\"".format(type(var)))
            ret_str += var.capitalize()
    return ret_str


def split_string_for_case_change(input_arg):
    str_list = list()
    if isinstance(input_arg, str):
        input_arg = [input_arg]
    for item in input_arg:
        item_strings = re.findall(r'[A-Z|a-z|1-9](?:[a-z]+|[A-Z|1-9]*(?=[A-Z|_]|$))', item)
        str_list += item_strings
    return str_list


@register_function(([{"name": "camelCase", "param": False}, {"name": "CamelCase", "param": True}]))
def function_camel_case(self, args, first_capital):
    input_arg = self.get_arg(args, 0, type_check=[str, list])
    str_list = split_string_for_case_change(input_arg)
    ret_str = ""
    for item in str_list:
        item = item.lower()
        if first_capital is False:
            ret_str += item
            first_capital = True
        else:
            ret_str += item.capitalize()
    return ret_str


@register_function(([{"name": "snake_case", "param": False}, {"name": "SNAKE_CASE", "param": True}]))
def function_snake_case(self, args, capitals):
    input_arg = self.get_arg(args, 0, type_check=[str, list])
    str_list = split_string_for_case_change(input_arg)
    ret_str = ""
    for item in str_list:
        if capitals:
            ret_str += item.upper() + '_'
        else:
            ret_str += item.lower() + '_'
    return ret_str[:-1]


@register_function("print")
def function_print(self, args, unused):
    to_print = self.get_arg(args, 0)
    print(to_print)
    return ""


@register_function("exec")
def function_exec(self, args, unused):
    ret_str = ""
    for arg in args:
        ret_str += self.get_arg([arg], 0, type_check=str)
    return ret_str


@register_function("len")
def function_len(self, args, unused):
    arg0 = self.get_arg(args, 0, type_check=[list, dict, str, OrderedDict])
    return len(arg0)


@register_function("include")
def function_include(self, args, unused):
    file_name = self.get_arg(args, 0, type_check=str)
    file_abs = find_file(file_name + ".py")
    spec = importlib.util.spec_from_file_location("", file_abs)

    if spec is not None:
        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)
        self.refresh_functions()
        
        if hasattr(module, "__main__") and callable(module.__main__):
            return module.__main__()
        else:
            return ""
    else:
        return None
