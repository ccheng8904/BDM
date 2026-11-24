"""
Copyright (c) 2024 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Author Name/Dept : Jens Lehmann
Description      : Takes a header and creates mocks and test dummy for all defined interfaces.
Usage            : python genDocTestFiles.py -i <header_file> [-s]
Arguments        : -i <header_file> : Path and name of the input header
                   -s               : option to mock static inline functions
"""

import re
import os
import argparse
import shutil
import time

from rplTmplt import TemplateFileReplace

def get_basename(file_path):
    return os.path.splitext(os.path.basename(file_path))[0]


class docTestReplacer(TemplateFileReplace):

    def __init__(self, dictionary, function_declarations, inline_mock = False):
        self.substDict = dictionary
        self.function_declarations = function_declarations
        self.inline_mock = inline_mock


        """
        Returns the name of the component based on the given file path.
        Args:
            file_path (str): The path of the file.
            e.g. common/prog/[component]/file.h
        Returns:
            str: The name of the component.
        """
    def get_component_name(self, file_path):
        dirname = os.path.dirname(file_path) 
        comp = os.path.basename(dirname)
        return comp

    # deliver the number of parameters for a function
    def calc_parameter_count(self, parameters):
        pcount = len(parameters)
        if pcount == 1:
            if parameters[0] == "void" or parameters[0] == "":
                pcount = 0
        return pcount

    # deliver the parameters for a function call
    def get_call_parameters(self,parameters):
        call_params = ""
        for param in parameters:
            match = re.search(r'\s*(\w+)(\[\])?\s*$', param)
            if match:
                if call_params != "":
                    call_params += ", "    
                call_params += match.group(1)
        call_params = re.sub(r'\*', '', call_params)
        return call_params

# ======================= Replacements START =======================
# The following functions are used to replace the placeholders in the template files.
# The placeholders are written in the form <$MOCK_MACROS> or <$UNIT_NAME>
    
    def MOCK_MACROS(self, example=False):
        ret_string = ""
        for function_name, info in self.function_declarations.items():
            if not info['inline'] or self.inline_mock:
                parameters = ', '.join(info['parameters'])
                parameters = re.sub(r'enum\s*|struct\s*', '', parameters)
                return_type = re.sub(r'enum\s*|struct\s*', '', info['return_type'])
                ret_string += f"        MAKE_MOCK{self.calc_parameter_count((info['parameters']))}({function_name:30}, {return_type:17} ( {parameters} ));\n"
                if example:
                    break
        
        return ret_string.rstrip("\n")
    
    def UNIT_ABBREVIATION(self):
        for function_name, info in self.function_declarations.items():
            match = re.search(r'^([A-Z]{2,8})_', function_name)
            if match:
                return match.group(1)
            
    
    def ext_mock_example(self):
        ret_string = self.MOCK_MACROS(True).lstrip()
        return f"      //{ret_string }"

    def int_mock_example(self):
        ret_string = f"      //MAKE_MOCK0({self.UNIT_ABBREVIATION()}_isValid                  , bool              ( void ));"
        return ret_string


    def MOCK_WRAPPERS(self):
        ret_string = ""
        for function_name, info in self.function_declarations.items():
            if not info['inline'] or self.inline_mock:
                parameters = ', '.join(info['parameters'])
                ret_string += f"    {info['return_type']} {function_name}({parameters})\n"
                ret_string += "    {\n"
                if info['return_type'] == "void":
                    ret_string += "        "
                else:
                    ret_string += "        return "
                ret_string += f"{self.unit_name()}_mock.{function_name}({self.get_call_parameters(info['parameters'])});\n"
                ret_string += "    }\n\n"

        return ret_string.rstrip("\n")
    
    def TEST_CASES(self):
        ret_string = ""
        for function_name, info in self.function_declarations.items():
            parameters = ', '.join(info['parameters'])
            params = self.get_call_parameters(info['parameters'])

            ret_string += f'        TEST_CASE("{function_name}")\n'
            ret_string +=  "        {\n"
            ret_string += f'            test_setup(); // prepare the test\n\n'
            ret_string += f'            SUBCASE("Check....")\n'
            ret_string +=  "            {"
            ret_string += f"   //{info['return_type']} {function_name}({parameters})\n"
            if info['return_type'] != "void":
                ret_string += f"                //{info['return_type']} ret = {function_name}({params});\n"
            else:
                ret_string += f"                //{function_name}({params});\n"
            ret_string += f"                CHECK(true);\n"
            ret_string +=  "            }\n"
            ret_string +=  "        }\n\n"

        return ret_string.rstrip("\n")
    
    def UNIT_NAME(self):
        return self.unit_name().upper()

    def unit_inc(self):
        include = ""
        path = os.path.normpath(self.get_component_name(self.substDict["header"])).replace("\\", "/")
        if path == ".":
            path = ""
        name   = os.path.basename(self.substDict["header"])
        include = os.path.join(path, name).replace("\\", "/")
        return include
    
    def priv_mock_inc(self):
        include = ""
        if self.substDict["header_priv"] is not None:
            name   = os.path.basename(self.substDict["header_priv"])
            name   = os.path.splitext(name)[0] + ".mock.h"
            include = f'#include \"{name}\"'
        return include
    
    def unit_name(self):
        return get_basename(self.substDict["header"])

    def unit_header(self):
        return os.path.basename(self.substDict["header"])
# ======================= Replacements END =======================

"""
Read the content of a file and return it as a string.
Args:
    file_path (str): The path to the file.
Returns:
    str: The content of the file.
Raises:
    FileNotFoundError: If the file is not found.
    IOError: If an error occurs while reading the file.
"""
def read_file_content(file_path):
    try:
        with open(file_path, 'r') as file:
            return file.read()
    except FileNotFoundError:
        print(f"File not found: {file_path}")
        return None
    except IOError:
        print(f"Error occurred while reading the file: {file_path}")
        return None



# Removes both C-style and C++ style comments from the given code.
def remove_comments(header):
    header = re.sub(r'/\*.*?\*/', '', header, flags=re.DOTALL)
    header = re.sub(r'//.*', '', header)
    return header


# Removes class definitions from the given header file content.
def remove_class_definitions(header):
    # Remove class definitions
    header = re.sub(r'class\s+\w+\s*\{.*?\};', '', header, flags=re.DOTALL)
    return header

# Removes typedef definitions from the given header file content.
def remove_typedefs(header):
    # Remove typedefs
    header = re.sub(r'typedef\s+.*?;', '', header, flags=re.DOTALL)
    return header

# Removes enum definitions from the given header file content.
def remove_enums(header):
    # Remove enums
    header = re.sub(r'enum\s+\w+\s*\{.*?\};', '', header, flags=re.DOTALL)
    return header

# Removes struct definitions from the given header file content.
def remove_structs(header):
    # Remove structs
    header = re.sub(r'struct\s+\w+\s*\{.*?\};', '', header, flags=re.DOTALL)
    return header

# Removes union definitions from the given header file content.
def remove_unions(header):
    # Remove unions
    header = re.sub(r'union\s+\w+\s*\{.*?\};', '', header, flags=re.DOTALL)
    return header

# Extracts external function declarations from the given header file content.
def extract_external_function_declarations(header):
    function_pattern = re.compile(r'(?:extern\s+)?([\w+\*\t ]+)\s+(\w+)\s*\(([\w\s\*\[\],]*?)\)\s*;', re.DOTALL)
    functions = {}

    for match in function_pattern.finditer(header):
        return_type, name, parameters = match.groups()
        parameters = [param.strip() for param in parameters.split(',')]
        functions[name] = {
            'return_type': return_type.strip(),
            'parameters': parameters,
            'inline': False
        }

    return functions

# Extracts static inline function declarations from the given header file content.
def extract_static_inline_function_declarations(header):
    function_pattern = re.compile(r'(?:static\s+inline\s+)([\w+\*\t ]+)\s+(\w+)\s*\(([\w\s\*\[\],]*?)\)', re.DOTALL)
    functions = {}

    for match in function_pattern.finditer(header):
        return_type, name, parameters = match.groups()
        parameters = [param.strip() for param in parameters.split(',')]
        functions[name] = {
            'return_type': return_type.strip(),
            'parameters': parameters,
            'inline': True
        }

    return functions

"""
Extracts function declarations from a header file.
Note: This is no a real parser, but a simple regular expression based extractor!

Args:
    header_file (str): The path to the header file.

Returns:
    dict: A dictionary containing the extracted function declarations.
            The keys are the function names, and the values are dictionaries
            with the following keys:
            - 'return_type': The return type of the function.
            - 'parameters': A list of the function's parameters.
            - 'inline': A boolean indicating whether the function is declared as static inline.
"""
def extract_function_declarations(header_file):
    header = read_file_content(header_file)
    # before parsing out the functions, remove all comments, class definitions, typedefs, enums, structs, and unions
    # This need to be done, as the function declaration parser is very simple and may not work correctly with complex code
    header = remove_comments(header)
    header = remove_class_definitions(header)
    header = remove_typedefs(header)
    header = remove_enums(header)
    header = remove_structs(header)
    header = remove_unions(header)
    external_functions = extract_external_function_declarations(header)
    static_inline_functions = extract_static_inline_function_declarations(header)
    return {**external_functions, **static_inline_functions}



"""
Creates a backup of a file in the specified backup folder.

Args:
    file_path (str): The path of the file to be backed up.
    backup_folder (str): The path of the folder where the backup will be created.

Returns:
    None
"""
def create_backup(file_path, backup_folder):
    file_path=file_path.replace("\\", "/")
    backup_folder=backup_folder.replace("\\", "/")
    if os.path.exists(file_path):
        try:
            os.mkdir(backup_folder)
        except:
            pass
        print(f"Creating backup of {file_path} in {backup_folder}")
        shutil.copy(file_path, backup_folder)



"""
Find a file with the given name in the specified search paths.

Args:
    file_name (str): The name of the file to find.
    search_paths (list): A list of paths to search for the file.

Returns:
    str or None: The full path of the file if found, or None if not found.
"""
def find_file(file_name, search_paths):
    for path in search_paths:
        full_path = os.path.join(path, file_name).replace("\\", "/")
        if os.path.isfile(full_path):
            return full_path
    return None

def main():
    # ============== Argument parsing ======================================
    parser = argparse.ArgumentParser(description='Creates mock definition form header declarations')
    parser.add_argument("-i", "--input",  dest="header", default="", required=True, help="Path and name of the input header")
    parser.add_argument("-s", "--sinline", dest="inline_mock", action='store_true', default=False, help="option to mock static inline functions")
    parser.add_argument("-f", "--search", dest="search_folders", action='append', default=[], help="search folder(s) for finding files")
    args = parser.parse_args()

    Header = find_file(args.header, args.search_folders)
    if Header is None:
        print(f'Header file "{args.header}" not found in search folders')
        return
    
    # find the private header file if it exists
    Header_priv = find_file(os.path.splitext(args.header)[0] + "_priv.h", args.search_folders)
    

    template_folder = os.path.join(os.path.dirname(os.path.dirname(os.path.abspath(__file__))), "templates")
    test_folder = os.path.join(os.path.dirname(Header), "test")
    try:
        os.mkdir(test_folder)
    except:
        pass
    
    # create a backup folder as former created files might be changed manually
    backup_subfolder_name = os.path.join(test_folder, str(int(time.time())) + ".bak")
    
    # crate the replacements dictionary
    replacements = dict(header=Header, header_priv=Header_priv)
    # parse the header file and extract the function declarations
    function_declarations = extract_function_declarations(Header)
    # create the Replacer object
    Replacer = docTestReplacer(replacements, function_declarations, args.inline_mock) 
    

    # create mock header for private header
    if Header_priv is not None:
        function_declarations_priv = extract_function_declarations(Header_priv)
        replacements_priv = dict(header=Header_priv, header_priv=None)
        Replacer_priv = docTestReplacer(replacements_priv, function_declarations_priv, args.inline_mock)        
        template = os.path.join(template_folder, "doctest_mock.tpl.h")
        outfile  = os.path.join(test_folder, f"{get_basename(Header_priv)}.mock.h")
        create_backup(outfile, backup_subfolder_name)
        Replacer_priv.replaceTemplate(template, outfile, False, False, '', True)
        template = os.path.join(template_folder, "doctest_mock.tpl.cpp")
        outfile  = os.path.join(test_folder, f"{get_basename(Header_priv)}.mock.cpp")  
        create_backup(outfile, backup_subfolder_name)
        # no need to create the test file for the private header, as it is not a public interface

    # create mock header
    template = os.path.join(template_folder, "doctest_mock.tpl.h")
    outfile  = os.path.join(test_folder, f"{get_basename(Header)}.mock.h")
    create_backup(outfile, backup_subfolder_name)
    Replacer.replaceTemplate(template, outfile, False, False, '', True)

    # create mock source
    template = os.path.join(template_folder, "doctest_mock.tpl.cpp")
    outfile  = os.path.join(test_folder, f"{get_basename(Header)}.mock.cpp")
    create_backup(outfile, backup_subfolder_name)
    Replacer.replaceTemplate(template, outfile, False, False, '', True)

    # create doctest dummy tests
    template = os.path.join(template_folder, "doctest.tpl.cpp")
    outfile  = os.path.join(test_folder, f"{get_basename(Header)}.dtest.cpp")
    create_backup(outfile, backup_subfolder_name)
    Replacer.replaceTemplate(template, outfile, False, False, '', True)



if __name__ == '__main__':
    main()


    


