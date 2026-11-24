"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : SourceCodeDescriptor.py
Date of creation : 7.8.2018
Author Name/Dept : Ulrike Brueggemann, Nikolai Ehrhardt
Organization     : 
Description      : Loads every smybol of the sourcecode from Axivion-Tree or the PSYD and PSTD file.
"""

import os
import re 
import collections
import time
import sys

import xml.dom.minidom as dom

import LogSupport

from pteng.globals.folder_utils import split_filepath
from pteng.soco.TypeSystemDescriptor import TypeSystemDescriptor

class Conformizer():
    
    major_version = sys.version_info[0]
    
    def __init__(self):
        self.version_info = sys.version_info
        
    @classmethod
    def next_token(cls, token):
        if cls.major_version == 2:
            return token.next()
        else:
            return next(token)  

               
SoCoLogClass = LogSupport.LoggerClass("global.soco")      
SoCoLogger = SoCoLogClass.Logger    

NO_OF_INDENTTATION_SPACES = 4
INDENTTATION = NO_OF_INDENTTATION_SPACES * " "  

HEADER_EXTENSION = '.h'
CFILE_EXTENSION = '.c'

RE_ENDING_NUMBER = re.compile('(.*?)(\d+)')


#todo if next ptest veriosn is available: remove if WITH_WORK_PROGESS_PSTD_XSDx if expressions and its else branch

WITH_WORK_PROGESS_PSTD = False

READ_WRITE_TMSSC = True
#READ_WRITE_TMSSC = False


WRITE_AX_INFO = False

CHECK_AXIVION_AGAINST_TYPE_SYSTEM = True

CHECK_AGAINST_SYMBOL_FILE = True

PRE_SIZE_BUILD = False

BUILD_CALL_DICT = False

_NORMALIZED_C_TYPE_TO_TMS_SIGNATURE_2008 = {
    'void':                 'V',
    'char':                 'SC1',
    'signed char':          'SC1',
    'unsigned char':        'UC1',
    'signed short':         'S1',
    'unsigned short':       'US1',
    'signed int':           'I1',
    'unsigned int':         'UI1',
    'signed long':          'L1',
    'unsigned long':        'UL1',
    'signed long long':     'LL1',
    'unsigned long long':   'ULL1',
    'bool':                 'SC1',
    'float':                'xxx1',
    'double':               'xxx2'
    }
    
_SIZED_C99_TYPE_TO_TMS_SIGNATURE_2016 = {
    'void':         'V',
    'int8_t':       'I8',
    'uint8_t':      'UI8',
    'int16_t':      'I16',
    'uint16_t':     'UI16',
    'int32_t':      'I32',
    'uint32_t':     'UI32',
    'int64_t':      'I64',
    'uint64_t':     'UI64',
    'int128_t':     'I128',
    'uint128_t':    'UI128',
    'bool':         'B',
    'float16_t':    'F16',
    'float32_t':    'F32',
    'float64_t':    'F64',
    'float128_t':   'F128'
    }

def search_return_typedesc_in_dict(prm_dict, typedesc, key):
    if not (key in prm_dict):
        prm_dict[key] = typedesc
        return typedesc
    else:
        return prm_dict[key]

# to do general: enum size and signdness: why char? other plattforms? on stm32f0 UC1, on intel UL1 but there are negative enum values?
# to do general: longlong
# to do general: C00 bool type
# to do general: float and double



        
class DomHelper:
    # @classmethod 
    #def GetMandatoryAttribute(cls, node, attribute_name):
       #return node.getAttribute('attribute_name')
                
    @classmethod 
    def GetOptionalAttribute(cls, node, attribute_name, return_for_not_existing=''):
        attribute_instance = node.getAttributeNode(attribute_name)
        if attribute_instance:
            return attribute_instance.value
        else:
            return return_for_not_existing
        
    @classmethod 
    def GetFirstElementChild(cls, node):
        element_child = node.firstChild
        while element_child.__class__ !=  dom.Element:
            element_child = element_child.nextSibling
        return element_child

class SymbolLocationType:
    
    def __init__(self):
        self.size_in_bytes = 0      # size in number of bytes
        self.location_str = '-'     # address resp. offset as hex value string
        self.location_int = 0       # address resp. offset as int value; contains offset, that considers the alignment and is no difference
        
    def readout(self):
        print(self.size_in_bytes)
        print(self.location_str)
        print(self.location_int)        
        
    @classmethod                 
    def CreateFrom_PSYD(cls, xml_dom_type_node):
        helper = cls()
        helper.SetFrom_PSYD(xml_dom_type_node)
        return helper   
        
    def SetFrom_PSYD(self, xml_dom_type_node): 
        self.size_in_bytes =  int(xml_dom_type_node.getAttribute('size'))
        self.location_str = xml_dom_type_node.getAttribute('location') 
        self.location_int = int(self.location_str,16)
    
        
class MemberSymbolLocationType(SymbolLocationType):
    
    def __init__(self):
        SymbolLocationType.__init__(self)
        self.bitmask = None
        
    @classmethod                 
    def CreateFrom_PSYD(cls, xml_dom_type_node):
        helper = cls()
        helper.SetFrom_PSYD(xml_dom_type_node)
        return helper   
        
    def SetFrom_PSYD(self, xml_dom_type_node): 
        SymbolLocationType.SetFrom_PSYD(self, xml_dom_type_node)
        self.bitmask = DomHelper.GetOptionalAttribute(xml_dom_type_node, 'bitmask', None)
        if self.bitmask:
            self.bitmask = int(self.bitmask)
            


class CTypeDescriptor:
    """  base class for serveral derived classes that describe types in the programming language C; 
    they are used in: variable, member, return type or parameter type declarations; usually the compiler will accept no unnamed structs, unions and ennums for return and parameter types """
    
    TYPEDEF_TYPES = ['typedef_type', 'struct_type', 'union_type','enum_type']
    #UNNAMED_TYPE_TAGS = ['basic_type'] + TYPEDEF_TYPES +  ['pointer_to', 'function_pointer', 'array_of']
    TYPE_SELECTION_TAGS =  ['basic_type'] + TYPEDEF_TYPES 
    INPLACE_TYPE_COMPOSITION_TAGS =  ['pointer_to', 'function_pointer', 'array_of']
    
           
    def __init__(self):
        self.size_in_bytes = 0          # 0: no value detmerined yet, -1: not applicable (e.g. void, a function etc,); > 0:nomen est omen; CFunctionsSignarueCType: can only be read from symbol file i.e. 0 if only pstd file read
        self.tms_signature_code_2008 = ''    # the so called signature code for working with the TCL  TestEngine
        self.tms_signature_code_2016 = ''    # the so called signature code for working with the Python TestEngine
        self.not_readable = False       # not (completely) readable from infoemation; used as primitvie form of error handling    
    
    @classmethod
    def GetFrom_AxivionType_Identifier(self, AxivionType):
        if AxivionType.is_of_type('Qualified_Type_Definition'):    
            return None    
              
        elif  AxivionType.is_of_type('Routine_Definition') or AxivionType.is_of_type('Routine_Type_Definition') or AxivionType.is_of_type('Function_Declaration'): 
            return None
        
        #third or AxivionType.is_of_type('Unscoped_Unbased_Enum_Type_Declaration') for Axivion 6_7_1
        elif AxivionType.is_of_type('Enumeration_Type_Definition') or AxivionType.is_of_type('Unnamed_Enumeration_Type_Definition') or AxivionType.is_of_type('Unscoped_Unbased_Enum_Type_Declaration'):
            
            return CEnumTypeDescriptor.GetFrom_Axivion_Identifier(AxivionType)

        elif AxivionType.is_of_type('Composite_Type_Definition') or AxivionType.is_of_type('Unnamed_Composite_Type_Definition'):
            return CCompositeTypeDescriptor.GetFrom_Axivion_Identifier(AxivionType)
       
        elif AxivionType.is_of_type('Unnamed_Type_Definition'):
            # after Routine_Type_Definition because otherwise Unnamed_Type_Definition would catch Routine_Type_Definition and Unnamed_Enumeration_Type_Definition or Unnamed_Composite_Type_Definition
            #return CUnnamedTypeDescriptor.CreateFrom_AxivionType(type_system, AxivionType, identifier) 
            if AxivionType.is_of_type('In_Place_Type_Composition'):
                return None
                
            elif AxivionType.is_of_type('Type_Selection'):
                return CTypeSelectionDescriptor.GetFrom_Axivion_Identifier(AxivionType)
  
            else:
                SoCoLogger.warning('Warning CTypeDescriptor.CreateFrom_AxivionType: Uncovered type inherited from Unnamed_Type_Definition')
     
        else:
            SoCoLogger.critical ("Error: TypeDescriptor.CreateFrom_AxivionType: fatal error - Axivion-Type is unknown " +str( AxivionType.type()) +" " + str(AxivionType.number()))
        
    @classmethod                 
    def CreateFrom_AxivionType(cls, prm_soco, AxivionType, identifier):  #identifier only there to write logs
        if AxivionType.is_of_type('Qualified_Type_Definition'):    
            helper = CTypeDescriptor.Try_CreateTypedefReference(AxivionType.Unqualified_Type)
            if not helper:        
                helper = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionType.Unqualified_Type, identifier)
            return helper     
              
        elif  AxivionType.is_of_type('Routine_Definition') or AxivionType.is_of_type('Routine_Type_Definition') or AxivionType.is_of_type('Function_Declaration'): 
            return CFunctionSignatureTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionType, identifier)
        
        elif AxivionType.is_of_type('Enumeration_Type_Definition') or AxivionType.is_of_type('Unnamed_Enumeration_Type_Definition'):
            
            type_identifier = CEnumTypeDescriptor.GetFrom_AxivionType_Identifier(AxivionType)
            if type_identifier and (type_identifier in prm_soco.Dict_Enumeration_Typedesc):
                return prm_soco.Dict_Enumeration_Typedesc[type_identifier]
            else:
                prm_soco.Dict_Enumeration_Typedesc[type_identifier] = CEnumTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionType, identifier)
                return prm_soco.Dict_Enumeration_Typedesc[type_identifier]
        
        elif AxivionType.is_of_type('Composite_Type_Definition') or AxivionType.is_of_type('Unnamed_Composite_Type_Definition'):
            type_identifier = CCompositeTypeDescriptor.GetFrom_AxivionType_Identifier(AxivionType)
            if type_identifier and (type_identifier in prm_soco.Dict_Composite_Typedesc):
                return prm_soco.Dict_Composite_Typedesc[type_identifier]
            else:
                prm_soco.Dict_Composite_Typedesc[type_identifier] = CCompositeTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionType, identifier)
                return prm_soco.Dict_Composite_Typedesc[type_identifier]
            
        elif AxivionType.is_of_type('Unnamed_Type_Definition'):
             # after Routine_Type_Definition because otherwise Unnamed_Type_Definition would catch Routine_Type_Definition and Unnamed_Enumeration_Type_Definition or Unnamed_Composite_Type_Definition
            #return CUnnamedTypeDescriptor.CreateFrom_AxivionType(type_system, AxivionType, identifier) 
            if AxivionType.is_of_type('In_Place_Type_Composition'):
                return CInPlaceTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionType, identifier) 
                
            elif AxivionType.is_of_type('Type_Selection'):
                
                type_identifier = CTypeSelectionDescriptor.GetFrom_AxivionType_Identifier(AxivionType)
                if type_identifier and (type_identifier in prm_soco.Dict_Selection_Typedesc):
                    return prm_soco.Dict_Selection_Typedesc[type_identifier]
                else:
                    prm_soco.Dict_Selection_Typedesc[type_identifier] = CTypeSelectionDescriptor.CreateFrom_AxivionType(prm_soco, AxivionType, identifier)
                    return prm_soco.Dict_Selection_Typedesc[type_identifier]
                
            else:
                SoCoLogger.warning('Warning CTypeDescriptor.CreateFrom_AxivionType: Uncovered type inherited from Unnamed_Type_Definition')
     
        else:
            SoCoLogger.critical ("Error: TypeDescriptor.CreateFrom_AxivionType: fatal error - not CType Creator found for " +str( AxivionType.type()) +" " + str(AxivionType.number()))   
    
    @classmethod
    def GetFrom_PSTD_Identifier(cls, xml_dom_type_node):
        #if xml_dom_type_node.nodeName == 'function_pointer':
            #return CFunctionSignatureTypeDescriptor.CreateFrom_PSTD(xml_dom_type_node)
 
        if xml_dom_type_node.nodeName in cls.TYPE_SELECTION_TAGS:
            
            return CTypeSelectionDescriptor.GetFrom_PSTD_Identifier(xml_dom_type_node)
            
        elif xml_dom_type_node.nodeName in cls.INPLACE_TYPE_COMPOSITION_TAGS:
            SoCoLogger.Error("While setting from PSTD a None-typedescriptor was returned.")
            return None          
        elif xml_dom_type_node.nodeName == 'enumeration':
            
            return CEnumTypeDescriptor.GetFrom_PSTD_Identifier(xml_dom_type_node)
            
        elif xml_dom_type_node.nodeName == 'structure' or xml_dom_type_node.nodeName == 'union':
            
            return CCompositeTypeDescriptor.GetFrom_PSTD_Identifier(xml_dom_type_node)

        else:
           SoCoLogger.warning('Warning CTypeDescriptor.CreateFrom_PSTD: Unexpected type element: ' + str(xml_dom_type_node.nodeName))
    
    @classmethod  
    def CreateFrom_PSTD(cls, prm_soco, xml_dom_type_node):
        #if xml_dom_type_node.nodeName == 'function_pointer':
            #return CFunctionSignatureTypeDescriptor.CreateFrom_PSTD(xml_dom_type_node)
 
        if xml_dom_type_node.nodeName in cls.TYPE_SELECTION_TAGS:
            
            type_identifier = CTypeSelectionDescriptor.GetFrom_PSTD_Identifier(xml_dom_type_node)
            if type_identifier and (type_identifier in prm_soco.Dict_Selection_Typedesc):
                return prm_soco.Dict_Selection_Typedesc[type_identifier]
            else:
                prm_soco.Dict_Selection_Typedesc[type_identifier] = CTypeSelectionDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_type_node)
                return prm_soco.Dict_Selection_Typedesc[type_identifier]
            
        elif xml_dom_type_node.nodeName in cls.INPLACE_TYPE_COMPOSITION_TAGS:
            return CInPlaceTypeDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_type_node)          
        elif xml_dom_type_node.nodeName == 'enumeration':
            
            type_identifier = CEnumTypeDescriptor.GetFrom_PSTD_Identifier(xml_dom_type_node)
            if type_identifier and (type_identifier in prm_soco.Dict_Enumeration_Typedesc):
                return prm_soco.Dict_Enumeration_Typedesc[type_identifier]
            else:
                prm_soco.Dict_Enumeration_Typedesc[type_identifier] = CEnumTypeDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_type_node)
                return prm_soco.Dict_Enumeration_Typedesc[type_identifier]
            
        elif xml_dom_type_node.nodeName == 'structure' or xml_dom_type_node.nodeName == 'union':
            
            type_identifier = CCompositeTypeDescriptor.GetFrom_PSTD_Identifier(xml_dom_type_node)
            
            
            if type_identifier and (type_identifier in prm_soco.Dict_Composite_Typedesc):
                return prm_soco.Dict_Composite_Typedesc[type_identifier]
            else:
                prm_soco.Dict_Composite_Typedesc[type_identifier] = CCompositeTypeDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_type_node)
                return prm_soco.Dict_Composite_Typedesc[type_identifier]
            
        elif xml_dom_type_node.nodeName == 'function_type':  
            return CFunctionSignatureTypeDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_type_node)  
        
        else:
            SoCoLogger.error('Error CTypeDescriptor.CreateFrom_PSTD: Unexpected type element: ' + str(xml_dom_type_node.nodeName))
           
        
    @classmethod 
    #still needed
    def Try_CreateTypedefReference(cls,AxivionType):
          
        ax_info =  AxivionType.type() + ' ' + str(AxivionType.number())   
        if AxivionType.is_of_type('Named_Type_Definition'):
            if AxivionType.is_of_type('Composite_Type_Definition'):
                if AxivionType.is_of_type('Normal_Struct_Type_Definition'):
                    return CTypeSelectionDescriptor.CreateFrom_ValuesForTypedefReference('struct_type', AxivionType.Logical.Name, ax_info)
                elif AxivionType.is_of_type('Union_Type_Definition'):
                    return CTypeSelectionDescriptor.CreateFrom_ValuesForTypedefReference('union_type', AxivionType.Logical.Name, ax_info)
            elif AxivionType.is_of_type('Enumeration_Type_Definition'):
                return CTypeSelectionDescriptor.CreateFrom_ValuesForTypedefReference('enum_type', AxivionType.Logical.Name, ax_info)
            else:
                SoCoLogger.warning('Warning CTypeDescriptor.Try_CreateTypedefReference: Unexpected Named_Type_Definition ' + str(AxivionType(type)) +' for node ' + str(AxivionType.number()))   
                return None  
            
        elif AxivionType.is_of_type('Type_Declaration'):    
            if AxivionType.is_of_type('Struct_Type_Declaration'):
                return CTypeSelectionDescriptor.CreateFrom_ValuesForTypedefReference('struct_type', AxivionType.Logical.Name, ax_info)
            elif AxivionType.is_of_type('Union_Type_Declaration'):
                return CTypeSelectionDescriptor.CreateFrom_ValuesForTypedefReference('union_type', AxivionType.Logical.Name, ax_info)
            #changed Enum_Type_Declaration to Enumeration_Type_Declaration
            elif AxivionType.is_of_type('Enumeration_Type_Declaration'):
                return CTypeSelectionDescriptor.CreateFrom_ValuesForTypedefReference('enum_type', AxivionType.Logical.Name, ax_info)
            else:
                SoCoLogger.warning('Warning CTypeDescriptor.Try_CreateTypedefReference: Type_Declaration ' +  str(AxivionType(type)) + ' for node ' + str( AxivionType.number()))   
                return None
            
        else:
            return None
        
        
    def is_typedef(self):
        if self._class_ == CEnumTypeDescriptor or self._class_ == CCompositeTypeDescriptor:  
            if self.tag_identifier:
                return True
            else:
                return False            
        else:
            return False
  
        
    def is_non_basic_type_selection(self): # wird ein nicht basaler Datentyp 'selected' d.h. entweder ein typedef, ein struct, ein enum oder ein enumed
        if self.__class__ == CTypeSelectionDescriptor:
            if self.type_definition_kind:               
                return True
            else:
                return False                      
        else:
            return False
        
    def is_typedef_type_selection(self):
        if self.__class__ == CTypeSelectionDescriptor:
            if self.type_definition_kind:               
                if self.type_definition_kind == 'typedef_type':
                    return True
                else:
                    return False
            else:
                return False                      
        else:
            return False
            
    def getCompositeMembers(self):
        return list()           
              
    def isArrayType(self):
        return -2
        
    def getResolvedType(self):
        return self
        

class CTypeSelectionDescriptor(CTypeDescriptor):
    """ describes the selection of an existing type in C programming language
    these types are either basic types (in C programming language = build in type) or named types introduced by a type definiton (by typedef, struct, union or enum)
    (see class SourceCodeDescriptor.TypeDefinitionDescriptor that references and  derived from class CType CDescriptor)""" 
     
              
          
    def __init__(self):
        
        CTypeDescriptor.__init__(self)
        
        self.ax_info = ''               # debugging information for reading from Axivion 
                                        
        self.original_type = ''         # type name used in source code; needed for stub generation; either sized_C99_type or type_definition_kind are set additionally; if sized_C99_type is not set then this is a tpyedef identifier resp. its tag      
                                        
        self.sized_C99_type = ''        # only set if it is an basic C type (incl. void and bool; incl. BSH or C99 typedefs for basic tpyes); used to build gtype (see comment below)
        self.normalized_C_type = ''     # only set if sized_C99_type is set; currently (2016-07-15 UlBr) used to build the tms tms_signature - may become obsolete;  will be build in the future from sized_C99_type; set by build_size if it is an BSH or C99 typedef for basic C type 
       
        self.type_definition_kind = ''  # only set if original_type is typedef identifier resp. its tag exclusive BSH or C99 typedefs for basic types; specifies the kind of typedef: typedef_type struct_type or enum_type (self.original_type is its identifier resp. its tag)
        self.resolved_type = None       # only set if original_type is typedef identifier resp. its tag
        
        self.gtype = ''                 # generic type; is used to build tms_signature; set by build_size
        
    def getResolvedType(self):
        if self.resolved_type == None:
            return self
        else:
            return self.resolved_type.getResolvedType()
    
    def readout(self):
        
        print("-->CTypeSelection")
        
        #print " ->modifier: " + str(self.is_const ) + " " + str(self.is_volatile)
        print(" ->ax-info: " + str(self.ax_info))
        print(" ->not readable: " + str(self.not_readable))
        print(" ->size in bytes: " + str(self.size_in_bytes))          # 0: no value detmerined yet, -1: not applicable (e.g. void, a function etc,); > 0:nomen est omen; CFunctionsSignarueCType: can only be read from symbol file i.e. 0 if only pstd file read
        print(" ->signature_2008: " + str(self.tms_signature_code_2008))     # the so called signature code for working with the TCL  TestEngine
        print(" ->signature_2016: " + str (self.tms_signature_code_2016))    # the so called signature code for working with the Python TestEngine

        print(" ->original_type: " + str(self.original_type))
        print(" ->sized_C99_type: " + str(self.sized_C99_type))
        print(" ->normalized_C_type: " + str(self.normalized_C_type))     
        print(" ->type_definition_kind: " + str(self.type_definition_kind ))
        print(" ->resolved_type: " + str(self.resolved_type ))
        if self.resolved_type:
            self.resolved_type.readout()
        print(" ->gtype: " + str(self.gtype ))
        
        
    def is_basic_type(self):
        if  self.type_definition_kind:
            return  False
        else:
            return True
          
    
    @classmethod                 
    def CreateFrom_ValuesForTypedefReference(cls, kind, orig_type_identifer, ax_info):
        helper = cls()
        helper.SetFrom_ValuesForTypedefReference(kind, orig_type_identifer,ax_info)
        return helper
                
    def SetFrom_ValuesForTypedefReference(self, kind, orig_type_identifer, ax_info): 
        self.ax_info = ax_info
        self.type_definition_kind = kind
        self.original_type = orig_type_identifer
        self.gtype = self.original_type
    
    @classmethod                 
    def CreateFrom_AxivionType(cls, prm_soco, AxivionType, identifier):
        helper = cls()
        helper.SetFrom_AxivionType(prm_soco, AxivionType, identifier)
        return helper
                
    def SetFrom_AxivionType(self, prm_soco, AxivionType, identifier):    
            
        self.ax_info = AxivionType.type() + ' ' + str(AxivionType.number())  
        
        if AxivionType.is_of_type('Error_Type_Selection'):
            SoCoLogger.error ('Error CTypeSelectionDescriptor.SetFrom_AxivionType: Corrupt TypeSelection for ' + str(identifier) + ' ' + str(AxivionType.number()) )
            self.not_readable = True
            self.size_in_bytes = -1
                       
        elif AxivionType.is_of_type('Qualified_Type_Selection'):
                
            if AxivionType.is_of_type('Qualified_Direct_Type_Selection'):
                """
                self.is_const = AxivionType.Qualifiers.is_const
                self.is_volatile = AxivionType.Qualifiers.is_volatile
                """
                # is_Restrict; is_UPtr? is_SPtr?
                self.SetFrom_AxivionType(prm_soco, AxivionType.Unqualified_Type, identifier)
                SoCoLogger.info('Info CTypeSelectionDescriptor.SetFrom_AxivionType: Qualified Direct-TypeSelection for: ' + str(identifier))
                
            else: 
                SoCoLogger.warning ("Warning CTypeSelectionDescriptor.SetFrom_AxivionType: Uncovered type inherited from Qualified_Type_Selection for " + str(AxivionType.type()) + " for " + str(identifier))  
                self.not_readable = True
                self.size_in_bytes = -1
                    
        elif AxivionType.is_of_type('Unqualified_Type_Selection'):
            
            if AxivionType.is_of_type('Other_Builtin_Type_Selection'): 
                pass
            elif AxivionType.is_of_type('Builtin_Type_Selection'):     
                self.normalized_C_type = self.AxivionLogicalTypeName_to_Normalized_CType[AxivionType.Logical.Name]  #dictionary   
                if self.normalized_C_type == '':
                    SoCoLogger.warning('Info CTypeSelectionDescriptor.SetFrom_AxivionType: Empty normalized_C_type for ID ' + str(identifier))        
                
                if AxivionType.is_of_type('Integer_Type_Selection'):                   
                    help = ''
                    token = AxivionType.SLoc.Token 
                    
                    #step over qualifiers
                    while token.__str__() == 'extern' or token.__str__() == 'static' or token.__str__() == 'const' or token.__str__() == 'volatile':
                        token = Conformizer.next_token(token)                             
                    while token.__str__() == 'short' or token.__str__() == 'long' or token.__str__() == 'signed' or token.__str__() == 'unsigned':
                        if help:
                            help += ' '
                        help += token.__str__()
                        token = Conformizer.next_token(token)   
                        
                    if  token.__str__() == 'int' or token.__str__() == 'char':
                        if help:
                            help += ' '
                        help += token.__str__()   
                   
                    if help:
                        self.original_type = help
                    else:
                       self.original_type = AxivionType.Logical.Name
                     
                    ax_size = AxivionType.Logical.Size        
                    
                
                elif AxivionType.is_of_type('Floating_Point_Type_Selection'):                   
                    help = ''
                    token = AxivionType.SLoc.Token
                    while token.__str__() == 'extern' or token.__str__() == 'static' or token.__str__() == 'const' or token.__str__() == 'volatile':
                        token = Conformizer.next_token(token)                             
                    while token.__str__() == 'long':
                        if help:
                            help += ' '
                        help += token.__str__()
                        token = Conformizer.next_token(token)
                    if  token.__str__() == 'float' or token.__str__() == 'double':
                        if help:
                            help += ' '
                        help += token.__str__()   
                   
                    if help:
                        self.original_type = help
                    else:
                       self.original_type = AxivionType.Logical.Name
                     
                    ax_size = AxivionType.Logical.Size                                         

                                      
                                            
                elif AxivionType.is_of_type('Void_Type_Selection'):
                    # i.e. self.normalized_C_type == 'void'
                    self.original_type = 'void'
                    ax_size = -1
                                       
                
                elif AxivionType.is_of_type('Boolean_Type_Selection'):
                     # i.e. self.normalized_C_type == 'bool'
                    self.original_type = 'bool'
                    ax_size = AxivionType.Logical.Size
                                                       
                else:
                   SoCoLogger.error('Error CTypeSelectionDescriptor.SetFrom_AxivionType: Uncovered Type inherited from Builtin_Type_Selection')
                                               
            
            elif AxivionType.is_of_type('Unqualified_Direct_Type_Selection'):
                
                self.original_type =  AxivionType.Logical.Name              
                
                # 2016-08-06 UlBr: please preserve comment: it revealed that debugging here is repeatedly necessary 
#                 if self.original_type == 'T_example_struct':                    
#                     token = AxivionType.SLoc.Token
#                     token_str = token.__str__()
#                     token = token.next()
#                     token_str += ' '
#                     token_str += token.__str__()
#                     token = token.next()
#                     token_str += ' '
#                     token_str += token.__str__()  
#                     token = token.next()
#                     token_str += ' '
#                     token_str += token.__str__() 
#                     print('hier TETEST_example_taged_struct_t - token: ', token_str)
                
                if self.original_type in self.C99_Sized_Types:
                    ax_size = AxivionType.Logical.Size
                                                         
                elif self.original_type in self.C_BSH_To_C99Sized_Types:
                    ax_size = AxivionType.Logical.Size
                
                #ToDo NiEh: token_str is defect, for some modifiers, should normally be able to be extracted from  "AxivionType"                         
                else:
                    token = AxivionType.SLoc.Token
                    token_str = token.__str__()
                    
                    """
                    #self.gtype = self.original_type
                    # could be enum or struct or untion tag

                    # if you have in the C source code "typedef(struct|union|enum) <tag> alias_name;" then the token starts with typedef
                    # if you have in the C source code "(static|const|volatile)+ (struct|union|enum) <tag> <variable_id>;" then the token starts with the modifiers
                    if token_str == 'typedef': 
                        token = token.next()
                        token_str = token.__str__()
                    while  token_str == 'static' or token_str == 'const' or token_str == 'volatile':
                        token = token.next()
                        token_str = token.__str__()
                    if token_str == 'struct' or token_str == 'union' or token_str == 'enum':  # if you have in the source "typedef struct/enum/union tag {...} alias_name;" then the token starts with struct/enum/union                          
                        self.type_definition_kind = token_str
                        self.type_definition_kind += '_type'
                    else:
                        self.type_definition_kind = 'typedef_type' 
                    
                    ToDo NiEh:
                    It can happen, that AxivionType.Symbol_Declaration.type() is None
                    Line 86 mal_context.h
                    Also: forward declaration and definition in more than one file
                    """

                    if AxivionType.Symbol_Declaration.type() in ["Normal_Struct_Type_Definition", "Struct_Type_Declaration"]:
                       self.type_definition_kind = "struct_type"
                    elif AxivionType.Symbol_Declaration.type() in ["Union_Type_Definition", "Union_Type_Declaration", "Unnamed_Union_Type_Definition"]:
                        self.type_definition_kind = "union_type"
                    elif AxivionType.Symbol_Declaration.type() in ["Enumeration_Type_Definition", "Unnamed_Enumeration_Type_Definition", "Ordinary_Unscoped_Unbased_Enum_Type_Definition"]:
                        self.type_definition_kind = "enum_type"
                    elif AxivionType.Symbol_Declaration.type() == None:

                        if AxivionType.Logical:
                            if AxivionType.Logical.Physical:
                                if AxivionType.Logical.Physical.is_of_type('Normal_Struct_Type_Definition'):
                                    self.type_definition_kind = "struct_type"
                                elif AxivionType.Logical.Physical.is_of_type('Union_Type_Definition'):
                                    self.type_definition_kind = "union_type"
                                elif AxivionType.Logical.Physical.is_of_type('Enumeration_Type_Definition'):
                                    self.type_definition_kind = "enum_type"
                                
                        else:
                            SoCoLogger.warning("CTypeSelectionDescriptor.SetFrom_AxivionType: There wasn't found any type_definition_kind for selection: {original_}, Axivion node: {node_}".format(original_ = self.original_type, node_ = str(AxivionType.number())))
                            self.type_definition_kind = ""
                    else:
                        self.type_definition_kind = "typedef_type" 
                    
                    """
                    if self.original_type == "_TETEST_used_taged_forw_typedefed_internal_enum_t":
                        print("Ax-type for forward")
                        print(AxivionType.Symbol_Declaration.type())    
                    """
                        
                    """    
                    if AxivionType.number() in [82515]:
                        print("token_string: {tk_str}".format(tk_str = token_str))
                        print("Ax: ")
                        print(AxivionType.Symbol_Declaration.type()) 
                        print(self.type_definition_kind)
                    """
                    """"
                    if (self.type_definition_kind == 'struct_type') and not (AxivionType.Symbol_Declaration.type() in ["Normal_Struct_Type_Definition", "Struct_Type_Declaration"]):
                        print("UNEXPECTED CASE1: {ident_}, {no_}, {type_}".format(ident_ = AxivionType.Logical.Name, no_ = AxivionType.number(), type_ = AxivionType.Symbol_Declaration.type()))
                        
                    if (self.type_definition_kind == 'union_type') and not (AxivionType.Symbol_Declaration.type() in ["Union_Type_Definition", "Union_Type_Declaration", "Unnamed_Union_Type_Definition"]):
                        print("UNEXPECTED CASE2: {ident_}, {no_}, {type_}".format(ident_ = AxivionType.Logical.Name, no_ = AxivionType.number(), type_ = AxivionType.Symbol_Declaration.type()))
                        
                    if (self.type_definition_kind == 'enum_type') and not (AxivionType.Symbol_Declaration.type() in ["Enumeration_Type_Definition", "Unnamed_Enumeration_Type_Definition"]):
                        print("UNEXPECTED CASE3: {ident_}, {no_}, {type_}".format(ident_ = AxivionType.Logical.Name, no_ = AxivionType.number(), type_ = AxivionType.Symbol_Declaration.type()))  
                    """
                    """    
                    if (self.type_definition_kind == 'typedef_type') and not (AxivionType.Symbol_Declaration.type() in ["General_Typedef_Type_Definition"]):
                        print("UNEXPECTED CASE4: {ident_}, {no_}".format(ident_ = AxivionType.Logical.Name, no_ = AxivionType.number()))
                    """ 
                    """
                    if (self.type_definition_kind == 'typedef_type') and (AxivionType.Symbol_Declaration.type() in ["Enumeration_Type_Definition", "Union_Type_Definition", "Normal_Struct_Type_Definition"]):
                        print("UNEXPECTED CASE5: {ident_}, {no_}, {type_}".format(ident_ = AxivionType.Logical.Name, no_ = AxivionType.number(), type_ = AxivionType.Symbol_Declaration.type()))
                    """     
                    # 2016-08-06 UlBr: please preserve comment - it revealed that debugging here is repeatedly necessary    
                    #if self.original_type == 'TETEST_point_type_tag':  
                        #print ('TETEST_point_type_tag has self.type_definition_kind : ', self.type_definition_kind ) 
                                                
                    ax_size = 0                      
                    
            else:
                SoCoLogger.warning("Info CTypeSelectionDescriptor.SetFrom_AxivionType: Uncovered type inherited from Unqualified_Type_Selection " + str(AxivionType.type()) + " for " + str( identifier))     
        
        #WORKAROUND for types without a real type
        ########################
        """
        if self.original_type in TypeSystemDescriptor.Normalized_CIntegerTypes:
            new_original_type = prm_soco.type_system.get_bytesize_SizedC99Type_of_NormalizedCType(self.original_type)[1]
            SoCoLogger.debug("There was found a normalized ctype: {nor_} in the original_type, therefore its changed to: {new_orig_}".format(nor_= self.original_type, new_orig_ = new_original_type))
            self.original_type = new_original_type 
            
        if self.original_type in ["unsigned", "signed", " "]:
            new_original_type = prm_soco.type_system.get_bytesize_SizedC99Type_of_NormalizedCType(self.normalized_C_type)[1]
            SoCoLogger.debug("There was an incomplete normalized ctype: {nor_} in the original_type! It is changed to: {orig_}".format(nor_= self.original_type, orig_ = new_original_type))
            self.original_type = new_original_type
            self.sized_C99_type = new_original_type
            
            for c99type in TypeSystemDescriptor.Generic_To_C99_Sized_Type:
                if TypeSystemDescriptor.Generic_To_C99_Sized_Type[c99type] == self.sized_C99_type:
                    self.gtype = c99type
                    break
            
            SoCoLogger.debug("There was an incomplete normalized ctype: {nor_} in the original_type! It is changed to: {orig_}; the gtype is set to: {new_gtype_}.".format(nor_= self.original_type, orig_ = new_original_type, new_gtype_ = self.gtype))
        ###########################
        """
         
        if self.size_in_bytes == 0:   
                
            if self.not_readable:
                #self.size_in_bytes = -1  
                pass       
                   
            elif self.type_definition_kind:  
                self.gtype = self.original_type           
                if self.resolved_type: 
                    if PRE_SIZE_BUILD:
                        self.resolved_type.build_size(prm_soco.type_system, only_try)
                        self.size_in_bytes = self.resolved_type.size_in_bytes
                    pass
                          
            elif self.original_type == 'void':   
                if self.normalized_C_type != 'void':
                    SoCoLogger.error('Error CTypeSelectionDescriptor.SetFrom_AxivionType: void for ID')            
                self.sized_C99_type = 'void'
                self.gtype = 'void' 

                
            elif self.original_type == 'bool':
                if self.normalized_C_type != 'bool':
                    SoCoLogger.error('Error CTypeSelectionDescriptor.SetFrom_AxivionType: bool')   
                self.sized_C99_type = 'bool'
                self.gtype = 'bool' 
                 

            elif self.original_type in TypeSystemDescriptor.Sized_C99_Types:
                self.sized_C99_type = self.original_type
                bytesize_normalized_tuple = prm_soco.type_system.get_bytesize_NormalizedCType_of_SizedC99Type(self.original_type)                
                self.normalized_C_type = bytesize_normalized_tuple[1]
                self.gtype = prm_soco.type_system.C99_Sized_To_Generic_Type[self.sized_C99_type]
                    
            elif self.original_type in TypeSystemDescriptor.Sized_BSH_Types:
                bytesize_sizedC99_normalized_tuple = prm_soco.type_system.get_bytesize_SizedC99Type_NormalizedCType_of_SizedBSHType(self.original_type)
                self.sized_C99_type = bytesize_sizedC99_normalized_tuple[1]                   
                self.normalized_C_type = bytesize_sizedC99_normalized_tuple[2]
                self.gtype = prm_soco.type_system.C99_Sized_To_Generic_Type[self.sized_C99_type]
                
            elif self.original_type in TypeSystemDescriptor.Implicit_Int_Types:
                bytesize_sizedC99_normalized_tuple = prm_soco.type_system.get_bytesize_SizedC99Type_NormalizedCType_of_Implicit_Int_Type(self.original_type)
                self.sized_C99_type = bytesize_sizedC99_normalized_tuple[1]                   
                self.normalized_C_type = bytesize_sizedC99_normalized_tuple[2]
                self.gtype = prm_soco.type_system.C99_Sized_To_Generic_Type[self.sized_C99_type]
                
            else:
                # i.e a basic interger type 
                if self.normalized_C_type == '':
                    SoCoLogger.error('Error CTypeSelectionDescriptor.SetFrom_AxivionType: hier in size 1')
                    SoCoLogger.info('Error Info CTypeSelectionDescriptor.SetFrom_AxivionType: orginal type: ' +  str(self.original_type) +  ' sized_C99_type: ' + str(self.sized_C99_type) + ' type_definition_kind: ' + str(self.type_definition_kind + " Node:" + str(AxivionType.number())))
            
                if self.normalized_C_type in TypeSystemDescriptor.Normalized_CTypes_To_BasicType:
                    bytesize_sizedC99_tuple =  prm_soco.type_system.get_bytesize_SizedC99Type_of_NormalizedCType(self.normalized_C_type)
                                                               
                    self.sized_C99_type= bytesize_sizedC99_tuple[1]   
    
                    self.gtype = prm_soco.type_system.C99_Sized_To_Generic_Type[self.sized_C99_type]
                else:
                    SoCoLogger.error("No Normalized C-Type found for: {norm_c_}.".format(norm_c_ = str(self.normalized_C_type)))
            
            if PRE_SIZE_BUILD:    
                self.build_size(prm_soco.type_system, True)     #optional parameter to resolve as far as possbile, dont raises errors.
          
            if PRE_SIZE_BUILD and CHECK_AXIVION_AGAINST_TYPE_SYSTEM:
                if self.size_in_bytes != ax_size:
                    SoCoLogger.warning("Warning  CTypeSelectionDescriptor.SetFrom_AxivionType: Missmatch between size from type_system (" + str(self.size_in_bytes)+ ") and AxivionType.Logical.Size (" + str(AxivionType.Logical.Size) + ") for " + str(identifier)) 
               
        else:
            SoCoLogger.warning("Warning CTypeSelectionDescriptor.SetFrom_AxivionType: Uncovered type inherited from Type_Selection " + str(AxivionType.type()) + " for " + str( AxivionType.number()) + " " + str(identifier))
            self.not_readable = True     
            if PRE_SIZE_BUILD:    
                self.build_size(prm_soco.type_system, True)  
        
        
    @classmethod
    def GetFrom_Axivion_Identifier(self, AxivionType):
        
        if AxivionType.is_of_type('Error_Type_Selection'):
            return None
                       
        elif AxivionType.is_of_type('Qualified_Type_Selection'):
                
            pass
            

                    
        elif AxivionType.is_of_type('Unqualified_Type_Selection'):
             
            if  AxivionType.is_of_type('Other_Builtin_Type_Selection'):
                if AxivionType.Logical.Name == "":
                    SoCoLogger.warning('Empty Name for logical Node: ') 
                
            elif AxivionType.is_of_type('Builtin_Type_Selection'):    
                if AxivionType.Logical.Name == "":
                    SoCoLogger.warning('Empty Name for logical Node: ' + str(AxivionType.Node)) 
                else:  
                    identifier = AxivionType.Logical.Name
                    self.normalized_C_type = self.AxivionLogicalTypeName_to_Normalized_CType[AxivionType.Logical.Name]    
                    if self.normalized_C_type == '':
                        SoCoLogger.warning('Info CTypeSelectionDescriptor.SetFrom_AxivionType: Empty normalized_C_type for ID ' + str(identifier))        
                    
                    if AxivionType.is_of_type('Integer_Type_Selection'):                   
                        help = ''
                        token = AxivionType.SLoc.Token
                        while token.__str__() == 'extern' or token.__str__() == 'static' or token.__str__() == 'const' or token.__str__() == 'volatile':
                            token = Conformizer.next_token(token)                             
                        while token.__str__() == 'short' or token.__str__() == 'long' or token.__str__() == 'signed' or token.__str__() == 'unsigned':
                            if help:
                                help += ' '
                            help += token.__str__()
                            token = Conformizer.next_token(token) 
                        if  token.__str__() == 'int' or token.__str__() == 'char':
                            if help:
                                help += ' '
                            help += token.__str__()   
                       
                        if help:
                            return help
                        else:
                            return AxivionType.Logical.Name
                          
                    
                
                    elif AxivionType.is_of_type('Floating_Point_Type_Selection'):                   
                        help = ''
                        token = AxivionType.SLoc.Token
                        while token.__str__() == 'extern' or token.__str__() == 'static' or token.__str__() == 'const' or token.__str__() == 'volatile':
                            token = Conformizer.next_token(token)                              
                        while token.__str__() == 'long':
                            if help:
                                help += ' '
                            help += token.__str__()
                            token = Conformizer.next_token(token) 
                        if  token.__str__() == 'float' or token.__str__() == 'double':
                            if help:
                                help += ' '
                            help += token.__str__()   
                        
                        ax_size = AxivionType.Logical.Size 
                       
                        if help:
                            return help
                        else:
                            return AxivionType.Logical.Name
                                                                                         
                                          
                                                
                    elif AxivionType.is_of_type('Void_Type_Selection'):
                        # i.e. self.normalized_C_type == 'void'
                        return 'void'
                        
                                           
                    
                    elif AxivionType.is_of_type('Boolean_Type_Selection'):
                         # i.e. self.normalized_C_type == 'bool'
                        return 'bool'
                        
                                                           
                    else:
                       SoCoLogger.error('Error CTypeSelectionDescriptor.SetFrom_AxivionType: Uncovered Type inherited from Builtin_Type_Selection')
                                                   
                
            elif AxivionType.is_of_type('Unqualified_Direct_Type_Selection'):
                
                return  AxivionType.Logical.Name 
                             
    @classmethod                 
    def CreateFrom_PSTD(cls, prm_soco, xml_dom_basic_type_node):
        helper = cls()
        helper.SetFrom_PSTD(prm_soco, xml_dom_basic_type_node)
        return helper
       
    def SetFrom_PSTD(self, prm_soco, xml_dom_type_node):
        
        """
        if DomHelper.GetOptionalAttribute(xml_dom_type_node, 'is_const', 'false') == 'true':
            self.is_const = True
        if DomHelper.GetOptionalAttribute(xml_dom_type_node, 'is_volatile', 'false') == 'true':
            self.is_volatile = True
        """
        
        if xml_dom_type_node.nodeName == 'basic_type':
            self.original_type = xml_dom_type_node.getAttribute('ctype')  
            self.gtype = xml_dom_type_node.getAttribute('gtype')
            if self.gtype == "":
                print(self.original_type)
            else:
                self.sized_C99_type = self.Generic_To_C_Sized_Type[self.gtype]  
            self.normalized_C_type = DomHelper.GetOptionalAttribute(xml_dom_type_node,'normalized_ctype')
            if self.normalized_C_type == '':
                self.normalized_C_type = CTypeSelectionDescriptor.C_Type_To_Normalized_CType(self.original_type)

            if READ_WRITE_TMSSC:
                self.tms_signature_code_2008 =  DomHelper.GetOptionalAttribute(xml_dom_type_node,'tms_signature_code_2008')
                self.tms_signature_code_2016 =  DomHelper.GetOptionalAttribute(xml_dom_type_node,'tms_signature_code_2016')

            help_string = DomHelper.GetOptionalAttribute(xml_dom_type_node,'size_in_bytes')
            if help_string:
                self.size_in_bytes = int(help_string) 
            
        elif  xml_dom_type_node.nodeName in self.TYPEDEF_TYPES:
            self.type_definition_kind = xml_dom_type_node.nodeName
            self.original_type = xml_dom_type_node.getAttribute('ctype')    
            self.ctype = self.original_type 
            self.gtype = xml_dom_type_node.getAttribute('gtype')
                                            
        else:
            SoCoLogger.error ('Error CTypeSelectionDescriptor.SetFrom_PSTD: unexpected element ' + str(xml_dom_type_node.nodeName))
    
    @classmethod        
    def GetFrom_PSTD_Identifier(self, xml_dom_type_node):
        if xml_dom_type_node.nodeName == 'basic_type':
            return xml_dom_type_node.getAttribute('ctype')                 
        elif  xml_dom_type_node.nodeName in self.TYPEDEF_TYPES:
            return xml_dom_type_node.getAttribute('ctype')                                     
        else:
            SoCoLogger.error ('Error CTypeSelectionDescriptor.SetFrom_PSTD: unexpected element ' + str(xml_dom_type_node.nodeName))
    
        
    def GetPstdTag(self, set_me_tag=None, qual_from_decl = None):

        if not self.type_definition_kind:
            result_tag = dom.Element('basic_type') 
            result_tag.setAttribute('ctype',self.original_type)
            if self.normalized_C_type:
                result_tag.setAttribute('normalized_ctype',self.normalized_C_type)
            if READ_WRITE_TMSSC:
                result_tag.setAttribute('tms_signature_code_2008',self.tms_signature_code_2008)
                result_tag.setAttribute('tms_signature_code_2016',self.tms_signature_code_2016)
            result_tag.setAttribute('gtype',self.gtype)
            if not self.normalized_C_type == 'void':
                result_tag.setAttribute('size_in_bytes', str(self.size_in_bytes))                     
            
        else:
            result_tag = dom.Element(self.type_definition_kind)
            result_tag.setAttribute('ctype',self.original_type)
            result_tag.setAttribute('gtype',self.gtype) 
            

        if WRITE_AX_INFO:
            result_tag.setAttribute('ax_info', self.ax_info)    
         
        return result_tag

            
                   
        
    #Is only there for adding user-specified quals, exg.: volatile for stub-variables
    def get_qualified_cast_or_dcl(self,id='',quals=''):
        return_value = ''
       
        if quals:
               return_value += quals
        
               return_value += ' '
               
        
        if self.type_definition_kind:
            if self.type_definition_kind == 'struct_type': 
                return_value += 'struct ' + self.original_type
            elif self.type_definition_kind == 'enum_type': 
                return_value += 'enum ' + self.original_type
            elif self.type_definition_kind == 'union_type': 
                return_value += 'union ' + self.original_type
            else:
                return_value += self.original_type
                
        else:
            return_value += self.original_type
            
        if id:
            return_value += ' '            
            return_value += id
            
        return return_value
   
    def clear_size_and_signature(self):        
        self.size_in_bytes = 0
        self.tms_signature_code_2008 = ''
        self.tms_signature_code_2016 = ''
         
         
    def resolve_type_selections(self, typedef_container_list):
    
        if self.type_definition_kind:            
            if self.original_type: # i.e. it is a named type
                if not self.resolved_type:
                    for typedef_container in typedef_container_list:
                        found_typedef = typedef_container.get_typedef_for_kind_and_ID (self.type_definition_kind, self.original_type)
                        if found_typedef:
                            break
                    if found_typedef:  
                        #if found_typedef.declaration_description.resolved_type:
                            self.resolved_type = found_typedef.declaration_description.resolved_type
                        #else:
                            #print('error kein resolved type')
                            #self.resolved_type = found_typedef.declaration_description.type
                else:
                    pass 
                    #ToDo NiEh, has to be strictly reviewed, why should it be problematic if there is a resolved type?!
                    #self.resolved_type.readout()
                    #SoCoLogger.error('Error CTypeSelectionDescriptor.resolve_type_selections for type selection: ' +  str(self.type_definition_kind) + ' ' + str( self.original_type))
            else:
                SoCoLogger.error('Error CTypeSelectionDescriptor.resolve_type_selections for type selection: ' + str(self.type_definition_kind) + ' given as kind but no original_type')
    
    def build_size(self, type_system, only_try=False):
                                     
        if self.size_in_bytes == 0:   
                
            if self.not_readable:
                self.size_in_bytes = -1         
                   
            elif self.type_definition_kind:             
                if self.resolved_type: 
                    self.resolved_type.build_size(type_system, only_try)
                    self.size_in_bytes = self.resolved_type.size_in_bytes
                          
            elif self.original_type == 'void':   
                if self.normalized_C_type != 'void':
                    SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: void')            
                self.size_in_bytes = -1

                
            elif self.original_type == 'bool':
                if self.normalized_C_type != 'bool':
                    SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: bool')   
                self.size_in_bytes = type_system.type_size_dict['bool']
                

            elif self.original_type in TypeSystemDescriptor.Sized_C99_Types:
                bytesize_normalized_tuple = type_system.get_bytesize_NormalizedCType_of_SizedC99Type(self.original_type)
                self.size_in_bytes = bytesize_normalized_tuple[0]                    
                     
            elif self.original_type in TypeSystemDescriptor.Sized_BSH_Types:
                bytesize_sizedC99_normalized_tuple = type_system.get_bytesize_SizedC99Type_NormalizedCType_of_SizedBSHType(self.original_type)
                self.size_in_bytes = bytesize_sizedC99_normalized_tuple[0] 
                if self.size_in_bytes == 0:
                    SoCoLogger.warning('Error CTypeSelectionDescriptor.build_size: size-in-bytes==0 for orignal_type: ' + str(self.original_type))
                 
            else:
                if self.normalized_C_type == '':
                    SoCoLogger.warning('Error CTypeSelectionDescriptor.build_size: hier in size 1')
                    SoCoLogger.info('Info: orginal type: ' + str(self.original_type) + 'sized_C99_type: ' + str(self.sized_C99_type) + 'type_definition_kind: ' + str(self.type_definition_kind))
                if self.normalized_C_type:
                    if self.normalized_C_type in type_system.Normalized_CTypes_To_BasicType:
                        bytesize_sizedC99_tuple =  type_system.get_bytesize_SizedC99Type_of_NormalizedCType(self.normalized_C_type)                                       
                        self.size_in_bytes = bytesize_sizedC99_tuple[0] 
                        
                    else:
                        SoCoLogger.warning('Error CTypeSelectionDescriptor.build_size: size-in-bytes unknown for normalized_Ctype: ' + str(self.original_type))  
                
                
                     
        else: # self.size_in_bytes != 0 # 2016-02-04 UlBr: branch used for checking/debugging
             
            if (only_try == False) and (self.size_in_bytes == 0):
                SoCoLogger.error ('Error CTypeSelectionDescriptor.build_size: ' + str(self.original_type) + ': size not determinable despite only_try==False.')
             
            if self.type_definition_kind:
                #print('should not pass 6')
                pass
                               
            else:
                 
                if self.original_type == 'void':              
                    if self.sized_C99_type != 'void':
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size:  12')
                    if self.gtype != 'void' :
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: 13')
                 
                elif self.original_type == 'bool':                   
                    if self.sized_C99_type != 'bool':
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: 15')
                    if self.gtype != 'bool':
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: 16')
                                 
                 
                elif self.original_type in TypeSystemDescriptor.Sized_C99_Types:
                    #if self.normalized_C_type != '':  
                        #print ('error for sized C99')
                         
                    if self.sized_C99_type != self.original_type:
                         SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: 1')
 
                    bytesize_normalized_tuple = type_system.get_bytesize_NormalizedCType_of_SizedC99Type(self.original_type)
                    if self.size_in_bytes != bytesize_normalized_tuple[0]:
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: 2')   
                    if self.normalized_C_type != bytesize_normalized_tuple[1]:
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: 3' + str(self.normalized_C_type) + ' vs. ' + str(bytesize_normalized_tuple[1]))
                    if self.gtype != type_system.C99_Sized_To_Generic_Type[self.original_type]:
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: 4')
                          
                elif self.original_type in TypeSystemDescriptor.Sized_BSH_Types:
                    #if self.normalized_C_type != '':  
                        #print ('error for BSH')
                         
                    bytesize_sizedC99_normalized_tuple = type_system.get_bytesize_SizedC99Type_NormalizedCType_of_SizedBSHType(self.original_type)
                    if self.size_in_bytes != bytesize_sizedC99_normalized_tuple[0]:
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: 5') 
                    if self.sized_C99_type != bytesize_sizedC99_normalized_tuple[1]:
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: ' + str(self.sized_C99_type) + ' vs. ' + str(bytesize_sizedC99_normalized_tuple[1]) ) 
                    if self.normalized_C_type != bytesize_sizedC99_normalized_tuple[2]:
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: ' + str(self.normalized_C_type) + ' vs. ' + str(bytesize_sizedC99_normalized_tuple[2]))
                    if self.gtype != type_system.C99_Sized_To_Generic_Type[self.sized_C99_type]:
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: 8')
                     
                else:
                     
                 
                    if self.normalized_C_type in TypeSystemDescriptor.Normalized_CTypes_To_BasicType:
                        bytesize_sizedC99_tuple =  type_system.get_bytesize_SizedC99Type_of_NormalizedCType(self.normalized_C_type)
                     
                        if self.size_in_bytes != bytesize_sizedC99_tuple[0]:
                            SoCoLogger.error ('Error CTypeSelectionDescriptor.build_size: 9 axivion ' + str(self.size_in_bytes) + ' Byte  vs. TypeSystem ' + str(bytesize_sizedC99_tuple[0]) + ' Byte for ' +str(self.normalized_C_type))
                            SoCoLogger.info ('ErrorInfo type system ID: ', type_system.type_system_id)    
                            SoCoLogger.info ('ErrorInfo :size of int: ', type_system.type_size_dict['int'])                                                       
                        if self.sized_C99_type != bytesize_sizedC99_tuple[1]:
                            SoCoLogger.error ('Error CTypeSelectionDescriptor.build_size: 10')      
                            SoCoLogger.info ('ErrorInfo: type system ID: ' + str(type_system.type_system_id))     
                            SoCoLogger.info ('ErrorInfo: self.original_type is: ' +  str(self.original_type) + '; sized_C99_type is: ' + str(self.sized_C99_type) + '; bytesize_sizedC99_tuple[1] is: ' + str(bytesize_sizedC99_tuple[1]))            
                        if self.gtype != type_system.C99_Sized_To_Generic_Type[self.sized_C99_type]:
                            SoCoLogger.error ('Error CTypeSelectionDescriptor.build_size: 11')  
                            
                    else:
                        SoCoLogger.error('Error CTypeSelectionDescriptor.build_size: A defect normalized C-type occured: {norm_c_} for axivion node:'.format(norm_c_ = self.normalized_C_type, node_ = str(self.ax_info)))
                        self.readout()
                    
 
                
    def build_tms_signature_code(self, prm_soco):
        if self.not_readable:
            self.tms_signature_code_2008 = 'INVALID'
            self.tms_signature_code_2016 = 'INVALID'
            return self.tms_signature_code_2016
        
        if self.tms_signature_code_2008 == '':        
            if self.normalized_C_type in _NORMALIZED_C_TYPE_TO_TMS_SIGNATURE_2008:
                self.tms_signature_code_2008 = _NORMALIZED_C_TYPE_TO_TMS_SIGNATURE_2008[self.normalized_C_type]
            else:
                if self.resolved_type:
                    self.resolved_type.build_tms_signature_code(prm_soco)
                    self.tms_signature_code_2008 = self.resolved_type.tms_signature_code_2008
                
        if self.tms_signature_code_2016 == '':        
            if self.sized_C99_type in _SIZED_C99_TYPE_TO_TMS_SIGNATURE_2016:
                self.tms_signature_code_2016 = _SIZED_C99_TYPE_TO_TMS_SIGNATURE_2016[self.sized_C99_type]
            else:
                if self.resolved_type:
                    self.resolved_type.build_tms_signature_code(prm_soco)
                    self.tms_signature_code_2016 = self.resolved_type.tms_signature_code_2016

                    
        return self.tms_signature_code_2016     

    # class attributes

    C99_Sized_Types = ['int8_t', 'uint8_t', 'int16_t', 'uint16_t', 'int32_t', 'uint32_t', 'int64_t', 'uint64_t', 'int128_t', 'uint128_t', 'void']
    
    C_BSH_To_C99Sized_Types = {  
        'schar':   'int8_t',                         
        'uchar':   'uint8_t',   
        'int16':   'int16_t',
        'uint16':  'uint16_t',
        'int32':   'int32_t',
        'uint32':  'uint32_t'  
        }
    
    
    AxivionLogicalTypeName_to_Normalized_CType = {
        'char':                 'char',
        'signed char':          'signed char',
        'unsigned char':        'unsigned char',
        'signed short':         'signed short',
        'unsigned short':       'unsigned short',
        'signed int':           'signed int',
        'unsigned int':         'unsigned int',
        'signed long':          'signed long',
        'unsigned long':        'unsigned long',
        'signed long long':     'signed long long',
        'unsigned long long':   'unsigned long long',
        'float':                'float',
        'double':               'double',
        'long double':          'long double',
        'void':                 'void',
        'bool':                 'bool'
    }
    
    @classmethod 
    def C_Type_To_Normalized_CType (cls, C_Type):
        # normalized form for a valid C type       
        if C_Type.find('void') > -1:
            return 'void'
        
        elif C_Type.find('bool')  > -1:
            return 'bool'
        
        else:      
     
            if C_Type.find('float') >  -1:
                return 'float'
            elif C_Type.find('double') >  -1:
                if  C_Type.find('long') >  -1:
                    return 'long double'
                else:
                    return 'double'
                
            elif C_Type.find('char') >  -1:
                if  C_Type.find('unsigned') >  -1:
                    return 'unsigned char'
                if  C_Type.find('signed') >  -1:
                    return 'signed char'
                else:
                    return 'char'
                
            elif C_Type in ["signed", "unsigned"]:
                return C_Type + " int"
            
            elif C_Type == "":
                return int
                
            else: # an int
                if C_Type.find('unsigned') >  -1:
                    result = 'unsigned '
                else:
                    result = 'signed '
                if C_Type.find('long long') >  -1:
                    result += 'long long'
                elif C_Type.find(' long') >  -1:
                    result += 'long'
                elif C_Type.find('short') >  -1:
                    result += 'short'
                else: 
                    result += 'int'
                return result
        
    
    C_Sized_To_Generic_Type = {
        'int8_t':       'int8',
        'uint8_t':      'uint8',   
        'int16_t':      'int16',
        'uint16_t':     'uint16',   
        'int32_t':      'int32',
        'uint32_t':     'uint32',  
        'int64_t':      'int64',
        'uint64_t':     'uint64',
        'uint128_t':    'uint128',  
        'int128_t':     'int128',              
        'float16_t':    'decimal16',  
        'float32_t':    'decimal32',  
        'float64_t':    'decimal64',
        'float128_t':   'decimal128',        
        'void':         'void'           
        }
 
    Generic_To_C_Sized_Type = {
        'int8':       'int8_t',
        'uint8':      'uint8_t',   
        'int16':      'int16_t',
        'uint16':     'uint16_t',   
        'int32':      'int32_t',
        'uint32':     'uint32_t',  
        'int64':      'int64_t',
        'uint64':     'uint64_t',
        'uint128':    'uint128_t',  
        'int128':     'int128_t',       
        'decimal16':  'float16_t',  
        'decimal32':  'float32_t',  
        'decimal64':  'float64_t',
        'decimal128': 'float128_t',        
        'void':       'void',
        'bool':       'bool'             
        }
     
class CInPlaceTypeDescriptor(CTypeDescriptor):
    """ describes a unnamed types in the C programming language i.e. types that are defined in the place where they are used;
    these types are: arrays, pointer, function pointer and unnamed structs, unions and ennums """  
    
          
          
    def __init__(self):
        CTypeDescriptor.__init__(self)
        
        self.ax_info = ''
                        
        self.is_pointer_to = False          # the CTypeDescriptor instance is assigned to included_type
        
        #rename: included_type_const
        self.const_included_type = False            #rename const_included_type 
        self.volatile_included_type = False         #rename volatile_included_type
        
        ####old
        #self.array_of_const = False 
        
        self.is_array_of = -1               # -1 means: no array; > 0 array of accordant size; 0 means: array of unspecified size; the CTypeDescriptor instance is assigned to included_type
             
        #2016-08-12 UlBr: could not construct example that fullfils this                       
        self.unnamed_struct_or_enum = False # the CTypeDescriptor instance is assigned to included_type (unnamed enumeration oder struct)

        self.included_type = None           # pointed to type resp. type of array element resp. unnamed struct or enum type
        self.resolved_type = None           # if included type is CCompositeTypeDescriptor or CEnumTypeDescriptor then the resolved type is the  type descriptor in the according type in the TypeDefinitionDescriptor
         
        self.resolved = False
            
    
    def formIDTypeListRecursive(self, last_identifier, max_array_index=5):
        id_type_list = list()
        
        if self.resolved_type == None:
            used_type = self.included_type.getResolvedType()
        else:
            used_type = self.resolved_type.getResolvedType()
            
        #used_type.readout()
        
        for i in range(0, min((max_array_index, self.is_array_of))):
            current_id = last_identifier+"["+str(i)+"]"
            if used_type.isArrayType() > 0:
                id_type_list.extend(used_type.formIDTypeListRecursive(current_id, max_array_index))
            elif len(used_type.getCompositeMembers()):
                id_type_list.extend(used_type.formIDTypeListRecursive(current_id))
            else:
                id_type_list.append((current_id, self))
        
        return id_type_list
        
    def formIDIndexListRecursive(self):
        id_index_list = list()
        
        if self.resolved_type == None:
            used_type = self.included_type.getResolvedType()
        else:
            used_type = self.resolved_type.getResolvedType()
            
        #used_type.readout()
        
        loops = int(self.is_array_of/5)
        rest = self.is_array_of%5
        
        if not rest == 0:
            loops += 1
            rest -= 1
        
        for i in range(0, loops, 1):
            ix_five = i*5
            if (i+1) == loops and not rest == 0:
                array_range = "["+str(ix_five)+"-"+str(ix_five+rest)+"]"
            else:
                array_range = "["+str(ix_five)+"-"+str(ix_five+4)+"]"
            
            current_id_index_list = [array_range]
            if used_type.isArrayType() > 0:
                current_id_index_list.append(used_type.formIDIndexListRecursive())
            elif len(used_type.getCompositeMembers()):
                current_id_index_list.append(used_type.formIDIndexListRecursive())
            else:
                current_id_index_list.append([])
            id_index_list.append(current_id_index_list)
        
        return id_index_list
        
    def isArrayType(self):
        return self.is_array_of
        

    def readout(self):
        
        print("-->CInPlaceTypeDescriptor")
        
        #print " ->modifier: " + str(self.is_const ) + " " + str(self.is_volatile)
        print(" ->not readable: " + str(self.not_readable))
        print(" ->size in bytes: " + str(self.size_in_bytes))          # 0: no value detmerined yet, -1: not applicable (e.g. void, a function etc,); > 0:nomen est omen; CFunctionsSignarueCType: can only be read from symbol file i.e. 0 if only pstd file read
        print(" ->signature_2008: " + str(self.tms_signature_code_2008))     # the so called signature code for working with the TCL  TestEngine
        print(" ->signature_2016: " + str (self.tms_signature_code_2016))    # the so called signature code for working with the Python TestEngine

        print(" ->is_array_of: " + str(self.is_array_of))
        print(" ->unnamed_struct_or_enum: " + str(self.unnamed_struct_or_enum))
        print(" ->included_type: " + str(self.included_type))     
        print(" ->resolved_type: " + str(self.resolved_type))
        print(" ->resolved: " + str(self.resolved))

        print(" ->points to const: " + str(self.const_included_type))     
        print(" ->array of const: " + str(self.volatile_included_type))
        
    
    @classmethod                 
    def CreateFrom_AxivionType(cls, prm_soco, AxivionType, identifier):
        helper = cls()
        helper.SetFrom_AxivionType(prm_soco, AxivionType, identifier)
        return helper
                
    def SetFrom_AxivionType(self, prm_soco, AxivionType, identifier):   
        
        self.ax_info = AxivionType.type() + ' ' + str(AxivionType.number())      
                                                                         
        if AxivionType.is_of_type('Pointer_Type_Definition'):
            self.is_pointer_to = True
         
            self.included_type = CTypeDescriptor.Try_CreateTypedefReference(AxivionType.Pointed_To_Type)
            if not self.included_type:
                self.included_type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionType.Pointed_To_Type, identifier)    #const points to const can be set
                if AxivionType.Pointed_To_Type.is_of_type('Qualified_Type_Definition') or AxivionType.Pointed_To_Type.is_of_type('Qualified_Direct_Type_Selection'):    
                    self.const_included_type = AxivionType.Pointed_To_Type.Qualifiers.is_const
                    self.volatile_included_type = AxivionType.Pointed_To_Type.Qualifiers.is_volatile
            
            if 'Size' in AxivionType.Logical.fields():
                ax_size = AxivionType.Logical.Size 
            else:
                ax_size = -1
                #print(str(AxivionType.number()), ' has no size attribute!') # todo: whats up? priority: low - if no problems if reading of symbol file works then maybe unnecassary ?
                
            if self.included_type:     
                if self.included_type.not_readable:
                    self.not_readable = True
                                      
        
        elif AxivionType.is_of_type('Array_Type_Definition'):  
            if AxivionType.is_of_type('Fixed_Length_Array_Type_Definition'):
                if AxivionType.is_of_type('Standard_Array_Type_Definition'):     
                    #print ('Standard_Array_Type_Definition for node ' , AxivionType.number(), ' for ' , identifier)
                    if AxivionType.Logical:
                        self.is_array_of = AxivionType.Logical.Upper_Bound
                    else:    
                        try:
                            self.is_array_of = AxivionType.Upper_Bound.Value
                        except:
                            self.is_array_of = -1
                    #print ('passed with value ',  self.is_array_of) 
                
                #causes Exception in Axivion 6.7.1: ValueError: Class 'Initialized_Array_Type_Definition' does not exist in part Physical
                #elif AxivionType.is_of_type('Initialized_Array_Type_Definition'):
                     #self.is_array_of = 0
                elif AxivionType.is_of_type('Unbounded_Array_Type_Definition'):
                     self.is_array_of = 0
                else:
                    SoCoLogger.warning ("Warning CInPlaceTypeDescriptor.SetFrom_AxivionType: Uncovered type inherited from Fixed_Length_Array_Type_Definition " + str(AxivionType.type()) + " for " + str(identifier))       
                         
            elif  AxivionType.is_of_type('Variable_Length_Array_Type_Definition'):
                if AxivionType.is_of_type('Bounded_VLA_Type_Definition'):
                    SoCoLogger.warning('Warning CInPlaceTypeDescriptor.SetFrom_AxivionType: Bounded_VLA_Type_Definition for node, ' + str(AxivionType.number()) + 'identifier: ' + str(identifier))
                    #self.is_array_of = AxivionType.Upper_Bound.Value # 2016-08-19 UlBr: Hier scheint es keinen Zahlenwert sondern nir Symbol zu geben?
                    #print ('passed with value ',  self.is_array_of) 
                    self.is_array_of = 0
                elif AxivionType.is_of_type('Unbounded_VLA_Type_Definition'):
                    SoCoLogger.warning('Warning CInPlaceTypeDescriptor.SetFrom_AxivionType: Unounded_VLA_Type_Definition for node, ' + str(AxivionType.number()) + 'identifier: ' + str( identifier))
                    self.is_array_of = 0
                else:
                    SoCoLogger.warning ("Warning CInPlaceTypeDescriptor.SetFrom_AxivionType: Uncovered type inherited from Variable_Length_Array_Type_Definition " + str(AxivionType.type()) +  " for " + str(identifier))  
               
            else:  
                SoCoLogger.warning ("Warning CInPlaceTypeDescriptor.SetFrom_AxivionType: Uncovered type inherited from Array_Type_Definition " + str(AxivionType.type()) + " for " + str(identifier))  
                self.not_readable = True
            
            if self.not_readable == False:
                self.included_type = CTypeDescriptor.Try_CreateTypedefReference(AxivionType.Element_Type)      
                if not self.included_type:                   
                    self.included_type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionType.Element_Type, identifier)
                    
                    
            if self.included_type:     
                if self.included_type.not_readable:
                    self.not_readable = True
                    
        # 2016-08-12 UlBr: could not construct example that reaches this code - do not delete until completion of tests   
                                                             
#        elif AxivionType.is_of_type('Unnamed_User_Type_Definition'):
#             print('Unnamed_User_Type_Definition ', self.ax_info)
#             
#             if AxivionType.is_of_type('Unnamed_Enumeration_Type_Definition'):
#                 self.unnamed_struct_or_enum = True
#                 self.included_type = CEnumTypeDescriptor.CreateFrom_AxivionType(type_system, AxivionType, identifier)
#                 
#             elif AxivionType.is_of_type('Unnamed_Composite_Type_Definition'):
#                 self.unnamed_struct_or_enum = True
#                 self.included_type = CCompositeTypeDescriptor.CreateFrom_AxivionType(type_system, AxivionType, identifier)
#                  
#             else:
#                print ("Uncovered type inherited from Unnamed_User_Type_Definition ", AxivionType.type(), " for " , identifier) 
#                self.not_readable = True  
#                
#             if self.included_type:     
#                 if self.included_type.not_readable:
#                     self.not_readable = True
               
        else:
            SoCoLogger.warning ("Warning: CInPlaceTypeDescriptor.SetFrom_AxivionType: Uncovered type inherited from In_Place_Type_Composition" + str(AxivionType.type()) + " for " + str(identifier)) 
            self.not_readable = True                        
     
        if PRE_SIZE_BUILD:   
            self.build_size(prm_soco.type_system, True) 
       
            if CHECK_AXIVION_AGAINST_TYPE_SYSTEM:
                if self.is_pointer_to:
                    if self.size_in_bytes != ax_size:
                        #print('Warning in CInPlaceTypeDescriptor.SetFrom_AxivionType: missmach between size from type_system (', self.size_in_bytes, ') and AxivionType.Logical.Size (', AxivionType.Logical.Size, ') for ', identifier)    
                        pass    
                           

 
    @classmethod                 
    def CreateFrom_PSTD(cls, prm_soco, xml_dom_basic_type_node):
        helper = cls()
        helper.SetFrom_PSTD(prm_soco, xml_dom_basic_type_node)
        return helper
    
    def SetFrom_PSTD(self, prm_soco, xml_dom_type_node):
        
        help_string = DomHelper.GetOptionalAttribute(xml_dom_type_node,'size_in_bytes')
        if help_string:
            self.size_in_bytes = int(help_string)   
                           
        if xml_dom_type_node.nodeName == 'pointer_to':
            self.is_pointer_to = True    
            self.size_in_bytes = int(xml_dom_type_node.getAttribute('size_in_bytes'))        
            self.included_type = CTypeDescriptor.CreateFrom_PSTD(prm_soco, DomHelper.GetFirstElementChild(xml_dom_type_node))
            
            if DomHelper.GetOptionalAttribute(DomHelper.GetFirstElementChild(xml_dom_type_node), 'is_const', 'false') == 'true':
                self.const_included_type = True
            if DomHelper.GetOptionalAttribute(DomHelper.GetFirstElementChild(xml_dom_type_node), 'is_volatile', 'false') == 'true': 
                self.volatile_included_type = True   
                
            
            #TODO NiEh Review 11.10 : rechecked
            if DomHelper.GetOptionalAttribute(xml_dom_type_node, 'is_const', 'false') == 'true':
                self.const_included_type = True
            
              
            if DomHelper.GetOptionalAttribute(xml_dom_type_node, 'is_volatile', 'false') == 'true':
                self.volatile_included_type = True
            
                
        elif xml_dom_type_node.nodeName == 'function_pointer':
            self.is_pointer_to = True 
            self.included_type = CFunctionSignatureTypeDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_type_node)
            
            if DomHelper.GetOptionalAttribute(xml_dom_type_node, 'is_const', 'false') == 'true':    #ToDo is this the correct Node?! Incosistency.
                self.const_included_type = True
            if DomHelper.GetOptionalAttribute(xml_dom_type_node, 'is_volatile', 'false') == 'true': 
                self.volatile_included_type = True 
            
            
        elif xml_dom_type_node.nodeName == 'array_of':
            self.is_array_of = int(xml_dom_type_node.getAttribute('number_of_elements'))
            self.included_type = CTypeDescriptor.CreateFrom_PSTD(prm_soco, DomHelper.GetFirstElementChild(xml_dom_type_node))    
            if READ_WRITE_TMSSC:
                self.tms_signature_code_2008 = DomHelper.GetOptionalAttribute(xml_dom_type_node,'tms_signature_code_2008')
                self.tms_signature_code_2016 = DomHelper.GetOptionalAttribute(xml_dom_type_node,'tms_signature_code_2016')   
                
            #ToDo Review 11.10: qualifier const_included_type/volatile for arrays do not exist, because the adresses are const.  (var_desc has qualifier)  
                                     
        else:
            SoCoLogger.warning('Warning CInPlaceTypeDescriptor.SetFrom_PSTD: unexpected element ' + str(xml_dom_type_node.nodeName))
    
    
    def GetPstdTag(self, set_me_tag=None):
        
        if self.is_pointer_to:              
            if  self.included_type.__class__ == CFunctionSignatureTypeDescriptor:                
                result_tag = dom.Element('function_pointer')   
                result_tag.setAttribute('size_in_bytes', str(self.size_in_bytes))  
                if READ_WRITE_TMSSC:
                    result_tag.setAttribute('tms_signature_code_2008', self.included_type.tms_signature_code_2008)
                    result_tag.setAttribute('tms_signature_code_2016', self.included_type.tms_signature_code_2016)
            else:
                result_tag = dom.Element('pointer_to')  
                result_tag.setAttribute('size_in_bytes', str(self.size_in_bytes))  
            
  
    

                    
            if WRITE_AX_INFO:
                result_tag.setAttribute('ax_info', self.ax_info)                                    
            
            if self.included_type.__class__ == CFunctionSignatureTypeDescriptor:    
                funcs_sign_tags= self.included_type.GetPstdTags()
                for tag in funcs_sign_tags:
                    result_tag.appendChild(tag) 
            else:
                inc_type_tag = self.included_type.GetPstdTag()
                
                if self.const_included_type:
                    inc_type_tag.setAttribute('is_const','true')            
                if self.volatile_included_type:
                    inc_type_tag.setAttribute('is_volatile','true')
                
                result_tag.appendChild(inc_type_tag) 
            return result_tag
                         
        elif self.is_array_of >= 0:
            result_tag = dom.Element('array_of') 
            result_tag.setAttribute('number_of_elements', str(self.is_array_of))
            if READ_WRITE_TMSSC:
                    result_tag.setAttribute('tms_signature_code_2008',self.tms_signature_code_2008)
                    result_tag.setAttribute('tms_signature_code_2016',self.tms_signature_code_2016)
            

            inc_type_tag = self.included_type.GetPstdTag()
                  
            
            if WRITE_AX_INFO:
                result_tag.setAttribute('ax_info', self.ax_info)   
            
            result_tag.appendChild(inc_type_tag) 
            return result_tag
         
                            
        else:
            SoCoLogger.error('Error CInPlaceTypeDescriptor.GetPstdTag: Uncovered in_place_type')
    
    def clear_size_and_signature(self):     
        self.size_in_bytes = 0
        self.tms_signature_code_2008 = ''
        self.tms_signature_code_2016 = ''
        if self.included_type:
            self.included_type.clear_size_and_signature()
            
    def resolve_type_selections(self, typedef_container_list):  
        self.resolved_type = SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_resolved_type(self.included_type, typedef_container_list)       
        self.included_type.resolve_type_selections(typedef_container_list)

    
    def build_size(self, type_system, only_try=False):
        
        if self.size_in_bytes == 0:  
            
            if self.not_readable:
                self.size_in_bytes = -1
                
            else:
                  
                if not self.included_type: # defensive
                    self.size_in_bytes = -1
                    
                else:
                                        
                    if self.included_type.not_readable:
                        self.size_in_bytes = -1   
                    
                    else:
                   
                        if self.is_pointer_to:  
                            self.size_in_bytes = type_system.type_size_dict['pointer']
                            self.included_type.build_size(type_system, only_try)
                                                                                                                                      
                        else:     
                            if self.is_array_of == 0: 
                                #print ('hier arrray size 0')
                                self.included_type.build_size(type_system, only_try) 
                                self.size_in_bytes = type_system.type_size_dict['pointer']
                                if self.resolved_type:
                                    self.resolved_type.build_size(type_system, only_try)
                                                                
                            elif self.is_array_of > 0:  
                                self.included_type.build_size(type_system, only_try)  
                                if self.resolved_type:
                                    self.resolved_type.build_size(type_system, only_try)                         
                                if self.included_type.size_in_bytes < 0: # i.e. not calculatable
                                    self.size_in_bytes = -1
                                else:
                                    self.included_type.build_size(type_system, only_try)   
                                    self.size_in_bytes = self.included_type.size_in_bytes 
                                    self.size_in_bytes = self.size_in_bytes * self.is_array_of
                            
                            else: # i.e. in place type composition  
                                self.included_type.build_size(type_system, only_try)                              
                                self.size_in_bytes = self.included_type.size_in_bytes

            if (only_try==False) and (self.size_in_bytes == 0):
                SoCoLogger.error ('Error CInPlaceTypeDescriptor.build_size : size not determinable despite only_try==False.')
                 
        
    def build_tms_signature_code(self, prm_soco, write_array_index_to_sig = True):
        global RE_ENDING_NUMBER
        
        if self.not_readable:
            self.tms_signature_code_2008 = 'INVALID'
            self.tms_signature_code_2016 = 'INVALID'
            return self.tms_signature_code_2016
        
        if self.included_type:
            self.included_type.build_tms_signature_code(prm_soco)
            if self.resolved_type:
                self.resolved_type.build_tms_signature_code(prm_soco)
        
        if self.tms_signature_code_2008 == '': 
         
            if self.is_pointer_to:    
                if self.included_type.__class__ == CTypeSelectionDescriptor:   
                    if self.included_type.normalized_C_type:                   
                        if self.included_type.normalized_C_type.endswith('char'):
                            self.tms_signature_code_2008='BPTR1'
                        elif self.included_type.normalized_C_type.endswith('short'):
                            self.tms_signature_code_2008='SPTR1'
                        elif self.included_type.normalized_C_type.endswith('long'):
                            self.tms_signature_code_2008='LPTR1'
                        else:
                            self.tms_signature_code_2008='LLPTR1'
                    else:
                        self.tms_signature_code_2008 = 'PTR1'
                
                elif self.included_type.__class__ == CFunctionSignatureTypeDescriptor:   
                   self.tms_signature_code_2008 = 'PTR1' 
                   
                else:
                    self.tms_signature_code_2008 = 'PTR1'
            
            elif self.is_array_of >= 0:
                #if self.included_type.__class__ == CTypeSelectionDescriptor:
                #       self.tms_signature_code_2008 = self.included_type.tms_signature_code_2008[0:-1]
                #       self.tms_signature_code_2008 += str(self.is_array_of)   
                #elif self.included_type.__class__ == CInPlaceTypeDescriptor:
                #    if self.included_type.is_array_of > 0:    
                        if self.included_type.tms_signature_code_2008:             
                            match = RE_ENDING_NUMBER.match(self.included_type.tms_signature_code_2008)
                            if match:
                                self.tms_signature_code_2008 = match.group(1) 
                                if self.is_array_of > 0:
                                    number = int(match.group(2))    
                                    number = number * self.is_array_of                              
                                    self.tms_signature_code_2008 += str(number)
                                else: # == 0
                                    self.tms_signature_code_2008 += '*x'
                        else:
                            self.tms_signature_code_2008 = ''
                                      
            else: #unnamed_struct_or_enum
                self.tms_signature_code_2008 = 'BS'     
                self.tms_signature_code_2008 += str(self.size_in_bytes)
                
                                
        if self.tms_signature_code_2016 == '': 
         
            if self.is_pointer_to:    
                self.tms_signature_code_2016 = 'PTR'
                self.tms_signature_code_2016 += str(self.size_in_bytes * 8)
                self.tms_signature_code_2016 += '_'
                
                if self.const_included_type:
                    self.tms_signature_code_2016 += 'C_'
                
                if self.resolved_type: 
                    self.tms_signature_code_2016 += self.resolved_type.tms_signature_code_2016                    
                
            elif self.is_array_of >= 0:
                
                if self.const_included_type:
                    self.tms_signature_code_2016 += 'C_'
                
                if self.resolved_type:   
                    self.tms_signature_code_2016 = self.resolved_type.tms_signature_code_2016
                
                index_list = []
                all_index = int(1)
                prod_index = []
                SourceCodeDescriptor._search_original_typedescriptor(self.included_type, [], index_list, True)
                for aindex in index_list:
                    all_index = all_index*aindex
                    prod_index.append(all_index)
                
                prod_index.reverse()    
                    
                for aindex in prod_index:
                    if self.tms_signature_code_2016.endswith(str(aindex)):
                       self.tms_signature_code_2016 = self.tms_signature_code_2016[:-(len(str(aindex))+1)]
                    break 
                    
                self.tms_signature_code_2016 += "_" + str(self.is_array_of*all_index)   
                                      
            else: #unnamed_struct_or_enum
                self.tms_signature_code_2016 = 'XXX'     
                           
         
        return self.tms_signature_code_2016            
    
    """
    This function is basically used to receive type-connected strings, such as declarations or casts, also for elements of arrays. 
    Especially for casts, that means the id is empty, brackets for arrays have to be neglected, otherwise they would cause syntax error.
    """
    def get_qualified_cast_or_dcl(self,id='',quals=''):    #ToDo NiEh in the case of pointer to pointer ... the pointer stars are seperated with a whitespace, exg. uint8_t * *
        return_value = ''
        
        if self.is_pointer_to:
            if  self.included_type.__class__ == CFunctionSignatureTypeDescriptor:  #ToDo NiEh: imagine an included type, that is a CTypeSelection for a CFunctionSignatureType
                return_value += self.included_type.get_source_code_return_type()
                return_value += ' '
                return_value += '(*'
                if quals:
                    return_value += quals 
                
                return_value += ' '
                return_value += id
                return_value += ')'
                return_value += ' '
                return_value += '('
                return_value += self.included_type.get_source_code_param_decls() 
                return_value += ')'
                
            else:
                
                qual_incl_type = ''

                if self.volatile_included_type:
                    qual_incl_type += 'volatile ' 
                if self.const_included_type:
                	qual_incl_type += 'const '  
                          
                return_value += self.included_type.get_qualified_cast_or_dcl('', qual_incl_type) + ' ' # at this point const_included_type has to be processed
                
                return_value += '* '                
                
                if quals:
                    return_value += quals + ' '   
                    
                return_value += id            

                        
        elif self.is_array_of >= 0:
            typedesc_indicator = list([])
            buffer_Cfunc_typedesc = SourceCodeDescriptor._search_original_typedescriptor(self.included_type, typedesc_indicator, [], 1)
            til = collections.Counter()
            for c in typedesc_indicator:
                til[c] += 1 
            
            if(til[0] > 0): #if we have an array of const pointers to const and, the quals have to be passed
                return_value += self.included_type.get_qualified_cast_or_dcl(id, quals = quals)
            else:
                if quals:
                    return_value += quals + ' '      
                    
                return_value += self.included_type.get_qualified_cast_or_dcl(id) #at this point const_included_type has to be processed          
                
                 
            #if id is empty, you want to get a cast, and in this case the brackets can not be added.
            #You can not move this process, because later, brackets can inherit from function-pointers
            #this is also in major correct: the id is passed through the recursive calls, therefore 
            #brackets are added always, if needed, also for more dimensional-arrays.
            if id == "":
                array_index=""
            else:
                array_index = '['
                if self.is_array_of > 0:
                    array_index += str(self.is_array_of)
                array_index +=']'
            
            if isinstance(buffer_Cfunc_typedesc, CFunctionSignatureTypeDescriptor) and (til[3] == 0):   #til[3] == 0 means no typedef in the list of types
                parameter_parts = return_value.split(")", 1) 
                
                if("[" in parameter_parts[0]):
                    parameter_parts[0] = parameter_parts[0].replace("[", array_index+"[", 1)
                else:
                    parameter_parts[0] += array_index
                
                return_value = parameter_parts[0] + ")" + parameter_parts[1] 
            else:
                if("[" in return_value):
                    return_value = return_value.replace("[", array_index+"[", 1)
                else:
                    return_value += array_index
                
            #if id:
            #    return_value += ' '
            #    return_value += id  
            
        elif self.unnamed_struct_or_enum:
            
            return_value = 'stub_error'
            SoCoLogger.warning("Warning CInPlaceTypeDescriptor.get_qualified_cast_or_dcl: The stub generation algorithm does not support enum or struct in place type compositons! Search for 'stub_error' to find problematic function and introduce a type definition that you use in the delcaring and defining the accordant function!")   
            
            if id:
                return_value += ' '
                return_value += id
            
        else:
            SoCoLogger.warning('Warning CInPlaceTypeDescriptor.get_qualified_cast_or_dcl: uncovered description')
            
        return return_value
    
    
     
      
                                    
class CFunctionSignatureTypeDescriptor(CTypeDescriptor):
    
    """ describes a function signature (unnamed type) in the C programming language; is used for describing functions and functions pointers """
                                
    def __init__(self): 
        CTypeDescriptor.__init__(self)
        self.ax_info = ''                  # debugging information for reading from Axivion 
        self.return_declaration = SourceCodeDescriptor.Declaration()
        self.parameter_declarations = []
        # the next 2 members are replaced by return_declaration resp. parameter_declarations
        #self.return_declaration.type = None # any C type; instance of a class derived from CTypeDescriptor         
        #self.parameter_descriptors = []    # list of tupels that describes the parameters; each tupel consist of parameter name (string) and type (instance of a class derived from CTypeDescriptor)
        self.all_types_resolved = None
        
    def getIdentifierTypeList(self):
        id_type_list = list()
        for prm_dcl in self.parameter_declarations:
            id_type_list.extend(prm_dcl.getIdentifierTypeList())
          
        return id_type_list, self.return_declaration.getIdentifierTypeList()
        
    def getIDIndexList(self):
        id_index_list = list()
        for prm_dcl in self.parameter_declarations:
            id_index_list.append(prm_dcl.getIDIndexListRecursive())
          
        return id_index_list, [self.return_declaration.getIDIndexListRecursive(start_id = "return_type")]        
            
    
    def readout(self):
        
        print("-->CFunctionSignatureTypeDescriptor")
        
        #print " ->modifier: " + str(self.is_const ) + " " + str(self.is_volatile)
        print(" ->not readable: " + str(self.not_readable))
        print(" ->size in bytes: " + str(self.size_in_bytes))          # 0: no value detmerined yet, -1: not applicable (e.g. void, a function etc,); > 0:nomen est omen; CFunctionsSignarueCType: can only be read from symbol file i.e. 0 if only pstd file read
        print(" ->signature_2008: " + str(self.tms_signature_code_2008))     # the so called signature code for working with the TCL  TestEngine
        print(" ->signature_2016: " + str (self.tms_signature_code_2016))    # the so called signature code for working with the Python TestEngine

        print(" ->return_declaration: " + str(self.return_declaration))
        print(" ->parameter_declarations: " + str(self.parameter_declarations))
        print(" ->all_types_resolved: " + str(self.all_types_resolved))     
                            
          
    @classmethod                 
    def CreateFrom_AxivionType(cls, prm_soco, AxivionFunctionDefinition, identifier):
       helper = cls()
       helper.SetFrom_From_AxivionType(prm_soco, AxivionFunctionDefinition, identifier)
       return helper
   
                   
    def SetFrom_From_AxivionType(self, prm_soco, AxivionFunctionDefinition, identifer):
        self.ax_info =  AxivionFunctionDefinition.type() + ' ' + str(AxivionFunctionDefinition.number())                   
        
        #if not prm_soco.set_typedescriptor_to_declaration_from_typedef_id_with_Axivion_type(AxivionFunctionDefinition.Return_Type, self.return_declaration): 
        self.return_declaration.type = CTypeDescriptor.Try_CreateTypedefReference(AxivionFunctionDefinition.Return_Type)
        if not self.return_declaration.type:               
            self.return_declaration.type =  CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionFunctionDefinition.Return_Type, identifer + '_return' )  
        self.return_declaration.set_qualifiers_from_Axivion(AxivionFunctionDefinition.Return_Type)
            
        if self.return_declaration.type.not_readable:
            self.not_readable = True 
           
          
        self.parameter_declarations = []   
        for param in AxivionFunctionDefinition.Parameters:  
            
            declaration = SourceCodeDescriptor.Declaration()
            declaration.identifier = param.Name
            
            declaration.type =  CTypeDescriptor.Try_CreateTypedefReference(param.Its_Type)    
            if not declaration.type:     
                declaration.type =  CTypeDescriptor.CreateFrom_AxivionType(prm_soco, param.Its_Type, identifer + '_' + param.Name)    
                
            #self.parameter_descriptors.append((param.Name, param_type))   
            declaration.set_qualifiers_from_Axivion(param.Its_Type)
            self.parameter_declarations.append(declaration)
            
			#was maintained to show behavior in axivion
            #if declaration.identifier in ["c_pt_c", "pt_c", "c_pt", "c_pt2", "c_pt3"]:
                #declaration.readout()
                #print(str(AxivionFunctionDefinition.number()))
            
            if declaration.type.not_readable:
                self.not_readable = True  
          
        if PRE_SIZE_BUILD:                   
            self.build_size(prm_soco.type_system, True)   
   
    @classmethod                 
    def CreateFrom_PSTD(cls, prm_soco, xml_dom_function_node):
        helper = cls()
        helper.SetFrom_PSTD(prm_soco, xml_dom_function_node)
        return helper 
    
    def SetFrom_PSTD(self, prm_soco, xml_dom_function_node):
        if READ_WRITE_TMSSC:
            self.tms_signature_code_2008 =  DomHelper.GetOptionalAttribute(xml_dom_function_node,'tms_signature_code_2008')
            self.tms_signature_code_2016 =  DomHelper.GetOptionalAttribute(xml_dom_function_node,'tms_signature_code_2016')
            #NiEh: The TengMs-Server does not know BPTR1. Workaround for problems with old signatures
            self.tms_signature_code_2008 = self.tms_signature_code_2008.replace("BPTR1", "PTR1")
            
        for child in xml_dom_function_node.childNodes:
            if child.nodeName == 'return_type':   
                element_child = DomHelper.GetFirstElementChild(child)                                   
                self.return_declaration.type  = CTypeDescriptor.CreateFrom_PSTD(prm_soco, element_child)   
                self.return_declaration.set_qualifiers_from_PSTD(child)    

            elif child.nodeName == 'param':  
                param_name = DomHelper.GetOptionalAttribute(child,'identifier')               
                # todo automated generation of test cases: pares optional attrubtes inout, min_value, max_value from Doxygen <- clarify Doxygen tagging  <- clarify if PythonTEng is running and we are starting to discuss automatically generated test cases 
                element_child = DomHelper.GetFirstElementChild(child)     
                param_type =  CTypeDescriptor.CreateFrom_PSTD(prm_soco, element_child)        
                self.parameter_declarations.append(SourceCodeDescriptor.Declaration(param_name, param_type))
                self.parameter_declarations[-1].set_qualifiers_from_PSTD(child)
    
        
     
    def GetPstdTags(self):
        result_tags = []
           
        return_tag = dom.Element("return_type")
        
        return_childtag = self.return_declaration.type.GetPstdTag()
        self.return_declaration.add_qualifier_to_tag(return_childtag)
        return_tag.appendChild(return_childtag)
        
        result_tags.append(return_tag)        
                      
        #for param, name in self.parameter_type_descriptions, self.parameter_names:
        for param in self.parameter_declarations: 
            param_tag = dom.Element('param')
            if  param.identifier:
                param_tag.setAttribute('identifier', param.identifier)
            param_childtag = param.type.GetPstdTag()  
            param.add_qualifier_to_tag(param_childtag)
            param_tag.appendChild(param_childtag)
            result_tags.append(param_tag)  
        

        return result_tags
                

    def get_source_code_return_type(self):
        if self.return_declaration.type:
            decl_string = ""
            qual = self.return_declaration.get_qualifier_string()
            return decl_string + self.return_declaration.type.get_qualified_cast_or_dcl(id='', quals=qual)    #a function has  declaration for return type
        else:
             return 'void'
        
    def get_source_code_param_decls(self):   
        if len(self.parameter_declarations) > 0:
            decl_string = ""
            for param in self.parameter_declarations:
                qual = param.get_qualifier_string()
                decl_string += param.type.get_qualified_cast_or_dcl(id=param.identifier, quals=qual)
                decl_string += ", " 
                
            return decl_string[:-2]
        
        else:
            return 'void'
     
    def resolve_type_selections(self, typedef_container_list):
        
        self.all_types_resolved = True
        
        self.return_declaration.resolve_type_selections(typedef_container_list)
        
        for param in self.parameter_declarations:
            param.resolve_type_selections(typedef_container_list)
            if param.resolved_type == None:
                self.all_types_resolved = False
        return    


    def clear_size_and_signature(self):       
        self.size_in_bytes = 0
        self.tms_signature_code_2008 = ''
        self.tms_signature_code_2016 = ''
        if self.return_declaration.type:
            self.return_declaration.type.clear_size_and_signature()   
        if self.parameter_declarations:
            for param in self.parameter_declarations:
                param.type.clear_size_and_signature()         
               
 
    def build_size(self, type_system, only_try=False):
        
#         if my_unit:
#             local_typedef_container = my_unit.typedef_container
#         else:
#             local_typedef_container = None
              
        if self.size_in_bytes == 0: 
            
            if self.not_readable:
                self.size_in_bytes = -1
                
            else:
                ready_with_size = True
                if self.return_declaration.type:
                    self.return_declaration.type.build_size(type_system, only_try)
                    
                    if self.return_declaration.resolved_type:
                        self.return_declaration.resolved_type.build_size(type_system, only_try)
                    
                    if self.return_declaration.type.size_in_bytes == 0:
                       ready_with_size = False
                if self.parameter_declarations:
                    for param in self.parameter_declarations:
                        param.type.build_size(type_system, only_try)  
                        if param.type.size_in_bytes == 0:
                            ready_with_size = False
                            
                        if param.resolved_type:
                            param.resolved_type.build_size(type_system, only_try)
                            
                if only_try==False:
                    if  (self.return_declaration.type.size_in_bytes == 0):
                        SoCoLogger.error ('Error CFunctionSignatureTypeDescriptor.build_size: size of return type not determinable despite only_try==False.')
                
                    for param in self.parameter_declarations:
                        if (only_try == False) and (param.type.size_in_bytes == 0):
                            SoCoLogger.error ('Error CFunctionSignatureTypeDescriptor.build_size: size of param ' + str(param.identifier) + ' not determinable despite only_try==False.')
                               
                       
                if ready_with_size:
                    self.size_in_bytes = -1
                
                           
    def build_tms_signature_code(self, prm_soco): 
        
        if self.not_readable:
            self.tms_signature_code_2008 = 'INVALID'
            self.tms_signature_code_2016 = 'INVALID'
            return self.tms_signature_code_2008
        
        if self.return_declaration.resolved_type:
            self.return_declaration.resolved_type.build_tms_signature_code(prm_soco) 
            self.return_declaration.build_tms_signature_code(prm_soco)
             
        if self.parameter_declarations:    
            for param in self.parameter_declarations:
                param.resolved_type.build_tms_signature_code(prm_soco) 
                param.build_tms_signature_code(prm_soco)    
        
        if self.tms_signature_code_2008 == '':             
            if self.return_declaration.resolved_type: 
				# NiEh: work around for problem with old signature
                if self.return_declaration.resolved_type.tms_signature_code_2008 == "BPTR1":
                    self.tms_signature_code_2008 =  'PTR1'   
                else:
              	    self.tms_signature_code_2008 = self.return_declaration.tms_signature_code_2008     
                                  
            
            if self.parameter_declarations:
                self.tms_signature_code_2008 +=  '_'       
                for param in self.parameter_declarations:   
					# NiEh: work around for problem with old signature
                    if  param.resolved_type.tms_signature_code_2008 == "BPTR1":
                        self.tms_signature_code_2008 += "PTR1" 
                    else:
                        self.tms_signature_code_2008 += param.tms_signature_code_2008                                     
            else:
                self.tms_signature_code_2008 += '_V'                  
                 
        
        if self.tms_signature_code_2016 == '':       
                       
            self.tms_signature_code_2016 = 'FB_'   
            
            if self.return_declaration.resolved_type: 
                self.tms_signature_code_2016 += self.return_declaration.tms_signature_code_2016 + '_'
            else:
                self.tms_signature_code_2016 += 'V_'
                                                          
            if self.parameter_declarations:       
                for param in self.parameter_declarations:
                    self.tms_signature_code_2016 += param.tms_signature_code_2016
                    self.tms_signature_code_2016 += '_'                                          
            else:
                self.tms_signature_code_2016 += 'V_'

            self.tms_signature_code_2016 += 'FE'
                    
        return self.tms_signature_code_2008  # to NiEh: reutrn 2008, 2016 tupel for all or remove if not used

 
class CCompositeTypeDescriptor(CTypeDescriptor):
    
    """ describes the struct and union (named and unnamed) type in the C programming language """
                                
    def __init__(self): 
        CTypeDescriptor.__init__(self)
        self.ax_info = ''           # debugging information for reading from Axivion 
        self.ax_size = 0
        self.kind = ''              # struct or union
        self.tag_identifier = ''    # tag identifier - may be empty     
        self.member_descriptions = []
        self.all_types_resolved = None
        self.is_bitfield = False     # indicator if struct or union is a bitfield - in this case the name_type tuple (see below) may have a third tuple element (see below)
        # the next 4 members are replaced by member_descriptions
        #self.name_type_tuples = []  # list of tuples, one for each member consisting of name and value and type; in case of a bit field there may be third tuple element that contains the size in bits (members without this third bit size element have the size of their type)
        #self.member_offsets = []    # if bit field: in bits; else: in byte
        #self.symbol_locations = []  # describes memory allocation and alignment (instance of class MemberSymbolLocationType); needed for work with TestEngine    
        #self.resolved_types = None

    def getCompositeMembers(self):
        return self.member_descriptions
        
        
    def formIDTypeListRecursive(self, last_identifier):
        id_type_list = list()
        for member in self.member_descriptions:
            id_type_list.extend(member.getIdentifierTypeList(last_identifier))
                    
        return id_type_list
        
    def formIDIndexListRecursive(self):
        id_index_list = list()
        for member in self.member_descriptions:
            id_index_list.append(member.getIDIndexListRecursive())
                    
        return id_index_list        
        
            
    def get_qualified_cast_or_dcl(self, id = "", quals= ""):
        if not (quals == ""):
            return quals + " " + self.kind + " " + self.tag_identifier
        else:
            return self.kind + " " + self.tag_identifier
    
    def readout(self):
        
        print("-->CCompositeTypeDescriptor")
        
        #print " ->modifier: " + str(self.is_const ) + " " + str(self.is_volatile)
        print(" ->not readable: " + str(self.not_readable))
        print(" ->size in bytes: " + str(self.size_in_bytes))          # 0: no value detmerined yet, -1: not applicable (e.g. void, a function etc,); > 0:nomen est omen; CFunctionsSignarueCType: can only be read from symbol file i.e. 0 if only pstd file read
        print(" ->signature_2008: " + str(self.tms_signature_code_2008))     # the so called signature code for working with the TCL  TestEngine
        print(" ->signature_2016: " + str (self.tms_signature_code_2016))    # the so called signature code for working with the Python TestEngine

        print(" ->kind: " + str(self.kind))
        print(" ->tag_identifier: " + str(self.tag_identifier))
        print(" ->is_bitfield: " + str(self.is_bitfield))
        print(" ->member_descriptions: " +str (self.member_descriptions))
        for member in self.member_descriptions:
            member.readout_member()
        print(" ->all_types_resolved: " + str(self.all_types_resolved)) 
                                            
    @classmethod                 
    def CreateFrom_AxivionType(cls, prm_soco, AxivionCompositeTypeDefinition, identifier):
       helper = cls()
       helper.SetFrom_From_AxivionType(prm_soco, AxivionCompositeTypeDefinition, identifier)
       return helper
                     
    def SetFrom_From_AxivionType(self, prm_soco, AxivionCompositeTypeDefinition, identifier):
        self.ax_info = AxivionCompositeTypeDefinition.type() + ' ' + str(AxivionCompositeTypeDefinition.number())
                     
        if AxivionCompositeTypeDefinition.is_of_type('Named_Type_Definition'):
            self.tag_identifier = AxivionCompositeTypeDefinition.Logical.Name
                
        if AxivionCompositeTypeDefinition.is_of_type('Record_Type_Definition') or AxivionCompositeTypeDefinition.is_of_type('Unnamed_Record_Type_Definition'):
            if AxivionCompositeTypeDefinition.is_of_type('Normal_Struct_Type_Definition') or  AxivionCompositeTypeDefinition.is_of_type('Unnamed_Struct_Type_Definition'): 
                self.kind = 'struct'           
                #for feld in AxivionCompositeTypeDefinition.descendants_of_type('Field_Definition'):  
                for child in AxivionCompositeTypeDefinition.Children: 
                    if child.is_of_type('Field_Definition'):
                        name = child.Logical.Name
                        type =  CTypeDescriptor.Try_CreateTypedefReference(child.Its_Type)    
                        if not type:
                            type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, child.Its_Type, str(identifier) + '_' + child.Logical.Name)  #ToDo NiEh an identifier of None is problematic!? As far as I can see, its not used.
                        if child.is_of_type('Normal_Field_Definition'):
                            self.member_descriptions.append(SourceCodeDescriptor.CompMemberDefinition(name,type))   
                            self.member_descriptions[-1].set_qualifiers_from_Axivion(child.Its_Type) 
                            if(child.Its_Type.is_of_type('Unqualified_Direct_Type_Selection')):
                                if(child.Its_Type.Logical.is_of_type('Typedef_Type')):
                                    self.member_descriptions[-1].set_qualifiers_from_Axivion(child.Its_Type.Symbol_Declaration.Original_Type)
                        elif child.is_of_type('Bit_Field_Definition'):  
                            self.is_bitfield = True                     
                            self.member_descriptions.append(SourceCodeDescriptor.CompMemberDefinition(name,type,child.Size_In_Bits.Value)) 
                            self.member_descriptions[-1].set_qualifiers_from_Axivion(child.Its_Type) 
                        else:
                            SoCoLogger.error("Error CCompositeTypeDescriptor.SetFrom_From_AxivionType: unexpectedd child type " + str(child.type()) + " in (Unndmaed_)Struct_Type_Definition " + str( AxivionCompositeTypeDefinition.number()))
                            self.not_readable = True
                        
                        if type:
                            if type.not_readable:
                                self.not_readable = True 
                                
                        
                         
                    elif child.is_of_type('Multi_Declaration'):
                        for grandchild in child.Children:
                            name = grandchild.Logical.Name
                            if grandchild.Its_Type.is_of_type('Common_Unnamed_Type_Selection'):
                                type =  CTypeDescriptor.Try_CreateTypedefReference(grandchild.Its_Type.Target)    
                                if not type: 
                                    type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, grandchild.Its_Type.Target, identifier + '_' + grandchild.Logical.Name)
                            else:
                                type =  CTypeDescriptor.Try_CreateTypedefReference(grandchild.Its_Type)    
                                if not type:
                                    type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, grandchild.Its_Type, identifier + '_' + grandchild.Logical.Name)
                            
                            if grandchild.is_of_type('Normal_Field_Definition'):
                                self.member_descriptions.append(SourceCodeDescriptor.CompMemberDefinition(name,type))  
                                self.member_descriptions[-1].set_qualifiers_from_Axivion(grandchild.Its_Type) 
                            elif grandchild.is_of_type('Bit_Field_Definition'):  
                                self.is_bitfield = True                        
                                self.member_descriptions.append(SourceCodeDescriptor.CompMemberDefinition(name,type,grandchild.Size_In_Bits.Value))
                                self.member_descriptions[-1].set_qualifiers_from_Axivion(grandchild.Its_Type) 
                            else:
                                SoCoLogger.error("Error in CCompositeTypeDescriptor.SetFrom_From_AxivionType: unexpectedd field type " + str(grandchild.type()) + " in Multi_Declaration " + str(child.number()))
                                self.not_readable = True
                            
                            if type:
                                if type.not_readable:
                                    self.not_readable = True 
                                    
                    
                                
                    else:
                        SoCoLogger.error("Error CCompositeTypeDescriptor.SetFrom_From_AxivionType: unexpected child type " + str(child.type()) + " in (Unndmaed_)Struct_Type_Definition " + str(AxivionCompositeTypeDefinition.number()))
                        self.not_readable = True
                                                                                                
            elif AxivionCompositeTypeDefinition.is_of_type('Final_Struct_Type_Definition'):
                SoCoLogger.error("Error in CCompositeTypeDescriptor.SetFrom_From_AxivionType: unexpectedd C++ type Final_Struct_Type_Definition")
                self.not_readable = True
                    
            elif AxivionCompositeTypeDefinition.is_of_type('Class_Type_Definition'):
                SoCoLogger.error("Error in CCompositeTypeDescriptor.SetFrom_From_AxivionType: unexpectedd C++ type Class_Type_Definition")
                self.not_readable = True
                    
            else:
                SoCoLogger.error("Error in CCompositeTypeDescriptor.SetFrom_From_AxivionType: unknown type " + str(AxivionCompositeTypeDefinition.type()) + " that inherits from (Unnamed_)Record_Type_Definition " + str( AxivionCompositeTypeDefinition.number()))
                self.not_readable = True
                            
        elif AxivionCompositeTypeDefinition.is_of_type('Union_Type_Definition') or  AxivionCompositeTypeDefinition.is_of_type('Unnamed_Union_Type_Definition'):
            self.kind = 'union' 
            #for feld in AxivionCompositeTypeDefinition.descendants_of_type('Field_Definition'):  
            for child in AxivionCompositeTypeDefinition.Children: 
                if child.is_of_type('Field_Definition'):
                    name = child.Logical.Name
                    type =  CTypeDescriptor.Try_CreateTypedefReference(child.Its_Type)    
                    if not type: 
                        type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, child.Its_Type, str(identifier) + '_' + child.Logical.Name)
                    if child.is_of_type('Normal_Field_Definition'):
                        self.member_descriptions.append(SourceCodeDescriptor.CompMemberDefinition(name,type))   
                        self.member_descriptions[-1].set_qualifiers_from_Axivion(child.Its_Type)
                        if(child.Its_Type.is_of_type('Unqualified_Direct_Type_Selection')):
                            if(child.Its_Type.Logical.is_of_type('Typedef_Type')):
                                self.member_descriptions[-1].set_qualifiers_from_Axivion(child.Its_Type.Symbol_Declaration.Original_Type)
                    elif child.is_of_type('Bit_Field_Definition'):         
                        self.is_bitfield = True                 
                        self.member_descriptions.append(SourceCodeDescriptor.CompMemberDefinition(name,type,child.Size_In_Bits.Value)) 
                        self.member_descriptions[-1].set_qualifiers_from_Axivion(child.Its_Type)
                    else:
                        SoCoLogger.error("Error in CCompositeTypeDescriptor.SetFrom_From_AxivionType: unexpectedd child type " + str(child.type()) + " in (Unndmaed_)Union_Type_Definition " + str(AxivionCompositeTypeDefinition.number()))
                        self.not_readable = True
                    
                    if type:
                        if type.not_readable:
                            self.not_readable = True  

                
                elif child.is_of_type('Multi_Declaration'):
                    for grandchild in child.Children:
                        name = grandchild.Logical.Name
                        if grandchild.Its_Type.is_of_type('Common_Unnamed_Type_Selection'):
                            type =  CTypeDescriptor.Try_CreateTypedefReference(grandchild.Its_Type.Target)    
                            if not type: 
                                type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, grandchild.Its_Type.Target, identifier + '_' + grandchild.Logical.Name)
                        else:
                            type =  CTypeDescriptor.Try_CreateTypedefReference(grandchild.Its_Type)    
                            if not type:
                                type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, grandchild.Its_Type, identifier + '_' + grandchild.Logical.Name)
                        
                        if grandchild.is_of_type('Normal_Field_Definition'):
                            self.member_descriptions.append(SourceCodeDescriptor.CompMemberDefinition(name,type)) 
                            self.member_descriptions[-1].set_qualifiers_from_Axivion(grandchild.Its_Type)  
                        elif grandchild.is_of_type('Bit_Field_Definition'):   
                            self.is_bitfield = True                       
                            self.member_descriptions.append(SourceCodeDescriptor.CompMemberDefinition(name,type,grandchild.Size_In_Bits.Value)) 
                            self.member_descriptions[-1].set_qualifiers_from_Axivion(grandchild.Its_Type)
                        else:
                            SoCoLogger.error("Error CCompositeTypeDescriptor.SetFrom_From_AxivionType: unexpected field type " + str(grandchild.type()) + " in Multi_Declaration " + str(child.number()))
                            self.not_readable = True
                        
                        if type:
                            if type.not_readable:
                                self.not_readable = True  

                
                else:
                    SoCoLogger.error("Error CCompositeTypeDescriptor.SetFrom_From_AxivionType: unexpectedd child type " + str(child.type()) + " in (Unnamed_)Union_Type_Definition " + str(AxivionCompositeTypeDefinition.number()))
                    self.not_readable = True               
                           
        else:
            SoCoLogger.error('Error in CCompositeTypeDescriptor.SetFrom_From_AxivionType: unknown type ' + str(AxivionCompositeTypeDefinition.type()) + ' ' + str(AxivionCompositeTypeDefinition.number()))
            self.not_readable = True
        
        self.ax_size = AxivionCompositeTypeDefinition.Logical.Size 
        
        #self.size_in_bytes = ax_size 
        
        
        #consideration of alignment missing in type system
         
#         self.build_size(type_system, None, None) 
#                 
#         if (CHECK_AXIVION_AGAINST_TYPE_SYSTEM == True) and (self.size_in_bytes != 0):            
#                 if self.size_in_bytes != ax_size:
#                     print('Warning in CCompositeTypeDescriptor.SetFrom_AxivionType: missmach between size from type_system (', self.size_in_bytes, ') and AxivionType.Logical.Size (', ax_size, ') for ', identifier) 
#                     self.size_in_bytes = ax_size
    @classmethod
    def GetFrom_Axivion_Identifier(self, AxivionCompositeTypeDefinition):
        if AxivionCompositeTypeDefinition.is_of_type('Named_Type_Definition'):
            return AxivionCompositeTypeDefinition.Logical.Name
        else:
            return None
    
    @classmethod                 
    def CreateFrom_PSTD(cls, prm_soco, xml_dom_basic_type_node):
        helper = cls()
        helper.SetFrom_PSTD(prm_soco, xml_dom_basic_type_node)
        return helper
    
    def SetFrom_PSTD(self, prm_soco, xml_dom_type_node):       
        
        if xml_dom_type_node.nodeName == 'structure':
            self.kind = 'struct'
        elif xml_dom_type_node.nodeName == 'union': 
            self.kind = 'union'
        else:
            SoCoLogger.error('Error CCompositeTypeDescriptor.SetFrom_PSTD: unknown composite type')  
            
        self.tag_identifier = DomHelper.GetOptionalAttribute(xml_dom_type_node, 'identifier', '')
        self.size_in_bytes = int(xml_dom_type_node.getAttribute('size_in_bytes'))   
        
        if READ_WRITE_TMSSC:
            self.tms_signature_code_2008 = DomHelper.GetOptionalAttribute(xml_dom_type_node,'tms_signature_code_2008')
            self.tms_signature_code_2016 = DomHelper.GetOptionalAttribute(xml_dom_type_node,'tms_signature_code_2016')
            
        for child in xml_dom_type_node.childNodes:   
            if child.__class__ !=  dom.Element:
                continue
            
            if child.nodeName != 'member':
                continue
            
            identifer = DomHelper.GetOptionalAttribute(child,'identifier','')
            size = DomHelper.GetOptionalAttribute(child,'size_in_bits','')
            if size:
                self.is_bitfield = True
            if self.kind == 'struct':  
                offset = DomHelper.GetOptionalAttribute(child,'offset_in_bytes','')
                if not offset:
                    offset = DomHelper.GetOptionalAttribute(child,'offset_in_bits','')                   

            type = CTypeDescriptor.CreateFrom_PSTD(prm_soco, DomHelper.GetFirstElementChild(child))
            
            if size:
                member = SourceCodeDescriptor.CompMemberDefinition(identifer, type , size)                
            else:
                member = SourceCodeDescriptor.CompMemberDefinition(identifer, type)
                
                
            self.member_descriptions.append(member)
            
            self.member_descriptions[-1].set_qualifiers_from_PSTD(child)
            
            if self.kind == 'struct':      
                if offset:
                    member.offset = int(offset)
                else:                
                    member.offset = -1
    
    @classmethod
    def GetFrom_PSTD_Identifier(self, xml_dom_type_node):
        return DomHelper.GetOptionalAttribute(xml_dom_type_node, 'identifier', '')
    
   
    def GetPstdTag(self, tag_self=None): 
                               
        if self.kind == 'struct':  
            result_tag = dom.Element('structure')
        elif self.kind == 'union':  
            result_tag = dom.Element('union')
        else:
            SoCoLogger.error('Error: unknown composite kind')
            
        if self.tag_identifier:
            result_tag.setAttribute('identifier', self.tag_identifier) 
          
        result_tag.setAttribute('size_in_bytes', str(self.size_in_bytes)) 
            
        if WRITE_AX_INFO:  
            result_tag.setAttribute('ax_info', self.ax_info) 
            
        if READ_WRITE_TMSSC:
            result_tag.setAttribute('tms_signature_code_2008',self.tms_signature_code_2008)
            result_tag.setAttribute('tms_signature_code_2016',self.tms_signature_code_2016)
                    
        #for member in self.name_type_tuples:
        for index in range(0,len(self.member_descriptions)):
            member=self.member_descriptions[index]
            tag_member = dom.Element('member') 
            if member.identifier:
                tag_member.setAttribute('identifier', member.identifier) 
            if WITH_WORK_PROGESS_PSTD:
                if self.is_bitfield:
                    if member.size_in_bits: # also in struct with with bitfields some of some may not have a bitsize which means they have the regular size type
                        tag_member.setAttribute('size_in_bits', str(member.size_in_bits)) 
                    if (self.kind == 'struct'):    
                        tag_member.setAttribute('offset_in_bits', str(self.member_descriptions[index].offset))  
                else:
                    if (self.kind == 'struct'):   
                        tag_member.setAttribute('offset_in_bytes', str(self.member_descriptions[index].offset)) 
            else:    
                pass
            
            member_childtag = member.type.GetPstdTag()
            member.add_qualifier_to_tag(member_childtag) 
            #member.add_signature_to_tag(member_childtag)       
            tag_member.appendChild(member_childtag)                
            result_tag.appendChild(tag_member)                
        
        return result_tag 
        
    def resolve_type_selections(self, typedef_container_list):
        #if not self.resolved:
            #self.resolved = True
            #self.resolved_types = []
            self.all_types_resolved = True
            for member in self.member_descriptions:
                member.type.resolve_type_selections(typedef_container_list)
                if member.resolved_type == None:
                    self.all_types_resolved = False
                
            
        
    def clear_size_and_signature(self):   
        self.size_in_bytes = 0
        self.tms_signature_code_2008 = ''
        
        for member in self.member_descriptions:
            member.type.clear_size_and_signature()
                            
    def build_size(self, type_system, only_try=False):
        
        # to do: struct enthaelt pointer aus sich selbst - darf nicht zur endlos schleife fuehren
        
        
        if self.size_in_bytes == 0:
            
            
            if self.not_readable:
                self.size_in_bytes = -1
            
            else:
            
                ready_with_size = True
                    
                for member in self.member_descriptions:
                    member.type.build_size(type_system, only_try)
                    if  member.type.size_in_bytes == 0: # i.e. not calculatable at this point of time
                        ready_with_size = False 
                                                                 
                if ready_with_size:   
                    # for member in self.name_type_tuples:
                    self.size_in_bytes = 0
                    for index in range(0,len(self.member_descriptions)):
                        member = self.member_descriptions[index]
                        if member.type.size_in_bytes < 0: # i.e. not calculatable 
                            self.size_in_bytes = -1
                            break
                        else:
                            self.size_in_bytes = self.size_in_bytes +  member.type.size_in_bytes
                    
                            
                    if self.size_in_bytes != -1:
                        if self.is_bitfield or (self.kind == 'union'):
                            # sum of member sizes is not struct resp. union size
                            self.size_in_bytes = self.ax_size 
                        else:
                            if self.size_in_bytes != self.ax_size:
                                # 2016-08-30 UlBr: yes, that happens - seems to be rounded up to multiple of rounded natural int - correct for an unpacked struct or union
                                #print ('Error in CCompositeTypeDescriptor.build_size: ax_size', self.ax_size, ' not equal to sum of member sizes: ', self.size_in_bytes, ' (correctd with ax_size).')
                                self.size_in_bytes = self.ax_size 
                        
                        if not (self.kind == 'union'):
                            sum = 0
                            if self.is_bitfield:
                                for index in range(0,len(self.member_descriptions)):
                                    member = self.member_descriptions[index]
                                    self.member_descriptions[index].offset = sum
                                    if member.size_in_bits:
                                        sum = sum + member.size_in_bits
                                    else:
                                        sum = sum + member.type.size_in_bytes*8
                            else:
                                for index in range(0,len(self.member_descriptions)):
                                    member = self.member_descriptions[index]
                                    self.member_descriptions[index].offset = sum # todo Axivion: information available ? - otherwise should be rounded up to multiple of rounded natural int (for unpacked struct or union)
                                    sum = sum + member.type.size_in_bytes
                       
                            
                    if (only_try == False) and (self.size_in_bytes == 0):
                        SoCoLogger.error('Error CCompositeTypeDescriptor.build_size: for ' + str(self.tag_identifier) + ': size not determinable despite only_try == False.')
                                      
    
    def build_tms_signature_code(self, prm_soco):
        
        if self.not_readable:
            self.tms_signature_code_2008 = 'INVALID'
            self.tms_signature_code_2016 = 'INVALID'
            return self.tms_signature_code_2016
                
        if self.tms_signature_code_2008 == '':            
            self.tms_signature_code_2008 = 'BS'   
            self.tms_signature_code_2008 += str(self.size_in_bytes) 
            
        if self.tms_signature_code_2016 == '':            
            self.tms_signature_code_2016 = 'S'   
            self.tms_signature_code_2016 += str(self.size_in_bytes * 8) 
        
        return self.tms_signature_code_2016 
    
    
    def build_tms_signature_code_for_member(self, prm_soco):
        for member in self.member_descriptions:
            member.type.build_tms_signature_code(prm_soco) 
            #member.resolved_type.build_tms_signature_code()
            member.build_tms_signature_code(prm_soco)
            #if isinstance(member.type, CInPlaceTypeDescriptor):
                #member.type.included_type.build_size()              
    
    #ToDo NiEh Review 11.10: What happens for arrays of structs for params?  
    def create_memberstring_list(self, prm_ccomposite=None, prm_current_member = "",  prm_unions_members = []):
        
        if prm_ccomposite:
            current_typedesc = prm_ccomposite
        else:
            current_typedesc = self
        
        if (prm_current_member == ""):    
            _struct_member = current_typedesc.tag_identifier
        else:
            _struct_member = prm_current_member
            
        if isinstance(current_typedesc, CCompositeTypeDescriptor):
            if current_typedesc.kind == "union" and not len(prm_unions_members): 
                union_member_id = current_typedesc.get_biggest_union_member_dscr().identifier
                union_memberlist = [union_member_id]
            else:
                union_memberlist = prm_unions_members
            
        prm_member_list = []  
               
        for member in current_typedesc.member_descriptions:  
            
            member_desc = member
            member_desc_type = member.type
            member_name = member.identifier
            
            if member.is_qualified_const():
                continue
            
            current_struct_member = _struct_member + ".{member}".format(member = member_name)
            
            if isinstance(current_typedesc, CCompositeTypeDescriptor):
                if current_typedesc.kind == "union":
                    if not member_name == union_memberlist[0]:
                        continue
            
            if isinstance(member_desc_type, CTypeSelectionDescriptor):
                if (not member_desc_type.resolved_type == None) or (not member_desc_type.type_definition_kind == ""):
                    while isinstance(member_desc_type, CTypeSelectionDescriptor) and ((not member_desc_type.resolved_type == None) or (not member_desc_type.type_definition_kind == "")):
                        if (not member_desc_type.resolved_type == None):
                            member_desc_type = member_desc_type.resolved_type
            
            if isinstance(member_desc_type, (CTypeSelectionDescriptor, CEnumTypeDescriptor)):           
                
                prm_member_list.append(current_struct_member)
            
            elif (isinstance(member_desc_type, CCompositeTypeDescriptor)):    

                prm_member_list.extend(member_desc_type.create_memberstring_list(prm_ccomposite = member_desc_type, prm_current_member = current_struct_member, prm_unions_members = [])) #At this point there is no interest of CTypeSelection and its resolved types
            
            elif (isinstance(member_desc_type, CInPlaceTypeDescriptor)):
                                
                call_list = []
                index_list = []
                #basic_typedescriptor = 
                inplace_orig_typedesc = SourceCodeDescriptor._search_original_typedescriptor(member_desc_type, call_list, index_list)
                
                if isinstance(inplace_orig_typedesc, CTypeSelectionDescriptor):
                    if (not inplace_orig_typedesc.resolved_type == None) or (not inplace_orig_typedesc.type_definition_kind == ""):
                        while isinstance(inplace_orig_typedesc, CTypeSelectionDescriptor) and ((not inplace_orig_typedesc.resolved_type == None) or (not inplace_orig_typedesc.type_definition_kind == "")):
                            if (not inplace_orig_typedesc.resolved_type == None):
                                inplace_orig_typedesc = inplace_orig_typedesc.resolved_type
                
                
                d = collections.Counter()         #here a dict is defined that will contain from
                for c in call_list:                 #now on: the number of array-dimensions d[1] or the grade of pointers d[0]
                    d[c] += 1 
                                    
                                            
                if d[1] >= 1:  #one or more array-dimensions were counted  #recode
                    
                    if (d[1] == len(index_list)): 
                              
                        for k in range(0, index_list[0]):
                            if len(index_list) >= 2:
                                for j in range(0, index_list[1]):                
                                    if len(index_list) >= 3:
                                        for l in range(0, index_list[2]):
                                            array_struct_member = current_struct_member+"[{index}][{index2}][{index3}]".format(index = int(k), index2 = int(j), index3 = int(l))
                                            if isinstance(inplace_orig_typedesc, CCompositeTypeDescriptor):
                                                prm_member_list.extend(inplace_orig_typedesc.create_memberstring_list(prm_ccomposite = inplace_orig_typedesc, prm_current_member = array_struct_member, prm_unions_members = union_memberlist))
                                            else:
                                                prm_member_list.append(array_struct_member)    
                                    else:
                                        array_struct_member = current_struct_member+"[{index}][{index2}]".format(index = int(k), index2 = int(j))
                                        if isinstance(inplace_orig_typedesc, CCompositeTypeDescriptor):
                                            prm_member_list.extend(inplace_orig_typedesc.create_memberstring_list(prm_ccomposite = inplace_orig_typedesc, prm_current_member = array_struct_member, prm_unions_members = union_memberlist))
                                        else:
                                            prm_member_list.append(array_struct_member)
                            else:
                                array_struct_member = current_struct_member+"[{index}]".format(index = int(k))
                                if isinstance(inplace_orig_typedesc, CCompositeTypeDescriptor):
                                    prm_member_list.extend(inplace_orig_typedesc.create_memberstring_list(prm_ccomposite = inplace_orig_typedesc, prm_current_member = array_struct_member, prm_unions_members = union_memberlist))
                                else:
                                    prm_member_list.append(array_struct_member)
                                    
                         
                            
                    else:
                        EngineLogger.error("Array-sizes were counted unequal for {name}.".format(member_name))
                        return
                                
                elif d[1] == 0 and d[0] > 0:   #It is no array, we can simply append it
                    prm_member_list.append(current_struct_member)
                    
                else:
                    EngineLogger.error("Arrays of pointers can't be resolved yet.")
                 
            else:
                SocoLogger.error("Encountered unknown typedescriptor for {typedesc} and member {member_}, while creating member-list.".format(typedesc = str(current_typedesc), member_ = member_name))
        
        retval_member_list = prm_member_list 
                
        return retval_member_list
    
    def get_biggest_union_member_dscr(self, ret_value=False):
        
        biggest_member = self.member_descriptions[0]
        if self.kind == "union":
            for member in self.member_descriptions:
                if member.type.size_in_bytes > biggest_member.type.size_in_bytes:
                    biggest_member = member
        else:
            return ret_value
            
        
           
        return biggest_member
    
    def containsConstElements(self):
        
        for member_desc in self.member_descriptions:
            if member_description.is_qualified_const():
                return true
            
            if isinstance(member_desc.resolved_type, CCompositeTypeDescriptor):
                return member_desc.resolved_type.containsConstElements()
            
        return false
                
            
    def returnStubvarTypedef_noConstMembers(self, origin_Type, StepRecurion = 0):
        
        definition = ""
        
        if self.is_bitfield:
            definition += "\t"*StepRecursion +"typedef "+ "union" + " " + self.tag_identifier + "_accesBitfieldUnion {\n" 
            StepRecursion += 1
        
        definition += "\t"*StepRecursion +"typedef "+self.kind + " " + self.tag_identifier + "_noConstStubtype {\n"
        
        if (self.is_bitfield):
            
            for member_desc in self.member_descriptions:
                definition += "\t"*(StepRecursion+1) + member_desc.get_qualified_dcl_or_cast(id = member_desc.identifier) +"\t:"+str(member_desc.size_in_bits)
            
        else:
        
            for member_desc in self.member_descriptions:
                if not isinstance(member_desc.resolved_type, CCompositeTypeDescriptor):
                    definition += "\t"*(StepRecursion+1) + member_desc.get_stubvar_type(id = member_desc.identifier)
                else:
                    definition += member_desc.resolved_type.returnStubvarTypedef_noConstMembers(member_desc.type.get_qualified_cast_or_dcl(), StepRecurion+1)
            
        definition +="\t"*StepRecursion + "}"+origin_Type+"_noConstStubtype"+";"   #close typedef
        
        definiton += "\t"*StepRecursion + "uint"+self.size_in_bytes+"_t" + self.tag_identifier + "_accesBitfieldInteger;"
        
        if self.is_bitfield:
            definition +="\t"*(StepRecursion-1) + "}"+origin_Type+"_accesBitfieldUnion"+";"  #close union
        
        return definition   

        
class CEnumTypeDescriptor(CTypeDescriptor):
    
    """ describes the enum type (named and unnamed) in the C programming language """
                                
    def __init__(self): 
        CTypeDescriptor.__init__(self)
        self.ax_info = ''                   # debugging information for reading from Axivion  
        self.tag_identifier = ''            # tag identifier - may be empty
        self.name_value_tuples = []         # list of tuples, one for each enumerator consisting of name and value
        self.normalized_int_type = ''       # todo NiEh: replace by C99_int_type, ---> why? evaluate enum is enough for me
        self.C99_int_type = ''
        #self.size_in_bytes_standard = 0
        
    def get_qualified_cast_or_dcl(self, id = "", quals= ""):
        if not (quals == ""):
            return quals + " " + "enum" + " " + self.tag_identifier
        else:
            return "enum" + " " + self.tag_identifier
        
    def readout(self): 
                                             
        print("-->CEnumTypeDescriptor")
        
        #print " ->modifier: " + str(self.is_const ) + " " + str(self.is_volatile)
        print(" ->not readable: " + str(self.not_readable))
        print(" ->size in bytes: " + str(self.size_in_bytes))          # 0: no value detmerined yet, -1: not applicable (e.g. void, a function etc,); > 0:nomen est omen; CFunctionsSignarueCType: can only be read from symbol file i.e. 0 if only pstd file read
        print(" ->signature_2008: " + str(self.tms_signature_code_2008))     # the so called signature code for working with the TCL  TestEngine
        print(" ->signature_2016: " + str (self.tms_signature_code_2016))    # the so called signature code for working with the Python TestEngine

        print("->tag_identifier: " + str(self.tag_identifier))
        print("->name_value_tuples : " + str(self.name_value_tuples ))
        print("->normalized_int_type: " +str (self.normalized_int_type))
        print("->C99_int_type: " + str(self.C99_int_type)) 
                                                 
    @classmethod                 
    def CreateFrom_AxivionType(cls, prm_soco, AxivionEnumTypeDefinition, identifier):
       helper = cls()
       helper.SetFrom_From_AxivionType(prm_soco, AxivionEnumTypeDefinition, identifier)
       return helper
                       
    def SetFrom_From_AxivionType(self, prm_soco, AxivionEnumTypeDefinition, identifier):
        self.ax_info = AxivionEnumTypeDefinition.type() + ' ' + str(AxivionEnumTypeDefinition.number())  

        self.tag_identifier = AxivionEnumTypeDefinition.Logical.Name
        
        ax_size = AxivionEnumTypeDefinition.Logical.Size
                                     
        for enumerator in AxivionEnumTypeDefinition.Logical.Enumerators:
            if enumerator.Physical.type() == 'Uninitialized_Enumerator_Definition':
                self.name_value_tuples.append((enumerator.Name, enumerator.Value))
                self.add_to_enum_dict(prm_soco, enumerator.Name, enumerator.Value)
                
            else:
                if 'Initialization' in enumerator.Physical.fields(): # todo check for new version: revmove workaround for axivion problem with enum gernaration from macros - solved in 6.4.1? in 6.5?
                    token = enumerator.Physical.Initialization.SLoc.Token
                    token_str =  token.__str__() 
                    if token.__str__() == '-': # check for new version: revmove workaround for axivion problem with negative initalizers - solved in 6.4.1? in 6.5?
                        token = Conformizer.next_token(token) 
                        value = -1*int(token.__str__())
                        self.name_value_tuples.append((enumerator.Name, value))
                        self.add_to_enum_dict(prm_soco, enumerator.Name, value)
                    else:             
                        self.name_value_tuples.append((enumerator.Name, enumerator.Value))
                        self.add_to_enum_dict(prm_soco, enumerator.Name, enumerator.Value)
                else:
                    self.name_value_tuples.append((enumerator.Name, enumerator.Value))
                    self.add_to_enum_dict(prm_soco, enumerator.Name, enumerator.Value)
            
        
                
        # UlBr 2015-09-03:  Axivion takes natural int - usually inorrect for embbeded compiler      
                               
#         if CHECK_AXIVION_AGAINST_TYPE_SYSTEM:
#             self.build_size(type_system, None, None)  
#             if self.size_in_bytes != ax_size:
#                 # UlBr 2015-09-03: in Axivion natural int - can be  inorrect for embbeded compiler
#                 print('Warning in CEnumTypeDescriptor: missmach between size from type_system (', self.size_in_bytes, ') and AxivionType.Logical.Size (', AxivionEnumTypeDefinition.Logical.Size, ') for ', identifier) 
#             
#             self.size_in_bytes = 0
    @classmethod
    def GetFrom_Axivion_Identifier(self, AxivionEnumTypeDefinition):
        return AxivionEnumTypeDefinition.Logical.Name

    @classmethod                 
    def CreateFrom_PSTD(cls, prm_soco, xml_dom_basic_type_node):
        helper = cls()
        helper.SetFrom_PSTD(prm_soco, xml_dom_basic_type_node)
        return helper
    
    def SetFrom_PSTD(self, prm_soco, xml_dom_type_node):  
             
        self.tag_identifier = DomHelper.GetOptionalAttribute(xml_dom_type_node, 'identifier', '')
        
        self.size_in_bytes = int(xml_dom_type_node.getAttribute('size_in_bytes'))
        
        if READ_WRITE_TMSSC:
                self.tms_signature_code_2008 = DomHelper.GetOptionalAttribute(xml_dom_type_node,'tms_signature_code_2008')
                self.tms_signature_code_2016 = DomHelper.GetOptionalAttribute(xml_dom_type_node,'tms_signature_code_2016')
            
        for child in xml_dom_type_node.childNodes:   
            if child.__class__ !=  dom.Element:
                continue
            
            if child.nodeName != 'enumerator':
                continue
            
            self.name_value_tuples.append((child.getAttribute('identifier'), int(child.getAttribute('value'))))
            self.add_to_enum_dict(prm_soco, child.getAttribute('identifier'), int(child.getAttribute('value')))
    
    @classmethod        
    def GetFrom_PSTD_Identifier(self, xml_dom_type_node):
        return DomHelper.GetOptionalAttribute(xml_dom_type_node, 'identifier', '')
    
    def GetPstdTag(self, tag_self=None):   
           
        result_tag = dom.Element('enumeration')
        if self.tag_identifier:
            result_tag.setAttribute('identifier', self.tag_identifier)  
        result_tag.setAttribute('size_in_bytes', str(self.size_in_bytes))   
        if WRITE_AX_INFO:  
            result_tag.setAttribute('ax_info', self.ax_info)    
        if READ_WRITE_TMSSC:
            result_tag.setAttribute('tms_signature_code_2008',self.tms_signature_code_2008)  
            result_tag.setAttribute('tms_signature_code_2016',self.tms_signature_code_2016) 
             
        for tuple in  self.name_value_tuples:
            tag_enumerator = dom.Element('enumerator')
            tag_enumerator.setAttribute('identifier', tuple[0])  
            tag_enumerator.setAttribute('value', str(tuple[1]))                                    
            result_tag.appendChild(tag_enumerator)  
        
        return result_tag 
        
    def resolve_type_selections(self, typedef_container_list):
        # no included type that has to be resolved
        return 
     
    def clear_size_and_signature(self):              
        self.size_in_bytes = 0
        self.tms_signature_code_2008 = ''  
        self.tms_signature_code_2016 = '' 
    
    def build_size(self, type_system, only_try=False):
      
        if self.size_in_bytes == 0:
            
            if self.not_readable:
                self.size_in_bytes = -1
                
            else:
                min_enumerator_value = min([enumerator_tuple[1] for enumerator_tuple in self.name_value_tuples])
                #print ('type von min_enumerator_value: ', type(min_enumerator_value))
                if type_system.type_size_dict['enum'] == 'depends':
                    max_enumerator_value = max([enumerator_tuple[1] for enumerator_tuple in self.name_value_tuples])
                    #print ('type von min_enumerator_value: ', type(max_enumerator_value))
                    range = (max_enumerator_value - min_enumerator_value) + 1
                    
                    if range <= 256:
                        self.size_in_bytes = 1
                    elif range <= 65536:
                        self.size_in_bytes = 2                    
                    elif range <= 4294967296:
                        self.size_in_bytes = 4
                    else: 
                        self.size_in_bytes = 8                               
                else:
                    self.size_in_bytes = type_system.type_size_dict['enum']                          
                
                if min_enumerator_value < 0:
                    self.normalized_int_type = 'signed '
                else:
                    self.normalized_int_type = 'unsigned '
                    
                self.normalized_int_type = self.normalized_int_type + type_system.int_bytesize_to_type[self.size_in_bytes]
                
                if min_enumerator_value < 0:
                    self.C99_int_type = 'int'
                else:
                    self.C99_int_type = 'uint'
                self.C99_int_type += str(self.size_in_bytes*8)
                self.C99_int_type += '_t'    
            
            if self.size_in_bytes > 0 and not (self.C99_int_type or self.normalized_int_type):
                
                min_enumerator_value = min([enumerator_tuple[1] for enumerator_tuple in self.name_value_tuples])
                
                if min_enumerator_value < 0:
                    self.normalized_int_type = 'signed '
                else:
                    self.normalized_int_type = 'unsigned '
                
                self.normalized_int_type = self.normalized_int_type + type_system.int_bytesize_to_type[self.size_in_bytes]
                    
                if min_enumerator_value < 0:
                    self.C99_int_type = 'int'
                else:
                    self.C99_int_type = 'uint'
                self.C99_int_type += str(self.size_in_bytes*8) + '_t' 
                
            if (only_try == False) and (self.size_in_bytes == 0):
                SoCoLogger.warning ('Warning CEnumTypeDescriptor.build_size: enum ' + str(self.tag_identifier) + ': size not determinable despite only_try==False.')
        
    def build_tms_signature_code(self, prm_soco):
        
        if self.not_readable:
            self.tms_signature_code_2008 = 'INVALID'
            self.tms_signature_code_2016 = 'INVALID'
            return self.tms_signature_code_2008
        
        if self.tms_signature_code_2008 == '': 
            if self.normalized_int_type:         
                self.tms_signature_code_2008 = _NORMALIZED_C_TYPE_TO_TMS_SIGNATURE_2008[self.normalized_int_type]
            else: 
                SoCoLogger.warning ("Warning CEnumTypeDescriptor.build_tms_signature_code: self.normalized_int_type is empty for {ident}.".format(ident = self.tag_identifier) ) 
                
        if self.tms_signature_code_2016 == '':   
            if self.C99_int_type: 
                if self.C99_int_type in _SIZED_C99_TYPE_TO_TMS_SIGNATURE_2016:                  
                    self.tms_signature_code_2016 = _SIZED_C99_TYPE_TO_TMS_SIGNATURE_2016[self.C99_int_type] 
                else:
                    SoCoLogger.error("Error CEnumTypeDescriptor.build_tms_signature_code: wrong self.C99_int_type: {C99_int}".format(C99_int = self.C99_int_type))
            else: 
                SoCoLogger.warning("Warning CEnumTypeDescriptor.build_tms_signature_code: self.C99_int_type is empty for {ident}.".format(ident = self.tag_identifier))
            #else:
                #self.tms_signature_code_2016 = 'XYZ'
                #print('empty C99_int_type for ', self.ax_info)

        return self.tms_signature_code_2008  
    
    def add_to_enum_dict(self,prm_soco, prm_key, prm_value):
        
        if prm_key not in prm_soco.enum_dict:
            prm_soco.enum_dict[prm_key] = prm_value
        elif self.tag_identifier:
            SoCoLogger.warning("Warning: {enumera} already exists. This is absolutely forbidden, the Python TestEngine won't be able to handle it.".format(enumera = prm_key))
        elif not self.tag_identifier:
            SoCoLogger.warning("Warning: {enumera} comes from an unnamed enumeration-type, the Python TestEngine won't be able to handle it.".format(enumera = prm_key))    #ToDo NiEh should be printed elsewhere
            
    
    
    def evaluate_enum(self):        
        
        enum_bool = False
        for i in self.name_value_tuples:
            if i[1] < 0:
                enum_bool = True
        if enum_bool: 
            type = "int{s_e}".format(s_e = str(self.size_in_bytes*8))
        else:
            type = "uint{s_e}".format(s_e = str(self.size_in_bytes*8))
                
        return type, enum_bool    
        

class SourceCodeDescriptor: 
    
    class Declaration:
        """
        Used by VariableDescriptor, CFunctionSignatureTypeDescriptor (return and parameter description), CCompositeTypeDescriptor(for member description), TypeDefinitionDescriptor.
        """
        
        def __init__(self, set_identifer='', set_type=None):
            self.identifier = set_identifer     # for VariableDescriptor: identifier of variable;
                                                # for CFunctionSignatureTypeDescriptor, return description: not set;
                                                # for CFunctionSignatureTypeDescriptor, parameter description: parameter name;
                                                # for CCompositeTypeDescriptor: member identifier; 
                                                # for TypeDefinitionDescriptor: type identifier i.e. alias name introduced by a typedef or tag of a struct, union or enum type - not set for unnamed enum type 
            self.type = set_type                # instance of a class derived from CTypeDescriptor
            self.resolved_type = None           # should always be set i.e. either type itself or completely resolved type if type is a type selection of a non basic type
            
            self.is_const       = False
            self.is_volatile    = False
            
            self.tms_signature_code_2008 = False
            self.tms_signature_code_2016 = False
        
        def readout(self):
            
            print("------->Declaration")
            print((" -> identifier: \t\t{id_}".format(id_ = self.identifier)))
            print("---> CTypeDescriptor")
            
            if self.type:
                self.type.readout()
            else:
                print("None")
            
            if self.resolved_type:
                print("---> resolved CTypeDescriptor")
                self.resolved_type.readout()
            
            print((" -> is const: \t\t{const_}".format(const_ = self.is_const)))
            print((" -> is volatile: \t\t{vol_}".format(vol_ = self.is_volatile)))
            
            print((" -> tmssc 2008: \t\t{tms_}".format(tms_ = self.tms_signature_code_2008)))
            print((" -> tmssc 2016: \t\t{tms_}".format(tms_ = self.tms_signature_code_2016)))
            
        def getIdentifierTypeList(self, last_id="", max_array_index=5):
            id_type_list = list()
            if last_id == "":
                act_id = self.identifier
            else:
                act_id = last_id+"."+self.identifier
            
            if self.resolved_type == None:
                used_type = self.type.getResolvedType()
            else:
                used_type = self.resolved_type.getResolvedType()
            
            
            if len(used_type.getCompositeMembers()):
                id_type_list.extend(used_type.formIDTypeListRecursive(act_id))
            elif used_type.isArrayType() > 0:
                id_type_list.extend(used_type.formIDTypeListRecursive(act_id, max_array_index))
            else:
                id_type_list.append((act_id, used_type))
            
            return id_type_list
            
        def getIDIndexListRecursive(self, start_id=None):
            if not start_id == None:
                id_index_list = [start_id]
            else:
                id_index_list = [self.identifier]

            if self.resolved_type == None:
                used_type = self.type.getResolvedType()
            else:
                used_type = self.resolved_type.getResolvedType()
            
            if len(used_type.getCompositeMembers()):
                id_index_list.append(used_type.formIDIndexListRecursive())
            elif used_type.isArrayType() > 0:
                id_index_list.append(used_type.formIDIndexListRecursive())
            else:
                id_index_list.append([])
            
            return id_index_list            
            
        def resolve_type_selections(self, typedef_container_list):         
            self.resolved_type = SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_resolved_type(self.type, typedef_container_list)            
            self.type.resolve_type_selections(typedef_container_list)
                
            
          
        def build_size(self,prm_soco):
            if not self.type.size_in_bytes:
                self.type.build_size(prm_soco.type_system)
            if self.resolved_type:
                if not self.resolved_type.size_in_bytes:
                    self.resolved_type.build_size(prm_soco.type_system)
                
        def set_qualifiers_from_PSTD(self, xml_dom_type_node):
            if DomHelper.GetOptionalAttribute(xml_dom_type_node, 'is_const', 'false') == 'true':
                self.is_const = True
                
            if DomHelper.GetOptionalAttribute(xml_dom_type_node, 'is_volatile', 'false') == 'true':
                self.is_volatile = True
        
        def set_qualifiers_from_Axivion(self, AxivionType):
            if AxivionType.is_of_type('Qualified_Type_Definition') or AxivionType.is_of_type('Qualified_Direct_Type_Selection'):    
                self.is_const = AxivionType.Qualifiers.is_const
                self.is_volatile = AxivionType.Qualifiers.is_volatile
   
        def set_qualifiers_from_decl(self, prm_declaration):
            self.is_const = prm_declaration.is_const
            self.is_volatile = prm_declaration.is_volatile
        
        def is_qualified(self):
            if self.is_const or self.is_volatile:
                return True
            else:
                return False
            
        def is_qualified_const(self):
            if self.is_const:
                return True
            else:
                return False
         
        def get_source_code_type(self, id):   
            #print("for id: "+id+" qual const: "+ str(self.is_const) + " "+self.get_qualifier_string()) 
            return self.type.get_qualified_cast_or_dcl(id = id, quals = self.get_qualifier_string())
                  
        """
        This function is used to receive the cast for the RET-Stub-var, to match with the function signature's return type
        It is hold flexible, exg.: specify a qualifier, to replace the qualifier of declaration, that is standardly used.
        """ 
        def get_cast_to_qualified_declaration_type(self, qualifier = ""):
            
            if qualifier == "":
                return self.type.get_qualified_cast_or_dcl(quals = self.get_qualifier_string())
            elif (qualifier != "") and (type(qualifier) == str()):
                return self.type.get_qualified_cast_or_dcl(quals = qualifier)
            else:
                return ""
        
        """
        This function will get the type for stub-variables and their arguments. They have to be volatile and also pointers have to point to volatile.
        Additionally an id can be used to receive a declaration with this type and id.
        """            
        def get_stubvar_type(self, id = ""):
            
            #search for basic-typedesc (function-pointer)
            basic_typedesc = SourceCodeDescriptor._search_original_typedescriptor(self.type, [], [], 1)
            
            #get the type-string (id) and eliminate every const and volatile, then replace * with *volatile, to have correct pointers
            arg_stubvar = self.type.get_qualified_cast_or_dcl(id=id, quals='volatile').replace(" const ", " ").replace(" volatile ", "").replace("*const", "*").replace("*volatile", "*").replace("*", "*volatile ")
            
            #eliminate leading const-qualifier
            if arg_stubvar.startswith("const "):
                arg_stubvar = arg_stubvar[6:]
            
            #add volatile if its not there, not possible for function-pointers    
            if (not arg_stubvar.startswith("volatile ")) and (not isinstance(basic_typedesc, CFunctionSignatureTypeDescriptor)): #and (not arg_stubvar.startswith("void")):
                arg_stubvar = "volatile "+arg_stubvar
                
                
            return arg_stubvar
            
            
            
        def get_qualifier_string(self, const_only=False):
            qual_string = ""
            if self.is_const:
                qual_string += "const"
            if self.is_volatile and not const_only:
                if self.is_const:
                    qual_string += " "
                qual_string += "volatile"
                
            return qual_string    
                
        def get_tms_signature_code_2016(self):
            code = ""
            if self.resolved_type:
                if self.is_const and not self.resolved_type.tms_signature_code_2016.startswith("C_"):
                    code = "C_" + self.resolved_type.tms_signature_code_2016 
                else:
                    code = self.resolved_type.tms_signature_code_2016
            else:
                if self.is_const and not self.type.tms_signature_code_2016.startswith("C_"):
                    code = "C_" + self.type.tms_signature_code_2016 
                else:
                    code = self.type.tms_signature_code_2016
                    
            return code
            
        def get_tms_signature_code_2008(self):
            if self.resolved_type:
                return self.resolved_type.tms_signature_code_2008
            else:
                return self.type.tms_signature_code_2008
        
        def build_tms_signature_code(self, prm_soco):
            self.build_size(prm_soco)
            
            if self.resolved_type:
                self.resolved_type.build_tms_signature_code(prm_soco)
                
            self.type.build_tms_signature_code(prm_soco)
            
            self.tms_signature_code_2008 = self.get_tms_signature_code_2008()
            self.tms_signature_code_2016 = self.get_tms_signature_code_2016()
            
        def add_qualifier_to_tag(self, xml_tag):
            if self.is_const:
                xml_tag.setAttribute('is_const','true')           
            if self.is_volatile:
                xml_tag.setAttribute('is_volatile','true')     
                
        def add_signature_to_tag(self, xml_tag, prm_soco=None):
            
            if (not self.tms_signature_code_2008) or (not self.tms_signature_code_2016):
                self.build_tms_signature_code(prm_soco)
             
            if self.tms_signature_code_2008:
                xml_tag.setAttribute('tms_signature_code_2008', self.tms_signature_code_2008)
            else:
                SoCoLogger.error("The tms_signature_code_2008 was not written for {id_}".format(id_ = self.identifier))
                   
            if self.tms_signature_code_2016:  
                xml_tag.setAttribute('tms_signature_code_2016', self.tms_signature_code_2016) 
            else:
                SoCoLogger.error("The tms_signature_code_2016 was not written for {id_}".format(id_ = self.identifier))   
            
    class Definition(Declaration): 
        # if the variable is declared or defined in a statement that also defines a type (programming rules violation!)
        # then the definition_description.type references a named type (i.e. a named struct, union or enum type); 
        # in this case also an according TypeDefnitionDescriptor instances exists
        # that is referenced by  definition_description.resolved_type which contains SymbolLocation information (if symbol file has been read) 
        def __init__(self, set_identifer='', set_type=None):
            SourceCodeDescriptor.Declaration.__init__(self, set_identifer, set_type)
            self.symbol_location_descr = None
            
            
        
    class CompMemberDefinition(Declaration): 
        def __init__(self, set_identifer='', set_type=None, set_size_in_bits=None):
            SourceCodeDescriptor.Declaration.__init__(self, set_identifer, set_type)
            self.size_in_bits = set_size_in_bits                                     # size in bits (members without this member (i.e size_in_bits == None) have the size of their type
            self.offset = -1                                                         # on the basis of the type size i.e. size if unpacked: if bit field: offset in bits; else: offset in byte 
            self.symbol_location_descr = None                                              # on the basis of the symbole file i.e. 'real' size (packed or unpacked)

        def readout_member(self):
            self.readout()
            if self.symbol_location_descr: 
                print(" ->size in bits: {bits_}".format(bits_ = str(self.size_in_bits)))
                print(" ->offset: {offset_}".format(offset_ = str(self.offset)))
                print(" ->size_in_bytes: {bytes_}".format(bytes_ = str(self.symbol_location_descr.size_in_bytes)))
                print(" ->location: {location_}".format(location_ = str(self.symbol_location_descr.location_int)))
            else:
                print(" -> WARNING no Symbol location saved")

    class FunctionDescriptor:
              
        """ describes a function in C programming language """
                          
        def __init__(self):
            
            self.ax_info = ''                            # debugging information for reading from Axivion 
            
            self.is_static = None                        # indicator for static = local function (bool)          
            self.function_name = ""                      # function name (string)
            self.signature_description = None            # functions signature (instance of class CFunctionSignatureType)
            
            self.define_unit = None                      # unit in which the function is defined (instance of class SourceCodeDescriptor.UnitDescriptor)
            self.declaration_units = {}                  # unit(s) in which the function is declared (instance of class SourceCodeDescriptor.UnitDescriptor)
                      
            self.SymbolLocation = SymbolLocationType()   # describes memory allocation and alignment (instance of class SymbolLocationType); needed for work with TestEngine
             
            self.type_pointercounter = 0                # counts the calls for pointers, exg. pointer to pointer to pointer for a parameter...     
            
            self.callee_dict = {}    
                
            self.code_position = [0, 0]
            
        def getQualifier(self):
            if self.is_static:
                return "static"
            else:
                return "-"
        
        def returnsVoid(self):
            if (isinstance(self.signature_description.return_declaration.type, CTypeSelectionDescriptor) and str(self.signature_description.return_declaration.type.original_type) == "void"):
                return True
            else:
                return False
                
        def paramsVoid(self):
            if len(self.signature_description.parameter_declarations) == 0:
                return True
            else:
                return False
                
        def getSignatureIdentifierTypeList(self):
            return self.signature_description.getIdentifierTypeList()
            
        def getSignatureIDIndexList(self):
            return self.signature_description.getIDIndexList()
            
            
        @classmethod                 
        def CreateFrom_AxivionFunctionDefiniton(cls, prm_soco, AxivionFunctionDefinition):
            helper = cls()
            helper.SetFrom_AxivionDefiniton(prm_soco, AxivionFunctionDefinition)
            return helper
        
        
        def addPhysicalPosition(self, AxivionFunctionDefinition):
            if len(AxivionFunctionDefinition.Children) >= 1:
                self.code_position[0] = (int(str(AxivionFunctionDefinition.Children[0].Original_Position).split(":", 2)[1]))
                self.code_position[1] = (int(str(AxivionFunctionDefinition.Children[0].Original_End_Position).split(":", 2)[1]))
        
        def SetFrom_AxivionDefiniton(self, prm_soco, AxivionFunctionDefinition):
            
            self.ax_info =  AxivionFunctionDefinition.type() + ' ' + str(AxivionFunctionDefinition.number())  
            
            if AxivionFunctionDefinition.is_of_type('Static_Function_Definition'):
                self.is_static = True
            elif AxivionFunctionDefinition.is_of_type('Function_Definition'):
                self.is_static = False
            elif AxivionFunctionDefinition.is_of_type('Static_Function_Declaration'):
                self.is_static = True
            elif AxivionFunctionDefinition.is_of_type('Function_Declaration'):
                self.is_static = False
            else:
                self.is_static = None   
               
            self.function_name = AxivionFunctionDefinition.Logical.Name 
        
            self.signature_description =  CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionFunctionDefinition, self.function_name + '_return' )
            
            for block in AxivionFunctionDefinition.Logical.Basic_Blocks:
                if block.is_of_type('CFG_Call_Block'):
                    for code in block.Code:
                        if code.is_of_type("Direct_Call_Instruction"):
                            try:
                                dir, name, ext = split_filepath(code.Callee.Physical.Parent.Name)
                                if name in prm_soco.unit_dictionary:
                                    #print("UFF: "+code.Callee.Name)
                                    self.callee_dict[code.Callee.Name] = prm_soco.unit_dictionary[name]
                                else:
                                    SoCoLogger.warning("Function: {func_} calls function -> {call_} and no unit-descriptor is created yet for {unit_}".format(func_ = self.function_name, call_ =  code.Callee.Name, unit_ = name))
                            except:
                                SoCoLogger.error("Function: {func_} calls function -> {call_} and no parent unit is found.".format(func_ = self.function_name, call_ =  code.Callee.Name))    
			
        
            
            units = ""
            for def_ in AxivionFunctionDefinition.Logical.Definitions:
                units += def_.Name
            
            if len(AxivionFunctionDefinition.Logical.Definitions) > 1:
                SoCoLogger.error("Function: {func_} has multiple definitions, defining units: {units_}".format(func_ = self.function_name, units_ = units))
            
            """
            for statement in AxivionFunctionDefinition.descendants_of_type('Statement_Sequence'):
                for decl in statement.descendants_of_type('Declaration_Statement'):
                    #if decl.is_of_type('Local_Variable_Definition'):
                        #print(var)
                    for var in decl.descendants_of_type('Local_Variable_Definition'):
                        pass
                    for static_var in decl.descendants_of_type('Local_Static_Ordinary_Variable_Definition'):
                        print(static_var)
                        #if var.is_of_type('Local_Static_Variable_Definition'):
                            #print(var)
              
         
            for var in AxivionFunctionDefinition.Logical.Variables:
                if var.is_of_type(['Local_Static_Variable']):
                    variable =  SourceCodeDescriptor.VariableDescriptor.CreateFrom_AxivionLocalStaticDefinition(self, var)  
                    print("Gefunden: "+variable.definition_description.identifier)
                    if variable.definition_description.type.not_readable:
                        SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the type for variable  ' + str(variable.definition_description.identifier) + ' defined in ' + str(File.Full_Name))   
                        read_error = True                 
                
                    else:
                        if not (variable.definition_description.identifier in prm_soco.variable_list):
                            prm_soco.variable_list.append(variable.definition_description.identifier)         
                            unit.variable_descriptions_dict[variable.definition_description.identifier] = variable
                            SoCoLogger.info("Local static Variable: {var_} occured in function: {func_}.".format(var_ = variable.definition_description.identifier, func_ = self.function_name))
                        else:
                            SoCoLogger.info("Local static Variable: {var_} occured in function: {func_}.".format(var_ = variable.definition_description.identifier, func_ = self.function_name))          
            """
        @classmethod                 
        def CreateFrom_PSTD(cls, prm_soco, xml_dom_function_node):
            helper = cls()
            helper.SetFrom_PSTD(prm_soco, xml_dom_function_node)
            return helper 
        
        def SetFrom_PSTD(self, prm_soco, xml_dom_function_node):
            
            if xml_dom_function_node.getAttribute('linkage') == 'internal':     
                self.is_static = True  
            else:
                self.is_static = False
                       
            self.function_name = xml_dom_function_node.getAttribute('identifier')
                                      
            self.signature_description = CFunctionSignatureTypeDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_function_node)
            
            for child in xml_dom_function_node.childNodes:
                if child.nodeName == 'defined_in':   
                    self.define_unit = child.firstChild.data
                elif child.nodeName == 'declared_in':       
                    self.declaration_units[child.firstChild.data] = child.firstChild.data
                elif child.nodeName == 'callee':
                    #inconsistency here: while creating the value is unit-descriptor, while reading its just name
                    self.callee_dict[child.firstChild.data] = child.getAttribute('defined_in')
            """        
            self.code_position = xml_dom_function_node.getAttribute('code_position')
            """
        
            
        @classmethod 
        def get_stub_stuff_of_functiondescriptorstring(self, func_desc_string): 
            
            func_desc_string = func_desc_string.replace("-", " ")
            decl_chain = func_desc_string.split(":")
            
            if len(decl_chain) == 1:
                function = decl_chain[0] 
                param_types = ["void"]
                ret_type = "void"    
            elif(decl_chain[0] == ""):
                function = decl_chain[1]
                param_types = decl_chain[2:]
                ret_type = "void"
            else:
                function = decl_chain[1]
                param_types = decl_chain[2:]
                ret_type =  decl_chain[0]
                
            
            param_string = ""
            
            stub_vars = []
            param_decls = []
            param_equation = ""
            stub_var_decls = ""
            
            if not ((param_types[0] == "") or (param_types[0] == "void")):
                counter = 0
                for paramt in param_types:
                    counter += 1
                    stub_vars.append(function+"_Stub_"+str(counter))
                    stub_var_decls += paramt + " " +function+"_Stub_"+str(counter)+";\n"
                    param_t_red = paramt
                    
                    counter2 = 0
                    while("*" in param_t_red):
                        param_t_red = param_t_red.replace("*", "", 1)
                        if not param_t_red == "void":
                            stub_var_decls += param_t_red + " " +function+"_Stub_"+str(counter)+"_argument"+str(counter2)+";\n" 
                            counter2 += 1
                    param_decls.append( "stub_param_"+str(counter))
                
                counter = 0    
                param_string = "("
                for pdecl in param_decls:
                    param_string += param_types[counter]+" "+pdecl+", "
                    param_equation += stub_vars[counter] +" = " + pdecl+";\n"
                    counter += 1
                
                param_string = param_string[:-2]+")"  
            else:
                param_string = "(void)"
                
            if not ret_type == "void": 
                param_equation += "return "+ function+"_Stub_RET;\n"
                stub_var_decls += ret_type + " " + function+"_Stub_RET;\n"
            
            stub_func_decl = "extern"+ " " +ret_type + " " + function+"_Stub" + param_string+";"
            
            stub_func_def = "#ifndef "+function.upper()+"_STUB\n"
            stub_func_def += ret_type + " " + function+"_Stub" + param_string+"\n{\n" + param_equation+"\n}\n"       
            stub_func_def += "#endif\n"
            
            unit_stub_bypass = "\t\t#define {func_name} {func_name}_Stub\n".format(func_name=function) 
            
            return stub_func_decl, stub_func_def, stub_var_decls, unit_stub_bypass
                             
        
        def getSignature(self, funcname_add=""):
            
            buffer_typedesc = SourceCodeDescriptor._search_original_typedescriptor(self.signature_description.return_declaration.type, [], [], 1)
            
            signatur = ""
            if isinstance(buffer_typedesc, CFunctionSignatureTypeDescriptor) and not isinstance(self.signature_description.return_declaration.type, CTypeSelectionDescriptor):
                signatur   = str(self.signature_description.get_source_code_return_type())
                sig_buffer_list = signatur.partition("*")
                param_buffer_list = sig_buffer_list[2].split(")" ,1)    #two parts now: )   (params of function pointer return type)
                function_name   = str(self.function_name)+funcname_add
                parameters       = str(self.signature_description.get_source_code_param_decls())
                signatur   = sig_buffer_list[0]+sig_buffer_list[1]+function_name + param_buffer_list[1] +")" + "("+parameters+")"
            else:
                signatur = "{return_type} {func_name}".format(return_type=self.signature_description.get_source_code_return_type(), func_name=funcname_add)+"({param_decls})".format(param_decls=self.signature_description.get_source_code_param_decls())
                
            return signatur
        
        def get_stub_stuff(self):
            stub_func_decl = ''
            stub_func_def = ''
            stub_var_decls = []
            pointer_argument_names = []
            
            stub_signatur = self.getSignature(funcname_add=self.function_name+"_Stub")   
                    
            if self.is_static:
                stub_func_decl += 'static '
            else:
                stub_func_decl += 'extern '   
    
            stub_func_decl += stub_signatur + ';'    
             
            stub_func_def = "#ifndef {upper_func_name}_STUB\n".format(upper_func_name=self.function_name.upper()) 
            
            if self.is_static:
                stub_func_def += 'static '
                
            stub_func_def += stub_signatur   
            stub_func_def += "\n{\n"
            
            for param in self.signature_description.parameter_declarations: 
                
                param_type = param.type
                param_name = param.identifier
                
                if(isinstance(param_type, CTypeSelectionDescriptor)):
                    while(isinstance(param_type, CTypeSelectionDescriptor) and not (param_type.resolved_type == None)):
                        param_type = param_type.resolved_type
                
                
                index_list = list([])
                call_list = list([])
                arg_buffer_type = SourceCodeDescriptor._search_original_typedescriptor(param_type, call_list, index_list)
                
                inplace_indicator = collections.Counter()
                for c in call_list:
                    inplace_indicator[c] += 1
                
                #receive cast for stub-var-assignment (possible qualifier is volatile only)
                possible_cast = "(" + param.get_stubvar_type() + ") "
                
                #delete cast completely for non-qualified non-poiner stub-vars 
                if not param.is_qualified_const() and not ("*" in possible_cast) and not isinstance(arg_buffer_type, CFunctionSignatureTypeDescriptor):
                    possible_cast = ""
                
                #delete volatile cast for non-pointers    
                if possible_cast.startswith("(volatile ") and not ("*" in possible_cast) and not isinstance(arg_buffer_type, CFunctionSignatureTypeDescriptor):
                    possible_cast = possible_cast.replace("(volatile ", "(")
                
                #make pointer variables non-volatile for stub-var assignment     
                if isinstance(param_type, CInPlaceTypeDescriptor) and not isinstance(arg_buffer_type, CFunctionSignatureTypeDescriptor):
                    param_qualified_type = param_type.get_qualified_cast_or_dcl()
                    if not (("const" in param_qualified_type) or param.is_qualified_const()):
                        if possible_cast.endswith("*volatile ) ") or possible_cast.endswith("*volatile) ") or possible_cast.endswith("*volatile  ) "):
                                possible_cast = possible_cast.rsplit("volatile", 1)[0]+")"
                                
                #delete stub-var cast for non-qualified function-pointer params 
                if isinstance(arg_buffer_type, CFunctionSignatureTypeDescriptor) and not param.is_qualified_const():
                    possible_cast = ""

                array_param = False
                if param_type.__class__ ==  CInPlaceTypeDescriptor:  
                    if param_type.is_array_of >= 0:
                        array_param = True
                
                composite_param = False        
                if isinstance(param_type, CCompositeTypeDescriptor):
                    composite_param = True                       
                    
                if array_param == True:
                    
                    #casts for array elements are not possible, unless you have complicated pointer cast
                    if not ("*"  in possible_cast):
                        possible_cast = ""
                    
                    if param_type.is_array_of == 0:
                        stub_func_def += INDENTTATION + "{func_name}_{param_name}[0] = {cast}{param_name}[0];\n".format(func_name=self.function_name, param_name=param_name, cast=possible_cast)
                    else:   
                        if len(index_list) == 1:
                            count_var_i = "for_count_i"+"_"+param_name
                            stub_func_def += INDENTTATION + "int {count1} = 0;\n".format(count1 = count_var_i)
                            stub_func_def += INDENTTATION + "for(; {count1} < ".format(count1 = count_var_i) + str(index_list[0])+ "U; {count1}++)\n".format(count1 = count_var_i)
                            stub_func_def += INDENTTATION + "{\n"
                            stub_func_def += 2*INDENTTATION + "{func_name}_{param_name}[{count1}] = {cast}{param_name}[{count1}];\n".format(func_name=self.function_name, param_name=param_name, cast=possible_cast, count1 = count_var_i)
                            stub_func_def += INDENTTATION + "}\n"
                        elif len(index_list) == 2:
                            count_var_i = "for_count_i"+"_"+param_name
                            count_var_j = "for_count_j"+"_"+param_name
                            stub_func_def += INDENTTATION + "int {count1} = 0;\n".format(count1 = count_var_i)
                            stub_func_def += INDENTTATION + "int {count2} = 0;\n".format(count2 = count_var_j)
                            stub_func_def += INDENTTATION + "for(; {count1} < ".format(count1 = count_var_i) + str(index_list[0]) + "U; {count1}++)\n".format(count1 = count_var_i)
                            stub_func_def += INDENTTATION + "{\n"
                            stub_func_def += 2*INDENTTATION + "for(; {count2} < ".format(count2 = count_var_j) + str(index_list[1]) + "U; {count2}++)\n".format(count2 = count_var_j)
                            stub_func_def += 2*INDENTTATION + "{\n"
                            stub_func_def += 3*INDENTTATION + "{func_name}_{param_name}[{count1}][{count2}] = {cast}{param_name}[{count1}][{count2}];\n".format(func_name=self.function_name, param_name=param_name, cast=possible_cast, count1 = count_var_i, count2 = count_var_j)
                            stub_func_def += 2*INDENTTATION + "}\n"
                            stub_func_def += INDENTTATION + "}\n"
                            
                        elif len(index_list) == 3:
                            count_var_i = "for_count_i"+"_"+param_name
                            count_var_j = "for_count_j"+"_"+param_name
                            count_var_k = "for_count_k"+"_"+param_name
                            stub_func_def += INDENTTATION + "int {count1}  = 0;\n".format(count1 = count_var_i)
                            stub_func_def += INDENTTATION + "int {count2}  = 0;\n".format(count2 = count_var_j)
                            stub_func_def += INDENTTATION + "int {count3}  = 0;\n".format(count3 = count_var_k)
                            stub_func_def += INDENTTATION + "for(; {count1} < ".format(count1 = count_var_i) + str(index_list[0]) + "U; {count1}++)\n".format(count1 = count_var_i)
                            stub_func_def += INDENTTATION + "{\n"
                            stub_func_def += 2*INDENTTATION + "for(; {count2} < ".format(count2 = count_var_j) + str(index_list[1]) + "U; {count2}++)\n".format(count2 = count_var_j)
                            stub_func_def += 2*INDENTTATION + "{\n"
                            stub_func_def += 3*INDENTTATION + "for(; {count3} < ".format(count3 = count_var_k) + str(index_list[2]) + "U; {count3}++)\n".format(count3 = count_var_k)
                            stub_func_def += 3*INDENTTATION + "{\n"
                            stub_func_def += 4*INDENTTATION + "{func_name}_{param_name}[{count1}][{count2}][{count3}] = {cast}{param_name}[{count1}][{count2}][{count3}];\n".format(func_name=self.function_name, param_name=param_name, cast=possible_cast, count1 = count_var_i, count2 = count_var_j, count3 = count_var_k)
                            stub_func_def += 3*INDENTTATION + "}\n"
                            stub_func_def += 2*INDENTTATION + "}\n"
                            stub_func_def += INDENTTATION + "}\n"
                        else:
                            SoCoLogger.error("Error get_stub_stuff: The passed parameter contains an array with more than 3-dimensions.")
                
                elif composite_param == True:
                    
                    struct_member_list = param_type.create_memberstring_list(prm_ccomposite=param_type, prm_current_member = "",  prm_unions_members = [])
                    for string in struct_member_list:
                        stub_func_def += INDENTTATION + "{func_name}_{param_name}.{mem_string_} = {cast}{param_name}.{mem_string_};\n".format(func_name=self.function_name, param_name=param_name, mem_string_=string.split(".", 1)[1], cast="") 
                                
                else:        
                    stub_func_def += INDENTTATION + "{func_name}_{param_name} = {cast}{param_name};\n".format(func_name=self.function_name, param_name=param_name, cast=possible_cast)   
            

                #generate stub-variables, that are volatile and poiners will point to volatile
                stub_var_decls += "".join("{0};\n".format(param.get_stubvar_type(id = self.function_name + '_' + param_name )))      #params have a declaration
            
                #if the parameter is a pointer to s.th. (inplace_indicator[0] > 0) then create a variable whose address is intended to be passed as argument for this parameter
                #inplace_indicator[0] > 0 is also true if we got an array of pointers.
                if inplace_indicator[0] > 0:
                    create_argument = True
                    if arg_buffer_type.__class__ == CFunctionSignatureTypeDescriptor:   
                        create_argument = False          # a variable to pass to a function pointer does not make sense                                     
                    elif arg_buffer_type.__class__ == CTypeSelectionDescriptor:
                        if arg_buffer_type.original_type == 'void': # a variable to pass to a void pointer does not make sense   
                            create_argument = False                             
                    if create_argument:
                        #gets the basic included type of the pointer and replaces any qualifier and stars  
                        type_string = param_type.get_qualified_cast_or_dcl().replace(" const ", " ").replace(" volatile ", "").replace("*const", "*").replace("*volatile", "*")
                        if type_string.startswith("const "):
                            type_string = type_string[6:]
                        type_string = type_string.replace("[", ".[").split(".", 1)[0]
                        type_string = type_string.replace(" *", "")
                        
                        #creates the argument variables and restores stars and volatile
                        for i in range(0, inplace_indicator[0]):  
                            var_name = self.function_name + '_' + param_name + '_argument' + str('_p'*i) 
                            #argument variables always point to volatile                                                    
                            stub_var_decls += "volatile {type_} {plvl_} {var_name_};\n".format(type_ = type_string, plvl_ = " *volatile"*i, var_name_ = var_name)
                            pointer_argument_names.append(var_name) 
            
            if not self.signature_description.get_source_code_return_type() == "void":
                # cast in any case is not OK because compile error if cast to structure (UlBr 30.09.14)
                # cast only if return_type != return_type_purged is not enough  because compile error for any pointer i.e. cast for all pointers necessary (UlBr 30.09.14))
                ret_call_list, ret_index_list = list(), list()
                basic_ret_type = SourceCodeDescriptor._search_original_typedescriptor(self.signature_description.return_declaration.type, ret_call_list, ret_index_list)
                
                ret_inplace_indicator = collections.Counter()
                for c in ret_call_list:
                    ret_inplace_indicator[c] += 1
                
                #dont cast, if theres a struct as ret-value, that is volatile only
                if (isinstance(basic_ret_type, CCompositeTypeDescriptor)) and (ret_inplace_indicator[0] == 0):   
                    possible_cast ="" 
                #otherwise receive cast for the return of stubvar, in that way it matches function-return-type 
                else:
                    possible_cast = "(" + self.signature_description.return_declaration.get_cast_to_qualified_declaration_type() + ") " 
                         
                stub_func_def += INDENTTATION + "return {cast}{func_name}_RET;\n".format(cast=possible_cast, func_name=self.function_name)
            
            stub_func_def += "}\n"
            stub_func_def += "#endif\n\n"
            
                                   
            if not self.signature_description.get_source_code_return_type() == "void":
                return_stubvar = self.signature_description.return_declaration.get_stubvar_type(id=self.function_name + '_RET')
                stub_var_decls += "{0};\n".format(return_stubvar);
            
            return (stub_func_decl, stub_func_def, stub_var_decls, pointer_argument_names)
            
        def get_stub_vars_as_list(self):
            var_list = []
            if not self.signature_description.get_source_code_return_type() == "void":
                var_list.append(self.function_name + '_RET')
                
            for param in self.signature_description.parameter_declarations:
                param_name = param.identifier
                var_list.append(self.function_name + '_' + param_name)
                
                            
            return var_list
            
        def get_arg_vars_as_list(self):
            arg_vars = []
            prm_arg_dict = {"order":[]}
                
            for param in self.signature_description.parameter_declarations:
                param_type = param.type
                param_name = param.identifier
                
                index_list = []
                call_list = []
                arg_buffer_type = SourceCodeDescriptor._search_original_typedescriptor(param_type, call_list, index_list)
                
                inplace_indicator = collections.Counter()
                for c in call_list:
                    inplace_indicator[c] += 1
                    
                if inplace_indicator[0] > 0:
                    create_argument = True
                    if arg_buffer_type.__class__ == CFunctionSignatureTypeDescriptor:   
                        create_argument = False          # a variable to pass to a function pointer does not make sense                                     
                    elif arg_buffer_type.__class__ == CTypeSelectionDescriptor:
                        if arg_buffer_type.original_type == 'void': # a variable to pass to a void pointer does not make sense   
                            create_argument = False                             
                    if create_argument:
                        prm_arg_dict["order"].append(param_name)
                        act_arg_vars = []
                        for i in range(0, inplace_indicator[0]):  
                            act_arg_vars.append(self.function_name + '_' + param_name + '_argument' + str('_p'*i))
                        act_arg_vars.reverse()
                        prm_arg_dict[param_name] = act_arg_vars
                        arg_vars.extend(act_arg_vars)
                            
            return arg_vars, prm_arg_dict
        
        def mk_func_ut_wrapper(self, prm_typesystem_id):
                     
            wrapper_signature = "void {func_name}_ut_wrapper (void);\n".format(func_name = self.function_name)
                      
            return_type = self.signature_description.return_declaration.type
            
            wrapper_function_parameters = ""             
            wrapper_prm_current_struct = ""
            
            prm_string = ""
            prm_string_all = ""
            
            wrapper_param_init = ""
            
            #variables for defining and passing the parameters
            wrapper_current_arg = ""
            wrapper_arg_define = ""
            
            if len(self.signature_description.parameter_declarations) <= 10:
                
                for param_index in range(0, len(self.signature_description.parameter_declarations)):
                    
                    wrapper_prm_current_struct = "TM_Wrapper_ut_prm"+str(int(param_index))
                    prm_pointer_counter = 0
                    
                    call_indicator_list = []
                    
                    param_type = self.signature_description.parameter_declarations[param_index] #Nun muss passend zum Typ der Union Member gesucht werden, oder im Fall einer struct  der Inhalt ab startadresse des bytearrays gegeben werden.
                    while isinstance(param_type.type, CTypeSelectionDescriptor) and not param_type.type.resolved_type == None and not isinstance(param_type.type.resolved_type, CEnumTypeDescriptor) and not isinstance(param_type.type.resolved_type, CCompositeTypeDescriptor):
                        param_type.type = param_type.type.resolved_type 
                        
                    
                    #Argument the function is called with #ToDo NiEh TM can be eliminated
                    wrapper_current_arg = "{func_name}_{prm_id}_argument".format(func_name = self.function_name, prm_id = str(param_type.identifier))
                    
                    cfuncp_indicator = list([])
                    buffer_typedescriptor = SourceCodeDescriptor._search_original_typedescriptor(param_type.type, cfuncp_indicator, [], 1)
                    if isinstance(buffer_typedescriptor, CFunctionSignatureTypeDescriptor): 
                        fp = collections.Counter()
                        wrapper_current_arg = wrapper_current_arg + "_fp"
                        for c in cfuncp_indicator:
                            fp[c] += 1
                        if fp[1] > 0:
                            wrapper_prm_current_struct += ".TM_Wrapper_res_content.TM_Wrapper_bytearray"
                            prm_string = "((void*)(&{wrapper_prm}))".format(wrapper_prm = wrapper_prm_current_struct)
                        else:
                            param_type_res_string = buffer_typedescriptor.get_source_code_return_type() + " (*)({param_decls})".format(param_decls = buffer_typedescriptor.get_source_code_param_decls())
                            wrapper_prm_current_struct += ".TM_Wrapper_p"                                
                            prm_string = "(({pointer_type})({wrapper_prm}))".format(pointer_type = param_type_res_string, wrapper_prm = wrapper_prm_current_struct)   
                    
                    elif isinstance(param_type.type, CInPlaceTypeDescriptor):
                        
                        if not param_type.type.unnamed_struct_or_enum:             
                            
                            if param_type.type.is_pointer_to and param_type.type.is_array_of == -1 :    #If we found a pointer, no arrays can follow.  
                                
                                basic_typedescriptor = SourceCodeDescriptor._search_original_typedescriptor(param_type.type.included_type, call_indicator_list, [], 1)
                                
                                if isinstance(basic_typedescriptor, CFunctionSignatureTypeDescriptor):
                                    param_type_res_string = basic_typedescriptor.get_source_code_return_type() + " (*)({param_decls})".format(param_decls = basic_typedescriptor.get_source_code_param_decls())
                                    
                                    wrapper_prm_current_struct += ".TM_Wrapper_p"                                
                                    prm_string = "(({pointer_type})({wrapper_prm}))".format(pointer_type = param_type_res_string, wrapper_prm = wrapper_prm_current_struct)   
                                
                                else:
                                    self.type_pointercounter = 1
                                    #type_pointercounter is incremented for every included type 
                                    buffer = self._search_param_type_string(param_type.type.included_type, 0)   
                                    param_type_res_string = param_type.type.get_qualified_cast_or_dcl()
                                
                                    #adapt wrapper define
                                    wrapper_current_arg = wrapper_current_arg + "_p"*self.type_pointercounter
                                    
                                    #set wrapper cast
                                    wrapper_prm_current_struct += ".TM_Wrapper_{ps}".format(ps = "p"*self.type_pointercounter)        
                                    prm_string = "(({pointer_type})({wrapper_prm}))".format(pointer_type = param_type_res_string, wrapper_prm = wrapper_prm_current_struct)   
                            
                            elif param_type.type.is_array_of >= 0:
                                
                                #define the current struct                                              
                                wrapper_prm_current_struct += ".TM_Wrapper_res_content.TM_Wrapper_bytearray"
                                
                                #search the basic typedescriptor with the list for array or reference
                                basic_typedescriptor = SourceCodeDescriptor._search_original_typedescriptor(param_type.type.included_type, call_indicator_list, [], 1)
                                #search the basic type string
                                param_type_res_string = self._search_param_type_string(basic_typedescriptor, 0)
                                #simply the pointer to array has to be passed.
                                
                                d = collections.Counter()
                                for c in call_indicator_list:
                                    d[c] += 1
                                
                                if d[0] == 0:   #No Pointers
                                    if isinstance(basic_typedescriptor, CTypeSelectionDescriptor):      #Fall: ein- oder mehrdimensionales array eines normalen Typs                                   
                                                                                                                     
                                        if basic_typedescriptor.size_in_bytes <= 8 and not basic_typedescriptor.sized_C99_type == "":
                                            #prm_string = "(({type} {stars})(void *){current_struct})".format(type = basic_typedescriptor.original_type, current_struct = wrapper_prm_current_struct, stars = "*"*(d[1]))
                                            prm_string = "((void *){current_struct})".format(current_struct = wrapper_prm_current_struct)
                                        
                                        elif basic_typedescriptor.sized_C99_type == "": 
                        
                                            if basic_typedescriptor.type_definition_kind == "typedef_type" or basic_typedescriptor.type_definition_kind == "struct_type" or basic_typedescriptor.type_definition_kind == "union_type":
                                                #print "Resolved type: {type}".format(type = basic_typedescriptor.resolved_type)
                                                #prm_string = "(({type} {stars})((void *){current_struct}))".format(type = basic_typedescriptor.original_type, current_struct = wrapper_prm_current_struct, stars = "*"*(d[1]))
                                                prm_string = "((void *){current_struct})".format(current_struct = wrapper_prm_current_struct)
                                            
                                            elif basic_typedescriptor.type_definition_kind == "enum_type":
                                                prm_string = "((void *){current_struct})".format(current_struct = wrapper_prm_current_struct)
                                                
                                            
                                            else:
                                                SoCoLogger.error( "Error Wrapper: The correct type of {prm_name} can not be found.".format(prm_name = basic_typedescriptor.original_type))
                                    
                                    elif isinstance(basic_typedescriptor, CCompositeTypeDescriptor):
                                        
                                        prm_string = "(((void *){current_struct}))".format(current_struct = wrapper_prm_current_struct)
                                        
                                    elif isinstance(basic_typedescriptor, CEnumTypeDescriptor):
                                        #prm_string = "((enum {type} *)((void *){current_struct}))".format(type = basic_typedescriptor.tag_identifier, current_struct = wrapper_prm_current_struct)
                                        prm_string = "(((void *){current_struct}))".format(current_struct = wrapper_prm_current_struct)
                                    
                                    elif isinstance(basic_typedescriptor, CFunctionSignatureTypeDescriptor):
                                        param_type_res_string = basic_typedescriptor.get_source_code_return_type() + " (*)({param_decls})".format(param_decls = basic_typedescriptor.get_source_code_param_decls())
                                        wrapper_prm_current_struct += ".TM_Wrapper_p"                                
                                        prm_string = "(({pointer_type})({wrapper_prm}))".format(pointer_type = param_type_res_string, wrapper_prm = wrapper_prm_current_struct)  
                                    else:
                                        SoCoLogger.error( "Error Wrapper: The basic-descriptor {desc} of {prm} can not be found. Function: {func_name}".format(desc= basic_typedescriptor, prm = param_type.identifier, func_name=str(self.function_name)))
                                    
                                elif d[0] > 0:  #Pointers
                                    wrapper_prm_current_struct = "TM_Wrapper_ut_prm"+str(int(param_index))
                                    if d[0] == 1:
                                        prm_string = "({pointer_type} {pointer_stars})((void {pointer_stars}){current_struct}.TM_Wrapper_res_content.TM_Wrapper_parray)".format(pointer_type = param_type_res_string, pointer_stars = "*"*(d[0]+1), current_struct = wrapper_prm_current_struct, ps = "p"*(d[0]+1))                                                            
                                    elif d[0] == 2:
                                        prm_string = "({pointer_type} {pointer_stars})((void {pointer_stars}){current_struct}.TM_Wrapper_res_content.TM_Wrapper_ptparray)".format(pointer_type = param_type_res_string, pointer_stars = "*"*(d[0]+1), current_struct = wrapper_prm_current_struct, ps = "p"*(d[0]+1))
                                    else:
                                        SoCoLogger.error( "Error Wrapper: {prm} implicates a pointer of grade 4 or even higher".format(prm = param_type.identifier)  ) 
                            
                                else:
                                    SoCoLogger.error("Error Wrapper: Something really strange happened!")
                            else: 
                                SoCoLogger.error("Error Wrapper: The searched type of {prm_name} is not a pointer, nor an array".format(prm_name = param_type.identifier) )                             
                        
                        else:
                            SoCoLogger.error("Error Wrapper: The searched type {prm_name} is unnamed, therefore it can not be passed to a function".format(prm_name = param_type.identifier) )

                        
                    elif isinstance(param_type.type, CCompositeTypeDescriptor): 
                        
                        wrapper_prm_current_struct = "&" + wrapper_prm_current_struct + ".TM_Wrapper_res_content"
                        
                        if param_type.type.kind == "struct":
                            prm_string = "*((struct {pointer_type} *)((void *){wrapper_prm}))".format(pointer_type = param_type.type.tag_identifier, wrapper_prm = wrapper_prm_current_struct)
                        
                        elif param_type.type.kind == "union":
                            prm_string = "*((union {pointer_type} *)((void *){wrapper_prm})".format(pointer_type = param_type.type.tag_identifier, wrapper_prm = wrapper_prm_current_struct)
                        else:
                            
                            SoCoLogger.error( "Error Wrapper: The searched type {prm_name} is nor a struct nor a union, therefore it can not be passed to a function".format(prm_name = param_type.identifier))
        
                      
                    elif isinstance(param_type.type, CTypeSelectionDescriptor):
                        
                        if param_type.type.size_in_bytes <= 8 and not param_type.type.sized_C99_type == "" and not (param_type.type.original_type == "float" or param_type.type.original_type == "double" or param_type.type.original_type == "long double"):
                            if param_type.type.sized_C99_type.startswith("u"): 
                                wrapper_prm_current_struct += ".TM_Wrapper_res_content.TM_Wrapper_uint{bytes}[0]".format(bytes = str(int(param_type.type.size_in_bytes*8)))
                            else:
                                wrapper_prm_current_struct += ".TM_Wrapper_res_content.TM_Wrapper_int{bytes}[0]".format(bytes = str(int(param_type.type.size_in_bytes*8)))
                            prm_string = "({type})({current_struct})".format(type = param_type.type.original_type, current_struct = wrapper_prm_current_struct)
                        
                        elif (param_type.type.original_type == "float" or param_type.type.original_type == "double" or param_type.type.original_type == "long double"):
                            prm_string = "*({type} *)(&{current_struct})".format(type = param_type.type.original_type, current_struct = wrapper_prm_current_struct)
                                
                        elif param_type.type.sized_C99_type == "":  
                        
                            if param_type.type.type_definition_kind == "typedef_type":
                                wrapper_prm_current_struct += ".TM_Wrapper_res_content.TM_Wrapper_bytearray"
                                prm_string = "*(({type} *)((void *){current_struct}))".format(type = param_type.type.original_type, current_struct = wrapper_prm_current_struct)
                            
                            elif param_type.type.type_definition_kind == "struct_type":  
                                wrapper_prm_current_struct += ".TM_Wrapper_res_content.TM_Wrapper_bytearray"
                                prm_string = "*((struct {type} *)((void *){current_struct}))".format(type = param_type.type.original_type, current_struct = wrapper_prm_current_struct)      
                                    
                            elif param_type.type.type_definition_kind == "union_type":  
                                wrapper_prm_current_struct += ".TM_Wrapper_res_content.TM_Wrapper_bytearray"
                                prm_string = "*((union {type} *)((void *){current_struct}))".format(type = param_type.type.original_type, current_struct = wrapper_prm_current_struct)      
                                
                            elif param_type.type.type_definition_kind == "enum_type": 
                                wrapper_prm_current_struct += ".TM_Wrapper_res_content.TM_Wrapper_int32[0]"
                                if not param_type.type.original_type == "":
                                    prm_string = "((enum {type}){current_struct})".format(type = param_type.type.original_type, current_struct = wrapper_prm_current_struct)    #ToDo NiEh normally wrong the original type saves a tag here, normally only enumtypedesc should save it 
                                else:
                                    #param_type.type.readout()
                                    #param_type.type.resolved_type.readout()
                                    prm_string = "((enum {type}){current_struct})".format(type = param_type.type.resolved_type.tag_identifier, current_struct = wrapper_prm_current_struct)    
                            else:
                                SoCoLogger.error("Error Wrapper: The correct type of {prm_name} can not be found.".format(prm_name = basic_typedescriptor.original_type))
                                      
                    
                        
                    elif isinstance(param_type.type, CEnumTypeDescriptor):
                        
                        enum_tuple = param_type.type.evaluate_enum()
                        
                        end = enum_tuple[0]+"[0]"
                        
                        prm_string = "(enum {cast})".format(cast = param_type.type.tag_identifier) + wrapper_prm_current_struct +".TM_Wrapper_res_content.TM_Wrapper_"  + end
                        
                    else:
                        SoCoLogger.error("Error Wrapper: The typedescriptor of the parameter {prm_name} can not be resolved.".format(prm_name = param_type.identifier))
                
                    wrapper_arg_define += "#define {wrapper_arg}\t\t\t{pointer}\n".format(wrapper_arg = wrapper_current_arg, func_name = self.function_name, prm_index = param_index, pointer = prm_string)                              
                    prm_string_all += wrapper_current_arg +", "                     
                    
            else:
                SoCoLogger.error("Error Wrapper: The function {func_name} needs more than 10 parameters, therefore not enough wrapper-parameters are initialized.".format(self.function_name))
            
            
            #wrapper_function_parameters = wrapper_function_parameters[:-1]      
            wrapper_function_call = "{func_name}({param_decls});\n".format(func_name = self.function_name, param_decls = prm_string_all[:-2])   
            
            wrapper_return_symbol = "{func_name}_return_argument".format(func_name = self.function_name)
            wrapper_return_define = ""

            wrapper_return_symbol_add = ""      #stores the cast of returnvalue and sign of equation
            wrapper_return_define_add = ""      #stores the correctly casted wrapper-struct-element
            
            #search for enumerations and resolve them
            while isinstance(return_type, CTypeSelectionDescriptor) and not return_type.resolved_type == None and return_type.type_definition_kind == "enum_type":
                return_type = return_type.resolved_type 
            
            ret_buffer_typedescriptor = SourceCodeDescriptor._search_original_typedescriptor(return_type, [], [], 1)
            if isinstance(ret_buffer_typedescriptor, CFunctionSignatureTypeDescriptor):
                
                wrapper_return_symbol += "_fp"
                
                wrapper_return_define_add = "TM_Wrapper_ut_RET.TM_Wrapper_p"
                wrapper_return_symbol_add = " = (void *)"
                
            elif not (self._search_param_type_string(return_type, 0) == "void" and isinstance(return_type, CTypeSelectionDescriptor)):   
                
                if isinstance(return_type, CInPlaceTypeDescriptor):     
                    buffer_typedescriptor = return_type.included_type
                    if isinstance(buffer_typedescriptor, CTypeSelectionDescriptor):
                        while isinstance(buffer_typedescriptor, CTypeSelectionDescriptor) and buffer_typedescriptor.resolved_type:
                            buffer_typedescriptor = buffer_typedescriptor.resolved_type
                          
                    if isinstance(buffer_typedescriptor, CFunctionSignatureTypeDescriptor):
                        wrapper_return_symbol += "_fp"
                        wrapper_return_define_add = "TM_Wrapper_ut_RET.TM_Wrapper_p"
                        wrapper_return_symbol_add = " = (void *)"
                    elif return_type.is_pointer_to and return_type.is_array_of == -1:
                        
                        self.type_pointercounter = 1
                        wrapper_return = self._search_param_type_string(return_type.included_type, 0)   #Only there to count the pointer-grade
                        wrapper_return_symbol += "_p"*self.type_pointercounter
                        wrapper_return_define_add = "TM_Wrapper_ut_RET.TM_Wrapper_{p}".format(p = "p"*self.type_pointercounter)
                        wrapper_return_symbol_add = " = (void {stars})".format(stars = "*"*self.type_pointercounter)
                        
                    else:
                        SoCoLogger.error("Error Wrapper: Someone defined an array as return value for function: {func}".format(func = self.function_name))
                
                elif isinstance(return_type, CTypeSelectionDescriptor):
                    
                    type_string = self._search_param_type_string(return_type)
                    if return_type.size_in_bytes <= 8 and not return_type.sized_C99_type == "" and not (return_type.original_type == "float" or return_type.original_type == "double" or return_type.original_type == "long double"):
                        if return_type.sized_C99_type.startswith("u"):
                            wrapper_return_define_add = "TM_Wrapper_ut_RET.TM_Wrapper_res_content.TM_Wrapper_uint{bytes}[0]".format(bytes = str(return_type.size_in_bytes*8))
                            wrapper_return_symbol_add = " = ({type})".format(type = type_string)
                            
                        else:
                            wrapper_return_define_add = "TM_Wrapper_ut_RET.TM_Wrapper_res_content.TM_Wrapper_int{bytes}[0]".format(bytes = str(return_type.size_in_bytes*8))
                            wrapper_return_symbol_add = " = ({type})".format(type = type_string)
                    
                    elif (return_type.original_type == "float" or return_type.original_type == "double" or return_type.original_type == "long double"):
                        wrapper_return_define_add = "*({orig_type} *)&TM_Wrapper_ut_RET.TM_Wrapper_res_content.TM_Wrapper_int{bytes}[0]".format(orig_type = return_type.original_type, bytes = str(return_type.size_in_bytes*8))
                        wrapper_return_symbol_add = " = ({type})".format(type = type_string)
                        
                    elif return_type.sized_C99_type == "": #and not return_type.resolved_type == None:    ##kein c99 type, also ist es ein typedef, 
                        
                        if not return_type.type_definition_kind == "enum_type":
                            wrapper_return_define_add = "(*(({type} *)((void *)TM_Wrapper_ut_RET.TM_Wrapper_res_content.TM_Wrapper_bytearray)))".format(type = return_type.original_type)   # ToRe NiEH deleted array indexes
                            
                            wrapper_return_symbol_add = " = " 
                        else:     
                            enum_tuple = return_type.evaluate_enum()
                            wrapper_return_define_add = "TM_Wrapper_ut_RET.TM_Wrapper_res_content.TM_Wrapper_{typus}[0]".format(typus = enum_tuple[0])
                            wrapper_return_symbol_add = " = ({typus}_t)".format(typus = enum_tuple[0]) 
                    else:
                        SoCoLogger.error("Error Wrapper: The correct type of {prm_name} can not be found.".format(prm_name = return_type.original_type))
                
                elif isinstance(return_type, CCompositeTypeDescriptor):
                    wrapper_return_define_add = "(*(({comp_type} {type} *)((void *)TM_Wrapper_ut_RET.TM_Wrapper_res_content.TM_Wrapper_bytearray)))".format(comp_type = return_type.kind, type = return_type.tag_identifier)
                    wrapper_return_symbol_add = " = "
                
                elif isinstance(return_type, CEnumTypeDescriptor):
                    
                    enum_tuple = return_type.evaluate_enum()
                
                    wrapper_return_define_add = "TM_Wrapper_ut_RET.TM_Wrapper_res_content.TM_Wrapper_{typus}[0]".format(typus = enum_tuple[0])
                    wrapper_return_symbol_add = " = ({typus}_t)".format(typus = enum_tuple[0]) 
                    
                else:
                    SoCoLogger.error("Error Wrapper:Can't find the CTypeDescriptor of the return value of function: {func_name}.".format(func_name = self.function_name))    
                    
            
            elif (self._search_param_type_string(return_type, 0) == "void" and isinstance(return_type, CTypeSelectionDescriptor)):
                wrapper_return_define = ""
                wrapper_return_define_add = ""
            
            if not wrapper_return_define_add == "":
                wrapper_return_define = "#define {ret_sym_}\t\t\t".format(ret_sym_ = wrapper_return_symbol) + wrapper_return_define_add
                wrapper_return_symbol += wrapper_return_symbol_add
                
                wrapper_function_call =  wrapper_return_symbol + wrapper_function_call
            
            wrapper_ifdef = "{func}_UT_WRAPPER".format(func = self.function_name)
            wrapper_ifdef = wrapper_ifdef.upper()
            wrapper_definition = "#ifndef " + wrapper_ifdef + "\n" + wrapper_signature[0:-2] + "\n{\n\n" + wrapper_function_call +  "\n}\n#endif\n\n"    
            
            wrapper_arg_defines = wrapper_arg_define + wrapper_return_define + "\n" + "\n"
            
            #print "wrapper: {sig}\n{def2}".format(sig = wrapper_signature, def2 = wrapper_definition)
            return ('extern ' + wrapper_signature, wrapper_definition, wrapper_arg_defines)
        
        #searches for the typestring in the c-code
        def _search_param_type_string(self, prm_CTypeDescriptor, prm_reset_pointercounter=1):
            
            if(prm_reset_pointercounter):
                self.type_pointercounter = 0
            
            if isinstance(prm_CTypeDescriptor, CTypeSelectionDescriptor):
                
                return prm_CTypeDescriptor.original_type
            
            elif isinstance(prm_CTypeDescriptor, CInPlaceTypeDescriptor):
                
                if prm_CTypeDescriptor.is_pointer_to:
                    self.type_pointercounter = self.type_pointercounter+1
                    return self._search_param_type_string(prm_CTypeDescriptor.included_type, 0)
            
            elif isinstance(prm_CTypeDescriptor, CCompositeTypeDescriptor):
                
                return  prm_CTypeDescriptor.kind + " " + prm_CTypeDescriptor.tag_identifier
            
            elif isinstance(prm_CTypeDescriptor, CEnumTypeDescriptor):
                
                return "enum " + prm_CTypeDescriptor.tag_identifier
                
            else:
                SoCoLogger.error("Error _search_param_type_string: The argument prm_CTypeDescriptor is no CTypeDescriptor.")
                return 
        
               
        def get_TET_test_script_stuff(self):
            
            #todo UlBr->NiEh:  hier eine zweite funktion fuer ein zweites test script mit wrapper nutzung schreiben? dass dann die Variablen in testsupportxs benutzt? incl. entsprechendem RegCmd?
            #NiEh: a tcl-script, that uses the wrapper function? How do you want to set the wrapper-structs via tcl? You want to implement a python programm that executes the task?
                         
            if self.signature_description.parameter_declarations:   
                TCL_testParam = ''
                TCL_testInvars = ''
                is_a_pointer_param = False
                #for param in self.parameter_descriptions:
                for param in self.signature_description.parameter_declarations:  
                    is_a_pointer_param = False
                    
                    pointer_indicator_list = list()
                    basic_param_typedescriptor = SourceCodeDescriptor._search_original_typedescriptor(param.type, pointer_indicator_list, [], 1)
                    #count the pointer level        
                    d = collections.Counter()
                    for c in pointer_indicator_list:
                        d[c] += 1
                    
                    if param.type.__class__ == CInPlaceTypeDescriptor:
                         if param.type.is_pointer_to:
                             if not basic_param_typedescriptor.__class__ == CFunctionSignatureTypeDescriptor:
                                  is_a_pointer_param = True
                    if is_a_pointer_param:
                        if d[0] == 1:                        
                            TCL_testParam += 'lappend matrix "PARAM {param_name}   [{function_name}_{param_name}_argument &]   [{function_name}_{param_name}_argument &]"\n'.format(function_name=self.function_name, param_name=param.identifier)
                        if d[0] == 2: 
                            TCL_testParam += 'lappend matrix "PARAM {param_name}   [{function_name}_{param_name}_argument_p &]   [{function_name}_{param_name}_argument_p &]"\n'.format(function_name=self.function_name, param_name=param.identifier)   
                            TCL_testInvars += 'lappend matrix "INVAR {function_name}_{param_name}_argument_p   0   0"\n'.format(function_name=self.function_name, param_name=param.identifier)
                        if d[0] == 3:
                            TCL_testParam += 'lappend matrix "PARAM {param_name}   [{function_name}_{param_name}_argument_p_p &]   [{function_name}_{param_name}_argument_p_p &]"\n'.format(function_name=self.function_name, param_name=param.identifier)
                            TCL_testInvars += 'lappend matrix "INVAR {function_name}_{param_name}_argument_p_p   0   0"\n'.format(function_name=self.function_name, param_name=param.identifier)
                            TCL_testInvars += 'lappend matrix "INVAR {function_name}_{param_name}_argument_p   0   0"\n'.format(function_name=self.function_name, param_name=param.identifier)
                            
                        TCL_testInvars += 'lappend matrix "INVAR {function_name}_{param_name}_argument   0   0"\n'.format(function_name=self.function_name, param_name=param.identifier)
                    else    :
                        TCL_testParam += 'lappend matrix "PARAM {param_name}   0   0"\n'.format(param_name=param.identifier)
                TCL_testParam +=  TCL_testInvars       
                
            else:
                TCL_testParam = "#** No parameters\n"
                TCL_testInvars = ''
            
             
            if self.signature_description.return_declaration.type.get_qualified_cast_or_dcl() != 'void':
                TCL_testReturn = 'lappend matrix \"RETRN                 0   0 \" \n'
            else:
                TCL_testReturn =  '#** No return value\n'
            stub_signatur = "{return_type} {func_name}_Stub ({param_decls})".format(return_type=self.signature_description.get_source_code_return_type(), func_name=self.function_name, param_decls=self.signature_description.get_source_code_param_decls())
            stub_func_decl = ''
            if self.is_static:
                stub_func_decl += 'static '
            else:
                stub_func_decl += 'extern '   
            stub_func_decl += stub_signatur + ';' 
                
            return (TCL_testParam, TCL_testReturn, stub_func_decl) 
         
            
        def GetPstdTag(self):
            result_tag = dom.Element("function")                      
              
            result_tag.setAttribute("identifier",self.function_name)
                       
            if self.is_static:
                result_tag.setAttribute("linkage","internal")
            else:
                result_tag.setAttribute("linkage","external")
              
            if READ_WRITE_TMSSC:
                result_tag.setAttribute('tms_signature_code_2008',self.signature_description.tms_signature_code_2008)
                result_tag.setAttribute('tms_signature_code_2016',self.signature_description.tms_signature_code_2016)
                
                
            funcs_sign_tags = self.signature_description.GetPstdTags()   #qual kmmt aus CFunCSigTypedesc
            for tag in funcs_sign_tags:
                result_tag.appendChild(tag)
                
            for key in sorted(list(self.callee_dict.keys())):
                tag_unit = dom.Element("callee")
                tag_unit.setAttribute('defined_in', self.callee_dict[key].unit_name)
                text = dom.Text()
                text.data = key
                tag_unit.appendChild(text)
                result_tag.appendChild(tag_unit)                
                
            
            for decl_unit in list(self.declaration_units.values()):
                tag_unit = dom.Element("declared_in")
                text = dom.Text()
                text.data = decl_unit.unit_name                          
                tag_unit.appendChild(text)
                result_tag.appendChild(tag_unit)
                                                                     
            if self.define_unit:
                tag_unit = dom.Element("defined_in")
                text = dom.Text()
                text.data = self.define_unit.unit_name                          
                tag_unit.appendChild(text)
                result_tag.appendChild(tag_unit)
            
            if WRITE_AX_INFO:
                result_tag.setAttribute('ax_info', self.ax_info)
            
            """    
            result_tag.setAttribute('code_position', self.code_position)
            """
                       
            return result_tag
            
        # end of class FunctionDescriptor   
 
    
    class VariableDescriptor:
        
        """ describes a variable in C programming language """
        
        #__slots__ = ('identifier', 'is_static', 'type_descriptor', 'SymbolLocation')
        
        def __init__(self):
            self.ax_info = ''                                                 # debugging information for reading from Axivion 
            # the next 3 members are replaced by definition_description
            #self.identifier = ''                                             # variable identifiert
            #self.type_descriptor = None                                      # variable type (instance of class derived from CTypeDescriptor)
            #self.SymbolLocation = SymbolLocationType()                                                                                                                              
            self.definition_description = SourceCodeDescriptor.Definition()   # variable defintion description (instance of type VariableDefintion)
            self.is_static = False                                            # indicator for static = local function (bool)

        def getIdentifierTypeList(self):
            return self.definition_description.getIdentifierTypeList()
            
        def getIdentifierIndexList(self):
            return [self.definition_description.getIDIndexListRecursive()]
            
        def getStatic(self):
            if self.is_static:
                return "static "
            else:
                return ""
       
                                                                                      
        def readout(self):
            self.definition_description.readout()
            print( "-> location: {ad_}".format(ad_ = self.definition_description.symbol_location_descr.location_int))  
            
        @classmethod                 
        def CreateFrom_AxivionGlobalVariableDefinition(cls, prm_soco, AxivionGlobalVariableDefinition):
            helper = cls()
            helper.SetFrom_AxivionGlobalVariableDefinition(prm_soco, AxivionGlobalVariableDefinition)
            return helper
        """
        @classmethod                 
        def CreateFrom_AxivionLocalStaticDefinition(cls, prm_soco, AxivionLocalStaticDefinition):
            helper = cls()
            helper.SetFrom_AxivionLocalStaticDefinition(prm_soco, AxivionLocalStaticDefinition)
            return helper
        """
        
        def SetFrom_AxivionGlobalVariableDefinition(self, prm_soco, AxivionGlobalVariableDefinition):    
            
            self.ax_info =  AxivionGlobalVariableDefinition.type() + ' ' + str(AxivionGlobalVariableDefinition.number())             
            
            self.definition_description.identifier = AxivionGlobalVariableDefinition.Logical.Name 
                           
            if AxivionGlobalVariableDefinition.is_of_type('Global_Static_Variable_Definition'):
                self.is_static = True
            elif AxivionGlobalVariableDefinition.is_of_type('Local_Static_Variable_Definition'):
                self.is_static = True
            elif AxivionGlobalVariableDefinition.is_of_type('Global_Variable_Definition'):
                self.is_static = False
            else:
                self.is_static = ''
                   
            self.definition_description.type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionGlobalVariableDefinition.Its_Type, self.definition_description.identifier)
            if isinstance(self.definition_description.type, CInPlaceTypeDescriptor):
                if (self.definition_description.type.is_array_of >= 0):
                    self.definition_description.set_qualifiers_from_Axivion(AxivionGlobalVariableDefinition.Its_Type.Element_Type)
                else:
                    self.definition_description.set_qualifiers_from_Axivion(AxivionGlobalVariableDefinition.Its_Type)    
            else:
                self.definition_description.set_qualifiers_from_Axivion(AxivionGlobalVariableDefinition.Its_Type)    
        
        
        """
        def SetFrom_AxivionLocalStaticDefinition(self, prm_soco, AxivionLocalStaticDefinition):    
            
            self.ax_info =  AxivionLocalStaticDefinition.type() + ' ' + str(AxivionLocalStaticDefinition.number())             
            
            self.definition_description.identifier = AxivionLocalStaticDefinition.Name 
                           

            self.is_static = True
                   
            self.definition_description.type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionLocalStaticDefinition.Its_Type, self.definition_description.identifier)
            if isinstance(self.definition_description.type, CInPlaceTypeDescriptor):
                if (self.definition_description.type.is_array_of >= 0):
                    self.definition_description.set_qualifiers_from_Axivion(AxivionLocalStaticDefinition.Its_Type.Element_Type)
                else:
                    self.definition_description.set_qualifiers_from_Axivion(AxivionLocalStaticDefinition.Its_Type)    
            else:
                self.definition_description.set_qualifiers_from_Axivion(AxivionLocalStaticDefinition.Its_Type)  
        """
        
        @classmethod                 
        def CreateFrom_PSTD(cls, prm_soco, xml_dom_variable_node):
            helper = cls()
            helper.SetFrom_PSTD(prm_soco, xml_dom_variable_node)
            return helper 
        
        def SetFrom_PSTD(self, prm_soco, xml_dom_variable_node):
            if xml_dom_variable_node.getAttribute('linkage') == 'internal':     
                self.is_static = True  
            else:
                self.is_static = False
                        
            self.definition_description.identifier = xml_dom_variable_node.getAttribute('identifier')
            
            # todo complete pstd - priority low: optional elements declared / defined in: set if read from Axivion resp. if read form pstd, write in pstd
                          
            self.definition_description.type = CTypeDescriptor.CreateFrom_PSTD(prm_soco, DomHelper.GetFirstElementChild(xml_dom_variable_node))
            
            var_type = self.definition_description.type
            if isinstance(var_type, CTypeSelectionDescriptor):
                while isinstance(var_type, CTypeSelectionDescriptor) and var_type.resolved_type:
                    var_type = var_type.resolved_type
            
            shift_node = xml_dom_variable_node      
            if isinstance(var_type, CInPlaceTypeDescriptor):    #for arrays read from most inner tag
                if var_type.is_array_of:
                    while shift_node.localName in ["array_of"]:
                        shift_node = shift_node.childNodes[0]
            
            self.definition_description.set_qualifiers_from_PSTD(DomHelper.GetFirstElementChild(shift_node))  

    
        def build_size(self, type_system, only_try=False):
            self.definition_description.type.build_size(type_system, only_try)               
        
        def build_tms_signature_code(self, prm_soco):  
            self.definition_description.type.build_tms_signature_code(prm_soco)
            if  self.definition_description.resolved_type:
                self.definition_description.resolved_type.build_tms_signature_code(prm_soco)
            
            self.definition_description.build_tms_signature_code(prm_soco)
           
        
        def GetPstdTag(self, prm_soco):
             
            result_tag = dom.Element('variable')                      
              
            result_tag.setAttribute('identifier',self.definition_description.identifier)
            
            if self.is_static:
                result_tag.setAttribute("linkage","internal")
            else:
                result_tag.setAttribute("linkage","external")  
            
            shift_node = None 
            result_childtag  = None
            
            if self.definition_description.type:
                
                result_childtag = self.definition_description.type.GetPstdTag()
                
                shift_node = result_childtag
                var_type = self.definition_description.type
                
                if isinstance(var_type, CTypeSelectionDescriptor):
                    while isinstance(var_type, CTypeSelectionDescriptor) and (var_type.resolved_type != None):  #if abfragen ausbessern
                        var_type = var_type.resolved_type
                     
                if isinstance(var_type, CInPlaceTypeDescriptor):    #for arrays write to most inner tag
                    
                    if var_type.is_array_of >= 0:
                        while shift_node.localName in ["array_of"]:
                            shift_node = shift_node.childNodes[0]
                        

            else:
                SoCoLogger.Error("There was found a variable without Typedescriptor: {var_name}".format(var_name = self.definition_description.identifier))  
                result_childtag  = dom.Element('basic_type') 
                shift_node  = result_childtag
                

            self.definition_description.add_qualifier_to_tag(shift_node)
                    
            self.definition_description.add_signature_to_tag(result_childtag, prm_soco)  
            result_tag.appendChild(result_childtag)
                    
            if WRITE_AX_INFO:
                result_tag.setAttribute('ax_info', self.ax_info)
                               
            return result_tag   
        

         
                                   
        # end of class VariableDescriptor  

                
    class TypeDefinitionDescriptor:
        
        """ describes a type definition in C programming language - this may be a named struct, union or enum type or a type alias introduced by a typedef """
        
        def __init__(self):
            
            self.ax_info = ''           # debugging information for reading from Axivion 
            self.kind = ''              # type definition kind, one of: typedef_type, struct_type, union_type, enum_type, unnamed_enum_type
            self.declaration_description = SourceCodeDescriptor.Declaration()
            self.is_local = None        # indicator for local type i.e. a type introduced within  a cfile (bool)
         
        def get_forward_declaration(self):
            fdecl_string = self.declaration_description.get_source_code_type("") 
            if(self.kind == "typedef_type"):
                if "(* )" in fdecl_string:
                    return "typedef "+ fdecl_string.replace("(* )", "(* "+self.declaration_description.identifier+")", 1)+";"
                else:
                    return "typedef "+ fdecl_string + " "+self.declaration_description.identifier+";" 
            else:
                return fdecl_string +";" 
                 
        @classmethod
        def GetFrom_AxivionType_Identifier(cls, AxivionTypedefDefinition):
            if AxivionTypedefDefinition.is_of_type('General_Typedef_Type_Definition'): 
                if AxivionTypedefDefinition.is_of_type('Typedef_Type_Definition'): 
                    identifier = CTypeDescriptor.GetFrom_AxivionType_Identifier(AxivionTypedefDefinition.Original_Type)
                else:
                    identifier = CTypeDescriptor.GetFrom_AxivionType_Identifier(AxivionTypedefDefinition)
            else:
                identifier = CTypeDescriptor.GetFrom_AxivionType_Identifier(AxivionTypedefDefinition)
                
            if identifier:
                return identifier
            else:
                return AxivionTypedefDefinition.Logical.Name
        
          
        @classmethod                 
        def CreateFrom_AxivionTypeDefinition(cls, prm_soco, AxivionTypedefDefinition):
            helper = cls()
            helper.SetFrom_AxivionTypeDefinition(prm_soco, AxivionTypedefDefinition)
            return helper
        
        def SetFrom_AxivionTypeDefinition(self, prm_soco, AxivionTypeDefinition):  
            self.ax_info =  AxivionTypeDefinition.type() + ' ' + str(AxivionTypeDefinition.number())  
            self.declaration_description.identifier = AxivionTypeDefinition.Logical.Name
            #SoCoLogger.log(70 ,"Error: TypeDescriptor.CreateFrom_AxivionType: fatal error - Axivion-Type is unknown " +str( AxivionTypeDefinition.type()) +" " + str(AxivionTypeDefinition.number()))
            if AxivionTypeDefinition.is_of_type('General_Typedef_Type_Definition'): 
                if AxivionTypeDefinition.is_of_type('Typedef_Type_Definition'):                        
                    self.kind = 'typedef_type'
                    self.declaration_description.type = CTypeDescriptor.Try_CreateTypedefReference(AxivionTypeDefinition.Original_Type)
                    if not self.declaration_description.type:
                        self.declaration_description.type = CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionTypeDefinition.Original_Type, AxivionTypeDefinition.Logical.Name)   
                    
                    self.declaration_description.set_qualifiers_from_Axivion(AxivionTypeDefinition.Original_Type) 
                    
                    """
                    if self.declaration_description.identifier in SourceCodeDescriptor.TypeDefinitionDescriptor.Dict_Typedef_type:
                        self.declaration_description = SourceCodeDescriptor.TypeDefinitionDescriptor.Dict_Typedef_type[self.declaration_description.identifier]
                    else:
                        SourceCodeDescriptor.TypeDefinitionDescriptor.Dict_Typedef_type[self.declaration_description.identifier] = self.declaration_description
                    """    
                else:
                    SocoLogger.error('TypeDefinitionDescriptor.SetFrom_AxivionTypeDefinition: uncovered heire ' + str(AxivionTypeDefinition.type()) + str(AxivionTypeDefinition.number()) + ' from General_Typedef_Type_Definition')
                    self.kind = 'typedef_uncovered_general'
                    self.declaration_description.type = AxivionTypeDefinition.type()
             
            elif AxivionTypeDefinition.is_of_type('Composite_Type_Definition') or AxivionTypeDefinition.is_of_type('Unnamed_Composite_Type_Definition'):                
                self.declaration_description.type =  CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionTypeDefinition,AxivionTypeDefinition.Logical.Name) 
                if self.declaration_description.type.kind == 'struct':
                    self.kind = 'struct_type'                    
                else:
                    self.kind = 'union_type'
                self.declaration_description.identifier = self.declaration_description.type.tag_identifier
                
                self.declaration_description.set_qualifiers_from_Axivion(AxivionTypeDefinition) 
                
                """
                if self.declaration_description.identifier in SourceCodeDescriptor.TypeDefinitionDescriptor.Dict_Composite_type:
                    self.declaration_description = SourceCodeDescriptor.TypeDefinitionDescriptor.Dict_Composite_type[self.declaration_description.identifier]
                else:
                    SourceCodeDescriptor.TypeDefinitionDescriptor.Dict_Composite_type[self.declaration_description.identifier] = self.declaration_description
                """
                                   
            elif AxivionTypeDefinition.is_of_type('Enumeration_Type_Definition') or AxivionTypeDefinition.is_of_type('Unnamed_Enumeration_Type_Definition'):               
                
                self.declaration_description.type =  CTypeDescriptor.CreateFrom_AxivionType(prm_soco, AxivionTypeDefinition,AxivionTypeDefinition.Logical.Name)
                
                if self.declaration_description.type.tag_identifier:
                    self.kind = 'enum_type'
                    self.declaration_description.identifier = self.declaration_description.type.tag_identifier
                    
                    self.declaration_description.set_qualifiers_from_Axivion(AxivionTypeDefinition) 
                    
                    """
                    if self.declaration_description.identifier in SourceCodeDescriptor.TypeDefinitionDescriptor.Dict_Enum_type:
                        self.declaration_description = SourceCodeDescriptor.TypeDefinitionDescriptor.Dict_Enum_type[self.declaration_description.identifier]
                    else:
                        SourceCodeDescriptor.TypeDefinitionDescriptor.Dict_Enum_type[self.declaration_description.identifier] = self.declaration_description
                    """ 
                else:
                    self.kind = 'unnamed_enum_type'
                    self.declaration_description.identifier = ''
                                                                     
            else:
                self.kind = 'typedef_uncovered'
                self.declaration_description.type = AxivionTypeDefinition.type()
                #print ("Uncovered Type_Definition type ", AxivionTypeDefinition.type(), " for " , AxivionTypeDefinition.Logical.Name)  
                  
           
               
        @classmethod    
        def CreateFrom_VariableDescriptor(cls, VariableDescriptor):   
            helper = cls()
            helper.SetFrom_VariableDescriptor(VariableDescriptor)
            return helper   
        
        def SetFrom_VariableDescriptor(self,VariableDescriptor): 
           
            self.declaration_description.type = VariableDescriptor.definition_description.type
            self.declaration_description.set_qualifiers_from_decl(VariableDescriptor.definition_description)
            
            if self.declaration_description.type.__class__ == CEnumTypeDescriptor:
                if self.declaration_description.type.tag_identifier == '':   
                    self.kind = 'unnamed_enum_type'
                    self.declaration_description.identifier = ''
                else:
                    self.kind = 'enum_type'
                    self.declaration_description.identifier = self.declaration_description.type.tag_identifier
                
            elif self.declaration_description.type.__class__ == CCompositeTypeDescriptor: 
                if self.declaration_description.type.kind == 'struct':
                    self.kind = 'struct_type'
                else:
                    self.kind = 'union_type'
                self.declaration_description.identifier = self.declaration_description.type.tag_identifier
                
            else:
                SoCoLogger.error('Error TypeDefinitionDescriptor.SetFrom_VariableDescriptor: unknown VariableDescriptor type') 
        
        @classmethod    
        def CreateFrom_CTypeDescriptor(cls, TypeDescriptor):    #ToDo NiEh at this point qualifier information can get lost.  
            helper = cls()
            helper.SetFrom_CTypeDescriptor(TypeDescriptor)
            return helper
                
        def SetFrom_CTypeDescriptor(self, TypeDescriptor): 
            self.declaration_description.type = TypeDescriptor 
            if self.declaration_description.type.__class__ == CCompositeTypeDescriptor: 
                if self.declaration_description.type.kind == 'struct':
                    self.kind = 'struct_type'
                else:
                    self.kind = 'union_type'  
                self.declaration_description.identifier = self.declaration_description.type.tag_identifier              
            elif self.declaration_description.type.__class__ == CEnumTypeDescriptor: 
                if self.declaration_description.type.tag_identifier: 
                    self.kind = 'enum_type'
                    self.declaration_description.identifier = self.declaration_description.type.tag_identifier
                else:
                    self.kind = 'unnamed_enum_type'
                    self.declaration_description.identifier = ''
            else:
                SoCoLogger.error('Error TypeDefinitionDescriptor.SetFrom_CTypeDescriptor: unexpeceted TypeDescriptor type') 
        
        
        @classmethod
        def GetFrom_PSTD_Identifier(cls, xml_node):
            if xml_node.nodeName == 'typedef':
                identifier = xml_node.getAttribute ('identifier')
            else:
                identifier = CTypeDescriptor.GetFrom_PSTD_Identifier(xml_node)
                
            if identifier:
                return identifier
            else:
                return None
             
        @classmethod                 
        def CreateFrom_PSTD(cls, prm_soco, xml_dom_variable_node):
            helper = cls()
            helper.SetFrom_PSTD(prm_soco, xml_dom_variable_node)
            return helper 
        
        def SetFrom_PSTD(self, prm_soco, xml_dom_typedef_node):
                                   
            
            #self.type = CTypeDescriptor.CreateFrom_PSTD(DomHelper.GetFirstElementChild(xml_dom_typedef_node)) #?! this is confusing: I think type is nt available anymore, instead type in declaration is set
            
            #ToDo NiEh wrong most inner tag, except for pointers
            self.declaration_description.set_qualifiers_from_PSTD(xml_dom_typedef_node)
                 
            if xml_dom_typedef_node.getAttribute('linkage') == 'internal':     
                self.is_local = True  
            else:
                self.is_local = False              
            
            if xml_dom_typedef_node.nodeName == 'typedef':
                self.kind = 'typedef_type'
                self.declaration_description.identifier = xml_dom_typedef_node.getAttribute ('identifier')
                self.declaration_description.type = CTypeDescriptor.CreateFrom_PSTD(prm_soco, DomHelper.GetFirstElementChild(xml_dom_typedef_node))
                
                """
                if DomHelper.GetOptionalAttribute(xml_dom_typedef_node, 'is_const', 'false') == 'true':
                    self.declaration_description.type.is_const = True
                if DomHelper.GetOptionalAttribute(xml_dom_typedef_node, 'is_volatile', 'false') == 'true':
                    self.declaration_description.type.is_volatile = True
                """
             
            elif xml_dom_typedef_node.nodeName == 'structure':        
                self.kind = 'struct_type' 
                
                identifier = CCompositeTypeDescriptor.GetFrom_PSTD_Identifier(xml_dom_typedef_node)
                if identifier and (identifier in prm_soco.Dict_Composite_Typedesc):
                    temp_typedesc = prm_soco.Dict_Composite_Typedesc[identifier]
                else:    
                    temp_typedesc = CCompositeTypeDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_typedef_node) 
                    prm_soco.Dict_Composite_Typedesc[identifier] = temp_typedesc     

                self.declaration_description.type = temp_typedesc
                self.declaration_description.identifier = self.declaration_description.type.tag_identifier
                              
            elif xml_dom_typedef_node.nodeName == 'union':        
                self.kind = 'union_type'             
                identifier = CCompositeTypeDescriptor.GetFrom_PSTD_Identifier(xml_dom_typedef_node)
                if identifier and (identifier in prm_soco.Dict_Composite_Typedesc):
                    temp_typedesc = prm_soco.Dict_Composite_Typedesc[identifier]
                else:    
                    temp_typedesc = CCompositeTypeDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_typedef_node) 
                    prm_soco.Dict_Composite_Typedesc[identifier] = temp_typedesc     

                self.declaration_description.type = temp_typedesc
                self.declaration_description.identifier = self.declaration_description.type.tag_identifier
                
            elif xml_dom_typedef_node.nodeName == 'enumeration': 
                
                identifier = CEnumTypeDescriptor.GetFrom_PSTD_Identifier(xml_dom_typedef_node)  
                if identifier and (identifier in prm_soco.Dict_Enumeration_Typedesc):
                    temp_typedesc = prm_soco.Dict_Enumeration_Typedesc[identifier]
                else:    
                    temp_typedesc = CEnumTypeDescriptor.CreateFrom_PSTD(prm_soco, xml_dom_typedef_node)  
                    prm_soco.Dict_Enumeration_Typedesc[identifier] = temp_typedesc  
                               
                self.declaration_description.type =  temp_typedesc
                
                if self.declaration_description.type.tag_identifier:
                    self.declaration_description.identifier = self.declaration_description.type.tag_identifier
                    self.kind = 'enum_type'  
                else:
                    self.declaration_description.identifier = ''
                    self.kind = 'unnamed_enum_type'                       
           
            else:
                SoCoLogger.error ('Error TypeDefinitionDescriptor.SetFrom_PSTD: unknown tag')
        
                  
        
            
        def is_it_a_typedef_TypeDefinition_for_a_typedef_TypeDefinition(self):  #ToDo NiEh endless recursion, resolved_type of typedesc/declaration is None
            if self.kind == 'typedef_type':
                if self.declaration_description.type.is_typedef_type_selection():
                    return True
                else:
                    return False
            else:
                return False
            
        def is_it_a_typedef_TypeDefinition_for_a_untaged_TypeDefinition(self):
            if self.kind == 'typedef_type':
                if (self.declaration_description.type.__class__ == CCompositeTypeDescriptor) or (self.declaration_description.type.__class__ == CEnumTypeDescriptor):
                     return True
                else:
                     return False
            else:
                return False
        
        def get_resolved_type(self):
            if self.declaration_description.resolved_type:
                return self.declaration_description.resolved_type
            else:
                SoCoLogger.warning('Warning TypeDefinitionDescriptor.get_resolved_type: should not be passed')
                return self.declaration_description.type
         
        def get_identifier_for_prefix_inference(self):
            if self.kind == 'typedef_type':
                return self.declaration_description.identifier
            elif self.kind == 'struct_type' or self.kind == 'union_type':
                return self.declaration_description.type.tag_identifier
            elif self.kind == 'enum_type':
               return self.declaration_description.type.tag_identifier
            elif self.kind == 'unnamed_enum_type':
                return self.declaration_description.type.name_value_tuples[0][0]
            else:
                SoCoLogger.error('Error: TypeDefinitionDescriptor.get_identifier_for_prefix_inference - unknown kind ' + str(self.kind))
                                     
        
        def GetPstdTag(self): 
                                                
            if self.kind == 'typedef_type':
                result_tag = dom.Element("typedef")
                result_tag.setAttribute('identifier', self.declaration_description.identifier) 
                
                if isinstance(self.declaration_description.type, CFunctionSignatureTypeDescriptor):
                    func_sig_tag = dom.Element("function_type")
                    func_tags = self.declaration_description.type.GetPstdTags()
                    for tag in func_tags:
                        func_sig_tag.appendChild(tag)
                        
                    result_tag.appendChild(func_sig_tag)
                else:
                    result_tag.appendChild(self.declaration_description.type.GetPstdTag())
                    
                    
                
                
                if self.declaration_description.type.is_typedef_type_selection ():   
                    tag_resolved_type = dom.Element('resolves_to')
                    if self.declaration_description.resolved_type:
                        if isinstance(self.declaration_description.resolved_type, CFunctionSignatureTypeDescriptor):
                            func_sig_tag = dom.Element("function_pointer")
                            func_sig_tag.setAttribute('size_in_bytes', str(self.declaration_description.type.size_in_bytes))  
                            tag_resolved_type.appendChild(func_sig_tag)
                            
                        else:
                            childtag_resolved_type = self.declaration_description.resolved_type.GetPstdTag() #verarbeitet das const nicht im type selber
                            self.declaration_description.add_qualifier_to_tag(childtag_resolved_type)
                            self.declaration_description.add_signature_to_tag(childtag_resolved_type)
                            tag_resolved_type.appendChild(childtag_resolved_type)
                        result_tag.appendChild(tag_resolved_type)
                            
                    else:
                        SoCoLogger.warning('Warning TypeDefinitionDescriptor.GetPstdTag - No PSTD-Tag \'resolves to\' written, because no resolved-type available for {id_}'.format(id_ = self.declaration_description.identifier))
                            
                
            elif self.kind == 'struct_type' or self.kind == 'union_type':  
                result_tag =  self.declaration_description.type.GetPstdTag ()      
         
            elif self.kind == 'enum_type' or self.kind == 'unnamed_enum_type':
                result_tag =  self.declaration_description.type.GetPstdTag () 

            else:
                SoCoLogger.error('Error TypeDefinitionDescriptor.GetPstdTag: unknown kind.')
                result_tag = dom.Element(self.kind)
                result_tag.setAttribute('identifier', self.declaration_description.identifier)
                result_tag.setAttribute('type', self.declaration_description.type)   
                
            #normally necessary but commented out due to pstd xsd
            #self.declaration_description.add_signature_to_tag(result_tag)
            #self.declaration_description.add_qualifier_to_tag(result_tag) 
   
             
            if self.is_local:
                result_tag.setAttribute("linkage","internal")
            else:
                result_tag.setAttribute("linkage","external")
            
            if WRITE_AX_INFO:
                result_tag.setAttribute('ax_info', self.ax_info)   
                                 
            return result_tag
        
        def build_tms_signature_code(self, prm_soco):
            if self.kind == "typedef_type":
                self.declaration_description.build_tms_signature_code(prm_soco)

    class TypedefDescriptorContainer_Iterator: # UlBr 2016-08-03: does not work; Python 2.7. vs.3 problem?
        
        def __init__(self, iter_me):
            container_to_iterate = iter_me
                    
        def __iter__(self):
            return self
        
        def __next__(self):              
            it = iter(self.container_to_iterate.typedef_TypeDefinitions.values())
            while True:
                try:
                    yield next(it)
                except StopIteration:
                    break
             
            it = iter(self.container_to_iterate.struct_TypeDefinitions.values())    
            while True:
                try:
                    yield next(it)
                except StopIteration:
                    break
                
            it = iter(self.container_to_iterate.union_TypeDefinitions.values())   
            while True:
                try:
                    yield next(it)
                except StopIteration:
                    break
                
            it = iter(self.container_to_iterate.enum_TypeDefinitions.values())   
            while True:
                try:
                    yield next(it)
                except StopIteration:
                    break
                
            for td in self.container_to_iterate.unnamed_enum_TypeDefinitions:            
                yield td
             
            raise StopIteration       
    
    class TypeDefinitionDescriptorContainer: # typedf type, struct type or (unanamed) enum type
               
        def __init__(self):        
            # to do clean up: may be summarized in one dictionary?    only possible if struct/enum/union is part of the id
            self.typedef_TypeDefinitions = {}
            self.struct_TypeDefinitions = {}
            self.union_TypeDefinitions = {}
            self.enum_TypeDefinitions = {}
            self.unnamed_enum_TypeDefinitions = []

            
        def __len__(self):
            return len(self.typedef_TypeDefinitions) + len(self.struct_TypeDefinitions)  + len(self.union_TypeDefinitions)  + len(self.enum_TypeDefinitions) + len(self.unnamed_enum_TypeDefinitions)
        
        #ToDo clean up, this is needed!?
        
        def __iter__(self):
            # return SourceCodeDescriptor.TypedefDescriptorContainer_Iterator(self)
            it = iter(self.typedef_TypeDefinitions.values())
            while True:
                try:
                    yield next(it)
                except StopIteration:
                    break
              
            it = iter(self.struct_TypeDefinitions.values())    
            while True:
                try:
                    yield next(it)
                except StopIteration:
                    break
                 
            it = iter(self.union_TypeDefinitions.values())   
            while True:
                try:
                    yield next(it)
                except StopIteration:
                    break
                 
            it = iter(self.enum_TypeDefinitions.values())   
            while True:
                try:
                    yield next(it)
                except StopIteration:
                    break
                 
            for td in self.unnamed_enum_TypeDefinitions:            
                yield td

            raise StopIteration 
        

        def get_all_typedefinitions_dict(self):  
            new_dict = dict()  
            new_dict.update(self.typedef_TypeDefinitions)
            new_dict.update(self.struct_TypeDefinitions)
            new_dict.update(self.union_TypeDefinitions)
            new_dict.update(self.enum_TypeDefinitions)
            
            return new_dict

           
        def is_in(self, check_me_TypeDefinition):    
            if check_me_TypeDefinition.kind == 'typedef_type':
                return check_me_TypeDefinition.declaration_description.identifier in self.typedef_TypeDefinitions                   
            elif check_me_TypeDefinition.kind == 'struct_type':
                return check_me_TypeDefinition.declaration_description.identifier in self.struct_TypeDefinitions   
            elif check_me_TypeDefinition.kind == 'union_type':
                return check_me_TypeDefinition.declaration_description.identifier in self.union_TypeDefinitions 
            elif check_me_TypeDefinition.kind == 'enum_type':
                return check_me_TypeDefinition.declaration_description.identifier in self.enum_TypeDefinitions 
            #elif check_me_TypeDefinition.kind == 'unnamed_enum_type' # not handled because call for unnamed type does not make sense
            
        def return_typedef_if_existing(self, check_me_TypeDefinition):
            if self.is_in(check_me_TypeDefinition):    
                if check_me_TypeDefinition.kind == 'typedef_type':
                        return self.typedef_TypeDefinitions[check_me_TypeDefinition.declaration_description.identifier]                  
                elif check_me_TypeDefinition.kind == 'struct_type':
                        return self.struct_TypeDefinitions[check_me_TypeDefinition.declaration_description.identifier]   
                elif check_me_TypeDefinition.kind == 'union_type':
                        return self.union_TypeDefinitions[check_me_TypeDefinition.declaration_description.identifier]  
                elif check_me_TypeDefinition.kind == 'enum_type':
                        return self.enum_TypeDefinitions[check_me_TypeDefinition.declaration_description.identifier]
            else:
                return None
            #elif check_me_TypeDefinition.kind == 'unnamed_enum_type' # not handled because call for unnamed type does not make sense
                        
        def get_typedef_for_kind_and_ID(self, kind, identifier):     
            if kind == 'typedef_type':
                return self.get_alias_typedef_for_identifer(identifier)                  
            elif kind == 'struct_type':
                 return self.get_struct_typedef_for_identifer(identifier)   
            elif kind == 'union_type':
                 return self.get_union_typedef_for_identifer(identifier) 
            elif kind == 'enum_type':
                 return self.get_enum_typedef_for_identifer(identifier) 
            else:           
                return None
            
        def get_typedef_for_anykind_and_ID(self, identifier, prm_soco = None):     
            
            buffer = self.get_alias_typedef_for_identifer(identifier)
            if buffer:
                return buffer  
            
            buffer = self.get_struct_typedef_for_identifer(identifier)
            if buffer:
                return buffer 
            
            buffer = self.get_union_typedef_for_identifer(identifier) 
            if buffer:
                return buffer  
            
            buffer = self.get_enum_typedef_for_identifer(identifier)  
            if buffer:
                return buffer   
            

            """Think about this Nikolai."""
            if prm_soco:
                if identifier in prm_soco.Dict_Selection_Typedesc:
                    return prm_soco.Dict_Selection_Typedesc[identifier]
                
            
            else:
                SoCoLogger.error("Did not find any tag-name or typedef for: {id_}".format(id_ = identifier))
                return None
                          
            
        def get_alias_typedef_for_identifer(self, identifier):
            return self.typedef_TypeDefinitions.get(identifier,None) 
           
        def get_struct_typedef_for_identifer(self, identifier):
            return self.struct_TypeDefinitions.get(identifier,None) 
        
        def get_union_typedef_for_identifer(self, identifier):
            return self.union_TypeDefinitions.get(identifier,None) 
        
        def get_enum_typedef_for_identifer(self, identifier):
            return self.enum_TypeDefinitions.get(identifier,None) 
        
        @classmethod
        def get_alias_typedef_for_identifer_in_container_list(cls, identifier, container_list):
            for container in container_list:
                typedesc = container.get_alias_typedef_for_identifer(identifier)
                if typedesc:
                    return typedesc    
            return None
        
        @classmethod          
        def get_struct_typedef_for_identifer_in_container_list(cls, identifier, container_list):
            for container in container_list:
                typedesc = container.get_struct_typedef_for_identifer(identifier)
                if typedesc:
                    return typedesc    
            return None 
        
        @classmethod
        def get_union_typedef_for_identifer_in_container_list(cls, identifier, container_list):
            for container in container_list:
                typedesc = container.get_union_typedef_for_identifer(identifier)
                if typedesc:
                    return typedesc    
            return None  
        
        @classmethod
        def get_enum_typedef_for_identifer_in_container_list(cls, identifier, container_list):
            for container in container_list:
                typedesc = container.get_enum_typedef_for_identifer(identifier)
                if typedesc:
                    return typedesc    
            return None 
        
        @classmethod
        def get_resolved_type (cls, type, typedef_container_list): 
            if (type.__class__ == CCompositeTypeDescriptor):
                if type.tag_identifier:
                    if type.kind == 'struct':
                        #print('jau, struct in get_resolved_type als included type')
                        return  SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_struct_typedef_for_identifer_in_container_list(type.tag_identifier, typedef_container_list).declaration_description.type
                    else: # type.kind == 'union':
                        #print('jau, union in get_resolved_type als included type')
                        return SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_union_typedef_for_identifer_in_container_list(type.tag_identifier, typedef_container_list).declaration_description.type
                else:
                    return None
            elif (type.__class__ == CEnumTypeDescriptor):
                if type.tag_identifier:
                    #print('jau, enum in get_resolved_type als included type')    
                    return SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_enum_typedef_for_identifer_in_container_list(type.tag_identifier, typedef_container_list).declaration_description.type  
                else:
                    return None
            else:     
                return type

            
            
        def add_typedef(self, add_me_TypeDefinitions, container=""):
            
            if self.is_in(add_me_TypeDefinitions):
                if container:
                    str_container = " in " + container + "-typedef-container"
                else:
                    str_container = ""
                SoCoLogger.info("The typedef {id_} already exists{cont_}.".format(id_ = str(add_me_TypeDefinitions.declaration_description.identifier), cont_ = str_container))
                return
            
            if add_me_TypeDefinitions.kind == 'typedef_type':
                    self.typedef_TypeDefinitions[add_me_TypeDefinitions.declaration_description.identifier] = add_me_TypeDefinitions
            elif add_me_TypeDefinitions.kind == 'struct_type':
                    self.struct_TypeDefinitions[add_me_TypeDefinitions.declaration_description.identifier] = add_me_TypeDefinitions
            elif add_me_TypeDefinitions.kind == 'union_type':
                    self.union_TypeDefinitions[add_me_TypeDefinitions.declaration_description.identifier] = add_me_TypeDefinitions
            elif add_me_TypeDefinitions.kind == 'enum_type':
                    self.enum_TypeDefinitions[add_me_TypeDefinitions.declaration_description.identifier] = add_me_TypeDefinitions
            elif add_me_TypeDefinitions.kind == 'unnamed_enum_type':
                    self.unnamed_enum_TypeDefinitions.append(add_me_TypeDefinitions)           
            else:
                SoCoLogger.warning('Warning TypeDefinitionDescriptorContainer.add_typedef: unknown kind ' + str(add_me_TypeDefinitions.kind))


                    
        def resolve_to_a_non_typedef_TypeDefinition_for_a_typedef_TypeDefinition (self, typedef):  
            if typedef.is_it_a_typedef_TypeDefinition_for_a_typedef_TypeDefinition():                                  
                found_typedef = self.get_alias_typedef_for_identifer(typedef.declaration_description.type.original_type)  #ToDo NiEh, has to search for aliase as along as there are no CComposites.  
                
                if typedef == found_typedef:
                    SoCoLogger.warning("Warning a typedef has same name with a tag for enum/struct/union: {tag_}. The Resolved type is searched in the containers for enum/struct/union.".format(tag_ = typedef.declaration_description.type.original_type))
                    
                    found_typedef = self.get_struct_typedef_for_identifer(typedef.declaration_description.type.original_type) 
                    if not found_typedef:
                        found_typedef = self.get_union_typedef_for_identifer(typedef.declaration_description.type.original_type) 
                    if not found_typedef:
                        found_typedef = self.get_enum_typedef_for_identifer(typedef.declaration_description.type.original_type) 
                    
                    
                if found_typedef:
                    if found_typedef.is_it_a_typedef_TypeDefinition_for_a_typedef_TypeDefinition(): 
                        return self.resolve_to_a_non_typedef_TypeDefinition_for_a_typedef_TypeDefinition(found_typedef) 
                    else:
                        return found_typedef                                                                         
                else:
                    return typedef    
            else:
                return typedef  
                           
         
        def resolve_TypeDefinitions (self, typedef_container_list): 
            
            for typedef in list(self.struct_TypeDefinitions.values()):
                typedef.declaration_description.resolved_type =  typedef.declaration_description.type 
                
            for typedef in list(self.union_TypeDefinitions.values()):
                typedef.declaration_description.resolved_type = typedef.declaration_description.type
                
            for typedef in list(self.enum_TypeDefinitions.values()):
                typedef.declaration_description.resolved_type = typedef.declaration_description.type
                
            for typedef in self.unnamed_enum_TypeDefinitions:
                typedef.declaration_description.resolved_type = typedef.declaration_description.type

                       
            for typedef in list(self.typedef_TypeDefinitions.values()):
                
                if  typedef.kind != 'typedef_type':     #programm error?!
                    # i.e. it is a struct, union or (unnamed) enum t
                    typedef.declaration_description.resolved_type = typedef.type
                    SoCoLogger.error('Error TypeDefinitionDescriptorContainer.resolve_TypeDefinitions: typdef.kind is not typdef_type but ' + str(typedef.kind) + ' for identifier ' + str(typdef.declaration_description.identifier))
                    
                else:
                
                    typedef.declaration_description.resolved_type = None
                    
                    resolved_typedef = None
                    
                    # resolve to a point where you have a typedef that is not an alias for another type
                    if not typedef.is_it_a_typedef_TypeDefinition_for_a_typedef_TypeDefinition(): 
                        #i.e. it is a typedef for a struct, union or (unnamed) enum type
                        resolved_typedef = typedef                        
                    else:
                        #print('resolving ', typedef.identifier)
                        resolved_typedef = typedef
                        if typedef_container_list:
                            
                            for typedef_container in typedef_container_list:                                   
                                resolved_typedef = typedef_container.resolve_to_a_non_typedef_TypeDefinition_for_a_typedef_TypeDefinition(resolved_typedef)
                                if not resolved_typedef.is_it_a_typedef_TypeDefinition_for_a_typedef_TypeDefinition():                                                   
                                    
                                    #ToDo NiEh strange this is never shown, so no resolving is executed for forward declaration
                                    #_TETEST_used_taged_forw_typedefed_internal_enum_t still shows typedef_type
                                    """
                                    if resolved_typedef.declaration_description.identifier == "_TETEST_used_taged_forw_typedefed_internal_enum_t": 
                                        typedef.declaration_description.type.readout()
                                        resolved_typedef.declaration_description.type.readout()
                                    """
 
                                    break   
                                
                                
                            if resolved_typedef.is_it_a_typedef_TypeDefinition_for_a_typedef_TypeDefinition():                                                                                     
                                SoCoLogger.warning('Error TypeDefinitionDescriptorContainer.resolve_TypeDefinitions: (even not in global container) ' + str(typedef.declaration_description.identifier))
                        else:
                           SoCoLogger.error('Error TypeDefinitionDescriptorContainer.resolve_TypeDefinitions: ' + str(typedef.declaration_description.identifier))                             
                    

                    # now resolved_typedef is a typedef for either a basic type selection or for a struct, union or (unnamed) enum type or for a in-place-type-composition
                    if resolved_typedef.declaration_description.type.is_non_basic_type_selection():
                        if  resolved_typedef.declaration_description.type.type_definition_kind == 'struct_type':
                            if typedef_container_list:
                                for typedef_container in typedef_container_list:
                                    found_struct_typedef = typedef_container.struct_TypeDefinitions.get(resolved_typedef.declaration_description.type.original_type)
                                    if found_struct_typedef:
                                        typedef.declaration_description.resolved_type = found_struct_typedef.declaration_description.type 
                                        break 
                                    
                        elif  resolved_typedef.declaration_description.type.type_definition_kind == 'union_type':
                              if typedef_container_list:
                                for typedef_container in typedef_container_list:
                                    found_union_typedef = typedef_container.union_TypeDefinitions.get(resolved_typedef.declaration_description.type.original_type)
                                    if found_union_typedef:
                                        typedef.declaration_description.resolved_type = found_union_typedef.declaration_description.type 
                                        break
                                    
                        elif resolved_typedef.declaration_description.type.type_definition_kind == 'enum_type':
                             if typedef_container_list:
                                for typedef_container in typedef_container_list:
                                    found_enum_typedef = typedef_container.enum_TypeDefinitions.get(resolved_typedef.declaration_description.type.original_type)
                                    if found_enum_typedef:
                                        typedef.declaration_description.resolved_type = found_enum_typedef.declaration_description.type  
                                        break
                                        
                        elif resolved_typedef.declaration_description.type.type_definition_kind == 'unnamed_enum_type':
                            #todo completion: generate code example and test processing for typedef for typdef for unnmaend enum
                            SoCoLogger.error('Error TypeDefinitionDescriptorContainer.resolve_TypeDefinitions: unhandled unnamed_enum_type ' + str(resolved_typedef.declaration_description.type.type_definition_kind) + ' for identifier ' + str(typdef.declaration_description.identifier))
                        
                        else:
                            pass
                            #print ('Error in resolve_TypeDefinitions: unknown resolved_typedef.type.type_definition_kind ', resolved_typedef.declaration_description.type.type_definition_kind, ' for identifier ', typdef.declaration_description.identifier)
                                        
                    else:
                        typedef.declaration_description.resolved_type = resolved_typedef.declaration_description.type
                
                if typedef.declaration_description.resolved_type == None:
                    SoCoLogger.info('Error TypeDefinitionDescriptorContainer.resolve_TypeDefinitions: not resovlved_type for ' +str( typedef.declaration_description.identifier))
         
        
        #called at end of pstd import
        def resolve_type_selections (self, typedef_container_list):    
                        
            for typedef in list(self.typedef_TypeDefinitions.values()):
                typedef.declaration_description.type.resolve_type_selections(typedef_container_list)
            
            for typedef in list(self.struct_TypeDefinitions.values()):          
                typedef.declaration_description.type.resolve_type_selections(typedef_container_list) 
                
            for typedef in list(self.union_TypeDefinitions.values()):
                typedef.declaration_description.type.resolve_type_selections(typedef_container_list)  
                
            for typedef in list(self.enum_TypeDefinitions.values()):
                typedef.declaration_description.type.resolve_type_selections(typedef_container_list) 
                
            for typedef in self.unnamed_enum_TypeDefinitions:
                typedef.declaration_description.type.resolve_type_selections(typedef_container_list)   

     
                 
        def build_size(self, type_system, only_try=False):
                        
            for typedef in list(self.typedef_TypeDefinitions.values()):
                typedef.declaration_description.type.build_size(type_system, only_try) 
            
            for typedef in list(self.struct_TypeDefinitions.values()):               
                typedef.declaration_description.type.build_size(type_system, only_try) 
                
            for typedef in list(self.union_TypeDefinitions.values()):
                typedef.declaration_description.type.build_size(type_system, only_try) 
                   
            for typedef in list(self.enum_TypeDefinitions.values()):
                typedef.declaration_description.type.build_size(type_system, only_try) 
                
            for typedef in self.unnamed_enum_TypeDefinitions:
                typedef.declaration_description.type.build_size(type_system, only_try) 
                 
                 
        def build_tms_signature_code(self, prm_soco):  
            
            #typedef_container_list_ext = [self]
            #typedef_container_list_ext.extend(typedef_container_list) 
            for typedef in list(self.typedef_TypeDefinitions.values()):
                typedef.build_tms_signature_code(prm_soco)     
   
            for typedef in list(self.struct_TypeDefinitions.values()):
                typedef.declaration_description.type.build_tms_signature_code(prm_soco) 
                
            for typedef in list(self.union_TypeDefinitions.values()):
                typedef.declaration_description.type.build_tms_signature_code(prm_soco) 
                
            for typedef in list(self.enum_TypeDefinitions.values()):
                typedef.declaration_description.type.build_tms_signature_code(prm_soco)     
                
            for typedef in self.unnamed_enum_TypeDefinitions:
                typedef.declaration_description.type.build_tms_signature_code(prm_soco)   
        
                
        def GetPstdTags(self): 
                                                   
            tag_typedef_list = []
            
            keys_list = list(self.typedef_TypeDefinitions.keys())
            keys_list.sort()
            for key in keys_list:     
                typedef = self.typedef_TypeDefinitions[key]            
                if not typedef.declaration_description.type.not_readable:                    
                    result_tag = typedef.GetPstdTag() 
                    tag_typedef_list.append(result_tag)  
                    
            keys_list = list(self.struct_TypeDefinitions.keys())
            keys_list.sort()
            for key in keys_list:     
                typedef = self.struct_TypeDefinitions[key]            
                if not typedef.declaration_description.type.not_readable:                    
                    result_tag = typedef.GetPstdTag() 
                    tag_typedef_list.append(result_tag) 
                    
            keys_list = list(self.union_TypeDefinitions.keys())
            keys_list.sort()
            for key in keys_list:     
                typedef = self.union_TypeDefinitions[key]            
                if not typedef.declaration_description.type.not_readable:                    
                    result_tag = typedef.GetPstdTag() 
                    tag_typedef_list.append(result_tag) 
                    
            keys_list = list(self.enum_TypeDefinitions.keys())
            keys_list.sort()
            for key in keys_list:     
                typedef = self.enum_TypeDefinitions[key]            
                if not typedef.declaration_description.type.not_readable:                    
                    result_tag = typedef.GetPstdTag() 
                    tag_typedef_list.append(result_tag) 
                    
            for typedef in self.unnamed_enum_TypeDefinitions:
                if not typedef.declaration_description.type.not_readable:
                    result_tag = typedef.GetPstdTag() 
                    tag_typedef_list.append(result_tag)                               
                                               
            return tag_typedef_list 
                                      

    class UnitDescriptor:
     
        # class attributes
        
        ReModulePrefix = re.compile('^_?(T_)?([A-Z]+)[0-9]{0,2}_') 
        
        def __init__(self): 
        
            # files_description
            self.unit_source_dir = ""
            self.unit_source_name = ""
            self.unit_source_extension = ""           
            self.unit_header_dir = ""
            self.unit_header_name = ""
            self.unit_header_extension = ""
            
            # derived: files_description
            # todo refactoring: filepath to naming convention
            self.unit_name = ''
            self.header_filepath = ''
            self.source_filepath =  ''
            
            # derived: unit_prefix        
            self.unit_prefix = ''  
            
            # is part of a software component?
            self.SWC_name = '' # todo testi (UlBr 2016-08-03): change into a list?
            
            # functions_description assigned to this unit accoding to the following rules:
            # - if a function is defined and no declaration is existing: home unit is the defining unit 
            # - if a function is defined and a declaration for the same unit is existing: home unit is the defining unit (independent if declarations in other units exist)
            # - if a function is defined and a declaration for the same unit is not existing:
            #   - in an application project: home unit is the defining unit
            #   - in a library projet: home unit is the first found declaring unit
            # - if a function is not defined:
            #   - only one declaratiosn is existing: home unit is the declaring unit
            #   - more then one declaratiosn is existing: try to find the home unit on the basis of the unit prefix: if the the prefix matches: home unit is matching unit; else: home unit is the first found declaring unit

            self.function_descriptions_local_dict = {}
            self.function_descriptions_global_dict = {}
            self.variable_descriptions_dict = {}
            self.ext_variable_descriptions_dict = {}
            self.typedef_container = SourceCodeDescriptor.TypeDefinitionDescriptorContainer() # non local typedefs are additionally contained in SourceCodeDescriptor.global_typedef_container 
            
            self.available_typedef_container_list = []
            
            # cfile include stuff
            self.cfile_is_included_in_cfile_of_unit = None  # cfile is included in cifle of other unit and may therefor uses local typedefs of that other unit
            self.cfile_includes_cfile_of_unit = []   # list of UnitDescriptor instances
            
            #hfile include stuff
            self.included_headers = []
            self.external_variables = []
            
            # from symbol file
            self.compiler = ''
                                  
            #end of UnitDescriptor.__init__(self, AxivionUnitDefinition): 
            
        def __str__(self):
            return_value = 'UnitDescriptor Instanz: '
            return_value += ' named ' + self.unit_name + ' is prefixed with ' + self.unit_prefix + '; '
            return_value += 'source_filepath ' + self.source_filepath  + ' consists of '
            return_value += self.unit_source_dir + ' ' + self.unit_source_name + ' ' + self.unit_source_extension + '; ' 
            return_value += 'header_filepath ' + self.header_filepath  + ' consists of '
            return_value += self.unit_header_dir + ' ' + self.unit_header_name + ' ' + self.unit_header_extension + '; '
            return return_value
            
        
        def readout(self):
            return_value = 'UnitDescriptor Instanz: '
            return_value += ' named ' + self.unit_name + ' is prefixed with ' + self.unit_prefix + '; '
            return_value += 'source_filepath ' + self.source_filepath  + ' consists of '
            return_value += self.unit_source_dir + ' ' + self.unit_source_name + ' ' + self.unit_source_extension + '; ' 
            return_value += 'header_filepath ' + self.header_filepath  + ' consists of '
            return_value += self.unit_header_dir + ' ' + self.unit_header_name + ' ' + self.unit_header_extension + '; '
            print(return_value)
                      
        @classmethod  
        def CreateFrom_Values(cls, dir, name, extension):  
            helper = cls()
            helper.SetFrom_Values(dir, name, extension)
            return helper
        
        def SetFrom_Values(self, dir, name, extension):
            if extension.lower() == CFILE_EXTENSION:
                self.unit_source_dir = dir.replace('/', '\\') 
                self.unit_source_name = name
                self.unit_source_extension = extension
            elif extension.lower() == HEADER_EXTENSION:
                self.unit_header_dir = dir.replace('/', '\\') 
                self.unit_header_name = name
                self.unit_header_extension = extension
            else:
                SoCoLogger.error('Error UnitDescriptor.SetFrom_Values: nothing set because unhandled extension ' + str(extension))
                        
        @classmethod                 
        def CreateFrom_AxivionUnitDefiniton(cls, AxivionUnitDefiniton):
            helper = cls()
            helper.Set_FilesDesrciption_FromAxivionUnitDefinition(AxivionUnitDefiniton)
            return helper
        
        def Set_FilesDesrciption_FromAxivionUnitDefinition(self, AxivionUnitDefinition):             
            #self.unit_source_dir, basename = os.path.split(AxivionUnitDefinition.Primary_Contents.Used_Name) 
            dir, basename = os.path.split(AxivionUnitDefinition.Primary_Contents.Parent.Full_Name) 
            self.unit_source_dir = dir.replace('/', '\\') 
            self.unit_source_name, self.unit_source_extension = os.path.splitext(basename)     
        
                        
        @classmethod                 
        def CreateFrom_PSTD(cls, prm_soco, xml_dom_unit_node):
            helper = cls()
            helper.SetFrom_PSTD(prm_soco, xml_dom_unit_node)
            return helper
        
        def SetFrom_PSTD(self, prm_soco ,xml_dom_unit_node):
            self.unit_name = xml_dom_unit_node.getAttribute('name')
            #print('read unit_name: ', self.unit_name )
            
            self.unit_prefix =xml_dom_unit_node.getAttribute('prefix')
            
            self.header_filepath = DomHelper.GetOptionalAttribute(xml_dom_unit_node,'header','')
            if self.header_filepath:
                self.unit_header_dir, basename = os.path.split(self.header_filepath) 
                self.unit_header_name, self.unit_header_extension = os.path.splitext(basename)
                
            self.source_filepath = DomHelper.GetOptionalAttribute(xml_dom_unit_node,'cfile','')
            if self.source_filepath:
                self.unit_source_dir, basename = os.path.split(self.source_filepath) 
                self.unit_source_name, self.unit_source_extension = os.path.splitext(basename)
            
            included_cfile_names = DomHelper.GetOptionalAttribute(xml_dom_unit_node,'included_cfile','')  
            if included_cfile_names:
                self.cfile_includes_cfile_of_unit = included_cfile_names.split(',')
                
            for child in xml_dom_unit_node.childNodes:
                if child.nodeName == 'typedef' or child.nodeName == 'structure' or child.nodeName == 'union' or child.nodeName == 'enumeration': #What happens if a typedef has same name with structname
                    typedef_id = SourceCodeDescriptor.TypeDefinitionDescriptor.GetFrom_PSTD_Identifier(child)
                    

                    if typedef_id in prm_soco.all_locals_typedef_container:
                        SoCoLogger.error("Warning double declared identifier for typedef {typedef_id_} !".format(typedef_id_ = typedef_id))                       
                        continue
                    else:
                        typedef_descr = SourceCodeDescriptor.TypeDefinitionDescriptor.CreateFrom_PSTD(prm_soco, child)
                        self.typedef_container.add_typedef(typedef_descr, container=self.unit_name)  
                        prm_soco.all_locals_typedef_container.add_typedef(typedef_descr, container="all-locals")
                        
            
            for child in xml_dom_unit_node.childNodes:
                if child.nodeName == 'function':   
                    func_descr = SourceCodeDescriptor.FunctionDescriptor.CreateFrom_PSTD(prm_soco, child)
                    if func_descr.is_static:
                        self.function_descriptions_local_dict[func_descr.function_name] = func_descr
                    else:
                        self.function_descriptions_global_dict[func_descr.function_name] = func_descr
                elif child.nodeName == 'variable':   
                    var_descr = SourceCodeDescriptor.VariableDescriptor.CreateFrom_PSTD(prm_soco, child)
                    if not (var_descr.definition_description.identifier in self.variable_descriptions_dict):                     
                        self.variable_descriptions_dict[var_descr.definition_description.identifier] = var_descr
                    else:
                        SoCoLogger.error("Variable {var_} read from PSTD-File is already registered for unit: {unit_}".format(var_ = var_descr.definition_description.identifier, unit_ = self.unit_name))
                                     
                                     
                       
        def BuildDerived_FilesDescription(self): 
            if  self.unit_header_name:
                self.header_filepath =  os.path.join (self.unit_header_dir, self.unit_header_name) + ".h"
            if self.unit_source_name:
                self.source_filepath =  os.path.join (self.unit_source_dir, self.unit_source_name) + ".c"   
                
            if self.unit_source_name:
                self.unit_name =  self.unit_source_name
            elif self.unit_header_name:
                self.unit_name =  self.unit_header_name      
   
                
        def BuildDerived_UnitPrefix(self):  
             
            self.unit_prefix = ''
               
                       
            if self.unit_prefix:
                #print('found prefix from extenral n.th. pedef for ', self.unit_name, ': ', self.unit_prefix)
                return
                          
            for typedef_descr in self.typedef_container:    
                if typedef_descr.is_local == False:
                    MeModulePrefix = self.ReModulePrefix.match(typedef_descr.get_identifier_for_prefix_inference())
                    if MeModulePrefix:
                        self.unit_prefix = MeModulePrefix.group(2)  
                        break
                        
            if self.unit_prefix:               
                #print('found prefix from external typedef for ', self.unit_name, ': ', self.unit_prefix)
                return
                       
            for func_descr in list(self.function_descriptions_global_dict.values()):  
                if func_descr.is_static == False:                                      
                    MeModulePrefix = self.ReModulePrefix.match(func_descr.function_name)  
                    if MeModulePrefix:
                        self.unit_prefix = MeModulePrefix.group(2) 
                        break 
                                                                 
                    
            if self.unit_prefix:              
                #print('found prefix from external function for ', self.unit_name, ': ', self.unit_prefix)
                return
            
                           
            for var_descr in list(self.variable_descriptions_dict.values()):   
                if var_descr.is_static == False:
                    MeModulePrefix = self.ReModulePrefix.match(var_descr.definition_description.identifier) 
                    if MeModulePrefix:
                        self.unit_prefix = MeModulePrefix.group(2) 
                        break
                        
            if self.unit_prefix:
                #print('found prefix from external variable for ', self.unit_name, ': ', self.unit_prefix)
                return
          
            for func_descr in list(self.function_descriptions_global_dict.values()):  
                if func_descr.is_static:                  
                    MeModulePrefix = self.ReModulePrefix.match(func_descr.function_name)  
                    if MeModulePrefix:
                        self.unit_prefix = MeModulePrefix.group(2) 
                        break                        
                                    
            if self.unit_prefix:
                #print('found prefix from internal function for ', self.unit_name, ': ', self.unit_prefix)
                return
            
                             
            for typedef_descr in self.typedef_container:    
                if typedef_descr.is_local:
                    MeModulePrefix = self.ReModulePrefix.match(typedef_descr.get_identifier_for_prefix_inference())
                    if MeModulePrefix:
                        self.unit_prefix = MeModulePrefix.group(2)  
                        break
                                                          
            if self.unit_prefix:
                #print('found prefix from internal typedef for ', self.unit_name, ': ', self.unit_prefix)
                return
                         
            for var_descr in list(self.variable_descriptions_dict.values()):   
                if var_descr.is_static:
                    MeModulePrefix = self.ReModulePrefix.match(var_descr.definition_description.identifier) 
                    if MeModulePrefix:
                        self.unit_prefix = MeModulePrefix.group(2) 
                        break
                    
            if self.unit_prefix:
                #print('found prefix from internal variable for ', self.unit_name, ': ', self.unit_prefix)
                return
                
                        
        def DefinesSomething(self):
            if (len(self.function_descriptions_local_dict) > 0) or (len(self.variable_descriptions_dict)) > 0 or (len(self.typedef_container) > 0):    
                return True
            else:
                return False
                
        def GetPstdTag(self, prm_soco):
            tag_unit = dom.Element("unit")
            
            tag_unit.setAttribute("name",self.unit_name)
            tag_unit.setAttribute("prefix",self.unit_prefix[:6])    #Workaround NiEh: added [:6] because of restriction in xsd-file
            if self.header_filepath:
                tag_unit.setAttribute("header",self.header_filepath)
            if self.source_filepath:    
                tag_unit.setAttribute("cfile",self.source_filepath)         
            if self.cfile_includes_cfile_of_unit:
                tag_unit.setAttribute("included_cfile",','.join([unit.unit_name for unit in self.cfile_includes_cfile_of_unit]))   
                
                         
            keys_list = list(self.function_descriptions_local_dict.keys())
            keys_list.sort()
            for key in keys_list:           
                tag_function = self.function_descriptions_local_dict[key].GetPstdTag()                         
                tag_unit.appendChild(tag_function)
             
            keys_list = list(self.function_descriptions_global_dict.keys())
            keys_list.sort()
            for key in keys_list:     
                tag_function = self.function_descriptions_global_dict[key].GetPstdTag()                        
                tag_unit.appendChild(tag_function)
                            
            keys_list = list(self.variable_descriptions_dict.keys())
            keys_list.sort()
            for key in keys_list:    
                tag_variable = self.variable_descriptions_dict[key].GetPstdTag(prm_soco)                          
                tag_unit.appendChild(tag_variable)  
            
            for result_tag in self.typedef_container.GetPstdTags():
                tag_unit.appendChild(result_tag)  
            
            return tag_unit

        def BuildAvailableTypedefContainer_List(self, global_typedef_container):
            
            self.available_typedef_container_list =  [self.typedef_container]
            
            if self.cfile_is_included_in_cfile_of_unit:
                
                actual_unit = self.cfile_is_included_in_cfile_of_unit
                
                while actual_unit:
                    self.available_typedef_container_list.append(actual_unit.typedef_container)
                    actual_unit = actual_unit.cfile_is_included_in_cfile_of_unit  
                
            self.available_typedef_container_list.append(global_typedef_container)  

            
        # end of class UnitDescriptor
    
    def __init__(self):
    
        self.basepath = ''
        self.project_name = ''
        self.project_folder = ''
        
        self.pstd_file = ""
        self.psyd_file = ""
        
        self.generator = ''
        self.origin = ''
        self.created=''
        self.encoding = ''
        
        self.type_system_ID = ''
        self.type_system = {}
        
        self.matching_units = []
        self.matching_units_upper = []
        
        self.unit_dictionary = {}
        
        self.global_typedef_container = self.TypeDefinitionDescriptorContainer()  # contained typedefs are also contained in unit.typedef_container
        self.all_locals_typedef_container = self.TypeDefinitionDescriptorContainer()    #will store all local typedefs to be sure, they are not redundant.
        
        self.Dict_Enumeration_Typedesc = dict()     #dict for all instances derived from CEnumTypeDescriptor, cares for no redundant creation
        self.Dict_Composite_Typedesc = dict()       #dict for all instances derived from CCompositeTypeDescriptor, cares for no redundant creation 
           
        self.Dict_Selection_Typedesc = dict()       #dict for all instances derived from CSelectionTypeDescriptor, cares for no redundant creation
        
        
        self.global_function_descriptions_dict = {}
        
        #A list only there to check if variable/typedef/function is defined more than one time
        self.variable_list = []
        self.function_list = [] 
        
        #global dict to save all enumerators
        self.enum_dict = dict()
        self.coverage_vars = dict()
    """
    def load_dicts_for_CTypeDescriptor(self):
        CTypeDescriptor.Dict_Enumeration_Typedesc = self.Dict_Enumeration_Typedesc
        CTypeDescriptor.Dict_Composite_Typedesc   = self.Dict_Composite_Typedesc
        CTypeDescriptor.Dict_Selection_Typedesc   = self.Dict_Selection_Typedesc
        
    def unload_dicts_for_CTypeDescriptor(self):
        CTypeDescriptor.Dict_Enumeration_Typedesc = None
        CTypeDescriptor.Dict_Composite_Typedesc   = None
        CTypeDescriptor.Dict_Selection_Typedesc   = None
    
       
    def load_typedef_global_all_local_containers_in_CTypeDescriptor(self):
        TypeDefinitionDescriptorContainer.Loaded_all_locals_typedef_container = self.all_locals_typedef_container
        
    def unload_typedef_global_all_local_containers_in_CTypeDescriptor(self):
        TypeDefinitionDescriptorContainer.Loaded_all_locals_typedef_container = None
    
    def load_typesystem(self):
        SourceCodeDescriptor.type_system = self.type_system
        
    def unload_typesystem(self):
        SourceCodeDescriptor.type_system = None   
        
    def load_classattributes_for_typedescription(self):
        self.load_dicts_for_CTypeDescriptor()
        self.load_typedef_global_all_local_containers_in_CTypeDescriptor()
        self.load_typedef_global_all_local_containers_in_CTypeDescriptor()
        
    def unload_classattributes_for_typedescription(self):
        self.unload_dicts_for_CTypeDescriptor()
        self.unload_typedef_global_all_local_containers_in_CTypeDescriptor()
        self.unload_typedef_global_all_local_containers_in_CTypeDescriptor()
    """
    
    #Typeresolving is complicated, those functions are not used at the moment, because its better to create a new typedef-reference for generation of stub-vars, wrappers, ...
    """    
    def set_typedescriptor_to_declaration_from_typedef_id_with_Axivion_type(self, axivion_type, declaration):
        typedef_id = CTypeDescriptor.GetFrom_AxivionType_Identifier(axivion_type)
        return self.set_type_of_id_to_declaration(typedef_id, declaration)
        
    def set_typedescriptor_to_declaration_from_typedef_id_with_PSTD_tag(self, pstd_tag, declaration):
        typedef_id = CTypeDescriptor.GetFrom_PSTD_Identifier(pstd_tag)
        return self.set_type_of_id_to_declaration(typedef_id, declaration)
    
        
    def set_type_of_id_to_declaration(self, typedef_id, declaration):
        if typedef_id:
            typedef = self.all_locals_typedef_container.get_typedef_for_anykind_and_ID(typedef_id, self)
            if isinstance(typedef, SourceCodeDescriptor.TypeDefinitionDescriptor):
                if typedef.declaration_description.type:
                    print("passiert")
                    declaration.type = typedef.declaration_description.type
                    declaration.resolved_type = typedef.declaration_description.resolved_type
                    return True
            elif isinstance(typedef, CTypeSelectionDescriptor):
                print("passiert")
                declaration.type = typedef
                declaration.resolved_type = typedef.resolved_type
                return True
            else:
                return None
        else:
            return None
    """
    #UnitDesc.function_descriptions_local_dict = {}
    #UnitDesc.function_descriptions_global_dict = {}
        
    def getKPIDict(self):
        KPI_dict = {}
        for unit in  self.unit_dictionary:
            for var in self.unit_dictionary[unit].variable_descriptions_dict:
                if var.endswith("_Line"):
                    function_name = var.rsplit("_Line", 1)[0]
                    func_desc = self.get_function_descriptor_by_string(function_name)
                    unit_name = func_desc.define_unit.unit_name
                    
                    if not unit_name in KPI_dict:
                        KPI_dict[unit_name] = {}
                                           
                    covr_var_desc = self.unit_dictionary[unit].variable_descriptions_dict[var]
                    if not covr_var_desc == None: 
                        type = covr_var_desc.definition_description.resolved_type
                        size = type.is_array_of
                        
                        for i in range(0, size):
                            var_index = var+"["+str(i)+"]"
                            KPI_dict[unit_name][var_index] = 0
                               
        return KPI_dict                        
                        
    def get_function_descriptor_by_string(self, prm_name_str):
        for unit in self.unit_dictionary.values():
            if prm_name_str in unit.function_descriptions_global_dict:
                #print unit.function_descriptions_global_dict[prm_name_str].SymbolLocation.location_int 
                return unit.function_descriptions_global_dict[prm_name_str]
            elif prm_name_str in unit.function_descriptions_local_dict:
                #print unit.function_descriptions_local_dict[prm_name_str].SymbolLocation.location_int 
                return unit.function_descriptions_local_dict[prm_name_str]
        return None                        
                
    
    @classmethod  
    def CreateFrom_AxivionGraphFilepath(cls, AxivionGraphFilepath, project_name, project_folder, type_system):
        helper = cls()
        helper.SetFrom_AxivionGraphFilepath(AxivionGraphFilepath, project_name, project_folder, type_system)
        return helper
    

    def SetFrom_AxivionGraphFilepath(self, AxivionGraphFilepath, project_name, project_folder, type_system_ID, isLibrary = False):
        
        from bauhaus import ir
        from bauhaus import scanner
        
        
        def _extract_typedefs(unit, is_local):
            #SoCoLogger.warning('slef class: ', self.__class__)
            for descendant in File.descendants_of_type('Named_Type_Definition'):
                typedef_id = SourceCodeDescriptor.TypeDefinitionDescriptor.GetFrom_AxivionType_Identifier(descendant)
                if typedef_id in self.all_locals_typedef_container:#ToDo NiEh, rethink
                    SoCoLogger.error("Warning double declared identifier {typedef_id_} !".format(typedef_id_ = typedef_id))                       
                    continue
                else:
                    typedef = SourceCodeDescriptor.TypeDefinitionDescriptor.CreateFrom_AxivionTypeDefinition(self, descendant)                    

                    typedef.is_local = is_local                    
                    
                    if typedef.declaration_description.type.not_readable:
                        if (hasattr(typedef,'identifier')):
                            SoCoLogger.error('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the type ' + str(typedef.identifier) + ' defined in ' + str(File.Full_Name))
                        else:
                            SoCoLogger.error('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read an anonymous type defined in ' + str(File.Full_Name))
                        read_error = True                                          
                                      
                    if typedef.declaration_description.identifier:                       
                        if unit.typedef_container.is_in(typedef):
                            if typedef.is_local:
                                SoCoLogger.warning ('Warning: double local typedef identifier ' + str(typedef.declaration_description.identifier) + ' can not be differed - may lead to unability to build stubs etc. ')
                            pass
                        else:
                            unit.typedef_container.add_typedef(typedef, container=unit.unit_name)
                            self.all_locals_typedef_container.add_typedef(typedef, container="all-locals")
                            if typedef.is_local == False:
                                self.global_typedef_container.add_typedef(typedef, container="global")
    
                    else:                      
                        SoCoLogger.error("Error SetFrom_AxivionGraphFilepath: TypeDefinitionDescriptor must have an identifier!")
                        read_error = True
                                       
                                     
            for descendant in File.descendants_of_type('Unnamed_Enumeration_Type_Definition'):         
                typedef = SourceCodeDescriptor.TypeDefinitionDescriptor.CreateFrom_AxivionTypeDefinition(self, descendant)  

                typedef.is_local = is_local 
                    
                if typedef.declaration_description.type.not_readable:
                    SoCoLogger.error ('Error in SourceCodeDescriptor.SetFrom_AxivionGraphFilepath: Axivion can not read the type for an unnamed enum defined in ' + str(File.Full_Name))
                    read_error = True
                
                else:
                    unit.typedef_container.add_typedef(typedef, container=unit.unit_name)
                    self.all_locals_typedef_container.add_typedef(typedef, container="all-locals")
                    
            ### end of_extract_typedefs  
                
        def _process_header(unit):  
            
            for descendant in File.descendants_of_type('Global_Extern_Variable_Declaration'): 
                if not descendant.Logical.Name in unit.external_variables:
                    unit.external_variables.append(descendant.Logical.Name)
                    
                    variable =  SourceCodeDescriptor.VariableDescriptor.CreateFrom_AxivionGlobalVariableDefinition(self, descendant)  
                
                    if variable.definition_description.type.not_readable:
                        SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the type for variable  ' + str(variable.definition_description.identifier) + ' defined in ' + str(File.Full_Name))   
                        read_error = True                 
                    
                    else:
                        if not (variable.definition_description.identifier in self.variable_list):       
                            unit.ext_variable_descriptions_dict[variable.definition_description.identifier] = variable
            
            #todo pstd completion: add global variable declaration
            for descendant in File.descendants_of_type(['User_Include_Declaration']):
                if not descendant.Used_Name in unit.included_headers:
                    unit.included_headers.append(descendant.Used_Name)

            
            for descendant in File.descendants_of_type('Static_Function_Declaration'): 
                func_descr = unit.function_descriptions_local_dict.get(descendant.Logical.Name)
                if not func_descr:
                    func_descr = SourceCodeDescriptor.FunctionDescriptor.CreateFrom_AxivionFunctionDefiniton(self, descendant)                  
                    
                    if func_descr.signature_description.not_readable:
                        SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read  a type (return or parameter) for static function ' + str(func_descr.function_name) + ' declared in ' + str(File.Full_Name))
                        read_error = True
                    else:

                        #unit.function_descriptions_local.append(func_descr)  
                        unit.function_descriptions_local_dict[func_descr.function_name] =  func_descr

                            
                func_descr.declaration_units.setdefault(unit.unit_name, unit)
                      
            for descendant in File.descendants_of_type('Function_Declaration'): 
                if descendant.is_of_type('Static_Function_Declaration'): # handled above
                    continue
                
                func_descr = self.global_function_descriptions_dict.get(descendant.Logical.Name)
                if not func_descr:
                    func_descr = SourceCodeDescriptor.FunctionDescriptor.CreateFrom_AxivionFunctionDefiniton(self, descendant)
                    
                    if func_descr.signature_description.not_readable:
                        SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the a type (return or parameter) for static function ' + str(func_descr.function_name) + ' declared in ' + str(File.Full_Name))
                        read_error = True
                    else: 
                        self.global_function_descriptions_dict[func_descr.function_name] =  func_descr

                
                func_descr.declaration_units.setdefault(unit.unit_name, unit)
            
            ### end of: _process_header 
            
        def _process_cfile(unit):   
                      
            #_extract_typedefs(unit, True) 
            
            #todo pstd completion: add global variable declaration
            for descendant in File.descendants_of_type(['User_Include_Declaration']):
                if not descendant.Used_Name in unit.included_headers:
                    unit.included_headers.append(descendant.Used_Name)
                
            for descendant in File.descendants_of_type('Global_Extern_Variable_Declaration'): 
                if not descendant.Logical.Name in unit.external_variables:
                    unit.external_variables.append(descendant.Logical.Name)
                    variable =  SourceCodeDescriptor.VariableDescriptor.CreateFrom_AxivionGlobalVariableDefinition(self, descendant)  
                
                    if variable.definition_description.type.not_readable:
                        SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the type for variable  ' + str(variable.definition_description.identifier) + ' defined in ' + str(File.Full_Name))   
                        read_error = True                 
                    
                    else:
                        if not (variable.definition_description.identifier in self.variable_list):       
                            unit.ext_variable_descriptions_dict[variable.definition_description.identifier] = variable
                
            for descendant in File.descendants_of_type(['Local_Variable_Definition']):
                if descendant.is_of_type("Local_Static_Variable_Definition"):
                    variable =  SourceCodeDescriptor.VariableDescriptor.CreateFrom_AxivionGlobalVariableDefinition(self, descendant)  
                
                    if variable.definition_description.type.not_readable:
                        SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the type for variable  ' + str(variable.definition_description.identifier) + ' defined in ' + str(File.Full_Name))   
                        read_error = True                 
                
                    else:
                        if not (variable.definition_description.identifier in self.variable_list):
                            self.variable_list.append(variable.definition_description.identifier)         
                            unit.variable_descriptions_dict[variable.definition_description.identifier] = variable
                        else:
                            SoCoLogger.error("Variable: {var_} of unit: {unit_} already exists, therefore it is denied.".format(var_ = variable.definition_description.identifier, unit_ = unit.unit_name))
           
            for descendant in File.descendants_of_type(['Global_Variable_Definition']):
                variable =  SourceCodeDescriptor.VariableDescriptor.CreateFrom_AxivionGlobalVariableDefinition(self, descendant)  
                
                if variable.definition_description.type.not_readable:
                    SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the type for variable  ' + str(variable.definition_description.identifier) + ' defined in ' + str(File.Full_Name))   
                    read_error = True                 
                
                else:
                    if not (variable.definition_description.identifier in self.variable_list):
                        self.variable_list.append(variable.definition_description.identifier)         
                        unit.variable_descriptions_dict[variable.definition_description.identifier] = variable
                    else:
                        SoCoLogger.error("Variable: {var_} of unit: {unit_} already exists, therefore it is denied.".format(var_ = variable.definition_description.identifier, unit_ = unit.unit_name))
                                    
            for descendant in File.descendants_of_type('Static_Function_Declaration'): 
                func_descr = unit.function_descriptions_local_dict.get(descendant.Logical.Name)
                if not func_descr:
                    #SoCoLogger.warning('jau hier decl of static ', descendant.Logical.Name)
                    func_descr = SourceCodeDescriptor.FunctionDescriptor.CreateFrom_AxivionFunctionDefiniton(self, descendant)                  
                    
                    if func_descr.signature_description.not_readable:
                        SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the a type (return or parameter) for static function ' + str( func_descr.function_name) + ' declared in ' + str(File.Full_Name))
                        read_error = True
                    else:
                        #unit.function_descriptions_local.append(func_descr)  
                        unit.function_descriptions_local_dict[func_descr.function_name] =  func_descr
                    
                func_descr.declaration_units.setdefault(unit.unit_name, unit)
                
             
            for descendant in File.descendants_of_type('Static_Function_Definition'):
                func_descr = unit.function_descriptions_local_dict.get(descendant.Logical.Name)
                if not func_descr:
                    #SoCoLogger.warning('jau hier def of static ', descendant.Logical.Name)
                    func_descr = SourceCodeDescriptor.FunctionDescriptor.CreateFrom_AxivionFunctionDefiniton(self, descendant)
                    
                    if func_descr.signature_description.not_readable:
                        SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the a type (return or parameter) for static function ' + str(func_descr.function_name) + ' defined in ' + str(File.Full_Name))
                        read_error = True
                    else:
                        #unit.function_descriptions_local.append(func_descr)   
                        if not (func_descr.function_name in self.function_list): 
                            self.function_list.append(func_descr.function_name)
                            unit.function_descriptions_local_dict[func_descr.function_name] =  func_descr
                        else:
                            SoCoLogger.error("Function: {func_} of unit: {unit_} already exists, therefore it is denied.".format(func_ = func_descr.function_name, unit_ = unit.unit_name))
                
                func_descr.define_unit = unit
            
                if func_descr:
                    func_descr.addPhysicalPosition(descendant)
                    
            for descendant in File.descendants_of_type('Function_Definition'): # static and external !!!
                
                if descendant.is_of_type('Static_Function_Definition'): # handled above
                    continue

                func_descr = unit.function_descriptions_local_dict.get(descendant.Logical.Name) 
                if not func_descr:   # i.e. is global because local was found above
                    #if len(descendant.Logical.Declarations) == 0:                           
                        #SoCoLogger.warning(descendant.Logical.Name, ' only defined')
                    func_descr = self.global_function_descriptions_dict.get(descendant.Logical.Name)
                    if not func_descr:
                        func_descr = SourceCodeDescriptor.FunctionDescriptor.CreateFrom_AxivionFunctionDefiniton(self, descendant)      
                        
                        if func_descr.signature_description.not_readable:
                            SoCoLogger.error ('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath:  Axivion can not read the a type (return or parameter) for global function ' + str( func_descr.function_name) + ' defined in ' + str(File.Full_Name))
                            read_error = True
                        else:  
                            if not (func_descr.function_name in self.function_list): 
                                self.function_list.append(func_descr.function_name)
                                self.global_function_descriptions_dict[func_descr.function_name] =  func_descr 
                            else:
                                SoCoLogger.error("Function: {func_} of unit: {unit_} already exists, therefore it is denied.".format(func_ = func_descr.function_name, unit_ = unit.unit_name))                                 
                                    
                     
                    func_descr.define_unit = unit  
                    
                if func_descr:
                    func_descr.addPhysicalPosition(descendant)
                             
            ### end of: _process_cfile        
                                 
        def _extract_all_included_cfile_names(file_of_file_contents):
            result_list = [] 
            for descendant in file_of_file_contents.descendants_of_type('Plain_User_Include_Declaration'):  
                desc_dir, desc_name, desc_ext = split_filepath(descendant.Used_Name)           
                if desc_ext.lower() == CFILE_EXTENSION:  
                    result_list.append(desc_name)
                    #todo testi: if include of cfile via a header: call _extract_all_included_cfile_names for header and cfiles
                    result_list.extend(_extract_all_included_cfile_names(descendant.Contents))
                    
            return result_list

        
        
        read_error = False
        
        Axivion_IR_Graph = ir.Graph(AxivionGraphFilepath) 
        outfile = AxivionGraphFilepath + '.copy'
        
        #SoCoLogger.warning('project_name: ', project_name, 'project_folder: ', project_folder, 'type_system_ID: ', type_system_ID)
    
        dir = Axivion_IR_Graph.basepath()
        self.basepath = dir.replace('/', '\\')      
        self.project_name = project_name
        if project_folder:
            self.project_folder = project_folder.replace('/', '\\')  
        else:
            self.project_folder = ''
                
        if type_system_ID:
            self.type_system_ID = type_system_ID   
            #SoCoLogger.warning('Generation for type_system_ID: ', type_system_ID)        
        else:
            self.type_system_ID = 'default'
            SoCoLogger.warning("Warning SetFrom_AxivionGraphFilepath: No type_system specified!")
                        
        self.type_system = TypeSystemDescriptor(self.type_system_ID)
        SourceCodeDescriptor.type_system = self.type_system
        
        # SoCoLogger.warning('project_name: ', project_name, 'project_folder: ', project_folder, 'type_system_ID: ', type_system_ID)
                   
        # create the basic unit descriptions (dir, name, etc.)
        for File in Axivion_IR_Graph.nodes_of_type(ir.Physical, 'File'):
            
            dir, name, ext = split_filepath(File.Full_Name)
                                 
            if ext.lower() != HEADER_EXTENSION and ext.lower() != CFILE_EXTENSION:
                if name == '.axivion' and ext == '.preinc':
                    continue
                else:
                    SoCoLogger.error('Error SourceCodeDescriptor._extract_all_included_cfile_names: Unexpected file extension ' + str(ext) + ' of File' + str(File.Full_Name) + ' in SourceCodeDescriptor.SetFrom_AxivionGraphFilepath!')
                    continue
                
            unit = self.unit_dictionary.get(name, None)
            if unit:
                unit.SetFrom_Values(dir, name, ext)
            else:
                unit = self.UnitDescriptor.CreateFrom_Values(dir, name, ext)
                self.unit_dictionary[name] = unit 
                
        for unit in list(self.unit_dictionary.values()):  
            unit.BuildDerived_FilesDescription()

        
        # extract: which cfile is included in which cfile?
        for File in Axivion_IR_Graph.nodes_of_type(ir.Physical, 'Primary_File'):
                    
            dir, name, ext = split_filepath(File.Full_Name)
                                  
            if  ext.lower() != CFILE_EXTENSION:
                SoCoLogger.error('Error SourceCodeDescriptor._extract_all_included_cfile_names: Unexpected file extension ' + str(ext) + ' of Primary_File in SourceCodeDescriptor.SetFrom_AxivionGraphFilepath!')
                continue 
                               
            included_cfile_names = _extract_all_included_cfile_names(File)      
                             
            # assumption: no cfile is included in more then one cfile  
            if included_cfile_names:  
                including_unit =  self.unit_dictionary[name]
                for incl_cfile_name in included_cfile_names:
                    unit_of_included_cfile =  self.unit_dictionary[incl_cfile_name]
                    including_unit.cfile_includes_cfile_of_unit.append(unit_of_included_cfile)
                    unit_of_included_cfile.cfile_is_included_in_cfile_of_unit = including_unit
                
        
        # collect typedef of process system inlcude files i.e. #include <...>.h     
        for File in Axivion_IR_Graph.nodes_of_type(ir.Physical, 'System_Include_File'):
            
            dir, name, ext = split_filepath(File.Full_Name)              
                    
            if ext.lower() != HEADER_EXTENSION:
                SoCoLogger.error('Error SourceCodeDescriptor.SetFrom_AxivionGraphFilepath: Unexpected file extension ' + str(ext) + ' of System_Include_File in SourceCodeDescriptor.SetFrom_AxivionGraphFilepath!')
                continue
            
            unit = self.unit_dictionary[name]      
            _extract_typedefs(unit, False)
        
        
        for File in Axivion_IR_Graph.nodes_of_type(ir.Physical, 'User_Include_File'):
           
            dir, name, ext = split_filepath(File.Full_Name)
                                                                        
            if  ext.lower() == HEADER_EXTENSION:                
                unit = self.unit_dictionary[name]
                _extract_typedefs(unit, False) 
                
            elif ext.lower() == CFILE_EXTENSION:               
                unit = self.unit_dictionary[name]                                  
                _extract_typedefs(unit, True)  
                
            else:
                SoCoLogger.warning('Warning SourceCodeDescriptor._extract_all_included_cfile_names: Unexpected file extension ' + str(ext) + ' of User_Include_File: ' + str(File.Full_Name) + ' -> no  evaluation of this file content!')
                continue 
        
                # process all translations units                         
        for File in Axivion_IR_Graph.nodes_of_type(ir.Physical, 'Primary_File'):
        
            dir, name, ext = split_filepath(File.Full_Name)
                                                                                    
            if  ext.lower() != CFILE_EXTENSION:
                SoCoLogger.error('Error SourceCodeDescriptor._extract_all_included_cfile_names: Unexpected file extension ' + str(ext) + ' of Primary_File in SourceCodeDescriptor.SetFrom_AxivionGraphFilepath!')
                continue   
            
            unit = self.unit_dictionary[name]          
            _extract_typedefs(unit, True)  
        
        if not read_error: # 2016-02-05 UlBr: to be on the save side because there was a not reproducable problem with an endless loop if an ErrorTypeSelection occured
            self._resolve_types_in_all_local_global()
            
        # process system inlcude files i.e. #include <...>.h     
        for File in Axivion_IR_Graph.nodes_of_type(ir.Physical, 'System_Include_File'):
            
            #SoCoLogger.warning(File.Full_Name, ' is a ',  File.type())  
            
            dir, name, ext = split_filepath(File.Full_Name)
            
            #SoCoLogger.warning('processing ', File.Full_Name)                
                    
            if ext.lower() != HEADER_EXTENSION:
                SoCoLogger.error('Error SourceCodeDescriptor._extract_all_included_cfile_names: Unexpected file extension ' + str(ext) + ' of System_Include_File in SourceCodeDescriptor.SetFrom_AxivionGraphFilepath!')
                continue
                       
             
            unit = self.unit_dictionary[name]      
            _process_header(unit)   
                   
         
        # process all included files (header nd cfiles)        
        for File in Axivion_IR_Graph.nodes_of_type(ir.Physical, 'User_Include_File'):
           
            dir, name, ext = split_filepath(File.Full_Name)
            
           # SoCoLogger.warning(File.Full_Name, ' is a ',  File.type())  
                                                                        
            if  ext.lower() == HEADER_EXTENSION:                
                unit = self.unit_dictionary[name]
                _process_header(unit)  
                
            elif ext.lower() == CFILE_EXTENSION:               
                unit = self.unit_dictionary[name]                                  
                _process_cfile(unit)   

                 
            else:
                SoCoLogger.warning('Warning SourceCodeDescriptor._extract_all_included_cfile_names: Unexpected file extension ' + str(ext) + ' of User_Include_File: ' + str(File.Full_Name) + ' -> no  evaluation of this file content!')
                continue 
          
      
        # process all translations units                         
        for File in Axivion_IR_Graph.nodes_of_type(ir.Physical, 'Primary_File'):
            
            #SoCoLogger.warning(File.Full_Name, ' is a ',  File.type())  
        
            dir, name, ext = split_filepath(File.Full_Name)
                                                     
            #included_cfile_names = _extract_all_included_cfile_names(File)

                                            
            if  ext.lower() != CFILE_EXTENSION:
                SoCoLogger.error('Error SourceCodeDescriptor._extract_all_included_cfile_names: Unexpected file extension ' + str(ext) + ' of Primary_File in SourceCodeDescriptor.SetFrom_AxivionGraphFilepath!')
                continue   
            
            unit = self.unit_dictionary[name]          
            _process_cfile(unit)    
 
        # home unit: for stubing: find a 'home' unit for every function: the stub function is defined for the home unit
        # - if a function is defined and no declaration is existing: home unit is the defining unit 
        # - if a function is defined and a declaration for the same unit is existing: home unit is the defining unit (independent if declarations in other units exist)
        # - if a function is defined and a declaration for the same unit is not existing:
        #   - in an application project: home unit is the defining unit
        #   - in a library projet: home unit is the first found declaring unit
        #- if a function is not defined:
        #   - only one declaratiosn is existing: home unit is the declaring unit
        #   - more then one declaratiosn is existing: try to find the home unit on the basis of the unit prefix: if the the prefix matches: home unit is matching unit; else: home unit is the first found declaring unit

                    
        # for stubbing: find a 'home' unit for every function: for functions only declared and defined in the same unit
        for func_descr in list(self.global_function_descriptions_dict.values()):        
            # declaration and defintion in same unit   
            if (func_descr.define_unit != None):   
                home_unit = func_descr.declaration_units.get(func_descr.define_unit.unit_name) 
                if home_unit:
                    home_unit.function_descriptions_global_dict[func_descr.function_name] = func_descr
                                    
        # for stubbing: find a 'home' unit for every function: for functions not only declared and defined in the same unit                            
        for func_descr in list(self.global_function_descriptions_dict.values()): 
            
            # find home_unit
            home_unit = None
                
            if len(func_descr.declaration_units) == 0: # only a definiton      
                home_unit = func_descr.define_unit                
                                                              
            else:
                
                if (func_descr.define_unit != None): 
                    home_unit = func_descr.declaration_units.get(func_descr.define_unit.unit_name) 
                    
                    if home_unit:
                         home_unit = None # declaration and defintion in same unit: covered by default above
                    else: # there is a defintion but declaration is not in same unit
                        if isLibrary:
                            home_unit = list(func_descr.declaration_units.values())[0]  # in Library: declaring unit
                        else:
                            home_unit = func_descr.define_unit # in applying project: defining unit  
                            #SoCoLogger.warning('exception: ', func_descr.function_name, ' assigned to ', home_unit.unit_name)                      
                
                else: # no definition
                    if len(func_descr.declaration_units) == 1: # no defintion - one declaration
                        home_unit = list(func_descr.declaration_units.values())[0]   
                
                    else: # no defintion - more then one declaration                                  
                        # find matching prefixes - if more then one with same prefix: shortest unit name
                        # if not matching prefix: take first declaration unit
                        pot_home_units = []
                        MeModulePrefix = SourceCodeDescriptor.UnitDescriptor.ReModulePrefix.match(func_descr.function_name) 
                        if MeModulePrefix:
                            func_descr_prefix = MeModulePrefix.group(2) 
                            for pot_home_unit in list(func_descr.declaration_units.values()):
                                if pot_home_unit.unit_prefix == func_descr_prefix:
                                   pot_home_units.append(pot_home_unit);
                                   
                        if pot_home_units:
                            if len(pot_home_units) == 1:
                                home_unit = pot_home_units[0]
                            else:
                                min_length = min([len(pot.unit_name) for pot in pot_home_units])
                                for pot in pot_home_units:
                                    if len(pot.unit_name) == min_length:
                                        home_unit = pot
                                        break  
                        else:
                            #SoCoLogger.warning('Warning: no home unit found for function: ', func_descr.function_name)
                            for home_unit in list(func_descr.declaration_units.values()):
                                break
                            
                    if home_unit == None:
                        SoCoLogger.warning('Warning SourceCodeDescriptor._extract_all_included_cfile_names: no home unit found for function: ' + str(func_descr.function_name))
                                                            
            if home_unit:                            
                home_unit.function_descriptions_global_dict[func_descr.function_name] = func_descr
                
         
        # build unit prefix    
        for unit in list(self.unit_dictionary.values()):   
            unit.BuildDerived_UnitPrefix()                        
                     
        
        if not read_error: # 2016-02-05 UlBr: to be on the save side because there was a not reproducable problem with an endless loop if an ErrorTypeSelection occured
            self._resolve_types ()   
            self._build_size_and_signature() 
        
                                   
    
    @classmethod  
    def CreateFrom_CreateFrom_PSTD_Filepath(cls, PSTD_Filepath):
        helper = cls()
        helper.SetFrom_PSTD_Filepath(PSTD_Filepath)
        return helper
             
    def SetFrom_PSTD_Filepath(self,PSTD_Filepath):
                
        if not os.path.exists(PSTD_Filepath):
             print('Warning SourceCodeDescriptor.AddFrom_PSYD_Filepath: no_symbol file')
             return
                        
        self.pstd_file = PSTD_Filepath
        
        baum = dom.parse(PSTD_Filepath)
        
        SoCoLogger.info('Adding from structure file ...')
        
        if baum.firstChild.nodeName != 'project_structure':
            SoCoLogger.error('Error Fatal SourceCodeDescriptor.SetFrom_PSTD_Filepath: wrong root element ' + str(baum.firstChild.nodeName) + ' - project_structure expected!')
            
            
        for child in baum.firstChild.childNodes:
            if child.nodeName == 'basepath':
                self.basepath = child.firstChild.data
                
            elif child.nodeName == 'type_system':
                self.type_system_ID = DomHelper.GetOptionalAttribute(child, 'type_system_ID', 'default')
                self.type_system = TypeSystemDescriptor(self.type_system_ID)
                SourceCodeDescriptor.type_system = self.type_system
                if self.type_system_ID == 'default':
                    SoCoLogger.warning("Warning SourceCodeDescriptor.SetFrom_PSTD_Filepath: No type_system specified!")
                
            elif child.nodeName == 'project_folder':   
                self.project_folder = child.firstChild.data    
                
            elif child.nodeName == 'project_name':   
                self.project_name = child.firstChild.data
                
            elif child.nodeName == 'unit':
                unit_descr = self.UnitDescriptor.CreateFrom_PSTD(self, child)
                self.unit_dictionary[unit_descr.unit_name] = unit_descr   
        
        #SoCoLogger.warning('project_name: ', self.project_name, 'project_folder: ', self.project_folder, 'type_system_ID: ', self.type_system_ID)        
        
        # replace names of including and included cfiles by unit
        for including_unit in list(self.unit_dictionary.values()): 
            if including_unit.cfile_includes_cfile_of_unit:
                for incl_cfile_name in including_unit.cfile_includes_cfile_of_unit:
                    self.unit_dictionary[incl_cfile_name].cfile_is_included_in_cfile_of_unit = including_unit 
                including_unit.cfile_includes_cfile_of_unit = [self.unit_dictionary[incl_cfile_name] for incl_cfile_name in including_unit.cfile_includes_cfile_of_unit]
        
        
         # replace define and declaration unit names by unit
        for unit in list(self.unit_dictionary.values()):  
            # for global functions
            for function in list(unit.function_descriptions_global_dict.values()):
                # replace define unit name by unit
                if function.define_unit:
                    function.define_unit = self.unit_dictionary[function.define_unit]
                # replace declaration unit names by unit    
                for key in list(function.declaration_units.keys()):
                    function.declaration_units[key] = self.unit_dictionary[key]      
            # for local functions                  
            for function in list(unit.function_descriptions_local_dict.values()):   
                if function.define_unit:
                    function.define_unit = self.unit_dictionary[function.define_unit]
                for key in list(function.declaration_units.keys()):
                    function.declaration_units[key] = self.unit_dictionary[key]
        
        
        # build up global functions definitions dict
        for unit in list(self.unit_dictionary.values()):  
            for function in list(unit.function_descriptions_global_dict.values()):
                self.global_function_descriptions_dict [function.function_name] = function
                
        
        # build up global type definitions container
        for unit in list(self.unit_dictionary.values()):      
            for typedef in unit.typedef_container:    
                if typedef.declaration_description.identifier:
                    if typedef.is_local == False:
                        self.global_typedef_container.add_typedef(typedef, container = "global")
                        
        self._resolve_types_in_all_local_global()                        
        self._resolve_types() 
        self._build_size_and_signature()    
        

    def _resolve_types_in_all_local_global(self):
        
        for unit in list(self.unit_dictionary.values()):
            unit.BuildAvailableTypedefContainer_List(self.global_typedef_container)
            
         # for all type definiton types: resolve type defnition types
        for unit in list(self.unit_dictionary.values()):  
            unit.typedef_container.resolve_TypeDefinitions(unit.available_typedef_container_list)   
        
        # for all type definiton types: resolve type selctions    
        for unit in list(self.unit_dictionary.values()):  
            unit.typedef_container.resolve_type_selections(unit.available_typedef_container_list)   

                  
    #global typedef container is not resolved?!
    def _resolve_types(self):
        
        """
        for unit in list(self.unit_dictionary.values()):
            unit.BuildAvailableTypedefContainer_List(self.global_typedef_container)
            
         # for all type definiton types: resolve type defnition types
        for unit in list(self.unit_dictionary.values()):  
            unit.typedef_container.resolve_TypeDefinitions(unit.available_typedef_container_list)   
        
        # for all type definiton types: resolve type selctions    
        for unit in list(self.unit_dictionary.values()):  
            unit.typedef_container.resolve_type_selections(unit.available_typedef_container_list)                      
        """                        
        
        # for functions and variables: resolve type selections                                                               
        for unit in list(self.unit_dictionary.values()):  #hier muss man differenzieren zwischen wirklichem resolven und der zuweisung eines resolved_types
                          
            # for all variables with linkage: build size and signature  
            for var_descr in list(unit.variable_descriptions_dict.values()):
                var_descr.definition_description.resolve_type_selections(unit.available_typedef_container_list)
                 
            # for all lokal functions: build size and signature for return and parameter types                 
            for function in list(unit.function_descriptions_local_dict.values()):   
                function.signature_description.resolve_type_selections(unit.available_typedef_container_list) 
            
            # for all global functions: build size and signature for return and parameter types
            for function in list(unit.function_descriptions_global_dict.values()):
                function.signature_description.resolve_type_selections(unit.available_typedef_container_list)                
                
            #SoCoLogger.warning('unit ', unit.unit_name, ' build ')   
            
        
    def _build_size_and_signature(self):
        
        for unit in list(self.unit_dictionary.values()):  
            unit.typedef_container.build_size(self.type_system) 
                                
        for unit in list(self.unit_dictionary.values()):                      
            unit.typedef_container.build_tms_signature_code(self)
            
        for unit in list(self.unit_dictionary.values()):  
            for struct_typedef in list(unit.typedef_container.struct_TypeDefinitions.values()):                     
                struct_typedef.declaration_description.type.build_tms_signature_code_for_member(self)
            for union_typedef in list(unit.typedef_container.union_TypeDefinitions.values()):                     
                union_typedef.declaration_description.type.build_tms_signature_code_for_member(self)

        
        for unit in list(self.unit_dictionary.values()):  
                                       
             # for all variables with linkage: build size and signature 
            for var_descr in list(unit.variable_descriptions_dict.values()):
                var_descr.build_size(self.type_system)
                var_descr.build_tms_signature_code(self)
             
            # for all lokal functions: build size and signature for return and parameter types    
            for function in list(unit.function_descriptions_local_dict.values()):   
                function.signature_description.build_size(self.type_system)
                function.signature_description.build_tms_signature_code(self)
            
            # for all global functions: build size and signature for return and parameter types
            for function in list(unit.function_descriptions_global_dict.values()):
                function.signature_description.build_size(self.type_system)
                function.signature_description.build_tms_signature_code(self)
                
            
        
    
    def AddFrom_PSYD_Filepath(self, PSYD_Filepath):
        
        SoCoLogger.info('Adding from symbol file ...')
                
        if not os.path.exists(PSYD_Filepath):
            print('Warning SourceCodeDescriptor.AddFrom_PSYD_Filepath: no_symbol file')
            return
        
        self.psyd_file = PSYD_Filepath
        baum = dom.parse(PSYD_Filepath)
        
        if baum.firstChild.nodeName != 'project_symbols':
            SoCoLogger.error('Error Fatal SourceCodeDescriptor.AddFrom_PSYD_Filepath: Wrong root element ' + str(baum.firstChild.nodeName) + ' - project_structure expected!')
            print("Error SourceCodeDescriptor.AddFrom_PSYD_Filepath: Wrong xml-structure in psyd-file.")
            return
        
        self.generator = DomHelper.GetOptionalAttribute(baum.firstChild,'generator','')
        self.origin = DomHelper.GetOptionalAttribute(baum.firstChild,'origin','')
        self.created = DomHelper.GetOptionalAttribute(baum.firstChild,'created','')
        self.encoding = baum.firstChild.getAttribute('encoding') 
         
        for child in baum.firstChild.childNodes:
                        
            if child.nodeName == 'unit':
                
                filename = child.getAttribute('file')
                dir, name, ext = split_filepath(filename)
                
                #debug
                #SoCoLogger.warning name
                           
                unit_descr = self.unit_dictionary.get(name, None)       #here the descriptor of a certain unit is received
                            
                if unit_descr:
                    unit_descr.compiler = DomHelper.GetOptionalAttribute(child,'compiler','')
                else:
                    SoCoLogger.warning("In AddFrom_PSYD_Filepath: For unit {name_} was not found any unit-descriptor. Added used definitions only.".format(name_ = name))
                    
                for grandchild in child.childNodes:
                    
                    if grandchild.nodeName == 'variable' and unit_descr:
                        identifier = grandchild.getAttribute('identifier')
                        var_descr = unit_descr.variable_descriptions_dict.get(identifier, None)
                        

                        #Userstub  etc.  
                        if not var_descr:

                            generated_files = ["_Stub", "_UserStub", "_IntStub", "_UserIntStub", "_ut_Wrapper", "_ut_UserWrapper", "_testframe", "_framed"]
                            for unit_gen in generated_files:
                                if name.endswith("_testframe"):
                                    new_name = name.rsplit("_", 1)[0]+unit_gen  
                                else:
                                    new_name = name+unit_gen
                                         
                                buffer_unit_dictionary = self.unit_dictionary.get(new_name, None)
                                if buffer_unit_dictionary:
                                    var_descr = buffer_unit_dictionary.variable_descriptions_dict.get(identifier, None)
                                    if var_descr:
                                        break
                                    
                                        
                        if var_descr:

                            if not var_descr.definition_description.tms_signature_code_2016:
                                var_descr.definition_description.build_tms_signature_code(self)
                            var_descr.definition_description.symbol_location_descr = SymbolLocationType.CreateFrom_PSYD(grandchild)

                            if CHECK_AGAINST_SYMBOL_FILE == True:
                                
                                if var_descr.definition_description.tms_signature_code_2008 !=  grandchild.getAttribute('signature_2008'):
                                    if  var_descr.definition_description.type.tms_signature_code_2008.endswith('*x'):
                                        #todo NiEh bei Bedarf: Zahl am Ende muss vielfaches von der Zahl (so vorhanden) vor dem *x sein
                                        pass
                                    elif var_descr.definition_description.type.__class__ == CEnumTypeDescriptor:
                                        pass     
                                    elif var_descr.definition_description.resolved_type.__class__ == CEnumTypeDescriptor:
                                        pass                                            
                                    else:
                                        pass
                                        #SoCoLogger.warning('Warning AddFrom_PSYD_Filepath for variable ' +str( identifier) + ': var_descr.definition_description.type.tms_signature_code_2008 ('  + str(var_descr.definition_description.tms_signature_code_2008) + ') != psyd signature(' + grandchild.getAttribute('signature_2008') + ')!')                                       
                                    var_descr.definition_description.tms_signature_code_2008 =  grandchild.getAttribute('signature_2008')  
                                
                                if var_descr.definition_description.tms_signature_code_2016 !=  grandchild.getAttribute('signature_2016'):
                                    SoCoLogger.info('Warning AddFrom_PSYD_Filepath for variable ' + str(identifier) + ': var_descr.definition_description.tms_signature_code_2016 (' + str(var_descr.definition_description.tms_signature_code_2016) + ') != psyd signature(' + grandchild.getAttribute('signature_2016') + ')!')                                       
                                    var_descr.definition_description.tms_signature_code_2016 =  grandchild.getAttribute('signature_2016')  
                                if var_descr.definition_description.type.__class__ == CTypeSelectionDescriptor:
                                     if not var_descr.definition_description.type.size_in_bytes == var_descr.definition_description.symbol_location_descr.size_in_bytes:
                                        SoCoLogger.warning('Warning in AddFrom_PSYD_Filepath for variable' + str(identifier) + ': var_descr.size_in_byte (' + str(var_descr.definition_description.type.size_in_bytes) + ') != var_descr.SymbolLocation.size_in_bytes (' + str(var_descr.definition_description.symbol_location_descr.size_in_bytes) + ')!')
                        else:
                            # UlBr 2015-09-03: object from libraries in symbol file?
                            #SoCoLogger.warning('Information in AddFrom_PSYD_Filepath: variable ', identifier, ' from symbol file not (yet) in SouceCodeDescriptor!')
                            #ToDo NiEh: It occurs, that variables are not in pstd, but in psyd, in this case no variable description is created.
                            #
                            SoCoLogger.error("Warning AddFrom_PSYD_Filepath for variable: {var_name_}. It was not found.".format(var_name_ = identifier))
                            
                    elif grandchild.nodeName == 'function' and unit_descr:
                        identifier = grandchild.getAttribute('identifier')
                        
                        
                                
                        func_descr = unit_descr.function_descriptions_local_dict.get(identifier, None)
                        if func_descr == None:
                            func_descr = unit_descr.function_descriptions_global_dict.get(identifier, None)    
                        
                        if func_descr == None:
                            for included_unit in unit_descr.cfile_includes_cfile_of_unit:
                            
                                if identifier in included_unit.function_descriptions_local_dict:
                                    func_descr = included_unit.function_descriptions_local_dict.get(identifier, None)
                                if identifier in included_unit.function_descriptions_global_dict:
                                    func_descr = included_unit.function_descriptions_global_dict.get(identifier, None) 
                                if not func_descr == None:
                                    break
                                                                          
                        #was there to debug
                        
                        if func_descr:
                            func_descr.SymbolLocation.SetFrom_PSYD(grandchild)
                            
                            if CHECK_AGAINST_SYMBOL_FILE == True:
                                
                                if func_descr.signature_description.tms_signature_code_2008 !=  grandchild.getAttribute('signature_2008'):
                                    #SoCoLogger.warning('Warning in AddFrom_PSYD_Filepath for function ' + str(identifier) + ': func_descr.signature_description.tms_signature_code_2008 (' + str( func_descr.signature_description.tms_signature_code_2008) + ') != psyd signature(' + grandchild.getAttribute('signature_2008') + ')!')
                                    func_descr.signature_description.tms_signature_code_2008 =  grandchild.getAttribute('signature_2008')  
                                       
                                if func_descr.signature_description.tms_signature_code_2016 !=  grandchild.getAttribute('signature_2016'):
                                    SoCoLogger.info('Warning in AddFrom_PSYD_Filepath for function ' + str(identifier) + ': func_descr.signature_description.tms_signature_code_2016 (' + str(func_descr.signature_description.tms_signature_code_2016) + ') != psyd signature(' + grandchild.getAttribute('signature_2016') + ')!')
                                    func_descr.signature_description.tms_signature_code_2016 =  grandchild.getAttribute('signature_2016')  
                                if func_descr.signature_description.size_in_bytes !=  grandchild.getAttribute('size_in_bytes') and grandchild.getAttribute('size_in_bytes'):
                                    pstd_size = func_descr.signature_description.size_in_bytes
                                    psyd_size = grandchild.getAttribute('size_in_bytes')
                                    func_descr.signature_description.size_in_bytes =  grandchild.getAttribute('size_in_bytes')
                                    SoCoLogger.warning ("Warning in AddFrom_PSYD_Filepath: The size in bytes of the PSTD-file of the function: {func_identifier} differs from PSYD-file, therefore its changed from {old_size} to {new_size}".format(func_identifier = identifier, old_size = pstd_size, new_size = psyd_size))
                        
                                                      
                         
                    elif grandchild.nodeName == 'structure':
                        
                        identifier = grandchild.getAttribute('identifier')
                        
                        struct_descr_td = None
                        
                        if unit_descr:
                        #struct_descr_td = unit_descr.typedef_container.get_struct_typedef_for_identifer(identifier)
                            struct_descr_td = SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_struct_typedef_for_identifer_in_container_list(identifier, unit_descr.available_typedef_container_list)
                            
                            if not struct_descr_td:
                                SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_alias_typedef_for_identifer_in_container_list(identifier, unit_descr.available_typedef_container_list)
                        
						#WORKAROUND
                        if not struct_descr_td:
                            struct_descr_td = self.all_locals_typedef_container.get_struct_typedef_for_identifer(identifier)
                            
                        if not struct_descr_td:
                            struct_descr_td = self.all_locals_typedef_container.get_alias_typedef_for_identifer(identifier)
                                
                        if not struct_descr_td:
                            struct_descr_td = self.global_typedef_container.get_struct_typedef_for_identifer(identifier)
                            
                        if not struct_descr_td:
                            struct_descr_td = self.global_typedef_container.get_alias_typedef_for_identifer(identifier)
                        ############           
                        #NiEh ToDo search in container list; Workaround if its not found, search in all units is not a good idea
                        #if not struct_descr_td:
                        #   for unit_descrs in self.unit_dictionary.values():
                        #        struct_descr_td = unit_descrs.typedef_container.get_struct_typedef_for_identifer(identifier)
                        #        if struct_descr_td:
                        #            break
                        #
                        #finds it
                        # --> at the moment no case appeared, that this is needed.
                            
                        if struct_descr_td:
                              # UlBr 2015-09-03: size taken from symbol file because of alignment and packed
#                                 if CHECK_AGAINST_SYMBOL_FILE == True:
#                                     if not struct_descr_td.type.size_in_bytes == int(grandchild.getAttribute('size')):
#                                         SoCoLogger.warning('Error in AddFrom_PSYD_Filepath for structure' , identifier, ': struct_descr_td.type.size_in_bytes (', struct_descr_td.type.size_in_bytes, ') != size in psyd (', int(grandchild.getAttribute('size')),')')
                            struct_descr_td.declaration_description.type.size_in_bytes = int(grandchild.getAttribute('size'))   
                             
                            member_index = 0
                            for greatgrandchild in grandchild.childNodes:
                                if greatgrandchild.nodeName == 'member':
                                    
                                    identifier_psyd = DomHelper.GetOptionalAttribute(greatgrandchild, 'identifier', '')                                       
                                    
                                    if identifier_psyd and struct_descr_td.declaration_description.type.member_descriptions[member_index].identifier: 
                                        if not struct_descr_td.declaration_description.type.member_descriptions[member_index].identifier == identifier_psyd:   
                                            SoCoLogger.debug('Error in AddFrom_PSYD_Filepath: member identifier missmatch while reading struct ' + str(struct_descr_td.declaration_description.type.tag_identifier))
                                            break 
                                        else:
                                            #it can happen, that the pstd does not store correct size,therefore check against psyd:
                                            if int(DomHelper.GetOptionalAttribute(greatgrandchild, 'size')):
                                                if not struct_descr_td.declaration_description.type.member_descriptions[member_index].type.size_in_bytes == int(DomHelper.GetOptionalAttribute(greatgrandchild, 'size')):
                                                    old_size = struct_descr_td.declaration_description.type.member_descriptions[member_index].type.size_in_bytes
                                                    new_size = int(DomHelper.GetOptionalAttribute(greatgrandchild, 'size'))
                                                    struct_descr_td.declaration_description.type.member_descriptions[member_index].type.size_in_bytes = new_size
                                                    SoCoLogger.warning ("Warning in AddFrom_PSYD_Filepath: The size in bytes of struct: {struct_name}, for member: {member_name}, from the PSTD-File does not correspond to the PSYD-File, therefore its changed from {pstd_size} to {psyd_size}.".format(struct_name = struct_descr_td.declaration_description.identifier, member_name = identifier_psyd, pstd_size = old_size, psyd_size = new_size)) 
                                    
                                    struct_descr_td.declaration_description.type.member_descriptions[member_index].symbol_location_descr = MemberSymbolLocationType.CreateFrom_PSYD(greatgrandchild)  
                                                                                                                   
                                    size_in_bits = DomHelper.GetOptionalAttribute(greatgrandchild, 'bitmask', None) 
                                    
                                    if not size_in_bits == None:
                                        SoCoLogger.warning('Encountered a member of a bitfield: {member_}. Do not use this member with PTEng)'.format(member_ = identifier_psyd))
                                        struct_descr_td.declaration_description.type.member_descriptions[member_index].size_in_bits = size_in_bits
                                        struct_descr_td.declaration_description.type.is_bitfield = True
                                         
                                   
                                    if CHECK_AGAINST_SYMBOL_FILE == True: 
                                        if not struct_descr_td.declaration_description.type.member_descriptions[member_index].size_in_bits:
                                            if not size_in_bits == None:
                                                SoCoLogger.warning('Warning in AddFrom_PSYD_Filepath: struct ' + str(struct_descr_td.declaration_description.type.tag_identifier) + ' member: ' +  str(identifier_psyd) + ': pstd has no bitmask but psyd has')
                                          
                                        else:
                                            if size_in_bits == None:
                                                SoCoLogger.warning('Warning in AddFrom_PSYD_Filepath: struct ' + str(struct_descr_td.declaration_description.type.tag_identifier) + ' member: ' + str(identifier_psyd) + ': pstd has bitmask but psyd has not')
                                                pass
                                            else:
                                                #todo check psyd against Axivion ? : check size_in_bits against bitmask?
                                                pass
                                            
                                    
                                    member_index = member_index + 1  
                        else:
                            SoCoLogger.error("Warning in AddFrom_PSYD_Filepath: The Typedescriptor was not found for struct {ident_} {type_}.".format(ident_=identifier, type_=type(identifier)))        
                    
                    elif grandchild.nodeName == 'union':
                        
                        identifier = grandchild.getAttribute('identifier')
                        union_descr_td = None
                        
                        if unit_descr:
                        
                            union_descr_td = SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_union_typedef_for_identifer_in_container_list(identifier, unit_descr.available_typedef_container_list)
                            
                            if not union_descr_td:
                                SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_alias_typedef_for_identifer_in_container_list(identifier, unit_descr.available_typedef_container_list)
                        
                        #WORKAROUND
                        if not union_descr_td:
                            union_descr_td = self.all_locals_typedef_container.get_union_typedef_for_identifer(identifier)
                        if not union_descr_td:
                            union_descr_td = self.all_locals_typedef_container.get_alias_typedef_for_identifer(identifier)
                            
                        if not union_descr_td:
                            union_descr_td = self.global_typedef_container.get_union_typedef_for_identifer(identifier)
                            
                        if not union_descr_td:
                            union_descr_td = self.global_typedef_container.get_alias_typedef_for_identifer(identifier)
                        ############  
                        
                        if union_descr_td:
#                                 if CHECK_AGAINST_SYMBOL_FILE == True:
#                                     if not union_descr_td.type.size_in_bytes == int(grandchild.getAttribute('size')):
#                                         SoCoLogger.warning('Error in AddFrom_PSYD_Filepath for union' , identifier, ': union_descr_td.type.size_in_bytes (', union_descr_td.type.size_in_bytes, ') != size in psyd (', int(grandchild.getAttribute('size')),')')                                
                            union_descr_td.declaration_description.type.size_in_bytes = int(grandchild.getAttribute('size')) 
                            
                            member_index = 0
                            for greatgrandchild in grandchild.childNodes:
                                if greatgrandchild.nodeName == 'member':
                                    identifier = DomHelper.GetOptionalAttribute(greatgrandchild, 'identifier', '') 
                                    if identifier:
                                        if not union_descr_td.declaration_description.type.member_descriptions[member_index].identifier == identifier:   
                                            SoCoLogger.error('Error in AddFrom_PSYD_Filepath: member identifier missmatch while reading struct ' + str(union_descr_td.declaration_description.type.tag_identifier))
                                            break
                                        else:
                                            #it can happen, that the pstd does not store correct size,therefore check against psyd:
                                            if int(DomHelper.GetOptionalAttribute(greatgrandchild, 'size')):
                                                if not union_descr_td.declaration_description.type.member_descriptions[member_index].type.size_in_bytes == int(DomHelper.GetOptionalAttribute(greatgrandchild, 'size')):
                                                    old_size = union_descr_td.declaration_description.type.member_descriptions[member_index].type.size_in_bytes
                                                    new_size = int(DomHelper.GetOptionalAttribute(greatgrandchild, 'size'))
                                                    union_descr_td.declaration_description.type.member_descriptions[member_index].type.size_in_bytes = new_size
                                                    SoCoLogger.warning("Warning in AddFrom_PSYD_Filepath: The size in bytes of struct: {struct_name}, for member: {member_name}, from the PSTD-File does not correspond to the PSYD-File, therefore its changed from {pstd_size} to {psyd_size}.".format(struct_name = union_descr_td.declaration_description.identifier, member_name = identifier, pstd_size = old_size, psyd_size = new_size))
                                            
                                    union_descr_td.declaration_description.type.member_descriptions[member_index].symbol_location_descr = MemberSymbolLocationType.CreateFrom_PSYD(greatgrandchild)
                                    

                                    
                                   
                                    size_in_bits = DomHelper.GetOptionalAttribute(greatgrandchild, 'bitmask', None) 
                                    
                                    if not size_in_bits == None:
                                        SoCoLogger.warning('Encountered a member of a bitfield: {member_}. Do not use this member with PTEng)'.format(member_ = identifier))
                                        struct_descr_td.declaration_description.type.member_descriptions[member_index].size_in_bits = size_in_bits
                                        struct_descr_td.declaration_description.type.is_bitfield = True 
                                    
                                    if CHECK_AGAINST_SYMBOL_FILE == True: 
                                        if not union_descr_td.declaration_description.type.member_descriptions[member_index].size_in_bits:
                                            if not size_in_bits == None:
                                                SoCoLogger.warning('Warning in AddFrom_PSYD_Filepath: union ' + str( union_descr_td.declaration_description.type.tag_identifier) + ' member: '+ str(identifier) +': pstd has no bitmask but psyd has')
                                          
                                        else:
                                            if size_in_bits == None:
                                                SoCoLogger.warning('Warning in AddFrom_PSYD_Filepath: union '+ str(union_descr_td.declaration_description.type.tag_identifier) + ' member: '+ str(identifier) +': pstd has bitmask but psyd has not')
                                            else:
                                                #todo check psyd against Axvion ? : check size_in_bits against bitmask?
                                                pass
                                                                                    
                                    member_index = member_index + 1 
                        else:
                            SoCoLogger.warning ("Warning in AddFrom_PSYD_Filepath: The Typedescriptor was not found for union {ident}.".format(ident = identifier))
                    
                    elif grandchild.nodeName == 'enumeration':
                        
                        identifier = grandchild.getAttribute('identifier')
                        
                        if identifier:
                            
                            enum_descr_td = None
                            
                            if unit_descr:
                            
                                enum_descr_td = SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_enum_typedef_for_identifer_in_container_list(identifier, unit_descr.available_typedef_container_list)
                                
                                if not enum_descr_td:
                                    SourceCodeDescriptor.TypeDefinitionDescriptorContainer.get_alias_typedef_for_identifer_in_container_list(identifier, unit_descr.available_typedef_container_list)
                                    
                                
                        
                            #WORKAROUND
                            if not enum_descr_td:
                                enum_descr_td = self.all_locals_typedef_container.get_enum_typedef_for_identifer(identifier)
                            
                            if not enum_descr_td:
                                enum_descr_td = self.all_locals_typedef_container.get_alias_typedef_for_identifer(identifier)    
                            
                            if not enum_descr_td:
                                enum_descr_td = self.global_typedef_container.get_enum_typedef_for_identifer(identifier)
                                
                            if not enum_descr_td:
                                enum_descr_td = self.global_typedef_container.get_alias_typedef_for_identifer(identifier)
                            ############  
                            
                            if enum_descr_td:
                                if CHECK_AGAINST_SYMBOL_FILE == True: 
                                    if not enum_descr_td.declaration_description.type.size_in_bytes == int(grandchild.getAttribute('size')):
                                        SoCoLogger.warning('Error in AddFrom_PSYD_Filepath: enumeration' + str( identifier) + ':enum_descr_td.type.size_in_bytes (' + str( enum_descr_td.declaration_description.type.size_in_bytes) + ') != size in psyd (' + str(int(grandchild.getAttribute('size'))) + ')')
                                enum_descr_td.declaration_description.type.size_in_bytes = int(grandchild.getAttribute('size')) 
                            else:
                                SoCoLogger.warning("Warning in AddFrom_PSYD_Filepath: The Typedescriptor was not found for enumeration {ident}.".format(ident = identifier))
                    else:
                        pass
                        

                    
                    
                              
                  
    
    def writePSTD(self, outName):
        baum = dom.Document()
        
        tag_project = dom.Element('project_structure')
        
        tag_project.setAttribute("xmlns", "http://www.bshg.com/pstd/v0")
        tag_project.setAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance")
        tag_project.setAttribute("xsi:schemaLocation","http://www.bshg.com/pstd/v0 pstd.xsd")
        
        """
        #deleted the basepath, its not used by PTEng, nor by Ptest. It causes problems while 
        #comparing the axivion-reference, because the tag differs for every get from dimensions.
        tag_basepath = dom.Element("basepath")
        text = dom.Text()
        text.data = self.basepath                            
        tag_basepath.appendChild(text)
        tag_project.appendChild(tag_basepath) 
        """           
        
        type_system = [
            ('char', 8), ('signed char', 8), ('unsigned char', 8),
            ('short int', 16), ('signed short int', 16), ('unsigned short int', 16),
            ('int', 32), ('signed int', 32), ('unsigned int', 32),  ('long int', 32),
            ('long int', 32), ('signed long int', 32), ('unsigned long int', 32), 
            ('float', 32), ('double', 32), ('long double', 64), 
            ]
        
        tag_type_system = dom.Element("type_system") 
        tag_type_system.setAttribute('type_system_ID', self.type_system_ID)  
        for type in type_system:           
            tag_type = dom.Element("type") 
            text = dom.Text()
            text.data = type[0]       
            tag_type.appendChild(text)
            tag_type.setAttribute('size_in_bytes', str(type[1]))            
            tag_type_system.appendChild(tag_type)
        tag_project.appendChild(tag_type_system)
        
        if self.project_folder:
            tag_project_folder = dom.Element("project_folder")
            text = dom.Text()
            text.data = self.project_folder
            tag_project_folder.appendChild(text)   
            tag_project.appendChild(tag_project_folder) 

        tag_project_name = dom.Element("project_name")
        text = dom.Text()
        text.data = self.project_name
        tag_project_name.appendChild(text)   
        tag_project.appendChild(tag_project_name) 
  
        # unit iteration
        unit_keys_list = list(self.unit_dictionary.keys())
        unit_keys_list.sort()
        for unit_key in unit_keys_list:                   
            tag_unit = self.unit_dictionary[unit_key].GetPstdTag(self)          
            tag_project.appendChild(tag_unit)
            
        
        baum.appendChild(tag_project)
 
        try:
            outFileXML = open(outName, "w")
        except:
            SoCoLogger.error ("Error SourceCodeDescriptor.writePSTD: File could not be open: " + outName)
            return
        
        baum.writexml(outFileXML,"","    ","\n","utf-8")
        
        outFileXML.close()        
    
    
    @classmethod 
    def _search_original_typedescriptor(cls, prm_CTypeDescriptor, prm_call_list, prm_index_list = [], prm_reset_list = 0 ):
            
        if (not len(prm_call_list) == 0) and prm_reset_list:
            prm_call_list = []
            
        if isinstance(prm_CTypeDescriptor, CInPlaceTypeDescriptor):
                
            if prm_CTypeDescriptor.is_pointer_to:
                prm_call_list.append(int(0))    #0 in the list means pointer
                return cls._search_original_typedescriptor(prm_CTypeDescriptor.included_type, prm_call_list)
                
            elif prm_CTypeDescriptor.is_array_of > -1:
                prm_call_list.append(int(1))    #1 in the list means array
                prm_index_list.append(int(prm_CTypeDescriptor.is_array_of))
                return cls._search_original_typedescriptor(prm_CTypeDescriptor.included_type, prm_call_list, prm_index_list)
                
            else:
                prm_call_list.append(int(2))    #2 in the list means pointer to function
                return cls._search_original_typedescriptor(prm_CTypeDescriptor.included_type, prm_call_list)
                
        elif isinstance(prm_CTypeDescriptor, CTypeSelectionDescriptor):
                
            if prm_CTypeDescriptor.type_definition_kind == "typedef_type" and not prm_CTypeDescriptor.resolved_type == CEnumTypeDescriptor:
                prm_call_list.append(int(3))    #3 in the list means selection of typedef_type
                return cls._search_original_typedescriptor(prm_CTypeDescriptor.resolved_type, prm_call_list)
            else:
                prm_call_list.append(prm_CTypeDescriptor)
                return prm_CTypeDescriptor
            
        else:
            prm_call_list.append(prm_CTypeDescriptor)   #the found basic typedescriptor is added to the list
            return prm_CTypeDescriptor
    
    def get_global_enum_dict(self):
        return self.enum_dict
    
    def set_outstream_loglevel(self, prm_level):
        SoCoLogClass.change_level_outstream(prm_level)
                
    def set_filestream_loglevel(self, prm_level):
        SoCoLogClass.change_level_filestream(prm_level)
        
    def set_log_propagation(self, prm_bool):
        SoCoLogClass.set_propagation(prm_bool)
        
    def close_outstreamhandler(self):
        SoCoLogClass.close_outstreamhandler()
        
    def close_filestreamhandler(self):
        SoCoLogClass.close_filestreamhandler()
        
    def close_all_handler(self):
        self.close_filestreamhandler()
        self.close_outstreamhandler() 
           
    # end of class SourceCodeDescriptor  
