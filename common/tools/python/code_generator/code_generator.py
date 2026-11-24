"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying  materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : library for generating of files based on template file and
                   configuration dictionary
"""

import re
import sys
from shutil import copy
from filecmp import cmp
from enum import Enum
from cg_tools import *


class ExpressionType(Enum):
    VARIABLE = 1
    STRING = 2
    LIST = 3
    DICTIONARY = 4
    FUNCTION = 5


class ExpressionParser:

    def __init__(self, expression):
        self.expression_base = ''
        self.args = list()
        self.expression_type = ExpressionType.VARIABLE
        self.parse(expression)

    def is_variable(self):
        return self.expression_type == ExpressionType.VARIABLE

    def is_string(self):
        return self.expression_type == ExpressionType.STRING

    def is_list(self):
        return self.expression_type == ExpressionType.LIST

    def is_dictionary(self):
        return self.expression_type == ExpressionType.DICTIONARY

    def is_function(self):
        return self.expression_type == ExpressionType.FUNCTION

    @staticmethod
    def get_arg_list(string):
        brace_start = '{(['
        brace_end = '})]'
        escape_chars = 'nt'
        brace_count = 0
        arg = ''
        arg_list = list()
        quote = False
        special_char = False

        for char in string:
            if quote and special_char:
                special_char = False
                if char in escape_chars:
                    arg += '\\'
            elif char == '\\':
                special_char = True
                continue
            elif char == '\"' and not special_char:
                quote = not quote
            elif not quote:
                if char in brace_start:
                    brace_count += 1
                elif char in brace_end:
                    brace_count -= 1
                elif char == "," and brace_count == 0:
                    arg_list.append(arg.strip())
                    arg = ''
                    continue
            arg += char
        
        # Add argument to list
        arg_list.append(arg.strip())
        
        # Check for brace mismatch
        if brace_count != 0:
            report_error("Braces mismatch for expression: \"" + string + "\"")
        return arg_list

    parse_dict = {
        "\"\"": ExpressionType.STRING,
        "[]": ExpressionType.LIST,
        "{}": ExpressionType.DICTIONARY
    }

    def parse(self, expression):
        expression = expression.strip()
        outer_expr = expression[0] + expression[-1]
        arg_str = None

        # Check if expression is string, list or dictionary
        self.expression_type = ExpressionParser.parse_dict.get(outer_expr, ExpressionType.VARIABLE)

        if expression == "":
            self.expression_base = ""
        elif not self.is_variable():
            self.expression_base = expression[1:-1]
            if not self.is_string():
                arg_str = self.expression_base
        # Search for function expression
        elif re.search(r"^[\w|$]+\(.*\)$", expression) is not None:
            self.expression_type = ExpressionType.FUNCTION
            brace_start = expression.find('(')
            if brace_start > 0:
                self.expression_base = expression[0:brace_start]
                arg_str = expression[brace_start+1:-1]
        else:
            self.expression_base = expression

        if arg_str is not None:
            self.args = self.get_arg_list(arg_str)


class ExpressionHandler:
    config_dict_orig = dict()
    re_expr = re.compile(r'<\$(.+)>')
    macro_rec_name = None
    macro_rec_str = ""

    def __init__(self, dictionary=None):
        import built_in_functions
        self.refresh_functions()
        if dictionary is not None:
            ExpressionHandler.config_dict_orig = dictionary
            ExpressionHandler.flush_dict()
        self.expr = None

    @staticmethod
    def flush_dict():
        set_variable_dictionary(ExpressionHandler.config_dict_orig)

    @staticmethod
    def macro_rec_start(macro_name):
        ExpressionHandler.macro_rec_name = macro_name
        ExpressionHandler.macro_rec_str = ""

    @staticmethod
    def macro_rec_append(string):
        ExpressionHandler.macro_rec_str += string

    @staticmethod
    def macro_rec_end():
        set_variable(ExpressionHandler.macro_rec_name, ExpressionHandler.macro_rec_str)
        ExpressionHandler.macro_rec_name = None

    @staticmethod
    def is_macro_rec():
        return ExpressionHandler.macro_rec_name is not None

    def process_line(self, line):
        ret_str = line
        expr = ExpressionHandler.re_expr.search(line)
        expr_body = None

        if expr is not None:
            # Separate expression body from tags
            expr_body = expr.group(1)

        if expr_body == "macro_end":
            self.macro_rec_end()
            return ""

        if self.is_macro_rec():
            self.macro_rec_append(ret_str)
            return ""

        if expr_body is not None:
            ret_str = self.process(expr_body)
            if not isinstance(ret_str, str):
                report_error("result of final expression must be string, not " + str(type(ret_str)))
                return None

            # Replace original expression with returned value
            ret_str = ExpressionHandler.re_expr.sub(ret_str, line)

            if ret_str == '\n':
                ret_str = ""

        if is_output_disabled():
            return ""

        return ret_str

    def process(self, expression):
        if isinstance(expression, str):
            if expression == "":
                return ""
            expression = ExpressionParser(expression)
        elif not isinstance(expression, ExpressionParser):
            report_error("wrong type of expression")

        if expression.is_variable():
            return get_variable(expression.expression_base)

        elif expression.is_string():
            return expression.expression_base

        elif expression.is_function():
            function_name = expression.expression_base
            args = expression.args
            return self.exec_function(function_name, args)

        elif expression.is_list():
            ret_list = list()
            for item in expression.args:
                ret_item = self.process(item)
                ret_list.append(ret_item)
            return ret_list

        elif expression.is_dictionary():
            ret_dict = dict()
            for item in expression.args:
                item_tuple = item.split(":")
                key = self.process(item_tuple[0])
                value = self.process(item_tuple[1])
                ret_dict[key] = value
            return ret_dict

        report_error("expression \"\" cannot be evaluated".format(expression))

    def refresh_functions(self):
        func_dict = get_functions()
        for func in func_dict:
            func_name = func_dict[func]["name"]
            func_func = func_dict[func]["function"]
            setattr(ExpressionHandler, func_name, func_func)

    def exec_function(self, function_name, args):
        function_to_exec = get_function(function_name)
        if function_to_exec is not None:
            name = function_to_exec["name"]
            param = function_to_exec["param"]

            try:
                return getattr(self, name)(args, param)
            except:
                report_error("in function \"{}\" with arguments: {}".format(function_name, args))
        else:
            report_error("unknown function \"{}\"".format(function_name))

    def get_arg(self, args, arg_num, default=None, process=True, type_check=None):
        if len(args) > arg_num:
            ret_val = args[arg_num]

            if process:
                ret_val = self.process(ret_val)

            if type_check is not None:
                type_check_list = var_to_list(type_check)[0]
                if type(ret_val) not in type_check_list:
                    report_error("type of argument nr.{} must be {}, but is {}".format(arg_num, type_check_list, type(ret_val)))

            return ret_val

        elif default is None:
            report_error("argument nr.{} is required".format(arg_num))
        else:
            return default


class CodeGenerator:

    str_begin = 'BEGIN'
    str_end = 'END'

    def __init__(self, config_dict=None):
        self.re_expr_user_section = re.compile(
            '\/\* USER (CODE|SECTION) (' + self.str_begin + '|' + self.str_end + ')\s*(\w*) \*\/')
        
        if config_dict is None:
            config_dict = dict()

        self.expr_handler = ExpressionHandler(config_dict)


    def replace_user_sections(self, template_lines, user_sections):
        output_string = ""
        is_user_section = False
        insert_user_section = None
        template_lines = template_lines.split("\n")

        for line in template_lines:
            expr = self.re_expr_user_section.search(line)
            if expr is not None:
                if expr.group(2) == self.str_begin:
                    is_user_section = True
                    insert_user_section = expr.group(3)
                    output_string += line + "\n"
                elif expr.group(2) == self.str_end:
                    is_user_section = False

            if is_user_section is False:
                output_string += line + "\n"

            if insert_user_section is not None:
                if insert_user_section in user_sections:
                    output_string += user_sections[insert_user_section]
                insert_user_section = None

        return output_string[:-1]

    @staticmethod
    def save_output_file(output_file, lines, overwrite):
        out_dir = os.path.dirname(output_file)

        if out_dir != "":
            try:
                os.makedirs(out_dir)
            except FileExistsError:
                # directory already exists
                pass

        output_file_tmp = output_file + ".tmp"

        with open(output_file_tmp, 'w') as file:
            file.writelines(lines)

        if not os.path.exists(output_file):
            copy(output_file_tmp, output_file)
            print("generating " + output_file)
        else:
            if not cmp(output_file_tmp, output_file) and overwrite:
                copy(output_file_tmp, output_file)
                print("regenerating " + output_file)
            else:
                print("keeping " + output_file)

        os.remove(output_file_tmp)

    def read_user_sections(self, output_file):
        user_sections = dict()
        all_lines = open(output_file, "r").readlines()
        is_user_section = False
        user_section_name = ''
        user_section_lines = ''

        for line in all_lines:
            expr = self.re_expr_user_section.search(line)
            if expr is not None:
                if expr.group(2) == self.str_begin:
                    is_user_section = True
                    user_section_lines = ''
                    user_section_name = str(expr.group(3))
                    continue

                elif expr.group(2) == self.str_end:
                    is_user_section = False
                    user_sections[user_section_name] = user_section_lines
                    continue

            if is_user_section:
                user_section_lines += line
        return user_sections

    @staticmethod
    def find_cfg_template(search_paths, cfg_file, variant):
        separator = '/'
        cfg_dir_name = ["cfg_templates"]
        dir_tuple = cfg_file.split(separator)
        dir_len = len(dir_tuple)

        if dir_len > 1 and variant is not None and dir_tuple[-2] == variant:
            dir_tuple.pop(-2)

        # Try every combination for all searchPaths
        for path in search_paths:
            if dir_len == 1:
                cfg_template = separator.join([path] + cfg_dir_name + dir_tuple)
                if os.path.exists(cfg_template):
                    return cfg_template

            else:
                for n in range(1, dir_len):
                    cfg_template = separator.join([path] + dir_tuple[:n] + cfg_dir_name + dir_tuple[n:])
                    if os.path.exists(cfg_template):
                        return cfg_template
        return None

    def generate_file(self, template_file, output_file, overwrite=False):
        # default variables defining module name
        moduleName = os.path.splitext(os.path.basename(output_file))[0]
        ExpressionHandler.flush_dict()
        user_sections = None
        set_variable("ModuleName", moduleName)
        set_variable("MODULE_NAME", moduleName.upper())

        # Check if template exists
        if template_file is None or not os.path.exists(template_file):
            print("Error, template for " + output_file + " not found")
            return None

        set_search_path(output_file, 0)
        set_search_path(template_file, 1)
        set_search_path(__file__, 2)

        tpl_lines = open(template_file, "r").readlines()

        # Get user sections and paste them in template
        if os.path.exists(output_file):
            user_sections = self.read_user_sections(output_file)

        # Process special expressions
        if self.expr_handler is not None:
            out_lines = ""
            line_num = 1
            for line in tpl_lines:
                try:
                    out_lines += self.expr_handler.process_line(line)
                    # print and clear error/warning
                    if is_error():
                        print(get_error_message())
                        print("in file: " + template_file + ":" + str(line_num))
                    line_num += 1
                except:
                    print(get_error_message())
                    sys.exit("error while processing file: " + template_file + ":" + str(line_num))
            tpl_lines = out_lines

            if ExpressionHandler.is_macro_rec():
                sys.exit("error, macro recording was not terminated until end of the file")

        if user_sections is not None:
            tpl_lines = self.replace_user_sections(tpl_lines, user_sections)

        # Save generated file
        self.save_output_file(output_file, tpl_lines, overwrite)
