"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : EhrhardtN
Date of creation : 16.03.2018
Author Name/Dept : 
Organization     : 
Description      : Abstraction of L3 to have a User-Interface, any from PTEng useable method is implemented here.
"""



import sys 
import time 
import os 
import logging 

from target import CxConfig, Target

EngineLogger = logging.getLogger("global")

"""
ToDO NiEh: do it more professionell.
"""            
def Change_Streams(prm_suffix, prm_outfile = None, prm_errorfile = None):
    
    if not type(prm_suffix) == str:
        EngineLogger.error("The first parameter needs to be a string. Streams are not changed.")
        return
        
    
    if prm_outfile: 
        full_filepath, file_name        = os.path.split(prm_outfile)
        new_path                        = full_filepath+ "\\" + prm_suffix + "_" +time.strftime("%Y%m%d_%H%M%S")+"_"+file_name
        sys.stdout = open(new_path, "w")
    if prm_errorfile:
        full_filepath, file_name        = os.path.split(prm_errorfile)
        new_path                        = full_filepath+ "\\" + prm_suffix + "_" + time.strftime("%Y%m%d_%H%M%S")+"_"+file_name    
        sys.stderr = open(new_path, "w")


class ComConfig:
    
    def __init__(self):
        pass
    
    @classmethod
    def create(cls, prm_local=False, prm_uda_port="UDA1", prm_host=12, prm_node=1, prm_subsystem=10, prm_message_base=36864, prm_flags="VDBUS_FLAG_9600_BAUD,VDBUS_FLAG_NO_COLLISION"):
        return CxConfig.create(prm_local, prm_uda_port, prm_host, prm_node, prm_subsystem, prm_message_base, prm_flags)
    
    def creat_from_file(self, prm_file):
        return CxConfig.create_from_cfg(prm_file)
    

class TargetDscr:
    
    def __init__(self):
        self.target = Target()

# -------------------------------------------------------------------------------
# FILLING A TARGET WITH INFORMATION
# -------------------------------------------------------------------------------

# AUTOMATICALLY VIA PSTD AND PSYD FILES
    def import_pstd(self,prm_path_pstd_file):
        return self.target.import_pstd(prm_path_pstd_file)
    
    def append_psyd(self,prm_path_psyd_file):
        return self.target.append_psyd(prm_path_psyd_file)
    
    def import_pstd_psyd(self,prm_path_pstd_file, prm_path_psyd_file):
        return self.target.import_pstd_psyd(prm_path_pstd_file, prm_path_psyd_file)
        
    def set_cvrli(self, cvrli_filepath):
        return self.target.set_cvrli(cvrli_filepath)
    
    def clean_pstd_and_psyd(self):
        return self.target.clean_pstd_and_psyd()
    

    
# -------------------------------------------------------------------------------
# EXECUTING PYTHON SCRIPTS IN THE TEST ENGINE (IN THE NAMESPACE OF THE TARGET!)
# -------------------------------------------------------------------------------
    def source_file(self, prm_path_pyfile, prm_linebyline=False):
        if prm_linebyline:
            return self.target.source_file_linebyline(prm_path_pyfile)
        else:
            return self.target.source_file(prm_path_pyfile)


# -------------------------------------------------------------------------------
# HANDLING THE TARGET CONNECTION
# -------------------------------------------------------------------------------
        
    def set_comconfig(self, prm_cx_config_type):
        return self.target.set_cx_config(prm_cx_config_type)
    
    def set_comconfig_from_file(self, prm_path_config):
        self.target.set_gen_cx_config(prm_path_config)
    
    def connect(self):
        return self.target.Connect()

    def disconnect(self):
        return self.target.Disconnect()
    
    def debug(self, prm_debugflag):
        return self.target.debug(prm_debugflag)
    
    
# -------------------------------------------------------------------------------
# Set global configuration
# -------------------------------------------------------------------------------       
    def reporting_on(self):
        return self.target.reporting_on()
           
    def reporting_off(self):
        return self.target.reporting_off()
        
    def wrapping_on(self):
        return self.target.wrapping_on()
        
    def wrapping_off(self):
        return self.target.wrapping_off()
    
    def pdf_reporting_on(self):
        return self.target.pdf_reporting_on()
        
    def pdf_reporting_off(self):
        return self.target.pdf_reporting_off()
        
    def constant_result_naming_on(self):
        self.target.constant_result_naming_on()
        
    def constant_result_naming_off(self):
        self.target.constant_result_naming_off()
    
    
      
# -------------------------------------------------------------------------------
# GET INFORMATION ABOUT REGISTERED FUNCTIONS OR VARIABLES
# -------------------------------------------------------------------------------
    
    def get_enumerator_value(self, prm_enumerator_str):
        return self.target.get_enumerator_value_from_enum_dict(prm_enumerator_str)
    
    def get_enumerator_name(self, prm_enumeration_str, prm_value_int):
        return self.target.get_enumerator_by_typedesc(prm_enumeration_str, prm_value_int)
    
    """ 
    def get_enumerators(self,prm_enumeration_str):
        return self.target.get_enumerators(prm_enumeration_str)
    """
    def var_address(self,prm_var_name):
        return self.target.VarAddress(prm_var_name)
        
    def func_address(self,prm_func_name):
        return self.target.FuncAddress(prm_func_name)
    

# -------------------------------------------------------------------------------
# HANDLING FUNCTIONS OR VARIABLES ON THE TARGET
# -------------------------------------------------------------------------------
    
    def get_var(self,prm_variable_str):
        if prm_variable_str.endswith(".&"):
            return self.target.VarAddress(prm_variable_str[:-2])
        elif prm_variable_str.endswith(".&&"):
            return self.target.FuncAddress(prm_variable_str[:-3])
        else:    
            return self.target.GetVar(prm_variable_str)
    
    def set_var(self, prm_var_name, prm_value):
        return self.target.SetVar(prm_var_name, prm_value)

    def call_func(self, prm_func_name, prm_parameterlist=[]):                                           #inwiefern ist dieser Aufruf beschraenkt, zurueckschreiben
        return self.target.CallFunc_catched(prm_func_name, prm_parameterlist)
    
    def call_func_by_wrapper(self, prm_func_name, prm_value_list = [], prm_parameter_identifier = []):
        return self.target.CallFunc_bywrapper(prm_func_name, prm_value_list, prm_parameter_identifier)
    
    
    
# -------------------------------------------------------------------------------
# TEST SUPPORT
# -------------------------------------------------------------------------------
    
    def execute(self, prm_path, suffix=None, project=""):
        return self.target.execute_unknown(prm_path, suffix, project=project)
        
    def read_out_json(self, prm_json):
        return self.target.readOutJson(prm_json)
# -------------------------------------------------------------------------------
# TEST REPORT
# -------------------------------------------------------------------------------   
    
        
    def get_testsequence_from_ResultXML(self, prm_pathto_xml_result):  
        return TestSequence.import_from_result_xml(prm_pathto_xml_result)    
    
        

# -------------------------------------------------------------------------------
# TEST Helper-functions
# ------------------------------------------------------------------------------- 

    #only for variable-strings
    def get_byteoffset(self, prm_structstring):
        return self.target.VarOffset(prm_structstring)
    
    #for strings starting with a definition
    def get_member_offset(self, prm_member_string):
        return self.target.find_member_info_of_definition_str(prm_member_string, "var")
    
    #ToDo NiEh: refactore with new soco-gui-function
    def create_member_list(self, prm_typedescriptor, prm_unions_members = []):
        pass
    
    def reset_wrapper_structs(self):
        self.target._reset_all_wrapper_structs()
        
    def readout_wrapper_barray(self, prm_no):
        self.target._read_out_wrapper_barray(prm_no)
        
    def execute_file(self, prm_filepath):
        self.target.execute_file(prm_filepath)
    

# -------------------------------------------------------------------------------
# TEST Logging-Functions
# -------------------------------------------------------------------------------            
    def set_global_log_level(self, prm_level):
        EngineLogger.setLevel(prm_level) 
        
    def set_SoCo_log_propagation(self, prm_bool):
        self.target.set_SoCo_log_propagation(prm_bool) 
    def set_SoCo_outstream_loglevel(self, prm_level):
        self.target.set_SoCo_stream_loglevel(prm_level)
    def set_SoCo_filestream_loglevel(self, prm_level):
        self.target.set_SoCo_file_loglevel(prm_level) 
    def close_SoCo_filestream(self):
        self.target.close_SoCo_filehandler()
    def close_SoCo_outstream(self):
        self.target.close_SoCo_streamhandler()
           
    def set_global_outstream_loglevel(self, prm_level):
        self.target.set_global_outstream_loglevel(prm_level)
    def set_global_filestream_loglevel(self, prm_level):
        self.target.set_global_filestream_loglevel(prm_level)
    def close_global_filestream(self):
        self.target.close_filestreamhandler()
    def close_global_outstream(self):
        self.target.close_outstreamhandler()
    def add_global_loghandler(self, prm_handler, prm_loglevel):
        self.target.add_handler(self, prm_handler, prm_loglevel)
    
# -------------------------------------------------------------------------------
# TEST Exit-functions
# -------------------------------------------------------------------------------    
    
    def destroy(self, prm_exit):
        
        logging.shutdown()
        sys.exit(prm_exit)