"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : Convert_Lint2Std_op.py
Date of creation : 7.8.2018
Author Name/Dept : 
Organization     : 
Description      : Here we take .lin files as input and deliver .stdlin files as out put with BSH format and Severities.
"""

import re
import os
import argparse
import csv
import LogSupport

mkLogClass1 = LogSupport.LoggerClass("global.lint.mk", 'PATH_mk_LOGFILE')  
mkLogClass1.set_propagation(False)
mkLogger1 = mkLogClass1.Logger  


def split_filepath(split_me): 
    """ This function is used to split1 the given file path into file1 name directory and extension accordingly1"""
    dir, basename = os.path.split(split_me) 
    name, extension = os.path.splitext(basename)   
    return (dir, name, extension) 

"""class MessageContainer:
    
    class Message:
       def read_from_lint_tranlation_unit_line(self):
           pass 
           
       
       def read_from_lint_SW_system_line(self):
           pass
       
       def read_from_axiion(self): 
       
           pass
       
       def get_in_std_format(self):yes I made new mistakes this time and durch
           pass
   
    cfile_msg_dict = {}
    header_msg_dict = {}
    system_msg_dict = {}
    linfile_msg_dict= {}
    
   def read_from_lint_folder(self):
       pass
   
   def read_from_axivion_folder(self):
       pass
   
   def write_as_std_to_lint_folder(self):
       pass
   
   def write_as_std_axivion_folder(self):
       pass 
       
        """
"""
Global Variables
"""
no_of_misra_errors_dict = {}
errors_per_translation_unit = {}
errors_per_system_unit = {}
file_name_dict = {}
all_files_as_row = []   
class BSH_MessageFormat ():
    #rename the class name 
    #split the class into message content and 
    """This class contains methods, dictionaries and lists which are used to convert the Lint Messages into the BSH Standard Format """
    cfile_msg_dict = {}
    header_msg_dict = {}
    system_msg_dict = {}
    linfile_msg_dict= {}
    """search the lint messages1  """
    search_line_lint = re.compile('\s*(.+?)\s{2,10}(\d+)\s{2,10}(\d+)\s{1,10}(.+)') 
    search_stdlint_line = re.compile('\s*(.+?)\s(\d+)\s(\w*)\s(\d*(.+?))\s(\w*)\s(.+)')
    My_Misra_Local = [
        'Rule 1.1',
        'Rule 1.2',
        'Rule 2.6',
        'Rule 2.7',
        'Rule 3.1',
        'Rule 3.2',
        'Rule 4.1',
        'Rule 4.2',
        'Rule 5.2',
        'Rule 5.3',
        'Rule 5.4',
        'Rule 5.5',
        'Rule 6.1',
        'Rule 6.2',
        'Rule 7.1',
        'Rule 7.2',
        'Rule 7.3',
        'Rule 7.4',
        'Rule 8.1',
        'Rule 8.2',
        'Rule 8.4',
        'Rule 8.8',
        'Rule 8.10',
        'Rule 8.11',
        'Rule 8.12',
        'Rule 8.14',
        'Rule 9.2',
        'Rule 9.3',
        'Rule 9.4',
        'Rule 9.5',
        'Rule 10.1',
        'Rule 10.2',
        'Rule 10.3',
        'Rule 10.4',
        'Rule 10.5',
        'Rule 10.6',
        'Rule 10.7',
        'Rule 10.8',
        'Rule 11.1',
        'Rule 11.2',
        'Rule 11.3',
        'Rule 11.4',
        'Rule 11.5',
        'Rule 11.6',
        'Rule 11.7',
        'Rule 11.8',
        'Rule 11.9',
        'Rule 12.1',
        'Rule 12.3',
        'Rule 12.4',
        'Rule 13.3',
        'Rule 13.4',
        'Rule 13.6',
    #    'Rule 14.4', # this rule will not be followed (working group decision)
        'Rule 15.1',
        'Rule 15.2',
        'Rule 15.3',
    #    'Rule 15.4', # this rule will not be followed (working group decision)
    #    'Rule 15.5', # this rule will not be followed (working group decision)
        'Rule 15.6',
        'Rule 15.7',
    #    'Rule 16.1', # this rule will not be followed (working group decision)
        'Rule 16.2',
        'Rule 16.3',
        'Rule 16.4',
        'Rule 16.5',
    #    'Rule 16.6', # this rule will not be followed (working group decision)
        'Rule 16.7',
        'Rule 17.1',
        'Rule 17.3',
        'Rule 17.4',  # no phyton file available
        'Rule 17.6',
        'Rule 17.7',
    #    'Rule 18.4', # this rule will not be followed (working group decision)
        'Rule 18.5',
        'Rule 18.7',
        'Rule 18.8',
        'Rule 19.2',
    #    'Rule 20.1', # this rule will not be followed (working group decision)
        'Rule 20.2',
        'Rule 20.3',
        'Rule 20.4',
        'Rule 20.5',
        'Rule 20.6',
        'Rule 20.7',
        'Rule 20.8',
        'Rule 20.9',  # no phyton file available
        'Rule 20.10',
        'Rule 20.11',
        'Rule 20.12',
        'Rule 20.13', # no phyton file available
        'Rule 20.14', # no phyton file available
        'Rule 21.1', 
        'Rule 21.2',
        'Rule 21.3',
        'Rule 21.4',
        'Rule 21.5',
        'Rule 21.6',
        'Rule 21.7',
        'Rule 21.8',
        'Rule 21.9',
        'Rule 21.10',
        'Rule 21.11',
        'Rule 21.12',
           
        ]  
    My_Misra_System = [
        'Rule 1.3',
        'Rule 2.1',
        'Rule 2.2',
        'Rule 2.3',
    #    'Rule 2.4', # this rule will not be followed (working group decision)
        'Rule 2.5',
        'Rule 5.1',
        'Rule 5.6',
        'Rule 5.7',
        'Rule 5.8',
        'Rule 5.9',
        'Rule 8.3',
        'Rule 8.5',
        'Rule 8.6',
        'Rule 8.7',
        'Rule 8.9',
        'Rule 8.13',
        'Rule 9.1', # no phyton file available
        'Rule 12.2',
        'Rule 13.1',
        'Rule 13.2',
    #    'Rule 13.5', # this rule will not be followed (working group decision)
        'Rule 14.1',
        'Rule 14.2',
        'Rule 14.3', # no phyton file available
        'Rule 17.2',
        'Rule 17.5',
    #    'Rule 17.8', # this rule will not be followed (working group decision)
        'Rule 18.1', # no phyton file available
        'Rule 18.2', # no phyton file available
        'Rule 18.3', # no phyton file available
        'Rule 18.6', # no phyton file available
        'Rule 19.1',
        'Rule 22.1', # no phyton file available
        'Rule 22.2', # no phyton file available
        'Rule 22.3', # no phyton file available
        'Rule 22.4', # no phyton file available
        'Rule 22.5',
        'Rule 22.6', # no phyton file available  
        
        ]
    def __init__(self):
        self.code_analyzer = " "    
        self.file_location = " "
        self.error_line = -1
        self.error_column = -1
        self.rule = " "
        self.severity = " "
        self.error_message = " "
        
    @classmethod
    def create_msg_from_lint_line(cls, code_analyzer, file_location, error_line, error_column, rule, severity, error_message):
        #  create_msg
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
        
    
    @classmethod
    def convert_lint_to_BSH_std(cls, header, lint_message, file_name, file_path, error_line, error_column):
    
        """convert the Lint severity to BSH severity """
        """Lists for storing multiple  multiple MISRA messages within a Lint message"""
        severity =[] 
        Rule_id = []
        misra_list = []
        message_string = ""
        code_analyzer = "lint:"
        """start the search""" 
        """Search for Note or Info in the message string """    
        match = re.search("((Note|Info|Warning|Error)\s[0-9]+)", lint_message)

        if not match:
            print("Starts other then expected: ", lint_message )
            pass
        else:
            pattern_end = match.end()
            pattern_start = match.start()
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
                        msg = cls.create_msg_from_lint_line(code_analyzer, file_path, error_line, error_column, Rule_id[index], severity[index],error_message)

                        if header == "Yes":
                            BSH_MessageFormat.header_msg_dict[(file_name +".h")].append(msg)       
                        elif header == "No":
                            BSH_MessageFormat.cfile_msg_dict[(file_name+".c")].append(msg)
                        elif header == "System":
                            BSH_MessageFormat.system_msg_dict[file_name].append(msg)
                        elif header == "None1" :
                            BSH_MessageFormat.linfile_msg_dict[file_name].append(msg)
                        else:
                            pass                    
            else:
                """Find INFO of Warning or Errors other  than MISRA Rule"""
                match = re.search("(Info|Warning|Error) [0-9]+", message_string)
                if match:
                    pattern_start = match.start()
                    pattern_end = match.end()
                    Rule_none = "no_rule"
                    severity_none = "no_severity"
                    msg = cls.create_msg_from_lint_line(code_analyzer, file_path, error_line, error_column, Rule_none, severity_none, error_message)

                    if header == "Yes":
                        BSH_MessageFormat.header_msg_dict[(file_name+".h")].append(msg)                        
                    elif header == "No":
                        BSH_MessageFormat.cfile_msg_dict[(file_name+".c")].append(msg)
                    elif header == "System":
                        BSH_MessageFormat.system_msg_dict[file_name].append(msg)                        
                    elif header == "None1":
                        BSH_MessageFormat.linfile_msg_dict[file_name].append(msg)
                    else:
                        pass   
                     
    
    @classmethod
    def read_msg_from_lin_file(cls, lin_file_path):  
        """processed files contains the list  of all header and source files that are present inside a .lin file"""
        processed_files = {}  
        if os.path.isfile(lin_file_path):
            opened_lin_file = open(lin_file_path)

            for index,line_in_file in enumerate(opened_lin_file):

                if  line_in_file.startswith( "lint:"):
                        
                    """Find Code analyzer"""
                    rest_of_line =   line_in_file [5:]                #Excluding the string "lint:" from the message
                    match =  BSH_MessageFormat.search_line_lint.match( rest_of_line)
                    if not match:
                        """Error Handling: This is a new lint message format"""
                        pass     
                    else:
                        """Store the message in corresponding dictionary """
                        new_file_path = match.group(1)      # the file path
                        new_error_line = match.group(2)     # error_line number
                        new_error_column = match.group(3)   # error_column number
                        new_original_info= match.group(4)   # Lint Message        
        
                        dir, file_name, extension = split_filepath(new_file_path)
                        shall_I_store_message = ""        
                        if extension == '.c':
                            header = "No"
                            if file_name+".c" in processed_files:
                               shall_I_store_message = True
                            else:
                                shall_I_store_message = True
                                processed_files[file_name+".c"] = shall_I_store_message
                                BSH_MessageFormat.cfile_msg_dict[file_name+".c"] = []         
                        elif extension == '.h':
                            header = "Yes"
                            if (file_name +extension) in list(processed_files):
                                shall_I_store_message = True
                                processed_files[(file_name+extension)] = shall_I_store_message
                            else:
                                if (file_name +extension) in list(BSH_MessageFormat.header_msg_dict.keys()):
                                    shall_I_store_message = False                 
                                else:
                                    shall_I_store_message = True                        
                                    processed_files[(file_name+extension)] = shall_I_store_message
                                    BSH_MessageFormat.header_msg_dict[(file_name+extension)] = []
                        else:
                            pass
                            #todo: error message    

                        if  shall_I_store_message:
                                                    
                            """ Search and modify the severities """
                            BSH_MessageFormat.convert_lint_to_BSH_std( header, new_original_info, file_name, new_file_path, new_error_line, new_error_column)
                        else:
                            pass 
                            #print("Is is ok this is a  double header message"+ header + file_name + new_original_info)
                else:
                    #print("this is a raw string:" + line_in_file )
                    pass
            opened_lin_file.close()   
    
                            
    
    
    @classmethod
    def read_msg_from_lint_file_for_SW_system(cls, lint_SW_system_filepath):
        """Read the Message from Software System file"""
        BSH_MessageFormat.cfile_msg_dict = {}
        BSH_MessageFormat.header_msg_dict = {}
        BSH_MessageFormat.system_msg_dict = {}
        lint_file_path =""
        """Check if the given path is a String """
        if type(lint_SW_system_filepath) == str:
            if lint_SW_system_filepath.endswith(".lint"):
                lint_system_file = open(lint_SW_system_filepath)
                """ Get all the lint files form the lint file path"""
                for line in lint_system_file:
                    if line.startswith("--- Object:   "):
                        line_copy = line[14:-1] # remove the string "Object"
                        if line_copy.endswith(".lob"):                  
                            lin_filepath = line_copy[:-4]+'.lin'                          
                            BSH_MessageFormat.read_msg_from_lin_file(lin_filepath)
                            #return
                        else:
                            pass
                    else:
                        match =  BSH_MessageFormat.search_line_lint.match( line)
                        if match:
                            new_file_path = match.group(1)
                            new_file_path = new_file_path[6:] #excluding the string "lint: " from the messages
                            new_error_line = match.group(2)
                            new_error_column = match.group(3)
                            new_original_info= match.group(4)
                            dir, filename, extension =split_filepath(new_file_path)
                            file_name_pure =  filename + extension
                            #print (file_name_pure)
                            if file_name_pure not in BSH_MessageFormat.system_msg_dict: 
                                BSH_MessageFormat.system_msg_dict[file_name_pure] = []
                            header = "System"
                            BSH_MessageFormat.convert_lint_to_BSH_std(header, new_original_info, file_name_pure, new_file_path, new_error_line, new_error_column)
                            
                lint_system_file.close()
                
            else:
                print ("invalid .lint-file: %s"%lint_file_path)
                exit()
        else:
            print ("invalid string: %s"%lint_file_path)
            exit()
          
     
    
    @classmethod
    def convert_to_stdlin_file(cls,input_file_path):
        #split the function into converting and creating stdlin files 
        """This function should go through each lin file separately and produce a  text file after it is finished"""
        BSH_MessageFormat.linfile_msg_dict= {}
        lin_file = open(input_file_path)
        for line in lin_file:
            """ Search and sort the line for file path, line no., column no. and message string"""
            match =  BSH_MessageFormat.search_line_lint.match( line)
            if not match:
                """Error Handling:Raw message stream """
                if "lint" in line:
                    
                    file_name_pure = "System_file_message"
                    BSH_MessageFormat.linfile_msg_dict[file_name_pure]= []
                    code_analyzer = "lint:"
                    file_path = "no_file_path"
                    error_line = "no_line"
                    error_column = "no_column"
                    Rule = "no_rule"
                    severity = "no_severity"
                    error_message = line[5:]
                    msg = cls.create_msg_from_lint_line(code_analyzer, file_path, error_line, error_column, Rule, severity, error_message)
                       
                    BSH_MessageFormat.linfile_msg_dict[file_name_pure].append(msg)
                
            else:
                new_file_path = match.group(1) 
                new_file_path= new_file_path[6:] # remove the string "lint: " from the message
                new_error_line = match.group(2)
                new_error_column = match.group(3)
                new_original_info= match.group(4)
                dir, filename, extension = split_filepath(new_file_path)
                file_name_pure =  filename + extension
                if file_name_pure not in BSH_MessageFormat.linfile_msg_dict:
                    BSH_MessageFormat.linfile_msg_dict[file_name_pure]= []
                header= "None1"   
                BSH_MessageFormat.convert_lint_to_BSH_std(header, new_original_info, file_name_pure, new_file_path, new_error_line, new_error_column)
                
        lin_file.close()
        
    @classmethod
    def create_stdlin_file(cls, output_file_path):
        with open(output_file_path,"w") as txtfile:
            """Write all the .lin file messages"""
            for  file_name, message_list in BSH_MessageFormat.linfile_msg_dict.items():  

                for message in message_list:                         
                    txtfile.write( message.code_analyzer +"\t"+ message.file_location+"\t" + message.error_line +"\t" 
                                   + message.error_column +"\t"+ message.rule +"\t" + message.severity +"\t" + message.error_message +"\n")
                      
           # txtfile.close()
    @classmethod
    def create_stdlint_file(cls,output_file_path):
        """Write all the system file messages"""
       
        with open(output_file_path,"w") as txtfile:
            for message_list in BSH_MessageFormat.system_msg_dict.values():  
                for message in message_list:
                    txtfile.write( message.code_analyzer +"\t" + message.file_location+"\t" + message.error_line +"\t" 
                                   + message.error_column +"\t"+ message.rule +"\t" + message.severity  +"\t"+ message.error_message +"\n")
           # txtfile.close()
                 
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

    def write_eclipse_line(self, uni_output_filename):
        """Function for writing the output into a .txt file"""
        with open(uni_output_filename,"w") as txtfile:
            """Write all the source file messages"""
            for  message_list in BSH_MessageFormat.cfile_msg_dict.values():  
                for message in message_list:
                    txtfile.write( message.code_analyzer +"\t" + message.file_location+"\t" + message.error_line +"\t" 
                                   + message.error_column +"\t"+ message.rule +"\t" + message.severity +"\t"+ message.error_message+"\n")
                    
            """Write all the header file messages"""
            for message_list in BSH_MessageFormat.header_msg_dict.values():  
                for message in message_list:
                   txtfile.write( message.code_analyzer +"\t" + message.file_location +"\t" + message.error_line +"\t" 
                                   + message.error_column +"\t"+ message.rule +"\t" + message.severity +"\t"+ message.error_message+"\n")     
                   #print (message.file_location +"\t"+ message.error_line +"\t" + message.error_column +"\t"+ message.rule +"\t") 
            
            #print (BSH_MessageFormat.header_msg_dict.keys() )
                        
            """System Messages """
            for message_list in BSH_MessageFormat.system_msg_dict.values():  
                for message in message_list:
                    txtfile.write( message.code_analyzer +"\t" + message.file_location+"\t" + message.error_line +"\t" 
                                   + message.error_column +"\t"+ message.rule +"\t" + message.severity  +"\t"+ message.error_message +"\n")
    
    def prepare_data_for_csv_file(self,unique_stdlint_file_path): #csv_output_filename
        
        global no_of_misra_errors_dict  
        global errors_per_translation_unit
        global errors_per_system_unit 
        global file_name_dict
        errors_per_translation_unit_new = {}
       
        for misra_rule in BSH_MessageFormat.My_Misra_System:
            errors_per_system_unit[misra_rule] = 0
    
        if type(unique_stdlint_file_path) == str and unique_stdlint_file_path.endswith(".stdlint") and os.path.isfile(unique_stdlint_file_path):            
           unique_stdlint_file = open(unique_stdlint_file_path)
        else:
           print ("invalid .stdlint: %s"%unique_stdlint_file_path)
           exit()
        """get the result_folderpath """    
        result_folder_path = os.path.realpath(os.path.dirname(unique_stdlint_file_path))
        if not os.path.isdir(result_folder_path):
           print ("invalid result folder path: %s"%result_folder_path)
           exit()
        
        
        for all_translation_rules in BSH_MessageFormat.My_Misra_Local:
                        no_of_misra_errors_dict[all_translation_rules] = 0 
                        
        
        for line in unique_stdlint_file:
            #print (line)
            match =  BSH_MessageFormat.search_stdlint_line.match( line[5:])
               #here search the .stdlin file so make necessary changes here
            if match:
                misra_rule= match.group(4)
                if misra_rule in BSH_MessageFormat.My_Misra_System:
                    errors_per_system_unit[misra_rule] = errors_per_system_unit[misra_rule] + 1
                elif misra_rule in BSH_MessageFormat.My_Misra_Local :   
                    #for count,misra_rule in enumerate(BSH_MessageFormat.My_Misra_Local):
                        
                        
                    file_path = match.group(1) 
                    #print (file_path)
                    dir, filename, extension = split_filepath(file_path)
                    file_name_full = filename + extension
                    
                   
                        #errors_per_translation_unit[file_name_full] = no_of_errors_dict[all_translation_rules]
                    
                    
                    if file_name_full in errors_per_translation_unit.keys():
                        no_of_misra_errors_dict[misra_rule] = no_of_misra_errors_dict[misra_rule] + 1  # the misra rule occuring in the file is set to one and gets incremented according to the number of occurances
                        errors_per_translation_unit[file_name_full].update( {misra_rule: no_of_misra_errors_dict[misra_rule]})
                    else: 
                        no_of_misra_errors_dict [misra_rule] = 1
                        errors_per_translation_unit[file_name_full] = { misra_rule: no_of_misra_errors_dict[misra_rule]}
                else:
                    pass
                #if rule in misra_rule_vs_filename_and_count_dict: # count the occurring of each message for each file
                #misra_rule_vs_filename_and_count_dict[rule].update({opened_filename : misra_rule_vs_count_in_opened_file_dict[rule] })
                #else:
                #misra_rule_vs_filename_and_count_dict[rule] = {opened_filename : misra_rule_vs_count_in_opened_file_dict[rule]}
                    #print(misra_rule + "is a directory")        
            else: 
                print("Something is wrong")
        #print (no_of_misra_errors_dict) 
        #print(errors_per_translation_unit.keys())
        
        
        
        """ make  the count for all  other misra_rules which are not found as zero"""
        for all_translation_rules in BSH_MessageFormat.My_Misra_Local:
                        no_of_misra_errors_dict[all_translation_rules] = 0 

        for file_name_full in errors_per_translation_unit.keys():
            
            for misra_rule in no_of_misra_errors_dict.keys():
                if misra_rule  in errors_per_translation_unit[file_name_full].keys():   #how to check this values within a dictonary
                    pass
                    #print ("This is fine")
                else:
                    errors_per_translation_unit[file_name_full][misra_rule] = no_of_misra_errors_dict[misra_rule]
            
        #print(errors_per_translation_unit)
        #print (errors_per_translation_unit) 
              #  file_name_dict[file_name_full] = 1
                    #else:
                       # no_of_errors_dict[misra_rule] = 1
                       # errors_per_translation_unit[file_name_full] = no_of_errors_dict[other_rules]
                              
    def write_data_to_csv_file(self,csv_output_filename):   
        global all_files_as_row
        global file_name_dict
        global errors_per_translation_unit 
        global errors_per_system_unit
        global  no_of_misra_errors_dict
        misra_count_for_each_file = []
        #misra_rules_list = []
        #sorted_misra_list = []
        
        #misra_rule_row = []
        if not csv_output_filename[-4:]==".csv":
           print ("Warning: Output file %s has no CSV-file ending!!"%args.csv_output_filename)
        
        with open(csv_output_filename,"w") as csvfile: #open in binary mode
            print ("Writing to file %s ..."%os.path.realpath(csv_output_filename))
            writer = csv.writer(csvfile, delimiter=";")

            for key in errors_per_translation_unit.keys(): 
                all_files_as_row.append(key)
            all_files_as_row.sort()
            all_files_as_row.insert(0, " ") 
            writer.writerow(all_files_as_row + ["SUM"])  
        
            writer.writerow("")
            writer.writerow("")
            misra_rules_sorted = []

            for misra_rule in no_of_misra_errors_dict.keys():
                misra_rules_sorted.append(misra_rule)
            misra_rules_sorted.sort()
               
            for misra_rule in misra_rules_sorted:
                misra_rule_row = [" "] * len(all_files_as_row)
                misra_rule_row[0] = misra_rule
                #print (all_files_as_row)
                #print (errors_per_translation_unit)    
                summed = 0
                for filename in errors_per_translation_unit.keys():
                    #print(filename)
                    filename_index = all_files_as_row.index(filename)
                    #print (filename_index)
                    #print (filename_index)
                    misra_rule_row[filename_index] =   errors_per_translation_unit[filename][misra_rule]
                    summed = summed + misra_rule_row[filename_index]
                    
                misra_rule_row.append(summed)
                writer.writerow(misra_rule_row)
            writer.writerow("")    
            writer.writerow("")    

            #print (errors_per_translation_unit) 
            for misra_rule,count in errors_per_system_unit.items():
                row = [misra_rule,errors_per_system_unit[misra_rule]]
                writer.writerow(row)
            writer.writerow("")        
        
                    
                    
def main( lint_file_directory, uni_stdlint_output_filepath,csv_output_filepath):
    #first go through .lint and prepare .stdlin files from them
    lint_file_path = lint_file_directory + "\\" + "TENG_TESTER.lint"
    stdlint_file_path = lint_file_directory + "\\" + "TENG_TESTER.stdlint"
    BSH_MessageFormat.read_msg_from_lint_file_for_SW_system(lint_file_path)
    BSH_MessageFormat.create_stdlint_file(stdlint_file_path)
    for data_file_name in os.listdir(lint_file_directory):
        """Check for .lin files or .lint file in the given path"""
            
        if data_file_name.endswith(".lin"): 
            lin_file_path = lint_file_directory + "\\" + data_file_name
            stdlin_file_path = lint_file_directory + "\\" + data_file_name[:-4] + ".stdlin"
            """ test function is used to get the BSH Message format and severity """
            BSH_MessageFormat.convert_to_stdlin_file( lin_file_path)
            BSH_MessageFormat.create_stdlin_file(stdlin_file_path)
        else:
            continue
    output_file =  BSH_MessageFormat()
    output_file.write_eclipse_line(uni_stdlint_output_filepath)
    output_file.prepare_data_for_csv_file(uni_stdlint_output_filepath)
    
    output_file.write_data_to_csv_file(csv_output_filepath)
    #message_list = []
    #for k,v in BSH_MessageFormat.header_msg_dict.items():
    #    for values in v:
    #         message_list.append(  k +"\t"+  values.rule)
    #print (BSH_MessageFormat.header_msg_dict)
if __name__ == '__main__':
        
    # ============== Argument parsing ======================================
    parser = argparse.ArgumentParser(description='Get one argument')
    #parser.add_argument('axivion_folder_path', help='path of the axivion files')
    parser.add_argument('lint_file_path', help='path of the .lint file')    
    parser.add_argument('output_file_path', help='path of the .stdlint file')
    parser.add_argument('csv_output_filepath', help='path of the .csv file')

    args = parser.parse_args()
    
    print ("*************************************************************************************************************************")
    #print ("    Trying to handle folder ... \n --> %s " % args.axivion_folder_path)
    print ("*************************************************************************************************************************")
    
    print ("*************************************************************************************************************************")
    print ("    Trying to handle file ... \n --> %s " % args.lint_file_path)
    print ("*************************************************************************************************************************")
    main(args.lint_file_path,args.output_file_path,args.csv_output_filepath)
    #main(r"C:\Data\Test_tester\Results\STM32f0\STM32F071XX\KeilArm\TENG_TESTER\Lint", r"C:\Data\Test_tester\Results\STM32f0\STM32F071XX\KeilArm\TENG_TESTER\Lint\TENG_TESTER_UNI.stdlint")
  
    print("\n")
    print ("*************************************************************************************************************************")
    print ("    ... FINISHED!")
    print ("*************************************************************************************************************************")