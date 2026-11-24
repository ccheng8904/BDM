"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : TypeSystemDescriptor.py
Date of creation : 7.8.2018
Author Name/Dept : 
Organization     : 
Description      : Provides the dictionaries with platform dependant .
"""


#todo: at_lest_types
#todo: UlBr 20016-09-07: signdness of char without given signdness will come from Alex
#todo: 2016-11-04: info from Alex by Email 22.9.2016: : There is nothing like stdfloat.h. Float size is Compiler / Platform  / Precision dependent.Closest thing is float.h -> delayed to: introduction of float


# depends for enum means: smallest (unsgined) int type than can cover the value range
_type_sizes_default = {'char': 1, 'short': 2, 'int': 4, 'long': 4, 'long long': 8, 'float': 4, 'double': 8, 'long double': 8, 'pointer': 4, 'enum': 'depends', 'bool': 1}
_type_sizes_KEIL_ARM = {'char': 1, 'short': 2, 'int': 4, 'long': 4, 'long long': 8, 'float': 4, 'double': 8, 'long double': 8, 'pointer': 4, 'enum': 'depends', 'bool': 1}
_type_sizes_RENESAS_RX = {'char': 1, 'short': 2, 'int': 4, 'long': 4, 'long long': 8, 'float': 4, 'double': 4, 'long double': 8, 'pointer': 4, 'enum': 'depends', 'bool': 1}
_type_sizes_Windows32 = {'char': 1, 'short': 2, 'int': 4, 'long': 4, 'long long': 8, 'float': 4, 'double': 8, 'long double': 8, 'pointer': 4, 'enum': 4, 'bool': 1}

_type_sizes_dictdict = {'default': _type_sizes_default, 'KEIL_ARM': _type_sizes_KEIL_ARM, 'RENESAS_RX': _type_sizes_RENESAS_RX, 'Windows32': _type_sizes_Windows32}


_int_bytesize_to_type_default = {1: 'char', 2: 'short', 4: 'int', 8: 'long long'}
_int_bytesize_to_type_KEIL_ARM = {1: 'char', 2: 'short', 4: 'int', 8: 'long long'}
_int_bytesize_to_type_RENESAS_RX = {1: 'char', 2: 'short', 4: 'int', 8: 'long long'}
_int_bytesize_to_type_Windows32 = {1: 'char', 2: 'short', 4: 'int', 8: 'long long'}

_int_bytesize_to_type_dictdict = {'default': _int_bytesize_to_type_default, 'KEIL_ARM': _int_bytesize_to_type_KEIL_ARM, 'RENESAS_RX': _int_bytesize_to_type_RENESAS_RX, 'Windows32': _int_bytesize_to_type_Windows32}



_decimal_bytesize_to_type_default = { 4: 'double', 8: 'long double'}
_decimal_bytesize_to_type_KEIL_ARM =  {4: 'double', 8: 'long double'}
_decimal_bytesize_to_type_RENESAS_RX = {4: 'double', 8: 'long double'}
_decimal_bytesize_to_type_Windows32 = {4: 'double', 8: 'long double'}

_decimal_bytesize_to_type_dictdict = {'default': _decimal_bytesize_to_type_default, 'KEIL_ARM': _decimal_bytesize_to_type_KEIL_ARM, 'RENESAS_RX': _decimal_bytesize_to_type_RENESAS_RX, 'Windows32': _decimal_bytesize_to_type_Windows32}

#todo: set to values delivered by Alex; set Axivion configs accordingly Axivion accepts --signed_chars resp. --unsigned chars
_signdness_for_char_with_unspecfied_signdness = {'default': 'unsigned', 'KEIL_ARM': 'unsigned', 'RENESAS_RX': 'unsigned', 'Windows32': 'unsigned'}

class TypeSystemDescriptor:
    
    #todo: alignment - check with Alex?  -> rely on MakSym for addresses and offsets in stucts?
    #todo: packed structs - which compiler 'packs' stucts? -> rely on MakSym
        
    Normalized_CIntegerTypes = ['char','signed char','unsigned char','signed short', 'unsigned short', 'signed int','unsigned int', 'signed long','unsigned long', 'signed long long','unsigned long long']
            
    Sized_C99_Types = ['int8_t', 'uint8_t', 'int16_t','uint16_t', 'int32_t','uint32_t', 'int64_t', 'uint64_t', 'int128_t', 'uint128_t', 'float16_t', 'float32_t', 'float64_t', 'float128_t']

    Sized_BSH_Types = ['schar', 'uchar', 'int16', 'uint16', 'int32', 'uint32']
    
    Implicit_Int_Types = ['unsigned', 'signed', '', ' ']
    
    
    C99_Sized_To_Generic_Type = {
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
 
    Generic_To_C99_Sized_Type = {
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
        'float16':    'decimal16_t',  
        'float32':    'decimal32_t',  
        'float64':    'decimal64_t',
        'float128':   'decimal128_t',        
        'void':       'void'           
        }

    
        
    def __init__(self, type_system_id):
        
        global _type_sizes_dictdict
        global _int_bytesize_to_type_dictdict
        
        self.type_system_id = type_system_id   
        
        self.signdness_for_char_with_unspecfied_signdness = _signdness_for_char_with_unspecfied_signdness[self.type_system_id]
        
        if type_system_id in _type_sizes_dictdict:
            self.type_size_dict = _type_sizes_dictdict[self.type_system_id] 
            self.int_bytesize_to_type = _int_bytesize_to_type_dictdict[self.type_system_id]
            self.decimal_bytesize_to_type = _decimal_bytesize_to_type_dictdict[self.type_system_id]
            #print ('type_system for ', type_system_id)
        else:
            self.type_size_dict = {}
            self.int_bytesize_to_type = {}
            self.decimal_bytesize_to_type = {}
            print(('No type system for type_system_ID ', type_system_id))
            
    def get_bytesize_SizedC99Type_of_NormalizedCType(self, NormalizedCType):
        
        if NormalizedCType == 'char': 
            if self.signdness_for_char_with_unspecfied_signdness == 'signed':
                SizedC99Type = 'int'
            else:
                SizedC99Type = 'uint'
        elif NormalizedCType.startswith('signed'):
            SizedC99Type = 'int'
        elif NormalizedCType.startswith('unsigned'):
            SizedC99Type = 'uint'
        else:
           SizedC99Type = 'float'
        
        basic_type = self.Normalized_CTypes_To_BasicType[NormalizedCType]
 
                
        bytesize = self.type_size_dict[basic_type]                
            
        SizedC99Type += str(bytesize * 8) +  '_t'   
        
        return (bytesize, SizedC99Type)
    
    def get_bytesize_NormalizedCType_of_SizedC99Type(self, SizedC99Type): 
        
        if SizedC99Type.startswith('f'):
            
            bytesize = self.Sized_C99_Type_To_Size[SizedC99Type]
            
            normalizedCFloatType += self.decimal_bytesize_to_type[bytesize]
            
            return (bytesize, normalizedCFloatType)
           
        else:             
            if SizedC99Type.startswith('u'):
                normalizedCIntegerType = 'unsigned '
            elif SizedC99Type.startswith('i'):
                normalizedCIntegerType = 'signed '
                
            bytesize = self.Sized_C99_Type_To_Size[SizedC99Type]
            
            normalizedCIntegerType += self.int_bytesize_to_type[bytesize]
            
            return (bytesize, normalizedCIntegerType)
    
    
    def get_bytesize_SizedC99Type_NormalizedCType_of_SizedBSHType(self, SizedBSHType): 
               
        if SizedBSHType.startswith('u'):
            normalizedCIntegerType = 'unsigned '
            SizedC99Type = 'uint'
        else:
            normalizedCIntegerType = 'signed '
            SizedC99Type = 'int'
            
        bytesize = self.Sized_BSH_Type_To_Size[SizedBSHType]
        
        normalizedCIntegerType += self.int_bytesize_to_type[bytesize]
        SizedC99Type += str(bytesize * 8) +  '_t'  
        
        return (bytesize, SizedC99Type, normalizedCIntegerType)

    def get_bytesize_SizedC99Type_NormalizedCType_of_Implicit_Int_Type(self, implicit_int_type):
        
        if implicit_int_type.startswith('u'):
            normalizedCIntegerType = 'unsigned '
            SizedC99Type = 'uint'
        else:
            normalizedCIntegerType = 'signed '
            SizedC99Type = 'int'
            
        bytesize = self.type_size_dict["int"]
        
        normalizedCIntegerType += self.int_bytesize_to_type[bytesize]
        SizedC99Type += str(bytesize * 8) +  '_t'  
        
        return (bytesize, SizedC99Type, normalizedCIntegerType)
        

    Normalized_CTypes_To_BasicType = { 
        'char':             'char',
        'signed char':      'char',
        'unsigned char':    'char',
        'signed short':     'short',
        'unsigned short':   'short',
        'signed int':       'int',
        'unsigned int':     'int',
        'signed long':      'long',
        'unsigned long':    'long',
        'signed long long':     'long long',
        'unsigned long long':   'long long',
        'float':                'float',
        'double':               'double',
        'long double':          'long double'
        }          
        
    Sized_C99_Type_To_Size = {
        'int8_t':       1,
        'uint8_t':      1,   
        'int16_t':      2,
        'uint16_t':     2,   
        'int32_t':      4,
        'uint32_t':     4,  
        'int64_t':      8,
        'uint64_t':     8, 
        'uint128_t':    16,  
        'int128_t':     16,              
        'float16_t':    2,  
        'float32_t':    4,  
        'float64_t':    8,
        'float128_t':   16        
        }    
        
    Sized_BSH_Type_To_Size = {
        'schar':      1,
        'uchar':      1,   
        'int16':      2,
        'uint16':     2,   
        'int32':      4,
        'uint32':     4    
        }   
    
   

                        
            
            
            
        