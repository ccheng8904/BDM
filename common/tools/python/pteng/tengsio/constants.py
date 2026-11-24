"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : EhrhardtN
Date of creation : 2020-05-22
Author Name/Dept : 
Organization     : 
Description      : Global definitions, in major for tengsio-interface
"""


import ctypes

class DLL:
    # ************************************************************************************
    # Load the TEngSIO.dll File
    # ************************************************************************************
    path_TEngSIO_DLL="TEngSIO.dll" # if a relative path is given, the file is searched at os.environ["PATH"]
    path_DbusDrv_DLL="DBusDrv.dll"
    path_TEng_Db2Wrapper_DLL="TEng_Db2Wrapper.dll"
    
    #from ctypes.util import find_library
    #print(find_library("ftd2xx.dll"))
    #print(find_library("DBusDrv.dll"))
    
    libgcc_s_dw2    = ctypes.cdll.LoadLibrary("libgcc_s_dw2-1.dll")
    ftd2xx          = ctypes.cdll.LoadLibrary("ftd2xx.dll")
    DBusDrv         = ctypes.cdll.LoadLibrary(path_DbusDrv_DLL) # load DBusDrv.dll
    TEng_Db2Wrapper = ctypes.cdll.LoadLibrary(path_TEng_Db2Wrapper_DLL) # load TEng_Db2Wrapper.dll
    TEngSIO         = ctypes.cdll.LoadLibrary(path_TEngSIO_DLL) # load TEngSIO.dll
    
class message_const:
    
    # ************************************************************************************
    # Message Constants and Dictionaries
    # ************************************************************************************
    REQ_MSG_CONNECT         =1       # Target Connection
    REQ_MSG_READ            =2       # Target Read
    REQ_MSG_WRITE           =3       # Target Write
    REQ_MSG_EXECUTE         =4       # Target Procedure Execution
    REQ_MSG_DISCONNECT      =5       # Target Disconnect
    REQ_MSG_PTR_READ        =6       # Target Pointer Read
    REQ_MSG_PTR_WRITE       =7       # Target Pointer Write
    
    MAXPARSIZE = 40                 #  size of Msg_data_class_replication in bytes
    MAXRETSIZE=16                   #  size of Msg_RetParam_replication in bytes
    MAXMSGSIZE=255                  #  maximum message size in bytes (size of TEng_Msg_replication in bytes)
    
    
    # gtype : size_in_bytes
    gtype2sizeinbytes = { 
            'int8':       1,
            'uint8':      1,   
            'int16':      2,
            'uint16':     2,   
            'int32':      4,
            'uint32':     4,  
            'int64':      8,
            'uint64':     8,  
            'int128':     16,
            'uint128':    16,
            'float16':    2,  
            'float32':    4,  
            'float64':    8,
            'float128':   16,        
            'void':       0           
            }
    
    
    # converting size_in_bytes : message.TEng_Msg.DataPart.type
    bytes2type={ 
                1:0,
                2:1,
                4:2,
                8:2 #3 # added for handling 8 byte data types type 3 not necessary, because UL[1] is not working
                }
    
    #BaseTypeINIT_iAPX
    message_stype_dict      ={ 
                    "B1"         :    1   ,
                    "SC1"        :    2   ,
                    "UC1"        :    3   ,
                    "US1"        :    4   ,
                    "S1"         :    5   ,
                    "UI1"        :    6   ,
                    "I1"         :    7   ,
                    "L1"         :    8   ,
                    "UL1"        :    9   ,
                    "PTR1"       :    6  ,
                    "LLPTR1"     :    6  ,                 #included by NiEh, TengMsgServer can't handle bigger than 24, was 13
                    "BPTR1"      :    6  ,
                    "SPTR1"      :    6  ,
                    "LPTR1"      :    6  ,
                    "NPTR1"      :    6  ,
                    "BNPTR1"     :    6  ,
                    "SNPTR1"     :    6  ,
                    "LNPTR1"     :    6  ,
                    "FPTR1"      :    6  ,
                    "BFPTR1"     :    6  ,
                    "SFPTR1"     :    6  ,
                    "LFPTR1"     :    6  ,
                    "BS1"        :    22  ,
                     "WS1"        :    23  ,
                     "LS1"        :    24
                     #else: 0  
                    }
    
    gtype2stype = {                 #ToDO NiEh later we will have to search an stype for UlBr definitions TypeSystemDescriptor
        
            'int8':       2,        #signed char
            'uint8':      3,        #unsigned char
            'int16':      5,
            'uint16':     4,   
            'int32':      7,
            'uint32':     6,  
            'int64':      6,
            'uint64':     6,
            'int128':     6,
            'uint128':    6 			
            #'int128':     16,
            #'uint128':    16,
            #'float16':    2,  
            #'float32':    4,  
            #'float64':    8,
            #'float128':   16,        
            #'void':       0 
        
        }
    

#     gtype2tengsiosignature = { 
#             'int8':       "SC1",
#             'uint8':      "UC1",   
#             'int16':      "S1",
#             'uint16':     "US1",   
#             'int32':      "I1",
#             'uint32':     "UI1",  
#             'int64':      "L1",
#             'uint64':     "UL1",  
#             'int128':     "LL1",
#             'uint128':    "ULL1",
#             'float16':    "SF1",  
#             'float32':    "IF1",  
#             'float64':    "LF1",
#             'float128':   "LLF1",        
#             'void':       "V"           
#             }

# standard_tengsio_signatures="SC","UC","SS","US","SI","UI","SL","UL","SLL","ULL","SF","IF","LF","LLF","V"]
#     tengsio_signature2size_in_bytes={
#                              "SC"   :   1,
#                              "UC"   :   1,
#                              "S"   :   2,
#                              "US"   :   2,
#                              "I"   :   4,
#                              "UI"   :   4,
#                              "L"   :   8,
#                              "UL"   :   8,
#                              "LL"  :   16,
#                              "ULL"  :   16,
#                              "SF"   :   2,
#                              "IF"   :   4,
#                              "LF"   :   8,
#                              "LLF"  :   16,
#                              "V"    :   4,
#                              "BPTR" :   1,
#                              "SPTR" :   2,
#                              "IPTR" :   4,
#                              "LPTR" :   8,
#                         }