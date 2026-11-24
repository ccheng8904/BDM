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
Description      : Provides basic classes for configuration of TEngSIO-Connection and execution of testcases
"""


import os

print("---PATH---")
print(os.environ["PATH"])
print("---PYTHONHOME---")
print(os.environ["PYTHONHOME"])
print("---PYTHONPATH---")
print(os.environ["PYTHONPATH"])
print("---END---")

import re
import sys
import json
import traceback
import collections
import ctypes

from tengsio.interface import TEngSIO_Interface
from tengsio.message import TEng_IO_Msg
from tengsio.constants import message_const

from globals.file_loader import FileLoader
from globals.folder_utils import split_filepath, PathFormer
from globals.constants import exec_const

#import all, for instance comparison
from soco.SourceCodeDescriptor import *

from report import report

from LogSupport import LoggerClass

#for constants
import logging

#for loading cxconfig
import xml.dom.minidom as dom

EngineLogClass = LoggerClass("global")
EngineLogger = EngineLogClass.Logger

#reg-expression to detect floats from unicode
float_regexp = r"\d+\.\d+"

#all spacebars
rep_sb_regexp = r"([\s]+)"

#regexpression for enumerators
enum_regexp = r"([\w]+)"

#find pointer arithmetic
find_summand_regexp = r"((([\+])|([\-]))?(([\d]+)|(((([\w]+(\[[\d]+\]){0,3})(\.)))+(([\&]{1,2})|([\*]{1,2})))))"

class CxConfig:
    def __init__(self):
        self.uda_port      = None
        self.host          = None
        self.target        = None
        self.subsystem     = None
        self.message_base  = None
        self.flags         = None
        self.local         = False            
    
    @classmethod
    def create(cls,local=False,uda_port="UDA1", host=12, node=1, subsystem=10, message_base=36864, flags="VDBUS_FLAG_9600_BAUD,VDBUS_FLAG_NO_COLLISION"):
        dummy = cls()
        if local:
            dummy.local = True
        else:
            dummy.uda_port      = uda_port
            dummy.host          = host
            dummy.node          = node
            dummy.subsystem     = subsystem
            dummy.message_base  = message_base
            dummy.flags         = flags

        return dummy

    
    @classmethod
    def create_from_cfg(cls, prm_comconfig):
        
        if os.path.isfile(prm_comconfig):
            cfg_xml = dom.parse(prm_comconfig)
        else:
            EngineLogger.error("The configuration for DBus-communication can not be loaded: {file_} does not exist.".format(file_ = prm_comconfig))
            return None
        
        FOLDER = ""
        UDA = None
        NODE = None
        DFLAGS = None
        HOST = None
        
        for node in cfg_xml.childNodes:
            if node.localName == "config":
                
                for tag in node.childNodes:
                    if tag.localName == "FOLDER":
                        if tag.firstChild:
                            FOLDER = tag.childNodes[0].data
                    if tag.localName == "UDA_TENG":
                        if tag.firstChild:
                            UDA = "UDA"+ str(int((tag.childNodes[0].data).split("-")[1]))
                        else:
                            UDA = "UDA1"
                    if tag.localName == "NODE_ADDRESS":
                        if tag.firstChild:
                            NODE = int(tag.childNodes[0].data)
                        else:
                            NODE = 0
                    if tag.localName == "TESTMSGSRV_SUBNODE_ADDRESS":
                        SUBNODE = int(tag.childNodes[0].data) 
                    if tag.localName == "DBUS_FLAGS":
                        if tag.firstChild:
                            DFLAGS = tag.childNodes[0].data  
                        else:
                            DFLAGS = ""
                    if tag.localName == "HOST":
                        if tag.firstChild:
                            HOST = int(tag.childNodes[0].data) 
                        else:
                            HOST = 0
        
        if FOLDER.upper().startswith("INTEL"):
            retval_cxconfig = cls.create(local=True)
        else:
            retval_cxconfig = cls.create(local=False, uda_port=UDA, host=HOST, node=NODE, subsystem=SUBNODE,message_base=36864,flags=DFLAGS)    
        
        return retval_cxconfig
            
    def check(self):
        cx_correct = True
        if self.local and type(self.local) == bool:
            return cx_correct
        else: 
            if self.uda_port and type(self.uda_port) == str:
                if not self.uda_port.startswith("UDA"):
                    cx_correct = False
                    EngineLogger.error("A wrong UDA-port was configured, exg.: \'UDA1\'.")
            else:
                cx_correct = False
                EngineLogger.error("A wrong UDA-port was configured it should be of type string, exg.: \'UDA1\'.")
                    
            if (not (self.node)) or (not (type(self.node) == int)):
                EngineLogger.error("Wrong target node configured, it has to be of type int, in the range: 1-255.")
                cx_correct = False
                
            if (not (self.subsystem)) or (not (type(self.subsystem) == int)):
                EngineLogger.error("Wrong subsystem of TEng-Msg-Server configured, it has to be of type int, in the range: 1-255. By default it is 10.")
                cx_correct = False
                
            if (not (self.message_base)) or (not (type(self.message_base) == int)):
                EngineLogger.error("Wrong message-base configured, it has to be of type int.")
                cx_correct = False
            
            if (not (self.flags)) or (not (type(self.flags) == str)):
                EngineLogger.error("Wrong D-Bus-flags configured, parameter has to be of type str. By defualt it is: \'VDBUS_FLAG_9600_BAUD,VDBUS_FLAG_NO_COLLISION\'")
                cx_correct = False
                
            if (not (self.host)) or (not (type(self.host) == int)):
                EngineLogger.error("Wrong host configured, it has to be of type int, in the range: 1-255. By default it is 12.")
                cx_correct = False
                
            return cx_correct

            
        
    def get_connection_parameters_as_string(self):
        if self.local:
            return "LOCAL"
        elif (not self.local) and self.uda_port and self.host and self.node and self.subsystem and self.message_base and self.flags:
            return "%s,%s,%s,%s,%s,%s"%(self.uda_port,self.host,self.node,self.subsystem,self.message_base,self.flags)
        else:
            return ""
        
    def get_connection_parameters_as_html_string(self):
        if self.local:
            return "LOCAL PC-Test"
        elif (not self.local) and self.uda_port and self.host and self.node and self.subsystem and self.message_base and self.flags:
            return "UDA-Port: %s<br/>Host: %s<br/>Node: %s<br/>Subsystem: %s<br/>MessageBase: %s<br/>Flags: %s"%(self.uda_port,self.host,self.node,self.subsystem,self.message_base,self.flags)
        else:
            return ""


class Target(TEngSIO_Interface):
    
    def __init__(self):
        
        self.connection_parameters          = None
        self._connected                     = False
        self.psyd_imported                  = False
        self.pstd_imported                  = False
        self.cvrli_imported                 = False
        self.cvrli_filepath                 = None
        #cvr-line-info[function] : [list(text), list(list(var_line_index))]
        self.cvr_line_info                  = None
        #kpi-dict[unit][_Line[x]] : 0
        self.prj_kpi                        = None
        self.source_description             = SourceCodeDescriptor()
        self.registered_enumerators         = {}
        self.message                        = TEng_IO_Msg() 
        
        self.wrapper_baseaddress_dict       = {}
        
        self.with_wrapper                   = True
        self.reporting                      = True
        self.pdf_reporting                  = False
        self.constant_result_naming         = False
        
        self.debugging                      = False
        
        self.poly_fileloader_execution = {
            FileLoader.TSEQ: self.execute_testsequence_fl,
            FileLoader.TSUITE: self.execute_testsuite_fl,
            FileLoader.INT: self.execute_inttest,
            FileLoader.INTSUITE: self.execute_intsuite
        }
        
        self.source_description.set_log_propagation(False)
        self.source_description.set_outstream_loglevel(logging.ERROR)
        self.source_description.close_outstreamhandler()
        self.set_global_outstream_loglevel(logging.ERROR)

    def debug_print(self, msg):
        if self.debugging:
            print(msg)
        
    def debug(self,value=1):
        self.message.debug(value)
    
    def import_pstd(self,pstd_filepath):
        if not os.path.isfile(pstd_filepath):
            EngineLogger.error("Wrong PSTD Filepath: %s not found!"%pstd_filepath)
            return
        
        self.source_description = SourceCodeDescriptor.CreateFrom_CreateFrom_PSTD_Filepath(pstd_filepath)
        self.pstd_imported = True

    def append_psyd(self,psyd_filepath):
        if self.psyd_imported:
            EngineLogger.error("A PSYD file is already imported!")
            return

        if not os.path.isfile(psyd_filepath):
            EngineLogger.error("Wrong PSYD Filepath! '%s' cannot be found!"%psyd_filepath)
            return
        
        self.source_description.AddFrom_PSYD_Filepath(psyd_filepath)
        self.psyd_imported = True
        
    def set_cvrli(self, cvrli_filepath):
        if self.cvrli_imported:
            EngineLogger.error("A CVRLI file is already imported!")
            return

        if not os.path.isfile(cvrli_filepath):
            EngineLogger.error("Wrong CVRLI Filepath! '%s' cannot be found!"%cvrli_filepath)
            return
        
        #works for python3 also with non-ascii characters
        with open(cvrli_filepath, "r") as f:
            self.cvr_line_info = json.load(f)
        
        if self.cvr_line_info == None:
            EngineLogger.error("Wrong CVRLI Fileformat! '%s' is not valid!"%cvrli_filepath)
            return

        self.cvrli_filepath = cvrli_filepath
        self.cvrli_imported = True
    
    def import_pstd_psyd(self,pstd_filepath,psyd_filepath):
        self.import_pstd        (pstd_filepath)
        self.append_psyd        (psyd_filepath)
        self.pstd_imported      = True
        self.psyd_imported      = True
        
    def clean_pstd_and_psyd(self):
        self.source_description = SourceCodeDescriptor()
        self.pstd_imported      = False
        self.psyd_imported      = False
    
    
    def pteng_is_initialized(self, prm_print = True):
        
        initialized = True
        
        if not self.pstd_imported:
            if prm_print:
                EngineLogger.error("No PSTD-file imported.")
            initialized = False       
        if not self.psyd_imported:
            if prm_print:
                EngineLogger.error("No PSYD-file imported.")
            initialized = False 
        if not self._connected:
            if prm_print:
                EngineLogger.error("The target is not connected.")
            initialized = False
            
        return initialized
    
    
    def pteng_is_initialized_with_error(self, prm_errorstring, prm_print = True):

        if not self.pteng_is_initialized(prm_print = prm_print):
            EngineLogger.error(prm_errorstring)
            return False
        else:
            return True
        
    # -------------------------------------------------------------------------------
    # Set global configuration
    # -------------------------------------------------------------------------------       
    def pdf_reporting_on(self):
        self.pdf_reporting = True
        EngineLogger.log(70, "Reporting to pdf-files activated.")
        return True
        
    def pdf_reporting_off(self):
        self.pdf_reporting = False
        EngineLogger.log(70, "Reporting to pdf-files deactivated.")
        return False
    
    def reporting_on(self):
        
        self.reporting = True
        EngineLogger.log(70, "Reporting to xml-files activated.")
        return True
        
    def reporting_off(self):
        
        self.reporting = False
        EngineLogger.log(70, "Reporting to xml-files deactivated.")
        return False
        
    def wrapping_on(self):
        
        self.with_wrapper = True
        EngineLogger.log(70, "Wrapping for execution of test is activated.")
        return True
        
    def wrapping_off(self):
        self.with_wrapper = False
        EngineLogger.log(70, "Wrapping for execution of test is deactivated.")
        return False
        
    def constant_result_naming_on(self):
        self.constant_result_naming = True
        EngineLogger.log(70, "Constant result naming for execution of test is activated.")
        return True
        
    def constant_result_naming_off(self):
        self.constant_result_naming = False
        EngineLogger.log(70, "Constant result naming for execution of test is deactivated.")
        return False
    
    
    
    
    # -------------------------------------------------------------------------------
    # LOGGING RELATED
    # -------------------------------------------------------------------------------    
    
    def set_SoCo_log_propagation(self, prm_bool):
        self.source_description.set_log_propagation(prm_bool)  
    def set_SoCo_outstream_loglevel(self, prm_level):
        self.source_description.set_outstream_loglevel(prm_level)
    def set_SoCo_filestream_loglevel(self, prm_level):
        self.source_description.set_filestream_loglevel(prm_level)
    def close_SoCo_outstreamhandler(self):
        self.source_description.close_outstreamhandler()
    def close_SoCo_filestreamhandler(self):
        self.source_description.close_filestreamhandler()
           
    def set_global_outstream_loglevel(self, prm_level):
        EngineLogClass.change_level_outstream(prm_level)
    def set_global_filestream_loglevel(self, prm_level):
        EngineLogClass.change_level_filestream(prm_level)
    def close_global_filestream(self):
        EngineLogClass.close_filestreamhandler()
    def close_global_outstream(self):
        EngineLogClass.close_outstreamhandler()
    def add_global_loghandler(self, prm_handler, prm_loglevel):
        EngineLogClass.add_handler(prm_handler, prm_loglevel)
        
        
    # -------------------------------------------------------------------------------
    # Functions to execute python-scripts in the namespace of TargetDescriptor.
    # -------------------------------------------------------------------------------
    
    """
    Source any Python py file in the namespace of target_descr line by line for better error handling
    
    With the aid of this method a file, which is located at the given path, is executed in the namespace of the according target descriptor. 
    For this reason the variable "self" can be used inside this file to access the instance of the target descriptor.
    This method reads the file line by line, so if an error occurs, it is easier to find out, at which line it occurs.
    """     
    def source_file_linebyline(self,path_to_pyFile):
        
        if os.path.isfile(path_to_pyFile):
            with open(path_to_pyFile, "r") as registration_file:
                for line in registration_file:
                    print(line)
                    exec(line)
        else:
            EngineLogger.error("File to execute line by line cannot be found at path "+str(path_to_pyFile))
            return None
    
    
    
    """
    Source any Python py file in the namespace of target_descr
    
    With the aid of this method a file, which is located at the given path, is executed in the namespace of the according target descriptor. 
    For this reason the variable "self" can be used inside this file to access the instance of the target descriptor.
    This methods reads the file AS ONE STRING, so if an error occurs, it is more complicated to find out where the error occurs.
    """     
    def source_file(self,path_to_pyFile):
        if os.path.isfile(path_to_pyFile):
            with open(path_to_pyFile, "r") as registration_file:
                exec(registration_file.read())  
        else:
            EngineLogger.error("File to execute cannot be found at path "+str(path_to_pyFile))
            return None





    # -------------------------------------------------------------------------------
    # HANDLING CONNECTIONS
    # -------------------------------------------------------------------------------

    
    """
    Handling Target Connections
    """           
    def set_cx_config(self,cx_config_type):
        
        if not cx_config_type.__class__ ==  CxConfig:
            EngineLogger.error("The input argument has to be an instance of class CxConfig!")
            #self.error_logging.attach_system_log("scc",  "The input argument has to be an instance of class CxConfig!")
            return None
        
        self.connection_parameters = cx_config_type
        
    def set_gen_cx_config(self, prm_path_comconfig):

        cx_cfg = CxConfig.create_from_cfg(prm_path_comconfig)
        if isinstance(cx_cfg, CxConfig): 
            self.set_cx_config(cx_cfg)
            return True
        else:
            return False
     
    """
    With the aid of this method a connection to the target is established.
    """    
    def Connect(self):

        if self._connected == True:
            EngineLogger.log(70, "Target is already connected!")
            #self.error_logging.attach_system_log("C",  "Target is already connected!")
            return 
        
        if not self.connection_parameters.__class__ == CxConfig:
            EngineLogger.error("Target Connection Parameters are not specified!")
            #self.error_logging.attach_system_log("C",  "Target Connection Parameters are not specified!")
            return 
        
        if not self.connection_parameters.check():
            EngineLogger.error("Wrong set of connection parameters!")
            #self.error_logging.attach_system_log("C",  "Wrong set of connection parameters!")
            return
        
        parameters=self.connection_parameters.get_connection_parameters_as_string()
        
        
        if self.IsolateSIOParameters(2,"TEngConnect /REMOTE="+parameters) == 0: # call function IsolateeSIOParameters to isolate configuration parameters
            # for example the whole string should be: "TEngConnect /REMOTE=UDA1,12,1,10,36864,VDBUS_FLAG_9600_BAUD,VDBUS_FLAG_NO_COLLISION"
            self.CloseSerialConnection();
            print( #has to be changed for Python
            """TEngInit Usage: 
            
            Remoteype = UDAx, x = UDA-Port-ID
            Remote parameters: 
                - uda_port  ="UDA1",
                - host      =12,
                - target    =1,
                - subsystem =10,
                - message_base  =36864,
                - flags="VDBUS_FLAG_9600_BAUD,VDBUS_FLAG_NO_COLLISION"
              UDAx,<own node id>,<target node id>,<Subsystem id>,<Message base>,[driver flag list]
            
            """
            )

        if ( self.IsSerialConnection()== 1 ) : #if Serial connection exists correctly and CPU type has been specified
            self._connected=True
            EngineLogger.log(logging.NOTSET, "Target connected.")
            
            if not self._get_wrapper_base_addresses():
                EngineLogger.error("Loading start-addresses for wrapper-structs failed!")
            
        else:
            EngineLogger.error("No Connection possible! Execution will be aborted ...")
        
        return self._connected
    
    def Disconnect(self):
        """
        With the aid of this method the connection to the according target is released.
        """
        self.CloseSerialConnection()
        self._connected=False
        EngineLogger.log(70, "Target disconnected.")
    
        return

    def get_print_target_info(self):
        target_information  = ""
        target_information += "GetSIOVersion:      %s\n"%self.GetSIOVersion()
        target_information += "GetLibVersion:      %s\n"%self.GetLibVersion()
        target_information += "GetVendor:          %s\n"%self.GetVendor()
        target_information += "GetCpuType:         %s\n"%self.GetCpuType()
        target_information += "GetDataModel:       %s\n"%self.GetDataModel()
        target_information += "GetDataModelString: %s"%self.GetDataModelString()
        
        print("\n------------------------------------------------------------")
        print("TENGSIO-VERSION")
        print(target_information)
        print("------------------------------------------------------------\n")
        return target_information.replace("\\t","\t").replace("\\r","\r").replace("\\n","\n")
        
  
    
    # -------------------------------------------------------------------------------
    # HELPER FUNCTIONS
    # ------------------------------------------------------------------------------- 
    
    def execute_file(self, prm_filepath):
        exec(compile(open(prm_filepath).read(), prm_filepath, 'exec'))       

  
    """
    Splits a Variable string. 
    Example: var_name.memberstruct.member[3] to [var_name, memberstruct, member[3]]
    Returns the list.
    """
    def make_element_list(self,var_name):
        return var_name.split(".")
    
    
    """
    Get the variable descriptor of a registered variable.
    A string would be resolved to its root.
    
    gsvd
    """
    def get_scd_variable_descriptor(self,variable_str):
        if not self.source_description:
            EngineLogger.error("No pstd and psyd file loaded. Please use at first method 'import_pstd' and 'append_psyd'!")
            return None
        
        root = self.make_element_list(variable_str.replace("[", ".[", 1))[0]
        
        for unit in self.source_description.unit_dictionary.values():
            desc = unit.variable_descriptions_dict.get(root, None)
            if not desc == None:
                return desc
        
        return None
                
                
    """
    Get the function descriptor of a registered function.
    """
    def get_scd_function_descriptor(self, prm_name_str):
        if not self.source_description:
            EngineLogger.error("No pstd and psyd file loaded. Please first use method 'import_pstd' and 'append_psyd'!")
            return None
        #print "In get funcdesc:"
        return self.source_description.get_function_descriptor_by_string(prm_name_str)
    
    
    
    
    
    """
    Check for a typeselection and return the resolved-type.
    """
    def resolve_type_selection(self, prm_typedescriptor):
        
        if isinstance(prm_typedescriptor, CTypeSelectionDescriptor) and not prm_typedescriptor.resolved_type == None:
            while isinstance(prm_typedescriptor, CTypeSelectionDescriptor) and not prm_typedescriptor.resolved_type == None:
                prm_typedescriptor = prm_typedescriptor.resolved_type
            
        return prm_typedescriptor
    
    
    """
    Returns the gtype for a typedescriptor
    
    Error-code: ggft 
    """
    def get_gtype_from_typedescriptor(self, prm_typedescriptor, prm_fromwrapper2 = False):

        if isinstance(prm_typedescriptor, CEnumTypeDescriptor):
            
            retval_gtype = prm_typedescriptor.evaluate_enum()[0]
    
        elif isinstance(prm_typedescriptor, CTypeSelectionDescriptor):
            
            #Only resolved typpedescriptors should be found at this point.
            if prm_typedescriptor.resolved_type == None:
            
                if prm_typedescriptor.original_type in ["float", "double"]:
                    retval_gtype = "int32"
                else:                
                    retval_gtype = prm_typedescriptor.gtype
                    
                if retval_gtype in ["bool", "BOOL", "Bool"]:
                    retval_gtype = "uint{bits}".format(bits = str(prm_typedescriptor.size_in_bytes*8))
                    
                #for void this function would not be executed
                if not ((retval_gtype in message_const.gtype2sizeinbytes) or (retval_gtype in message_const.gtype2stype)):
                    #EngineLogger.error("The basic type: {var} is unknown. Typedefinition-kind: {tk}. Resolved type: {rt}".format(var = retval_gtype, tk = prm_typedescriptor.type_definition_kind, rt = prm_typedescriptor.resolved_type))
                    return None
            
            else:
                #EngineLogger.error("The given typedescriptor still contains a resolved type, therefore its not resolved correctly.")
                return None
                
        
        elif isinstance(prm_typedescriptor, CCompositeTypeDescriptor):
        
            #EngineLogger.error("The given typedescriptor still contains a CCompositeType, therefore its not resolved correctly.")
            return None
        
        elif isinstance(prm_typedescriptor, CInPlaceTypeDescriptor):
            if prm_typedescriptor.is_pointer_to and (prm_typedescriptor.size_in_bytes == 4 or prm_typedescriptor.size_in_bytes == 3):
                retval_gtype = "uint32"
                #prm_typedescriptor.readout()
            elif prm_typedescriptor.is_pointer_to and prm_typedescriptor.size_in_bytes == 2:
                retval_gtype = "uint16"    
            #elif prm_typedescriptor.is_array_of > -1:
                #original_typedesc = self.source_description._search_original_typedescriptor(prm_typedescriptor, [], [])
                #retval_gtype = get_gtype_from_typedescriptor(original_typedesc)
            elif prm_fromwrapper2:
                if prm_typedescriptor.is_array_of > -1:
                    typedesc = prm_typedescriptor
                    while isinstance(typedesc, CInPlaceTypeDescriptor) and (typedesc.is_array_of > -1):
                        typedesc = typedesc.included_type
                        typedesc = self.resolve_type_selection(typedesc)
                        
                    retval_gtype = self.get_gtype_from_typedescriptor(typedesc)       
                
            else:
                #EngineLogger.error("The typedescriptor is still of type CInPlaceTypeDescriptor, describing an array, therefore its not resolved correctly.")
                return None
        
        return retval_gtype
    
    # -------------------------------------------------------------------------------
    # Basic read/write function
    # ------------------------------------------------------------------------------- 

    def _read_basic_type_from_address_64bit(self,  prm_address, prm_gtype, prm_sig, prm_offset=0):
        if prm_gtype.endswith("64"):
            ret_high = self._read_basic_type_from_address(prm_address+4, prm_gtype, prm_sig, prm_offset=prm_offset)
            ret_low = self._read_basic_type_from_address(prm_address, "uint32", prm_sig, prm_offset=prm_offset)
            return (ret_high<<32) | ret_low
        else:
            return self._read_basic_type_from_address(prm_address, prm_gtype, prm_sig, prm_offset=prm_offset)

	
	
    def _write_basic_type_at_address_64bit(self, prm_value, prm_address, prm_gtype, prm_sig, prm_offset=0):
        if prm_gtype.endswith("64"):
            high_word = prm_value>>32
            low_word = prm_value&0b11111111111111111111111111111111 
            ret_high = self._write_basic_type_at_address(high_word, prm_address+4, prm_gtype, prm_sig, prm_offset=prm_offset)
            ret_low = self._write_basic_type_at_address(low_word, prm_address, "uint32", prm_sig, prm_offset=prm_offset)
            return (ret_high<<32) | ret_low
        else:
            return self._write_basic_type_at_address(prm_value, prm_address, prm_gtype, prm_sig, prm_offset=prm_offset)
    
    """
    This function will enable you to read a basic gtype from a given address.
    """
    def _read_basic_type_from_address(self,  prm_address, prm_gtype, prm_sig, prm_offset=0): 
        
        if not type(prm_address) == int:
            EngineLogger.error("The first argument prm_address has to be of type int.The given variable is of {type}".format(type=type(prm_address)))
            return None
        #prm_gtype = prm_gtype    
        if not type(prm_gtype) == str:
            EngineLogger.error("The second argument prm_gtype has to be of type string. The given variable is of {type}".format(type=type(prm_gtype)))
            return None
        
        vtype = message_const.bytes2type[message_const.gtype2sizeinbytes[prm_gtype]]
        stype = message_const.gtype2stype[prm_gtype]
        
        message_request       = message_const.REQ_MSG_READ
        self.message.request  = ctypes.c_ubyte(message_request)
        
        #tengsio_signature   = type_descriptor.tms_signature_code
        
        DataPart_pSign      = prm_sig.encode()
        DataPart_pName      = str(prm_address).encode()
           
        self.message.create_var_message(prm_address, prm_offset, vtype, stype, DataPart_pSign, DataPart_pName)
        
        sign = prm_gtype.startswith("u")   #Done NiEh floats? floats are of type int32
                                   
        retval_value=self.message_send(sign)
            
        return retval_value
              
    """
    This function will enable you to write a very basic gtype to a given address.
    #At the moment prm_offset is not used.
    """    
    def _write_basic_type_at_address(self, prm_value, prm_address, prm_gtype, prm_sig, prm_offset=0):
        
        if not type(prm_value) == int:
            EngineLogger.error("The first argument: prm_value={value_} has to be of type int. The given variable is of {type}".format(value_=str(prm_value), type=type(prm_value)))
            return None
        
        if not type(prm_address) == int:
            EngineLogger.error("The second argument: prm_address has to be of type int.The given variable is of {type}".format(type=type(prm_address)))
            return None
            
        if not type(prm_gtype) == str:
            EngineLogger.error("The third argument: prm_gtype has to be of type string. The given variable is of {type}".format(type=type(prm_gtype)))
            return None
        
        if not ((prm_gtype in message_const.gtype2sizeinbytes) or (prm_gtype in message_const.gtype2stype)):
            EngineLogger.error("The prm_gtype {var} is unknown.".format(var = prm_gtype))
            return None
        
        vtype = message_const.bytes2type[message_const.gtype2sizeinbytes[prm_gtype]]
        stype = message_const.gtype2stype[prm_gtype]
        
        message_request       = message_const.REQ_MSG_WRITE
        self.message.request  = ctypes.c_ubyte(message_request)
        
        #tengsio_signature   = type_descriptor.tms_signature_code
        
        DataPart_pSign      = prm_sig.encode()
        DataPart_pName      = str(prm_address).encode()
          
        self.message.create_var_message(prm_address, prm_offset, vtype, stype, DataPart_pSign, DataPart_pName)

        if vtype == 0: # 1 byte
            self.message.TEng_Msg.DataPart.data.UC_data[0] = prm_value%(2**8)
        elif vtype == 1: # 2 bytes
            self.message.TEng_Msg.DataPart.data.US_data[0] = prm_value%(2**16)
        elif vtype == 2: # 4 bytes
            self.message.TEng_Msg.DataPart.data.UL_data[0] = prm_value%(2**32)
        else:
            EngineLogger.error("Invalid v-type to write.")
            return None
        
        sign = prm_gtype.startswith("u")        
                    
        retval_value=self.message_send(sign)
            
        return retval_value
    
    """
    Message send with catch of exceptions.
    ToDo NiEh: work with traceback.
    """
    def message_send(self, prm_sign):
        
        try:
            self.message.send()
        except:
            exc_info_ = sys.exc_info()
            formatted_exc = traceback.format_exception(exc_info_[0], exc_info_[1], exc_info_[2])
            EngineLogger.error("The TEngSIO message send operation caused: {exc_val_}".format(exc_val_ = formatted_exc[3]))
            return None
        
        retval_value=self.message.getVarRetVal(prm_sign)
        self.message.reset()
        
        return retval_value
            
    # -------------------------------------------------------------------------------
    # Functions to get address, offset, typedescriptor (->addressinfo)
    # -------------------------------------------------------------------------------
    
    """
    Resolves the string and returns the address.
    Does it work for referenced structs? Yes.
    """
    def get_address(self, prm_varstring):
        
        if not type(prm_varstring) == str:
            EngineLogger.error("Given Parameter has to be a string. It is of type: {typs}.".format(typs = type(prm_varstring)))
            return None
        
        element_list = self.make_element_list(prm_varstring.replace("[", ".["))
        
        variable_descriptor = self.get_scd_variable_descriptor(element_list[0])
        
        if not variable_descriptor:
            EngineLogger.error("No variable-descriptor found for: {var_}".format(var_ = prm_varstring))
            return None
        
        if not variable_descriptor.definition_description.symbol_location_descr:
            EngineLogger.error("No address was found for variable: {var_}".format(var_ = prm_varstring))
            return None
        
        retval_address = variable_descriptor.definition_description.symbol_location_descr.location_int
        
        if variable_descriptor.definition_description.resolved_type:
            found_type = variable_descriptor.definition_description.resolved_type
        else:
            found_type = variable_descriptor.definition_description.type
        
        retval_last_pointertype = None
            
        if isinstance(variable_descriptor.definition_description.type, CInPlaceTypeDescriptor):
            if variable_descriptor.definition_description.type.is_pointer_to:    
                retval_last_pointertype = variable_descriptor.definition_description.type

            
        found_type = self.resolve_type_selection(found_type)
        
        #if isinstance(found_type, CCompositeTypeDescriptor) or isinstance(found_type, CEnumTypeDescriptor):
            #if found_type.tag_identifier == "":
                #EngineLogger.error("The type of variable {var_} is an unnamed type, therefore it can't be used from the Testengine.".format(var_ = prm_varstring))
                #return None
        
        add_info = self.get_addressinfo_from_element_list(element_list, found_type, retval_address, retval_last_pointertype)
        
        if add_info:
            retval_address, retval_offset, retval_found_type, retval_last_pointertype, retval_last_member_desc  = add_info[0], add_info[1], add_info[2], add_info[3], add_info[4]     #return retval_address, retval_offset, retval_found_type, retval_last_pointertype
        
            retval_address += retval_offset    
        
            return retval_address, retval_offset, retval_found_type, retval_last_pointertype, retval_last_member_desc
        else:
            EngineLogger.error("No address-information received from element list for {var_}.".format(var_ = prm_varstring))
            return None
            

    """
    Calculates the offset and address for a given element list and a typedescriptor to start with, also saves the last found pointer-typedescriptor. 
    """
    def get_addressinfo_from_element_list(self, prm_element_list, prm_found_type, prm_start_address, prm_lastpointer_type):
        
        retval_found_type = prm_found_type
        retval_address = prm_start_address
        retval_offset = 0
        retval_last_pointer_type = prm_lastpointer_type
        retval_last_member_desc = None
        
        for el_index in range(1, len(prm_element_list)):
    
            if prm_element_list[el_index].startswith("["):
                
                if retval_found_type.resolved_type:
                    resolved_type = retval_found_type.resolved_type
            
                else:
                    resolved_type = retval_found_type                 
                
                resolved_type = self.resolve_type_selection(resolved_type)
                
                retval_offset += resolved_type.size_in_bytes * int(prm_element_list[el_index][1:-1])
                
                retval_found_type = retval_found_type.included_type
            
            elif prm_element_list[el_index] == "*":
                
                if el_index < len(prm_element_list)-1:          #the scripts needs to continue, because otherwise the pointer are readout two times, due to recursion.
                    if prm_element_list[el_index+1] == "*":
                        continue
                
                if isinstance(retval_found_type, CInPlaceTypeDescriptor):
                    
                    current_var_string = prm_element_list[0]
                    
                    for mem_index in range(1, el_index+1):
                        if prm_element_list[mem_index].startswith("["): # and not prm_element_list[mem_index+1].startswith("["):
                            current_var_string += prm_element_list[mem_index]
                        else:
                            current_var_string += "." + prm_element_list[mem_index] 
                    
                    if current_var_string.endswith(".*"):
                        current_var_string = current_var_string[:-2]
                    else:
                        EngineLogger.debug("Normally current_var_string should have ended with \'.*\'")        
                            
                    retval_address = self.GetVar(current_var_string)
                    retval_offset = 0
                    retval_last_pointer_type = retval_found_type
                    retval_found_type = retval_found_type.included_type
                else:
                    EngineLogger.debug("String was not able to be resolved, because a CInPlaceTypeDescriptor was expected for the identifier.")
                    return None
            
            else:
                found_the_member = False    
                
                for member_descr in retval_found_type.member_descriptions:
                    if member_descr.identifier == prm_element_list[el_index]:
                        
                        retval_found_type = member_descr.type
                        retval_last_member_desc = member_descr
                        try:
                            retval_offset += member_descr.symbol_location_descr.location_int
                        except:
                            EngineLogger.error("{member_} has no symbol_location.".format(member_ = member_descr.identifier))
                            return None
                        
                        if not (member_descr.size_in_bits == None):
                            EngineLogger.error("Bitfields can not be handled by PTeng: {member_} is a member of a bitfield.".format(member_ = member_descr.identifier))
                            return None
                            
                        
                        found_the_member = True
                        
                        break
                    
                if not found_the_member:    
                    EngineLogger.error("There was not found a member named: {member_}, while typeresolving.".format(member_ = prm_element_list[el_index]))
                    #for member_descr in retval_found_type.member_descriptions:
                        #member_descr.readout()
                    return None
            
            retval_found_type = self.resolve_type_selection(retval_found_type)
            
            #to be sure a last-pointer-type is set, if any pointer has occured
            if isinstance(retval_found_type, CInPlaceTypeDescriptor):
                if retval_found_type.is_pointer_to: 
                    retval_last_pointer_type = retval_found_type
        
        return retval_address, retval_offset, retval_found_type, retval_last_pointer_type, retval_last_member_desc
    
    # -------------------------------------------------------------------------------
    # Functions to read/set variables
    # -------------------------------------------------------------------------------
       
    def GetVar_evaluated(self, prm_variable_str):
        if prm_variable_str.endswith(".&"):
            return self.VarAddress(prm_variable_str[:-2])
        elif prm_variable_str.endswith(".&&"):
            return self.FuncAddress(prm_variable_str[:-3])
        else:    
            return self.GetVar(prm_variable_str)
    
    """
    Reads any value of the given string, that will be resolved (extracting address and type).
    """
    def GetVar(self, prm_var_name, origin_typedesc=None, var_desc=None):
        """
        With the aid of this method a variable on the target can be read.
        The instance "message" of TEng_IO_Msg is created in class "message_definition" (L1_C_Struct.py)
        """
        
        if not self.pteng_is_initialized_with_error("Getting Variable is not possible, because Python TestEngine is not initialized."):
            return None
    
        add_info = self.get_address(prm_var_name)
        if add_info:
            address, offset, typedesc, Neglect = add_info[0], add_info[1], add_info[2], add_info[3]
        else:
            if not var_desc == None:
                var_desc.add_info_string("No address Information received.")
            return None
        
        gtype = self.get_gtype_from_typedescriptor(typedesc)
        
        if not gtype:
            EngineLogger.error("No basic type found for variable: {var_}.".format(var_ = prm_var_name))
            if not var_desc == None:
                var_desc.add_info_string("No basic-type found.")
            return None
        
        set_value = self._read_basic_type_from_address_64bit(address, gtype, typedesc.tms_signature_code_2008)
        
        #print(typedesc.tms_signature_code_2016)
        
        if set_value == None:
            return None
        
        if not origin_typedesc == None:
            typedesc = origin_typedesc
        res_set_value = self._resolve_value_for_typedesc(set_value, typedesc, prm_cast=False, prm_eval_or_exp=True, prm_cnv_enum=True)    
        
        return res_set_value
    
    
    """
    Sets any value for the given variable-string (information about address and type), that will be resolved.
    """
    def SetVar(self, prm_var_name, prm_value, var_desc=None):
        """
        With the aid of this method a variable on the target can be set to the given value
        The instance "message" of TEng_IO_Msg is created in class "message_definition" (L1_C_Struct.py)
        """
        if not self.pteng_is_initialized_with_error("Setting Variable is not possible, because Python TestEngine is not initialized."):
            return None
        
        element_list = self.make_element_list(prm_var_name.replace("[", ".["))
        
        variable_descriptor = self.get_scd_variable_descriptor(element_list[0])
        
        if self.debugging:
            print(variable_descriptor)
            print(element_list[0])
            print(prm_var_name)
            
        if variable_descriptor == None and var_desc == None:
            print("The variable {var_} is no existent in the project-structure!".format(var_=prm_var_name))
            return None
        
        if variable_descriptor.definition_description.is_const:
            EngineLogger.warning("The variable: {var_} is qualified constant, therefore it is not written maybe.".format(var_ = prm_var_name))
            if not var_desc == None:
                var_desc.add_info_string("The variable is qualified constant.")
            #return None
        
        
        add_info = self.get_address(prm_var_name)
        if add_info:
            address, offset, typedesc, neglect, member_desc = add_info[0], add_info[1], add_info[2], add_info[3], add_info[4]
        else:
            if not var_desc == None:
                var_desc.add_info_string("No address Information received.")
            return None
        
        if prm_var_name.endswith("*") and isinstance(neglect, CInPlaceTypeDescriptor):
            if neglect.const_included_type:
                EngineLogger.warning("The pointer-variable: {var_} is pointing to constant, therefore the content is not written maybe.".format(var_ = prm_var_name))
                if not var_desc == None:
                    var_desc.add_info_string("The content of a pointer to const can not be written.")
                #return None
            
        if isinstance(member_desc, SourceCodeDescriptor.CompMemberDefinition):
            if member_desc.is_const:
                EngineLogger.warning("The member-variable: {var_} is qualified constant, therefore it is not written maybe.".format(var_ = prm_var_name))
                if not var_desc == None:
                    var_desc.add_info_string("The member of struct is qualified const.")
                #return None
                
            
        #cast if required value to float, also evaluate the pointer and dont expand it.
        res_value = self._resolve_value_for_typedesc(prm_value, typedesc, prm_cast=True, prm_eval_or_exp=False)
        
        gtype = self.get_gtype_from_typedescriptor(typedesc)
        
        if not gtype:
            EngineLogger.error("No basic type found for variable: {var_}.".format(var_ = prm_var_name))
            if not var_desc == None:
                var_desc.add_info_string("No basic-type found.")
            return None
        
        set_value = self._write_basic_type_at_address_64bit(res_value, address, gtype, typedesc.tms_signature_code_2008)
        
        #print(typedesc.tms_signature_code_2016)
        
        if set_value == None:
            if not var_desc == None:
                var_desc.add_info_string("There was no value set.")
            return None
        
        res_set_value = self._resolve_value_for_typedesc(set_value, typedesc, prm_cast=False, prm_eval_or_exp=True, prm_cnv_enum=True)
        
        return res_set_value
        
    def SetArray(self, identifier, a_range, value, var_desc=None):
        for i in range(0, a_range):
            ret = self.SetVar(identifier+"["+str(i)+"]", value, var_desc=var_desc)
            if ret == None:
                return None
                
        return True
            
    def GetArray(self, identifier, a_range, var_desc=None):
        values = []
        
        for i in range(0, a_range):
            values.append(self.GetVar(identifier+"["+str(i)+"]", var_desc=var_desc))
        
        return values
        
        
    """
    One function for every tupel-element.
    Both call get_address(...).
    """
    def VarAddress (self,var_name):
        
        if not self.pteng_is_initialized_with_error("Can not get address of variable, because Python TestEngine is not initialized."):
            return None
        
        if type(var_name)!=str:
            EngineLogger.error(("Argument (Variable name) must be of type 'str'! Here '%s' is of type '%s'")%(var_name,type(var_name)))
            return None
        
        retval_address = self.get_address(var_name)
        if retval_address:
            return retval_address[0]
        else:
            return None
    
    def VarOffset (self,var_name):
        if type(var_name)!=str:
            EngineLogger.error(("Argument (Variable name) must be of type 'str'! Here '%s' is of type '%s'")%(var_name,type(var_name)))
            return None
        
        if not self.pteng_is_initialized_with_error("Can not get offset of variable-string: {var_}, because Python TestEngine is not initialized.".format(var_ = var_name)):
            return None
        
        retval_offset = self.get_address(var_name)
        
        if retval_offset:
            return retval_offset[0]
        else:
            return None
    
    """
    Reads the address of any function from SoCoDe.    
    """
    def FuncAddress(self, func_name):
        
        if not self.pteng_is_initialized_with_error("Can not get address of function, because Python TestEngine is not initialized."):
            return None
        
        if type(func_name)!=str:
            EngineLogger.error(("Argument (Function name) must be of type 'str'! Here '%s' is of type '%s'")%(func_name,type(func_name)))
            return None
        
        function_descriptor = self.get_scd_function_descriptor(func_name)
        if not function_descriptor:
            EngineLogger.error(("Function '%s' is unknown!")%func_name)
            return None
        retval = function_descriptor.SymbolLocation.location_int
        if retval == 0: 
            retval = None
        return retval
    
    # -------------------------------------------------------------------------------
    # Methods to call functions
    # -------------------------------------------------------------------------------
    
    """
    Calls any function with given value_list.
    Can't interprete structs as a return-type or argument.
    """    
    def CallFunc(self, func_name, parameterlist=[]):
        """
        With the aid of this method a function on the target can be executed with the argument values given in "parameterlist".
        The instance "message" of TEng_IO_Msg is created in class "message_definition" (L1_C_Struct.py)
        """
        
        if not self.pteng_is_initialized_with_error("Call of function is not possible, because Python TestEngine is not initialized."):
            return None
        
        if type(parameterlist)!=list:
            EngineLogger.error("Parameters has to be given as a list, e.g. [1,2,3]! Here '%s' is of type '%s'!"%(parameterlist,type(parameterlist)))
            return None
        
        if not type(func_name) == str:
            EngineLogger.error("The function name needs to be a string! Here '%s' is of type '%s'!"%(func_name,type(func_name)))
            return None
        
        function_descriptor = self.get_scd_function_descriptor(func_name)
        
        if function_descriptor == None:
            EngineLogger.error("There wasn't found any function-descriptor for function: {func_}, therefore it is not executed.".format(func_ = func_name))
            return None
        
        return_typedesc = self.resolve_type_selection(function_descriptor.signature_description.return_declaration.type)
        
        if isinstance(return_typedesc, CCompositeTypeDescriptor):
            EngineLogger.error("Functions with structs/unions as return-type can not be called without the wrapper.")
            return None
        
        gtype = self.get_gtype_from_typedescriptor(return_typedesc)
        
        if gtype == None:
            EngineLogger.error("No basic-type found for the return-type of function: {func_}".format(func_ = func_name))
            return None
        
        if gtype.startswith("u"):
            sign = True
        else: 
            sign = False

        func_prm_declarations = function_descriptor.signature_description.parameter_declarations
        parameterlist_int = list()
        
        if not len(parameterlist) == len(func_prm_declarations):
            EngineLogger.error("Function call: {func_} not executed, the count of passed parameters is unequal with argument count.".format(func_ = func_name))
        
        for index in range(0, len(func_prm_declarations)):
        
            typedesc = self.resolve_type_selection(func_prm_declarations[index].type)
            
            if isinstance(typedesc, CCompositeTypeDescriptor):
                EngineLogger.error("Functions with structs/unions as parameters can not be called without the wrapper.")
                return None
            
            parameterlist_int.append(self._resolve_value_for_typedesc(parameterlist[index], typedesc, prm_cast=True, prm_eval_or_exp=True, prm_cnv_enum=False))
          
        message_request     = message_const.REQ_MSG_EXECUTE 
        ProcPart_address    = function_descriptor.SymbolLocation.location_int
        
        if not ProcPart_address:
            EngineLogger.error("Invalid address: 0x0 for execution of function %s"%func_name)
            return None
        
        ProcPart_pSign      = str(function_descriptor.signature_description.tms_signature_code_2008).encode()
        
        #print(str(function_descriptor.signature_description.tms_signature_code_2016))

        if not ProcPart_pSign: # pre_inizialize for TECCT_square_me. If invalid signature --> error in TEngSIO.dll
            EngineLogger.error("The PTEng did not find any signature for function: {func_name_}, as default \"US1_US1\" is used.".format(func_name_ = func_name))
            ProcPart_pSign      = "US1_US1" # ToKl can be deleted if SCD works fine
            
        ProcPart_pName      = str(func_name).encode()
        ProcPart_argCount   = len(function_descriptor.signature_description.parameter_declarations)

        if len(parameterlist_int) != ProcPart_argCount:
            EngineLogger.error("Wrong count of numbers. %s given but %s expected for function %s!")%(len(parameterlist_int),ProcPart_argCount,func_name)
            return None
        
        if not ProcPart_pSign:
            EngineLogger.error("Wrong function signature '%s'!") % ProcPart_pSign
            return None
        
        self.message.request                            = ctypes.c_ubyte    (message_request)
        self.message.TEng_Msg.ProcPart.address          = ctypes.c_void_p   (ProcPart_address)
        self.message.TEng_Msg.ProcPart.pSign            = ctypes.c_char_p   (ProcPart_pSign)
        self.message.TEng_Msg.ProcPart.pName            = ctypes.c_char_p   (ProcPart_pName)
        self.message.TEng_Msg.ProcPart.argCount         = ctypes.c_int      (ProcPart_argCount)
        
        if len(parameterlist_int)>0:                    
            self.message.TEng_Msg.ProcPart.FirstParam                = ctypes.c_int(parameterlist_int[0])
            for i in range(1,len(parameterlist_int)):     
                self.message.TEng_Msg.ProcPart.FurtherParams[i-1]    = ctypes.c_int(parameterlist_int[i])
        
        self.message.send()
        retval=self.message.getFuncRetVal(sign, return_typedesc.size_in_bytes)
        self.message.reset()
        
        if retval == None:
            return None
        
        if isinstance(function_descriptor.signature_description.return_declaration.type, CTypeSelectionDescriptor) and str(function_descriptor.signature_description.return_declaration.type.original_type) == "void":
            retval = "void"
        else:
            retval = self._resolve_value_for_typedesc(retval, return_typedesc, prm_cast=False, prm_eval_or_exp=True, prm_cnv_enum=True)

        return retval
        
   
    """
    Calls a function with the wrapper.
    The identifier-list expects a list with the values of the arguments, 
    if an argument is a struct, the corresponding index needs to provide 
    another list with the variable-strings of the struct-members the user wants to set.
    """
    """Legacy-Code, is not working because test-descripor was refactored"""
    def CallFunc_bywrapper(self, prm_funcname, prm_tuples_id_value, prm_tuple_return_id_exp):
       
        if not self.pteng_is_initialized_with_error("Execution of function with wrapper not possible, because Python TestEngine is not initialized."):
            return None 

        self.CallFunc_catched("TM_resetAllWrapperStructs", [])

        #def __init__(self,prm_function_parameter_value_list, prm_comparator, prm_reference_value, prm_testcase_identifier = "", prm_function_parameter_identifier_list=None, prm_result_value=None,prm_identifier_value = None, prm_passed=None, description=""):        
        
        identifier_list = list()
        value_list = list()
        
        #what prm_tuples_id_value has to look like: [(["a"], [5]), (["b.el1", "b.el2"], [2, 4]), (["c"], [90])]
        for tuple in prm_tuples_id_value:
            if type(tuple[0]) == list:
                identifier_list.append(tuple[0])
            else:
                EngineLogger.error("First element of argument-tuple has to be a list with the argument-identifier/s of corresponding parameter.")
                return None
                
            if type(tuple[1]) == int:
                EngineLogger.error("Second element of argument-tuple has to be a list with the arguments value/s of corresponding parameter.")
            elif type(tuple[1]) == list:
                value_list.extend(tuple[1])
        
        return_id_list = prm_tuple_return_id_exp[0]
        return_exp_list = prm_tuple_return_id_exp[1]
        
        testcase = TestCaseDescriptor(value_list, None, return_exp_list, 1, prm_function_parameter_identifier_list=identifier_list, prm_result_value=None, prm_identifier_value = return_id_list, prm_passed=None, description="")

        passed_function_descriptor = self.get_scd_function_descriptor(prm_funcname)
        
        if passed_function_descriptor == None:            
            EngineLogger.error("{fname} is unknown.".format(fname = prm_funcname))
            return None

        if not self._set_wrapper_parameter_struct(passed_function_descriptor, value_list, testcase):
            EngineLogger.error("Wrapper structs, were not set correctly.")
        
        self.CallFunc_catched(prm_funcname+"_ut_wrapper", [])
        
        returned_values = self._get_wrapper_return_struct(passed_function_descriptor, testcase)
      
        if self.debugging:
            print("Expectation:")
            print(return_exp_list)
            print("Outcome:")
            print(returned_values)
      
        return  None
    
    
    """
    Uses CallFunc to call any function, but Exceptions are catched and logged.
    """ 
    def CallFunc_catched(self, prm_func_name, prm_parameter_list):
        try:
            return self.CallFunc(prm_func_name, prm_parameter_list)
        except:
            exc_info_ = sys.exc_info()
            formatted_exc = traceback.format_exception(exc_info_[0], exc_info_[1], exc_info_[2])
            EngineLogger.error("Call of function {func_} caused: {exc_val_}".format(func_ = prm_func_name, exc_val_ = formatted_exc[-1]))
            
        return None
        
        
    def readOutJson(self, prm_json):
        json_path = PathFormer.prj_path +"/"+prm_json
        dir, file, ext = split_filepath(json_path)
        var_json = dict()
        with open(json_path, "r") as f:
            var_json = json.load(f)
        
        result_dict = {}
        for var in var_json:
            result = self.GetVar(var)
            exp = var_json[var]
            prnt_str = str(var)+" "+str(exp)+" == "+str(result)+" "

            if result == exp:
                prnt_str += "TRUE"
                result_dict[var] = [exp, result ,1]
            else:
                prnt_str += "FALSE"
                result_dict[var] = [exp, result ,0]
            
            print(prnt_str)
        ro_ci_logs_path = PathFormer.ci_logs_path+"RO_"+time.strftime("%Y_%m_%d-%H_%M_%S_")+"_"+file+".json"
        
        
        print("READOUT:VAR:"+ro_ci_logs_path)
        with open(ro_ci_logs_path, "w") as f:
            json.dump(result_dict, f)
                
        
    
    # -------------------------------------------------------------------------------
    # Methods for the execution of tests
    # -------------------------------------------------------------------------------
    
    def execute_unknown(self, prm_testinfo, prm_suffix=None, project=""):
        
        if not self.pteng_is_initialized_with_error("Execution of Test not possible, because Python TestEngine is not initialized."):
            return exec_const.exception_int
        
        if not self._connected:
            EngineLogger.error("Test cannot be executed beause the target is not connected!")
            return 
        file_loader = None
        if type(prm_testinfo) == str:
            file_loader = FileLoader(prm_testinfo)
            file_loader.analyseFile()
        else:
            file_loader = prm_testinfo
        if project == "":
            file_loader.test_desc.project = file_loader.basename
        else:
            file_loader.test_desc.project = project
        file_loader.test_desc.timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        file_loader.test_desc.target_information = self.get_print_target_info()
        file_loader.test_desc.conn_flags = self.connection_parameters.get_connection_parameters_as_string()
        file_loader.getSuffix(self.constant_result_naming, prm_suffix)
        if file_loader.loaded_type <= FileLoader.TSUITE:
            if self.with_wrapper:
                file_loader.test_desc.execution = "Wrapper-Function"
            else:
                file_loader.test_desc.execution = "TEngMsgSrv-Signature"
        
        exe_func = self.poly_fileloader_execution.get(file_loader.loaded_type, None)
        if exe_func:
            exe_func(file_loader)
        else:
            EngineLogger.error("The parameter: {prm} does not contain a valid fileloader.".format(prm = prm_testinfo))
            return exec_const.exception_int
        
        if file_loader.loaded_type <= FileLoader.TSUITE:
            self.getKPIDict()  
            self.combineKPIDict_gettingMissing(self.prj_kpi, file_loader.test_desc.kpi)
            file_loader.test_desc.prj_covr_string = getCOVRPercentageOfKPIDict(self.prj_kpi)
        
        file_loader.test_desc.prj_summary()
        file_loader.test_desc.print_summary()
        
        self.report_file_loader(file_loader)
        
        return file_loader.test_desc.validation

    def report_file_loader(self, file_loader): 
        creating_report = report(file_loader, self.prj_kpi, self.cvr_line_info)
        creating_report.generateReports(self.reporting, self.pdf_reporting)            
        creating_report.writeHTMLResultFiles()
        if file_loader.loaded_type <= FileLoader.TSUITE:
            self.dumpKPIDictToJSON(file_loader.basename)          
        
    """
    Executes a test-sequence, by instance of TestSequence.
    """    
    def execute_testsequence_fl(self, fl):    #report from testdescriptor          
        
        self.execute_test_by_descriptor(fl.test_desc)
        if fl.test_desc.covr_vars_exist: 
            fl.test_desc.covr_string = getCOVRPercentageOfKPIDict(fl.test_desc.kpi)


    """
    Executes a test-suite, by instance of TestSuite.
    """
    def execute_testsuite_fl(self, file_loader):
    
        if len(file_loader.test_desc.file_loader_list):
            
            file_list = list()
            for index, fl in enumerate(file_loader.test_desc.file_loader_list):
                if self.debugging:
                    print("Executing fl: {bn_}".format(bn_=fl.basename))
                fl.suffix = file_loader.suffix
                fl.test_desc.target_information = file_loader.test_desc.target_information 
                fl.test_desc.conn_flags = file_loader.test_desc.conn_flags
                self.execute_testsequence_fl(fl) 
                
                combineKPIDict(file_loader.test_desc.kpi, fl.test_desc.kpi)
                
                #dealing with double identifiers
                if fl.basename not in file_list:
                    file_list.append(fl.basename)
                else:
                    file_list.append(fl.basename+str(index))
                    fl.suffix += str(index)
            
            file_loader.test_desc.covr_string = getCOVRPercentageOfKPIDict(file_loader.test_desc.kpi)
            file_loader.test_desc.evaluate()
    
    def execute_intsuite(self, file_loader):
        for fl in file_loader.test_desc.file_loader_list:
            self.execute_inttest(fl)
            
        file_loader.test_desc.evaluate()
        
    def execute_inttest(self, file_loader):
        for tid in file_loader.test_desc.test_ids:
            start_m_time = time.time()
                 
            if tid.cleanup:
                for sa in tid.cleanup.set_array:
                    ret = self.SetArray(sa.identifier, sa.range, sa.value, var_desc=sa)
                    if ret == None:
                        tid.validation = exec_const.exception_int 
                
                for cf in tid.cleanup.call_function:
                    cf.return_value = self.CallFunc_catched(self, cf.identifier, cf.parameters)
                    if not cf.return_value:
                        cf.add_info_string("CleanUp-function caused exception.")
                        tid.validation = exec_const.exception_int           
                
                for ga in tid.cleanup.get_variable:
                    ga.value = self.GetVar(sa.identifier, var_desc=ga)
                    if None == ga.value:
                        tid.validation = exec_const.exception_int 
                    
            for e in tid.exe:
                for sa in e.set_array:
                    ret = self.SetArray(sa.identifier, sa.range, sa.value, var_desc=sa)
                    if ret == None:
                        tid.validation = exec_const.exception_int 
                
                for cf in e.call_function:
                    cf.return_value = self.CallFunc_catched(self, cf.identifier, cf.parameters)
                    if not cf.return_value:
                        cf.add_info_string("Exe-function caused exception.")
                        tid.validation = exec_const.exception_int 
                
                for ga in e.get_variable:
                    ga.value = self.GetVar(sa.identifier, var_desc=ga)
                    if None in ga.value:
                        tid.validation = exec_const.exception_int 
                    
            ba = tid.result.buffer.array
            ba.values = self.GetArray(ba.identifier, ba.range, var_desc=ba) 
            tid.delta_time = time.time()-start_m_time
            tid.evaluate()
            
        file_loader.test_desc.evaluate()
                    
                    
            
            
    """
    Executing tests on a target by testdescriptor without the wrapper.
    """
    def execute_test_by_descriptor(self,test_desc):
        
        test_desc.covr_vars_exist = False
        
        for test_sequence in test_desc.test_sequences_list:
            self.execute_testsequence(test_desc, test_sequence)
            
        test_desc.evaluate()
        
    def execute_testsequence(self, test_desc, test_sequence):
        #Find typedescriptor of function
        test_sequence.function_desc  = self.get_scd_function_descriptor(test_sequence.function)
        
        #make all testcases results exception
        if test_sequence.function_desc == None:
            test_sequence.add_info_string("Testfunction not existent in the project-structure.")
            
            for test_case in test_sequence.test_cases_list:
                test_case.evaluate()  #will make testcase fail, because no validation-set                 
            test_sequence.evaluate()
            return
        
        test_sequence.covr_vars_exist = self.isExistingCOVR(test_sequence.function_desc)
        if not test_desc.covr_vars_exist and test_sequence.covr_vars_exist:
            test_desc.covr_vars_exist = test_sequence.covr_vars_exist
        
        test_sequence.function_sig = test_sequence.function_desc.getSignature(funcname_add="-")
                 
        for test_case in test_sequence.test_cases_list:
            self.execute_testcase(test_sequence, test_case)
        
        if test_sequence.function_desc.define_unit:
            test_sequence.unit = test_sequence.function_desc.define_unit.unit_name
        else:
            test_sequence.unit = "None"
        
        if test_sequence.covr_vars_exist:
            if not test_sequence.validation == exec_const.exception_int:
                
                if not test_sequence.unit in test_desc.kpi:
                    test_desc.kpi[test_sequence.unit] = {}
            
                combineCOVRDict(test_desc.kpi[test_sequence.unit], test_sequence.covr_vars)     
                test_sequence.setCOVRPercentageOfDict()
        
        test_sequence.evaluate()        
        
    #einen sogenannten range-testcase, hat listen statt einzelnen values und bei der ausfuehrung einfach loopen     
    def execute_testcase(self, test_sequence, test_case):
        for i in range(0, test_case.range_test_len):
        
            if test_sequence.covr_vars_exist:
                if not test_case.validation == exec_const.exception_int:
                    self.resetCOVR(test_sequence.function_desc)
                    
            #Reset all structs
            if self.with_wrapper:
                self.reset_wrapper_structs(test_case)
            
            start_m_time = time.time()
            test_case.start_time = time.strftime("%H:%M:%S")
            
            self.execute_ramp_up(test_case)
                
            #set testcase variables
            self._set_testcase_variables(test_case)                  
            
            #set the Wrapper-structs       
            if len(test_case.parameter_list) == 0:
                structs_set = True
            else:
                if self.with_wrapper:
                    structs_set = self._set_wrapper_parameter_struct(test_sequence.function_desc, test_case)
                else:
                    structs_set = True
                    for prm in test_case.parameter_list:
                        prm.set_solved_in_indexed(self._resolve_value(prm.get_in_value_indexed()))
                        if prm.get_solved_in_indexed() == None:
                            structs_set = None
                     
            
            if structs_set == None:
                test_case.validation = exec_const.exception_int
                if self.with_wrapper:
                    test_case.add_info_string("Wrapper-argument-structs were not set correctly.")
                else:
                    test_case.add_info_string("Parameter-values were not resolved and set correctly.")
                returned_value = None
            else:
                if self.with_wrapper:
                    #debugging: self._read_out_wrapper_barray(0)
                    try:
                        if self.CallFunc(test_sequence.function_wrapper, []) == None:
                            test_case.add_info_string("Calling wrapper-function failed, unknown exception.")
                            test_case.validation = exec_const.exception_int
                    except:
                        test_case.validation = exec_const.exception_int
                        exc_info_ = sys.exc_info()
                        formatted_exc = traceback.format_exception(exc_info_[0], exc_info_[1], exc_info_[2])
                        test_case.add_info_string(formatted_exc[-1])
                                
                    returned_value = self._get_wrapper_return_struct(test_sequence.function_desc, test_case)  
                else:
                    #callFunc resolves values, because call_func can be used without testdescirptor
                    returned_value = self.CallFunc_catched(test_sequence.function, test_case.get_resolved_prm_list())
                    if returned_value == None:
                        pass
                    elif returned_value == "void":
                        returned_value = []
                    else:
                        returned_value = [returned_value]
            
            if test_sequence.covr_vars_exist:
                if not test_case.validation == exec_const.exception_int:
                    test_case.covr_vars = self.readoutCOVR(test_sequence.function_desc)
                    combineCOVRDict(test_sequence.covr_vars, test_case.covr_vars)
            
            if not type(returned_value) == list:
                test_case.add_info_string("Return values not read correctly!")
                test_case.validation = exec_const.exception_int
                
            for ret in test_case.return_list:
                ret.validate()
            
            if not test_case.validation == exec_const.exception_int:
                self._read_testcase_variables(test_case)
              
            self.execute_tear_down(test_case)
            
            test_case.delta_time      =  time.time()-start_m_time
            test_case.incrementRangeTestIndex()
        test_case.evaluate()       
        
    

    def execute_ramp_up(self, test_case):
        if test_case.set_up_function:
            if not self.CallFunc_catched(test_case.set_up_function, []):
                test_case.add_info_string("Set-UP-function caused exception.")
                test_case.validation = exec_const.exception_int
            
    def execute_tear_down(self, test_case):
        if test_case.tear_down_function:
            if not self.CallFunc_catched(test_case.tear_down_function, []):
                test_case.add_info_string("Tear-DOWN-function caused exception.")
                test_case.validation = exec_const.exception_int            

       
    # -------------------------------------------------------------------------------
    # Functions to resolve parameter values
    # -------------------------------------------------------------------------------
    
    
    
    """
    Resolve a sing value for any typedesc, the typedesc is needed to know for instance the size of the float value.
    
    This function is very flexible:
    with typedesc: enables to evaluate pointers, floats or enumerators
    with prm_cast = True: before resolving, possibly any int value is casted to float
    prm_eval_or_exp: The script will evaluate for False and Expand Pointer for True
    prm_cnv_enum: if true, for any int value the enumerator is searched for corresponding typedescriptor
    
    """        
    def _resolve_value_for_typedesc(self, prm_value, prm_typedesc = None, prm_cast=False, prm_eval_or_exp = False, prm_cnv_enum=False):
        
        buffer_value = prm_value
            
        #this is needed because: if someone types an int (exg.: 1, instead of 1.0) value in the field of 
        #ptest, that will expect floats, python can not know to treat this value as float
        if prm_cast:
            buffer_value = self.cast_int_to_float_for_typedesc(prm_typedesc, prm_value)
        
        if type(buffer_value) == str and buffer_value.endswith(".&&"):
            
            buffer_value = self.FuncAddress(buffer_value[:-3])
            
        elif type(buffer_value) == str and buffer_value.startswith("-") and buffer_value[1:].isdigit():
            
            buffer_value = (int(buffer_value))
            
        elif type(buffer_value) == str and buffer_value.startswith(exec_const.dont_care):
        
            return exec_const.dont_care
            
        elif type(buffer_value) == str and buffer_value == "void":
            
            buffer_value = buffer_value
            
        elif type(buffer_value) == str and buffer_value == "true":
            
            buffer_value = 1
        
        elif type(buffer_value) == str and buffer_value == "false":
            
            buffer_value = 0
            
        elif type(buffer_value) == str:
            
            buffer_value = self._evaluate_parameter_string(buffer_value)
            
        elif type(buffer_value) == int:
            
            buffer_value = self.unpack_float_value_of_typedesc(prm_typedesc, buffer_value)
            
            if prm_cnv_enum and isinstance(prm_typedesc, CEnumTypeDescriptor):
                buffer_value = self.get_enumerator_by_typedesc(prm_typedesc, buffer_value)
            
        elif type(buffer_value) == float:
            if prm_typedesc == None:
                pass
            else:
                buffer_value = self.pack_float_value_of_typedesc(prm_typedesc, buffer_value)
        elif type(buffer_value) in [list, tuple]:
        
            return (self._resolve_value_for_typedesc(buffer_value[0], prm_typedesc=prm_typedesc, prm_cast=prm_cast, prm_eval_or_exp=prm_eval_or_exp,prm_cnv_enum=prm_cnv_enum), self._resolve_value_for_typedesc(buffer_value[1], prm_typedesc=prm_typedesc, prm_cast=prm_cast, prm_eval_or_exp=prm_eval_or_exp,prm_cnv_enum=prm_cnv_enum))
            
        else:
            print("Von {value_} {type_}".format(value_=str(buffer_value), type_=type(buffer_value)))
            #("resolved zu {value_} {type_}".format(value_=str(prm_value), type_=type(prm_value)))
            return None
        
        if buffer_value == None:
            print("Von {value_} {type_}".format(value_=str(prm_value), type_=type(prm_value)))
            print("Resolved None {value_} {type_}".format(value_=str(buffer_value), type_=type(buffer_value)))
        
        return buffer_value
            
    def _resolve_value(self, values):
        #There to resolve possible address strings (string.&) in values
        
        if not type(values) in [tuple, list]:
            
            return self._resolve_value_for_typedesc(values)
           
        else:
            #only resolve two indexes for ranges, because we get value indexed
            if len(values) > 2:
                print("Warning: On one index in a value-list to resolve is a list found with len>2.")

            return (self._resolve_value_for_typedesc(values[0]), self._resolve_value_for_typedesc(values[1]))
                                                     
        
        

    
    def _pack_float_to_int(self, prm_float):
        
        i = ctypes.c_float(prm_float)
        pf = ctypes.pointer(i)
        pl = ctypes.cast(pf, ctypes.POINTER(ctypes.c_int))
        
        return int(pl.contents.value)
    
    def _unpack_int_to_float(self, prm_int):
        
        i = ctypes.c_int(prm_int)
        pf = ctypes.pointer(i)
        pl = ctypes.cast(pf, ctypes.POINTER(ctypes.c_float))
        
        return float(pl.contents.value)

    def _pack_double_to_int(self, prm_double):
        
        i = ctypes.c_double(prm_double)
        pf = ctypes.pointer(i)
        pl = ctypes.cast(pf, ctypes.POINTER(ctypes.c_int))
        
        return int(pl.contents.value)
    
    def _unpack_int_to_double(self, prm_int):
        
        i = ctypes.c_int(prm_int)
        pf = ctypes.pointer(i)
        pl = ctypes.cast(pf, ctypes.POINTER(ctypes.c_double))
        
        return float(pl.contents.value)
    
    """
    find enumerators
    find pointer arithmetic
    find mathematical arithmetic
    """
    def _evaluate_parameter_string(self, prm_string):
        
        new_string = re.sub(rep_sb_regexp, "", prm_string)
        p_mo = re.match(find_summand_regexp, new_string)
        e_mo = re.match(enum_regexp, new_string)
        if p_mo:
            summands = re.findall(find_summand_regexp, new_string)
            eval_string = ""
            
            summand_list = list([])
            
            for summand in summands:
                summand_list.append(summand[0])
              
            for summand in summand_list:
                #At this point there is no validation, if with .* or .** an address is get.
                if summand.endswith(".&") or summand.endswith(".*") or summand.endswith(".**") or summand.endswith(".&&"):                
                    
                    if summand.startswith("+") or summand.startswith("-"):
                        VALUE = self.GetVar_evaluated(summand[1:])
                        eval_string += str(summand[0])+str(VALUE) 
                    else:
                        VALUE = self.GetVar_evaluated(summand)
                        eval_string += str(VALUE)
                        
                else:
                    eval_string += str(summand)  
             
        
        #check for enumerators in the second step, because simple strings can be pointers.          
        elif e_mo:
            if e_mo.group(0) == "true":
                eval_string = str(1)
            elif e_mo.group(0) == "false":
                eval_string = str(0)
            else:     
                eval_string = str(self.get_enumerator_value_from_enum_dict(e_mo.group(0)))
             
        else:
            return None
        
        if eval_string:
            try:
                return eval(eval_string)
            except:
                print("Error: The string {paramstring} was not able to be evaluated.".format(paramstring = eval_string))
                return None
        else:
            print("Error: Returning None for string: {string_}, due to an empty string to evaluate.".format(string_ = prm_string))
            return None
    
    
    
    def unpack_float_value_of_typedesc(self, prm_typedesc, prm_value):
        retval = prm_value 
        if isinstance(prm_typedesc,  CTypeSelectionDescriptor):
            if prm_typedesc.original_type == "float":
                retval = self._unpack_int_to_float(retval)
            elif prm_typedesc.original_type == "double":
                retval = self._unpack_int_to_double(retval) 
        return retval
    
    def pack_float_value_of_typedesc(self, prm_typedesc, prm_value):
        retval = prm_value 
        if isinstance(prm_typedesc,  CTypeSelectionDescriptor):
            if prm_typedesc.original_type == "float":
                retval = self._pack_float_to_int(retval)
            elif prm_typedesc.original_type == "double":
                retval = self._pack_double_to_int(retval)
        return retval
    
    def cast_int_to_float_for_typedesc(self, prm_typedesc, prm_value):
        retval = prm_value
        if isinstance(prm_typedesc,  CTypeSelectionDescriptor) and (type(retval) == int):
            if prm_typedesc.original_type == "float":
                retval = float(retval)
            elif prm_typedesc.original_type == "double":
                retval = float(retval) 
                    
        return retval
    
    
    # -------------------------------------------------------------------------------
    # Functions for getting informations about enumerations.
    # -------------------------------------------------------------------------------

    def get_enumerator_value_from_enum_dict(self, prm_enumerator, var_desc=None):
        temp_enum_dict = self.source_description.get_global_enum_dict()
        
        if prm_enumerator in temp_enum_dict:
            return temp_enum_dict[prm_enumerator]
        else:
            if not var_desc == None:
                var_desc.add_info_string("Enumerator: {enumera} was not found.".format(enumera=prm_enumerator))
            return None
        
    def get_enumerator_by_typedesc(self, prm_typedesc, prm_value):
        if not type(prm_value) == int:
            print("Error: The type for second paramater needs to be int.")
        if not isinstance(prm_typedesc, CEnumTypeDescriptor):
            print("Error: The type for first paramater needs to be CEnumTypeDescriptor or a string for enum type definition.")
        if type(prm_typedesc) == str:
            typedef = self.source_description.global_typedef_container.get_enum_typedef_for_identifer(prm_typedesc)
            typedesc = typedef.declaration_description.type
        elif isinstance(prm_typedesc, CEnumTypeDescriptor):
            typedesc = prm_typedesc
        else:
            print("Error: The type for first paramater needs to be CEnumTypeDescriptor or a string for enum type definition.")
            return
            
        for enum_tuple in typedesc.name_value_tuples:
            if enum_tuple[1] == prm_value:
                EngineLogger.info("A value was found by its enumerator.")
                return enum_tuple[0]
            
        print("Error: There wasn't found any enumerator for {enum_tag} and value {enum_value}".format(enum_tag = typedesc.tag_identifier, enum_value = prm_value))
        return None  
      
    # -------------------------------------------------------------------------------
    # Functions to help out the execution of tests
    # -------------------------------------------------------------------------------
    """
    Set the variables for testcases.
    """
    def _set_testcase_variables(self, test_case):
        
        for variable in test_case.variables_list:
            variable.validation = exec_const.passed_int
            
            in_value = variable.get_in_value_indexed()
            solved_in = self._resolve_value(in_value)
            
            exp_value = variable.get_exp_value_indexed()
            solved_exp = self._resolve_value(exp_value)
            variable.set_solved_in_indexed(solved_in)     
            variable.set_solved_exp_indexed(solved_exp) 
            
            if self.get_scd_variable_descriptor(variable.identifier) == None:
                variable.add_info_string("Variable not found in project-structure.")
                variable.validation = exec_const.exception_int
                return

            if type(solved_in) != int:
                variable.add_info_string("Unresolved value {in_}, its type: {type_}.".format(in_=in_value, type_=type(in_value)))
                variable.validation = exec_const.exception_int
            
            received_value = self.SetVar(variable.identifier, solved_in, var_desc=variable)
            
            if type(received_value) == str:
                received_value = self.get_enumerator_value_from_enum_dict(received_value, var_desc=variable)
            
            if not (type(received_value)==int or type(received_value)==float):
                variable.add_info_string("Could not set value: {val_bt_}.".format(val_bt_ = solved_in))
                variable.validation = exec_const.exception_int
            
            
        
            
    def _read_testcase_variables(self, test_case):
              
        for variable in test_case.variables_list:
           
            out_value = self.GetVar(variable.identifier, var_desc=variable)  
                
            if type(out_value) == str:
                out_value = self.get_enumerator_value_from_enum_dict(out_value, var_desc=variable)
            
            variable.set_out_value_indexed(out_value)
            
            if not (type(out_value)==int or type(out_value)==float):
                variable.add_info_string("Could not receive value.")
                variable.validation = exec_const.exception_int
            else:
                variable.validate()
            
    
    def readoutCOVR(self, function_desc):
        covr_dict = {}
        var_name = function_desc.function_name+"_Line"
        covr_var_desc = self.get_scd_variable_descriptor(var_name)
        if not covr_var_desc == None: 
            type = covr_var_desc.definition_description.resolved_type
            size = type.is_array_of
            
            for i in range(0, size):
                var_index = var_name+"["+str(i)+"]"
                get_covr_value = self.GetVar(var_index)
                if not get_covr_value == None:
                    covr_dict[var_index] =  get_covr_value
                
            return covr_dict
        
        else:
            return None
                
    def resetCOVR(self, function_desc):
        covr_dict = {}
        var_name = function_desc.function_name+"_Line"
        covr_var_desc = self.get_scd_variable_descriptor(var_name)
        if not covr_var_desc == None: 
            type = covr_var_desc.definition_description.resolved_type
            size = type.is_array_of
            
            for i in range(0, size):
                var_index = var_name+"["+str(i)+"]"
                self.SetVar(var_index,  0)
                
    def isExistingCOVR(self, function_desc): 
        var_name = function_desc.function_name+"_Line"
        covr_var_desc = self.get_scd_variable_descriptor(var_name)
        if not covr_var_desc == None:
            return True
        else:
            return False  
    
    def getKPIDict(self):
        self.prj_kpi = self.source_description.getKPIDict() 
        
    def combineKPIDict_gettingMissing(self, dict_all, dict):
        for unit in dict_all:
            if unit in dict:
                for key in dict_all[unit]:
                    if key in dict[unit]:
                        dict_all[unit][key] += dict[unit][key]
                    else:
                        get_covr_value = self.GetVar(key)
                        if not get_covr_value == None:
                            dict_all[unit][key] = get_covr_value  

    def dumpKPIDictToJSON(self, file):
        kpi_ci_logs_path = PathFormer.prj_path+"/out/_ci_logs/"+"KPI_"+file+".json"
        with open(kpi_ci_logs_path, "w") as f:
            json.dump(self.prj_kpi, f)            
     
        
    def _reset_all_wrapper_structs(self):
        self.CallFunc_catched("TM_resetAllWrapperStructs", [])
        
    def _read_out_wrapper_barray(self, prm_no):
        
        if type(prm_no) == int and prm_no < 6:
            prm_str = "prm"+str(prm_no)
        elif prm_no == "RET":
            prm_str = "RET"
        else:
            EngineLogger.error("Invalid parameter. No corresponding wrapper-struct exists for {prm_no}".format(prm_no = prm_no))
            return
        
        value_list = []

        for i in range(0, 200):
            value_list.append("[{index}]: ".format(index=i)+str(self.GetVar("TM_Wrapper_ut_{prm}.TM_Wrapper_res_content.TM_Wrapper_bytearray[{index}]".format(index=i, prm = prm_str))))
        
        print(prm_str)        
        for i in range(0, 40):
            x_list = list()
            for k in range(0, 5):    
                x_list.append(value_list[5*i + k])

            print(x_list)
    # -------------------------------------------------------------------------------
    # Functions for writing the wrapper-argument-structs and reading the wrapper-return-struct
    # -------------------------------------------------------------------------------
    
    def _get_wrapper_base_addresses(self):  
        
        if not self.pteng_is_initialized(prm_print = False):
            EngineLogger.error("The wrapper-struct-base-addresses can not be loaded, because Python-TestEngine is not initialized.")
            return None
        
        for i in range(0, 6):
            
            add_info = self.get_address("TM_Wrapper_ut_prm{i}.TM_Wrapper_res_content".format(i = i))
            
            if add_info:
                buffer_dict = {int(i): add_info[0]}
                self.wrapper_baseaddress_dict.update(buffer_dict)
        
        add_info = self.get_address("TM_Wrapper_ut_RET.TM_Wrapper_res_content")
        
        if add_info:    
            buffer_dict = {"RET": add_info[0]}
            self.wrapper_baseaddress_dict.update(buffer_dict)
        
        #print "Wrapper-struct base-addresses:"
        #print self.wrapper_baseaddress_dict
        
        return True  
    
    def reset_wrapper_structs(self, test_case):
        if not self.CallFunc_catched("TM_resetAllWrapperStructs", []):
            test_case.validation = exec_const.exception_int  
            test_case.add_info_string("Wrapper-structs were not resetted.")        
    """
    This function will prepare the parameter structs for the call by wrapper.
    Awaits any function descriptor, that will be read-out to set the wrapper-structs.
    """
    def _set_wrapper_parameter_struct(self, func_desc, test_case):
        
        func_prm_declarations = func_desc.signature_description.parameter_declarations
        
        for param in test_case.parameter_list:
            current_id = param.identifier
            current_element_list = self.make_element_list(current_id.replace("[", ".["))
            current_root = current_element_list[0]
            current_value = param.get_in_value_indexed()
            
            prm_no = None
            
            for index, prm_decl in enumerate(func_prm_declarations):
                if prm_decl.identifier == current_root:
                    prm_no = index
                    break
                else:
                    prm_no = None
            
            current_typedesc = None
            if prm_no == None:
                test_case.add_info_string("No parameter named: {root_}.".format(root_ = current_root))
                return None
            else:
                #ToDo NiEh: resolve_typeselection with soco
                current_typedesc = self.resolve_type_selection(func_prm_declarations[prm_no].type)  
                
            if isinstance(current_typedesc, CInPlaceTypeDescriptor):
                if current_typedesc.is_pointer_to and current_typedesc.is_array_of == -1:    #If we found a pointer, no arrays can follow. (No)
                    
                    call_indicator_list  = []    
                    basic_typedescriptor = self.source_description._search_original_typedescriptor(current_typedesc.included_type, call_indicator_list, prm_reset_list=1)
                    basic_typedescriptor = self.resolve_type_selection(basic_typedescriptor)
                    
                    d = collections.Counter()         #here a dict is defined that will contain from now on:
                    for c in call_indicator_list:       #the number of array-dimensions d[1] or the grade of pointers d[0]
                        d[c] += 1  
                    
                    #increment the pointer count, because we know already, that we resolved a pointer.
                    d[0] = d[0] +1 
                    
                    resolved_value = self._resolve_value_for_typedesc(current_value, current_typedesc)
                    
                    if resolved_value == None:
                        test_case.add_info_string("Value was not resolved: {current_value_}".format(current_value_ = current_value))
                        return None
                        
                    
                    if isinstance(basic_typedescriptor, CFunctionSignatureTypeDescriptor):
                        #write the functionpointer
                        prm_struct_pointer = "TM_Wrapper_ut_prm{no}.TM_Wrapper_p".format(no = prm_no)
                        self.SetVar(prm_struct_pointer, resolved_value)
                    else:
                        #Write the pointer
                        prm_struct_pointer = "TM_Wrapper_ut_prm{no}.TM_Wrapper_{p}".format(no = prm_no, p = "p"*d[0])
                        self.SetVar(prm_struct_pointer, resolved_value)
                    
                    param.solved_in = resolved_value
                        
                    continue
                    
            wrapper_add = self.wrapper_baseaddress_dict[prm_no]
            add_info = self.get_addressinfo_from_element_list(current_element_list, current_typedesc, wrapper_add, None)
            
            if add_info:
                address, offset, found_type, last_pointer = add_info[0], add_info[1], add_info[2], add_info[3]
            else:
                test_case.add_info_string("No Adress-Info received: {current_value_}".format(current_value_ = current_id))
                return None
            
            arg_gtype = self.get_gtype_from_typedescriptor(found_type, prm_fromwrapper2=True)
            
            if not arg_gtype:
                test_case.add_info_string("No basic type for parameter identifier {ret_id_}".format(ret_id_ = str(param_index_id))) 
                return None
            
            resolved_value = self._resolve_value_for_typedesc(current_value, found_type, prm_cast=True, prm_eval_or_exp=False, prm_cnv_enum=False)
            
            if not type(resolved_value) == int:
                test_case.add_info_string("Value was not resolved: {current_value_}".format(current_value_ = current_value))
                return None
            
            written_value = self._write_basic_type_at_address_64bit(resolved_value, address+offset, arg_gtype, found_type.tms_signature_code_2008)
            
            if written_value == None:
                test_case.add_info_string("No value written for: {current_value_}".format(current_value_ = resolved_value)) 
                return None
            
            self.debug_print("PRM:"+str([param.get_in_value_indexed(), resolved_value]))
            param.set_solved_in_indexed(resolved_value)
        
        
        return True     

            
    """
    This function will read out the return struct specified for the called function.
    """  
    def _get_wrapper_return_struct(self, func_desc, test_case):
        
        if func_desc.signature_description.return_declaration.resolved_type and not isinstance(func_desc.signature_description.return_declaration.type,  CInPlaceTypeDescriptor):  #only take the resolved type if its no InPlace
            func_return_type = func_desc.signature_description.return_declaration.resolved_type
        else:
            func_return_type = func_desc.signature_description.return_declaration.type  
        
        if func_return_type == None:
            test_case.add_info_string("Unknown return type of function")
            return None
        
        func_return_type = self.resolve_type_selection(func_return_type) 
        
        return_value_list = []
        
        if isinstance(func_return_type, CTypeSelectionDescriptor):
            if func_return_type.sized_C99_type == "void":
                return return_value_list

        for ret in test_case.return_list:
            exp_value = ret.get_exp_value_indexed()
            #print(exp_value)
            solved_exp = self._resolve_value(exp_value)
            #print(solved_exp)
            if solved_exp == None:
                test_case.add_info_string("Could not resolve return value {value_}, type {type_}.".format(value_=exp_value, type_=type(exp_value))) 
            ret.set_solved_exp_indexed(solved_exp)
        
        for ret in test_case.return_list:
            
            current_id = ret.identifier
            current_element_list = self.make_element_list(current_id.replace("[", ".["))
            current_typedesc = func_return_type
            resolved_returned_value = None
                       
            if isinstance(current_typedesc, CInPlaceTypeDescriptor):
                if current_typedesc.is_pointer_to and current_typedesc.is_array_of == -1:    #If we found a pointer, no arrays can follow.
                    
                    call_indicator_list  = []    
                    basic_typedescriptor = self.source_description._search_original_typedescriptor(current_typedesc.included_type, call_indicator_list, prm_reset_list=1)
                    basic_typedescriptor = self.resolve_type_selection(basic_typedescriptor)
                    
                    d = collections.Counter()         #here a dict is defined that will contain from now on:
                    for c in call_indicator_list:       #the number of array-dimensions d[1] or the grade of pointers d[0]
                        d[c] += 1  
                    
                    #increment the pointer count, because we know already, that we resolved a pointer.
                    d[0] = d[0] +1 
                    
                    if isinstance(basic_typedescriptor, CFunctionSignatureTypeDescriptor):
                        #write the functionpointer
                        prm_struct_pointer = "TM_Wrapper_ut_RET.TM_Wrapper_p"
                        returned_value = self.GetVar(prm_struct_pointer, origin_typedesc=current_typedesc)
                    else:
                        #Write the pointer
                        prm_struct_pointer = "TM_Wrapper_ut_RET.TM_Wrapper_{p}".format(p = "p"*d[0])
                        returned_value = self.GetVar(prm_struct_pointer, origin_typedesc=current_typedesc)
                    
                    resolved_returned_value = self._resolve_value_for_typedesc(returned_value, current_typedesc, prm_eval_or_exp = True)
                    ret.set_out_value_indexed(resolved_returned_value)
                    return_value_list.append(resolved_returned_value)
                    continue
                    
            wrapper_add = self.wrapper_baseaddress_dict["RET"]
            
            add_info = self.get_addressinfo_from_element_list(current_element_list, current_typedesc, wrapper_add, None)
            
            if add_info:
                address, offset, found_type, last_pointer = add_info[0], add_info[1], add_info[2], add_info[3]
            else:
                test_case.add_info_string("No Adress-Info received: {ret_id_}".format(ret_id_ = current_id))
                return None
            
            arg_gtype = self.get_gtype_from_typedescriptor(found_type, prm_fromwrapper2 = True)
            
            if not arg_gtype:
                test_case.add_info_string("No basic type for {ret_id_}".format(ret_id_ = current_id))
                return None

            returned_value = self._read_basic_type_from_address_64bit(address+offset, arg_gtype, found_type.tms_signature_code_2008)
            
            if returned_value == None:
                test_case.add_info_string("No returned value for {ret_id_}".format(ret_id_ = current_id))
                return None
            
            #will resolve a returned value, especially floats, and expanded pointer
            resolved_returned_value = self._resolve_value_for_typedesc(returned_value, found_type, prm_eval_or_exp = True)
            ret.set_out_value_indexed(resolved_returned_value)
            return_value_list.append(resolved_returned_value)
        
        self.debug_print("RET:"+str(return_value_list))
        return return_value_list  



#irgendwann nochmal auslagern
def combineCOVRDict(dict, dict_low):
    for key in dict_low:
        if key in dict:
            dict[key] += dict_low[key]
        else:
            dict[key] = dict_low[key]
    

def combineKPIDict(dict, dict_low):
    for unit in dict_low:
        if unit in dict:
            for key in dict_low[unit]:
                if key in dict[unit]:
                    dict[unit][key] += dict_low[unit][key]
                else:
                    dict[unit][key] = dict_low[unit][key]
        else:
            dict[unit] = dict_low[unit]
    
def getCOVRPercentageOfKPIDict(kpi_dict):
    size = 0
    counter = 0
    for unit in kpi_dict:
    
        size += len(kpi_dict[unit])
        
        for key in kpi_dict[unit]:
            if kpi_dict[unit][key] > 0:
                counter += 1
        
    if (size == 0):
        percentage = 0
    else:
        percentage = (counter/size) * 100
          
    return "{:.2f}%".format(percentage)  
    