"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : Convert_Lint2Std.py
Date of creation : 7.8.2018
Author Name/Dept : 
Organization     : 
Description      : Here we take .lin files as input and deliver .stdlin files as out put with BSH format and Severities.
"""

import re
import os
import pickle
import argparse
import glob
import json
import ast

"""Global Variables"""
message_dict = {}
file_dict = {}
file_error_list = []
header_file_list = []        
source_file_list = []

""" This function is used to split the given file path into file name directory and extension """
def split_filepath(split_me): 
    dir, basename = os.path.split(split_me) 
    name, extension = os.path.splitext(basename)   
    return (dir, name, extension) 

class BSH_MessageFormat ():
    
    cfile_msg_dict = {}
    header_msg_dict = {}
    system_msg_dict = {}
    linfile_msg_dict= {}
    """search the lint message  """
    search_line_lint = re.compile('\s*(.+?)\s{2,10}(\d+)\s{2,10}(\d+)\s{1,10}(.+)') 
    
    def __init__(self):
        self.code_analyzer = " "    
        self.file_location = " "
        self.error_line = -1
        self.error_column = -1
        self.rule = ""
        self.severity = ""
        self.error_message = ""
        
    @classmethod
    def create_msg_from_lint_line(cls, code_analyzer, file_location, error_line, error_column, rule, severity, error_message):  
        helper = cls()
        helper.code_analyzer = code_analyzer
        helper.file_location = file_location
        helper.error_line = error_line
        helper.error_column = error_column
        helper.rule = rule
        helper.severity = severity
        helper.error_message = error_message
        helper.build_key()
        return helper
        
    """Modify the Lint severity to BSH severity """
    @classmethod
    def modify_lint_line(cls, header, lint_message, file_name, file_path, error_line, error_column):
        """Lists for storing multiple  multiple MISRA messages within a Lint message"""
        severity =[] 
        Rule_id = []
        misra_list = []
        message_string = ""
        code_analyzer = ""
        """start the search"""
        if "lint:" in lint_message: 
            code_analyzer = "lint:"
        """Search for Note or Info in the message string """    
        match = re.search("((Note|Info|Warning|Error)\s[0-9]+)", lint_message)
        if not match:
            print("Starts other then expected: ", lint_message )
            pass
        else:
            pattern_end = match.end()
            pattern_start = match.start()
            original_info = lint_message[:pattern_end].strip()
            message_string = lint_message[pattern_start:]
            error_message = lint_message
            """Search for MISRA rule in the message string """  
            match = re.search("\[MISRA .*\]", message_string)
            if match:
                """Search for multiple MISRA Rules """
                for index,value in enumerate(re.finditer("MISRA", message_string)):
                    pattern_start = match.start()
                    message_string = message_string[pattern_start:]
                    match = re.search("\[MISRA .*\]", message_string)  
                    if match:
                        pattern_start = match.start()
                        pattern_end = match.end()
                        original_message = message_string[pattern_start:pattern_end].strip()
                        message_string = message_string[pattern_start:]
                        match=re.search("(Rule|Directive) [0-9]+\.[0-9]+", message_string)
                        if match:
                            pattern_start = match.start()
                            pattern_end = match.end()
                            """search for the MISRA Rule in bsh_rules """
                            if message_string[pattern_start:pattern_end] in BSH_MessageFormat.bsh_rules:
                                rule = message_string[pattern_start:pattern_end]
                                misra_list.append(rule)
                if misra_list != []:
                    """Check for more than one MISRA Violation  and change the severity accordingly"""
                    for misra_rule in misra_list:
                        severity.append( BSH_MessageFormat.bsh_rules[misra_rule ])
                        Rule_id.append(misra_rule)
                    for index,value in enumerate(severity):
                        msg = cls.create_msg_from_lint_line( code_analyzer, file_path, error_line
                                                                , error_column,Rule_id[index], severity[index],error_message)
                        if header == "Yes":
                            BSH_MessageFormat.header_msg_dict[file_name+".h"].append(msg)
                        elif header == "No":
                            BSH_MessageFormat.cfile_msg_dict[file_name+".c"].append(msg)
                        elif header == "System":
                            BSH_MessageFormat.system_msg_dict[file_name].append(msg)
                        elif header == "None" :
                            BSH_MessageFormat.linfile_msg_dict[file_name].append(msg)
                        else:
                            continue                       
            else:
                """Find INFO of Warning or Errors other  than MISRA Rule"""
                match = re.search("(Info|Warning|Error) [0-9]+", message_string)
                if match:
                    pattern_start = match.start()
                    pattern_end = match.end()
                    original_message = message_string [pattern_start:]
                    Rule_none = "no_rule"
                    severity_none = "none"
                    msg = cls.create_msg_from_lint_line( code_analyzer, file_path, error_line
                                                                , error_column,Rule_none, severity_none, error_message)
                    if header == "Yes":
                        BSH_MessageFormat.header_msg_dict[file_name+".h"].append(msg)
                    elif header == "No":
                        BSH_MessageFormat.cfile_msg_dict[file_name+".c"].append(msg)
                    elif header == "System":
                        BSH_MessageFormat.system_msg_dict[file_name].append(msg)
                    elif header == "None":
                        BSH_MessageFormat.linfile_msg_dict[file_name].append(msg)
                    else:
                        pass    
    """Read the lint message from the C file"""
    @classmethod
    def read_msg_from_lint_file_for_cfile(cls, lin_file):  
        """processed files contains the list of all header and source files that are present inside a .lin file """
        processed_files = {}  
        if os.path.isfile(lin_file):
            opened_lin_file = open(lin_file) 
            lint_message_line = {}  
            for index,line_in_file in enumerate(opened_lin_file):
                if not line_in_file.startswith( "lint:"):
                    continue                
                """Find Code analyzer"""
                rest_of_line =   line_in_file [5:]                
                match =  BSH_MessageFormat.search_line_lint.match( rest_of_line)
                if not match:
                    """Error Handling: This is a new lint message format"""
                    #print("No match for: ", rest_of_line)
                    dir, filename, extension =split_filepath(lin_file)
                    output_filename =  filename + ".stdlin"
                    output_filepath = "C:\Data\Test_tester\Results\STM32f0\STM32F071XX\KeilArm\TENG_TESTER\Lint\\"  + output_filename 
                    
                    with open(output_filepath,"a") as txtfile:
                        txtfile.write( line_in_file +"\n")
                             
                else:
                    """Store the message in corresponding dictionary"""
                    new_file_path = match.group(1) 
                    new_error_line = match.group(2)
                    new_error_column = match.group(3)
                    new_original_info= match.group(4)                
                    dir, filename, extension = split_filepath(new_file_path)
                    shall_I_store_message = ""
                    file_name_pure =  filename                  
                    if extension == '.c':
                        header = "No"
                        if file_name_pure+".c" in processed_files:
                           shall_I_store_message = True
                        else:
                            shall_I_store_message = True
                            processed_files[file_name_pure+".c"] = shall_I_store_message
                            BSH_MessageFormat.cfile_msg_dict[file_name_pure+".c"] = []         
                    elif extension == '.h':
                        header = "Yes"
                        if file_name_pure+".h" in processed_files:
                            shall_I_store_message = True
                            processed_files[file_name_pure+".h"] = shall_I_store_message
                        else:
                            if file_name_pure+".h" in BSH_MessageFormat.header_msg_dict:
                                shall_I_store_message = False      
                                processed_files[file_name_pure+".h"] = shall_I_store_message                    
                            else:
                                shall_I_store_message = True                        
                                processed_files[file_name_pure+".h"] = shall_I_store_message
                                BSH_MessageFormat.header_msg_dict[file_name_pure+".h"] = []
                    else:
                        pass
                        #todo: error message    
                    if not shall_I_store_message:
                        continue                        
                    """ Search and modify the severities """
                    BSH_MessageFormat.modify_lint_line( header, new_original_info, file_name_pure, new_file_path, new_error_line, new_error_column)
            opened_lin_file.close()                    
    
    """Read the Message from Software System file"""
    @classmethod
    def read_msg_from_lint_file_for_SW_system(cls, lint_SW_system_filepath):
        BSH_MessageFormat.cfile_msg_dict = {}
        BSH_MessageFormat.header_msg_dict = {}
        BSH_MessageFormat.system_msg_dict = {}
        lint_file_path =""
        """Check if the given path is a String """
        if type(lint_SW_system_filepath) == str:
            lint_SW_system_filepath = "C:\Data\Test_tester\Results\STM32f0\STM32F071XX\KeilArm\TENG_TESTER\Lint\\"  + lint_SW_system_filepath 
            if lint_SW_system_filepath.endswith(".lint"):
                lint_file_path = open(lint_SW_system_filepath)
            else:
                print ("invalid .lint-file: %s"%lint_file_path)
                exit()
        else:
            print ("invalid string: %s"%lint_file_path)
            exit()
        """ Get all the lint files form the lint file path"""
        for line in lint_file_path:
            if line.startswith("--- Object:   "):
                line_copy = line[14:-1]
                if line_copy.endswith(".lob"):                  
                    lint_cfile_filepath = line_copy[:-4]+'.lin'
                    BSH_MessageFormat.read_msg_from_lint_file_for_cfile(lint_cfile_filepath)
            else:
                match =  BSH_MessageFormat.search_line_lint.match( line)
                if match:
                    new_file_path = match.group(1) 
                    new_error_line = match.group(2)
                    new_error_column = match.group(3)
                    new_original_info= match.group(4)
                    dir, filename, extension =split_filepath(new_file_path)
                    shall_I_store_message = ""
                    file_name_pure =  filename + extension
                    if file_name_pure not in BSH_MessageFormat.system_msg_dict: 
                        BSH_MessageFormat.system_msg_dict[file_name_pure] = []
                    header = "System"
                    BSH_MessageFormat.modify_lint_line(header, new_original_info, file_name_pure, new_file_path, new_error_line, new_error_column)
        lint_file_path.close()  
     
    """This function should go through each lin file separately and produce a  text file after it is finished"""
    @classmethod
    def create_txt_file(self,folder_path,output_filename):
        BSH_MessageFormat.linfile_msg_dict= {}
        folder_file = open(folder_path)
        for line in folder_file:
            """ Search and sort the line for file path, line no., column no. and message string"""
            match =  BSH_MessageFormat.search_line_lint.match( line)
            if not match:
                """Error Handling:Raw message stream """
                output_filepath = "C:\Data\Test_tester\Results\STM32f0\STM32F071XX\KeilArm\TENG_TESTER\Lint\\"  + output_filename 
                with open(output_filepath,"w") as txtfile:
                    txtfile.write( line +"\n")
            else:
                new_file_path = match.group(1) 
                new_error_line = match.group(2)
                new_error_column = match.group(3)
                new_original_info= match.group(4)
                dir, filename, extension = split_filepath(new_file_path)
                shall_I_store_message = ""
                file_name_pure =  filename + extension
                if file_name_pure not in BSH_MessageFormat.linfile_msg_dict:
                    BSH_MessageFormat.linfile_msg_dict[file_name_pure]= []
                header= "None"   
                BSH_MessageFormat.modify_lint_line(header, new_original_info, file_name_pure, new_file_path,new_error_line,new_error_column)
        folder_file.close()
        output_filepath = "C:\Data\Test_tester\Results\STM32f0\STM32F071XX\KeilArm\TENG_TESTER\Lint\\"  + output_filename 
        with open(output_filepath,"a") as txtfile:
            all_code_analyzer =[]
            """Write all the .lin file messages"""
            for  file_name, message_list in BSH_MessageFormat.linfile_msg_dict.items():  
                msge = message_list
                for message in msge:
                    message_instance = message
                    all_code_analyzer.append(  message_instance.code_analyzer)
                    txtfile.write( message_instance.code_analyzer +"\t" + message_instance.file_location+"\t" + message_instance.error_line +"\t" \
                                   + message_instance.error_column +"\t"+ message_instance.rule +"\t" + message_instance.severity +"\t" \
                                   +"\t"+message_instance.error_message+"\n")
            txtfile.close()
                 
    def build_key(self):
        self.key = self.code_analyzer + "_" + self.file_location  + "_" + self.error_line +" _" \
                   + self.error_column + "_" + self.rule + "_" + self.severity
    
    bsh_rules = {
        'Directive 1.1' : 'required', 
        'Directive 2.1' : 'required',
        'Directive 3.1' : 'required',
        'Directive 4.1' : 'required',
        'Directive 4.2' : 'required',
        'Directive 4.3' : 'required',
        'Directive 4.4' : 'required',
        'Directive 4.5' : 'advisory',
        'Directive 4.6' : 'advisory',
        'Directive 4.7' : 'required',
        'Directive 4.8' : 'advisory',
        'Directive 4.9' : 'advisory',
        'Directive 4.10' : 'required',
        'Directive 4.11' : 'advisory',
        'Directive 4.12' : 'required',
        'Directive 4.13' : 'required',
        'Rule 1.1' : 'required',
        'Rule 1.2' : 'advisory',
        'Rule 1.3' : 'required', 
        'Rule 2.1' : 'required',
        'Rule 2.2' : 'advisory',
        'Warning 550' : 'advisory',
        'Rule 2.3' : 'advisory',
        'Rule 2.4' :  'advisory',
        'Rule 2.5' : 'advisory',
        'Rule 2.6' : 'mandatory',
        'Rule 2.7' : 'required',
        'Rule 3.1' : 'mandatory',
        'Rule 3.2' : 'mandatory',
        'Rule 4.1' : 'required',
        'Rule 4.2' : 'mandatory',
        'Rule 5.1' : 'required',
        'Rule 5.2' : 'required',
        'Rule 5.3' : 'mandatory',
        'Rule 5.4' : 'required',
        'Rule 5.5' : 'required',
        'Rule 5.6' : 'mandatory',
        'Rule 5.7' : 'mandatory',
        'Rule 5.8' : 'mandatory',
        'Rule 5.9' : 'mandatory',
        'Rule 6.1' : 'required',
        'Rule 6.2' : 'mandatory',
        'Rule 7.1' : 'mandatory',
        'Rule 7.2' : 'advisory',
        'Rule 7.3' : 'advisory',
        'Rule 7.4' : 'required',
        'Rule 8.1' : 'mandatory',
        'Rule 8.2' : 'mandatory',
        'Rule 8.3' : 'mandatory',
        'Rule 8.4' : 'required',
        'Rule 8.5' : 'mandatory',
        'Rule 8.6' : 'mandatory',
        'Rule 8.7' : 'advisory',
        'Rule 8.8' : 'required',
        'Rule 8.9' : 'advisory',
        'Rule 8.10' : 'mandatory',
        'Rule 8.11' : 'required',
        'Rule 8.12' : 'mandatory',
        'Rule 8.13' : 'advisory',
        'Rule 8.14' : 'mandatory',
        'Rule 9.1' : 'mandatory',
        'Rule 9.2' : 'mandatory',
        'Rule 9.3' : 'required',
        'Rule 9.4' : 'required',
        'Rule 9.5' : 'required',
        'Rule 10.1' : 'required',
        'Rule 10.2' : 'required',
        'Rule 10.3' : 'required',
        'Rule 10.4' : 'required',
        'Rule 10.5' : 'advisory',
        'Rule 10.6' : 'required',
        'Rule 10.7' : 'required',
        'Rule 10.8' : 'required',
        'Rule 11.1' : 'required',
        'Rule 11.2' : 'required',
        'Rule 11.3' : 'required',
        'Rule 11.4' : 'required',
        'Rule 11.5' : 'required',
        'Rule 11.6' : 'required',
        'Rule 11.7' : 'required',
        'Rule 11.8' : 'required',
        'Rule 11.9' : 'mandatory',
        'Rule 12.1' : 'required',
        'Rule 12.2' : 'required',
        'Rule 12.3' : 'advisory',
        'Rule 12.4' : 'required',
        'Rule 13.1' : 'required',
        'Rule 13.2' : 'mandatory',
        'Rule 13.3' : 'advisory',
        'Rule 13.4' : 'required',
        'Rule 13.6' : 'mandatory',
        'Rule 14.1' : 'required',
        'Rule 14.2' : 'required',
        'Rule 14.3' : 'advisory',
        'Rule 15.1' : 'required',
        'Rule 15.2' : 'required',
        'Rule 15.3' : 'required',
        'Rule 15.6' : 'required',
        'Rule 15.7' : 'required',
        'Rule 16.2' : 'required',
        'Rule 16.3' : 'advisory',
        'Rule 16.4' : 'required',
        'Rule 16.5' : 'required',
        'Rule 16.7' : 'required',
        'Rule 17.1' : 'required',
        'Rule 17.2' : 'required',
        'Rule 17.3' : 'mandatory',
        'Rule 17.4' : 'mandatory',
        'Rule 17.5' : 'required',
        'Rule 17.6' : 'mandatory',
        'Rule 17.7' : 'required',
        'Rule 18.1' : 'required',
        'Rule 18.2' : 'required',
        'Rule 18.3' : 'required',
        'Rule 18.5' : 'advisory',
        'Rule 18.6' : 'required',
        'Rule 18.7' : 'required',
        'Rule 18.8' : 'required',
        'Rule 19.1' : 'mandatory',
        'Rule 19.2' : 'advisory',
        'Rule 20.2' : 'required',
        'Rule 20.3' : 'required',
        'Rule 20.4' : 'required',
        'Rule 20.5' : 'advisory',
        'Rule 20.6' : 'required',
        'Rule 20.7' : 'required',
        'Rule 20.8' : 'required',
        'Rule 20.9' : 'required',
        'Rule 20.10' : 'advisory',
        'Rule 20.11' : 'required',
        'Rule 20.12' : 'required',
        'Rule 20.13' : 'required',
        'Rule 20.14' : 'required',
        'Rule 21.1' : 'required',
        'Rule 21.2' : 'required',
        'Rule 21.3' : 'required',
        'Rule 21.4' : 'required',
        'Rule 21.5' : 'required',
        'Rule 21.6' : 'advisory',
        'Rule 21.7' : 'required',
        'Rule 21.8' : 'required',
        'Rule 21.9' : 'required',
        'Rule 21.10' : 'required',
        'Rule 21.11' : 'required',
        'Rule 21.12' : 'required',
        'Rule 22.1' : 'required',
        'Rule 22.2' : 'mandatory',
        'Rule 22.3' : 'required',
        'Rule 22.4' : 'mandatory',
        'Rule 22.5' : 'mandatory',
        'Rule 22.6' : 'mandatory'
         }
  
    def write_eclipse_line(self, txt_output_filename):
        with open(txt_output_filename,"a") as txtfile:
            all_code_analyzer =[]
            """Write all the source file messages"""
            for  file_name, message_list in BSH_MessageFormat.cfile_msg_dict.items():  
                msge = message_list
                for message in msge:
                    message_instance = message
                    all_code_analyzer.append(  message_instance.code_analyzer)
                    txtfile.write( message_instance.code_analyzer +"\t" + message_instance.file_location+"\t" + message_instance.error_line +"\t" \
                                   + message_instance.error_column +"\t"+ message_instance.rule +"\t" + message_instance.severity +"\t"+ message_instance.severity \
                                   +"\t"+message_instance.error_message+"\n")
            """Write all the header file messages"""
            for  file_name, message_list in BSH_MessageFormat.header_msg_dict.items():  
                msge = message_list
                for message in msge:
                    message_instance = message
                    all_code_analyzer.append(  message_instance.code_analyzer)
                    txtfile.write( message_instance.code_analyzer +"\t" + message_instance.file_location+"\t" \
                                   + message_instance.error_line +"\t" + message_instance.error_column +"\t"+ message_instance.rule +"\t" + message_instance.severity \
                                   +"\t"+ message_instance.severity+"\t"+message_instance.error_message+"\n")        
            """Write all the system file messages"""
            for  file_name, message_list in BSH_MessageFormat.system_msg_dict.items():  
                msge = message_list
                for message in msge:
                    message_instance = message
                    all_code_analyzer.append(  message_instance.code_analyzer)
                    txtfile.write( message_instance.code_analyzer +"\t" + message_instance.file_location+"\t" \
                                   + message_instance.error_line +"\t" + message_instance.error_column +"\t"+ message_instance.rule +"\t" + message_instance.severity \
                                   +"\t"+ message_instance.severity+"\t"+message_instance.error_message+"\n")   
            
def main( lint_file_path, txt_output_filename):
    for linfile_name in os.listdir(lint_file_path):
        """Check for .lin files or .lint file in the given path"""
        if linfile_name.endswith(".lin"): 
            linfile_name = lint_file_path + "\\" + linfile_name
            dir, filename, extension =split_filepath(linfile_name)
            new_txt_output_filename =  filename + ".stdlin"
            """ test function is used to get the BSH Message format and severity """
            BSH_MessageFormat.create_txt_file( linfile_name, new_txt_output_filename)
       
        elif linfile_name.endswith(".lint"):
            exception_list = []
            BSH_MessageFormat.read_msg_from_lint_file_for_SW_system(linfile_name)
            new_output = BSH_MessageFormat()
            new_output.write_eclipse_line(txt_output_filename)
           
        else:
            continue
    

if __name__ == '__main__':
        
    # ============== Argument parsing ======================================
    #parser = argparse.ArgumentParser(description='Get one argument')
    #parser.add_argument('axivion_folder_path', help='path of the axivion files')
    #parser.add_argument('lint_file_path', help='path of the axivion files')    
    #parser.add_argument('txt_output_filename', help='path of the .lint file')

   # args                = parser.parse_args()
    
    print ("*************************************************************************************************************************")
    #print ("    Trying to handle folder ... \n --> %s " % args.axivion_folder_path)
    print ("*************************************************************************************************************************")
    
    print ("*************************************************************************************************************************")
    #print ("    Trying to handle file ... \n --> %s " % args.lint_file_path)
    print ("*************************************************************************************************************************")
    
    main(r"C:\Data\Test_tester\Results\STM32f0\STM32F071XX\KeilArm\TENG_TESTER\Lint", r"C:\Data\Test_tester\Tools\tester_output.txt")
  
    print("\n")
    print ("*************************************************************************************************************************")
    print ("    ... FINISHED!")
    print ("*************************************************************************************************************************")