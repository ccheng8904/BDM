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
Description      : In file C_Struct.py the replication of the TEngSIO message in Python is implemented.
                   In the first step each sub-structure or sub-union of this TEngSIO message is replicated.
                   In the next step the TEngSIO message itself is replicated as a class (TEng_IO_Msg).
"""


import ctypes

from .constants import message_const, DLL
import logging

EngineLogger = logging.getLogger("global")

class Msg_data_class_replication(ctypes.Union):
    """
    This class builds the replication of the union "data" of the C-structure (TEng_IO_Msg.TEng_Msg.DataPart.data).
    """
    __slots__=()
    _fields_ = [    ("UC_data", ctypes.c_ubyte *     int(message_const.MAXPARSIZE)                                ),
                    ("US_data", ctypes.c_ushort *     int(message_const.MAXPARSIZE / int(ctypes.sizeof(ctypes.c_ushort)))      ),
                    ("UL_data", ctypes.c_ulong *     int(message_const.MAXPARSIZE / int(ctypes.sizeof(ctypes.c_ulong)))      )
               ]


class Msg_RetParam_replication(ctypes.Union):
    """
    This class builds the replication of the union "RetParam" of the C-structure (TEng_IO_Msg.TEng_Msg.ProcPart.RetParam).
    """
    __slots__=()
    _fields_ = [    ("RetVal", ctypes.c_int                            ),
                    ("RetBuf", ctypes.c_ubyte * int(message_const.MAXRETSIZE)         )
               ]

    
class Msg_DataPart_replication(ctypes.Structure):
    """
    This class builds the replication of the structure "DataPart" of the C-structure (TEng_IO_Msg.TEng_Msg.DataPart).
    """
    __slots__=()
    _pack_=1
    _fields_ = [    ("type", ctypes.c_ubyte),
                    ("address", ctypes.c_void_p),
                    ("pSign", ctypes.c_char_p),
                    ("pName", ctypes.c_char_p),
                    ("offset", ctypes.c_int),
                    ("stype", ctypes.c_int),
                    ("data", Msg_data_class_replication)
               ]
        
class Msg_ProcPart_replication(ctypes.Structure):
    """
    This class builds the replication of the structure "ProcPart" of the C-structure (TEng_IO_Msg.TEng_Msg.ProcPart).
    """
    __slots__=()
    _pack_=1
    _fields_ = [    ("address", ctypes.c_void_p),
                    ("pSign", ctypes.c_char_p),
                    ("pName", ctypes.c_char_p),
                    ("argCount", ctypes.c_int),
                    ("retsize", ctypes.c_int),
                    ("RetParam", Msg_RetParam_replication),
                    ("FirstParam", ctypes.c_int),
                    ("FurtherParams", ctypes.c_int * 10)                    
               ]
    
class TEng_Msg_replication(ctypes.Union):
    """
    This class builds the replication of the union "TEng_Msg" of the C-structure (TEng_IO_Msg.TEng_Msg).
    """
    __slots__=()
    _fields_ = [    ("msg", ctypes.c_ubyte * int(message_const.MAXMSGSIZE)    ),
                    ("DataPart", Msg_DataPart_replication            ),
                    ("ProcPart", Msg_ProcPart_replication            )
               ]
        
    
class TEng_IO_Msg(ctypes.Structure):
    """
    This Python class builds the replication of the C-structure (TEng_IO_Msg.TEng_Msg), whose typedef can be found in the TEng msg server header file. 
    The replicated class has the same size and structure (Aufbau) like the C-structure and is stored in the memory at an specific address. In C it is possible to access this memory-address to work with the structure.   
    
    This class has different methods to work with the message. 
    """
    __slots__= ("debugflag")
    _pack_        =    1
    _fields_     =     [    ("request", ctypes.c_ubyte),
                            ("TEng_Msg", TEng_Msg_replication)
                       ]
    def __init__(self):
        self.debugflag = 0
    
    def send(self):
        """
        The most important method for the work of the TEng is implemented there: The method "send()".
        This method is calling from TEngSIO.dll (The address of the replicated TEngSIO message in the memory is given as parameter for function TEngIO_SendMessage of the DLL)
            --> corresponding to Call-by-Reference, so the message can also be changed by the TEngSIO 
        """
        if (self.debugflag==1):
            print("\n \n Sended Message: \n")
            self.print_msg()
        DLL.TEngSIO.TEngIO_SendMessage(ctypes.addressof(self)) 
        if (self.debugflag==1):
            print("\n \n Received Message: \n")
            self.print_msg()
    
    
    def reset(self):
        self.request    =ctypes.c_ubyte(0)
        for i in range(0,message_const.MAXMSGSIZE):
            self.TEng_Msg.msg[i]=0
            
    def print_msg(self):
        """
        This method can be used to print the message. Each byte of the message is printed as hex value!
        """
        line=1
        print(("Line 0 (0-7):\t\t%s\t"%(str(hex(self.request))[2:4])))
        for i in range(0,message_const.MAXMSGSIZE):
            if (i%8==7): 
                print("")
                print(("Line %s(%s-%s):\t"%(line,i+1,i+8)))
                if(line<=11): print(("\t")) # adjusting alignment
                line=line+1
            print(("%s\t"%(str(hex(self.TEng_Msg.msg[i]))[2:4])))
        print("\n \n")
        
        
        if(self.request==2 or self.request==3):
            address=self.TEng_Msg.DataPart.address
            if not address: address=0
            
            print(("DATA - type       \t= \t%s = %s" %(self.TEng_Msg.DataPart.type, hex(self.TEng_Msg.DataPart.type))))
            print(("DATA - address    \t= \t"+str(self.TEng_Msg.DataPart.address)+" = "+str(hex(int(self.TEng_Msg.DataPart.address)))))
            print(("DATA - pSign      \t= \t%s"      %(self.TEng_Msg.DataPart.pSign)))
            print(("DATA - pName      \t= \t%s"      %(self.TEng_Msg.DataPart.pName)))
            print(("DATA - offset     \t= \t%s = %s" %(self.TEng_Msg.DataPart.offset, hex(self.TEng_Msg.DataPart.offset))))
            print(("DATA - stype      \t= \t%s = %s" %(self.TEng_Msg.DataPart.stype, hex(self.TEng_Msg.DataPart.stype))))
            print(("DATA - UC_Data    \t=   \t"))
            for i in range(0,message_const.MAXPARSIZE):   
                print(("%s" % (hex(self.TEng_Msg.DataPart.data.UC_data[message_const.MAXPARSIZE-1-i]))[2:4]))
            print("")
            print(("DATA - Value      \t= \t%s"%(self.TEng_Msg.DataPart.data.UL_data[0])))
            
        elif(self.request==4):
            address=self.TEng_Msg.ProcPart.address
            if not address: address=0
            
            print(("PROC - address    \t= \t%s = %s" %(address,hex(address))))
            print(("PROC - pSign      \t= \t%s"      %(self.TEng_Msg.ProcPart.pSign)))
            print(("PROC - pName      \t= \t%s"      %(self.TEng_Msg.ProcPart.pName)))
            print(("PROC - argCount   \t= \t%s = %s" %(self.TEng_Msg.ProcPart.argCount,   hex(self.TEng_Msg.ProcPart.argCount))))
            print(("PROC - retsize    \t= \t%s = %s" %(self.TEng_Msg.ProcPart.retsize,    hex(self.TEng_Msg.ProcPart.retsize))))
            print(("PROC - FirstParam \t= \t%s = %s" %(self.TEng_Msg.ProcPart.FirstParam, hex(self.TEng_Msg.ProcPart.FirstParam))))
            print(("PROC - RetBuf     \t=   \t"      %()))
            for i in range(0,message_const.MAXRETSIZE):   
                print((str(hex(self.TEng_Msg.ProcPart.RetParam.RetBuf[message_const.MAXRETSIZE-1-i]))[2:4]))
            print("\n")
            print(("PROC - RetValue   \t= \t%s = %s"%(self.TEng_Msg.ProcPart.RetParam.RetVal, hex(self.TEng_Msg.ProcPart.RetParam.RetVal))))
            
        print("\n \n")
        
        
        
    @classmethod    
    def print_structure(cls):
        """
        This classmethod can be used to print the structure of the message. For each member its size, type and offset (related to its direct parent) are printed. 
        """
        print("")
        print(("TEng_IO_Msg\t\t\t\t%s"                   %(TEng_IO_Msg)))
        print(("TEng_IO_Msg.request\t\t\t%s"             %(TEng_IO_Msg.request)))
        print(("TEng_IO_Msg.TEng_Msg\t\t\t%s"            %(TEng_IO_Msg.TEng_Msg)))
        print("")
        print(("TEng_Msg_replication\t\t\t%s"            %(TEng_Msg_replication)))
        print(("TEng_Msg_replication.msg\t\t%s"          %(TEng_Msg_replication.msg)))
        print(("TEng_Msg_replication.DataPart\t\t%s"     %(TEng_Msg_replication.DataPart)))
        print(("TEng_Msg_replication.ProcPart\t\t%s"     %(TEng_Msg_replication.ProcPart)))
        print("")
        print(("Msg_DataPart_replication\t\t%s"          %(Msg_DataPart_replication)))
        print(("Msg_DataPart_replication.type\t\t%s"     %(Msg_DataPart_replication.type)))
        print(("Msg_DataPart_replication.address\t%s"    %(Msg_DataPart_replication.address)))
        print(("Msg_DataPart_replication.pSign\t\t%s"    %(Msg_DataPart_replication.pSign)))
        print(("Msg_DataPart_replication.pName\t\t%s"    %(Msg_DataPart_replication.pName)))
        print(("Msg_DataPart_replication.offset\t\t%s"   %(Msg_DataPart_replication.offset)))
        print(("Msg_DataPart_replication.stype\t\t%s"    %(Msg_DataPart_replication.stype)))
        print(("Msg_DataPart_replication.data\t\t%s"     %(Msg_DataPart_replication.data)))
        print("")
        print(("Msg_data_class_replication\t\t%s"        %(Msg_data_class_replication)))
        print(("Msg_data_class_replication.UC_data\t%s"  %(Msg_data_class_replication.UC_data)))
        print(("Msg_data_class_replication.US_data\t%s"  %(Msg_data_class_replication.US_data)))
        print(("Msg_data_class_replication.UL_data\t%s"  %(Msg_data_class_replication.UL_data)))
        print("")
        print(("Msg_ProcPart_replication\t\t%s"          %(Msg_ProcPart_replication)))
        print(("Msg_ProcPart_replication.address\t%s"    %(Msg_ProcPart_replication.address)))
        print(("Msg_ProcPart_replication.pSign\t\t%s"    %(Msg_ProcPart_replication.pSign)))
        print(("Msg_ProcPart_replication.pName\t\t%s"    %(Msg_ProcPart_replication.pName)))
        print(("Msg_ProcPart_replication.argCount\t%s"   %(Msg_ProcPart_replication.argCount)))
        print(("Msg_ProcPart_replication.retsize\t%s"    %(Msg_ProcPart_replication.retsize)))
        print(("Msg_ProcPart_replication.RetParam\t%s"   %(Msg_ProcPart_replication.RetParam)))
        print(("Msg_ProcPart_replication.FirstParam\t%s" %(Msg_ProcPart_replication.FirstParam)))
        print(("Msg_ProcPart_replication.FurtherParams\t%s"%(Msg_ProcPart_replication.FurtherParams)))
        print("")
        print(("Msg_RetParam_replication\t\t%s"          %(Msg_RetParam_replication)))
        print(("Msg_RetParam_replication.RetVal\t\t%s"   %(Msg_RetParam_replication.RetVal)))
        print(("Msg_RetParam_replication.RetBuf\t\t%s"   %(Msg_RetParam_replication.RetBuf)))
        print("")
        print("")
 

    def debug(self,value=1):
        """
        This classmethod can be used to turn on or turn off the debug mode. If the debug mode is turned on, each message is printed (before sending and after sending to the target, so the changes by TEngSIO can be tracked)
        Valid input parameters:
        Turn on:     <empty>, 1, True
        Turn off:    0, False
        """
        if(value==1 or value==True):
            self.debugflag=1
        elif(value==0 or value==False):
            self.debugflag=0
        else:
            EngineLogger.error("Incorrect use of method debug!")
    
    
    
    def getVarRetVal(self, prm_signed = True):  # get either UC, US or UL value
        """
        This method helps to get the return value of the variable according to its size.
        DataPart_type value        size of the variable
        0                        1 byte        
        1                        2 bytes        
        2                        4 bytes        
        3                        8 bytes        
        """
        
        if prm_signed:
            if   self.TEng_Msg.DataPart.type == (0): return int(self.TEng_Msg.DataPart.data.UC_data[0]) # case 1 byte:  return UC_data
            elif self.TEng_Msg.DataPart.type == (1): return int(self.TEng_Msg.DataPart.data.US_data[0]) # case 2 bytes: return US_data
            elif self.TEng_Msg.DataPart.type == (2): return int(self.TEng_Msg.DataPart.data.UL_data[0]) # case 4 bytes: return UL_data
            else:
                EngineLogger.Error("Invalid value of TEng_Msg.DataPart.type. Only values 0,1,2 or 3 are allowed, but it has value %s!"%(self.TEng_Msg.DataPart.type))
                return None
        else:
            if   self.TEng_Msg.DataPart.type == (0): return int(ctypes.c_int8(self.TEng_Msg.DataPart.data.UC_data[0]).value) # case 1 byte:  return UC_data
            elif self.TEng_Msg.DataPart.type == (1): return int(ctypes.c_int16(self.TEng_Msg.DataPart.data.US_data[0]).value) # case 2 bytes: return US_data
            elif self.TEng_Msg.DataPart.type == (2): return int(ctypes.c_int32(self.TEng_Msg.DataPart.data.UL_data[0]).value) # case 4 bytes: return UL_data 
            else:
                EngineLogger.error("Invalid value of TEng_Msg.DataPart.type. Only values 0,1,2 or 3 are allowed, but it has value %s!"%(self.TEng_Msg.DataPart.type))
                return None 

    
    def getFuncRetVal(self, prm_sign, size):
        
        if(size <= -1):
            return "void"
 
        type = message_const.bytes2type[size]
    
        if prm_sign == True:   
            if type == (0): return int( ctypes.c_uint8(self.TEng_Msg.ProcPart.RetParam.RetVal).value) 
            elif type == (1): return int( ctypes.c_uint16(self.TEng_Msg.ProcPart.RetParam.RetVal).value) 
            elif type == (2): return int( ctypes.c_uint32(self.TEng_Msg.ProcPart.RetParam.RetVal).value) 
            else:
                EngineLogger.error("64bit as return size is not possible,please use the call_func_by_wrapper method.")
                return None
        elif prm_sign == False:     
            if type == (0): return int( ctypes.c_int8(self.TEng_Msg.ProcPart.RetParam.RetVal).value) 
            elif type == (1): return int( ctypes.c_int16(self.TEng_Msg.ProcPart.RetParam.RetVal).value) 
            elif type == (2): return int( ctypes.c_int32(self.TEng_Msg.ProcPart.RetParam.RetVal).value) 
            else:
                EngineLogger.error("64bit as return size is not possible,please use the call_func_by_wrapper method.")
                return None
        else:
            EngineLogger.error("Incorrect sign defined getFuncRetVal.")
            return None

    
    
    def create_var_message(self,DataPart_address,DataPart_offset,DataPart_type,DataPart_stype,DataPart_pSign,DataPart_pName):
#         self.request                             = ctypes.c_ubyte  (message_const.REQ_MSG_READ)
        if type(DataPart_address) == str:        DataPart_address = int(DataPart_address, 16)
        self.TEng_Msg.DataPart.address           = ctypes.c_void_p (DataPart_address)
        self.TEng_Msg.DataPart.offset            = ctypes.c_int    (DataPart_offset)
        self.TEng_Msg.DataPart.type              = ctypes.c_ubyte  (DataPart_type)
        self.TEng_Msg.DataPart.stype             = ctypes.c_int    (DataPart_stype)
        self.TEng_Msg.DataPart.pSign             = ctypes.c_char_p (DataPart_pSign)
        self.TEng_Msg.DataPart.pName             = ctypes.c_char_p (DataPart_pName)
         

   