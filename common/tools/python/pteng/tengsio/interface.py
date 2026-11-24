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
Description      : In TEngSIO_Interface all functions of the TEngSIO DLL are wrapped.
"""


import ctypes

from .constants import message_const, DLL


#This Class builds the interface between the TEngSIO.dll-functions and the Python-function
class TEngSIO_Interface(object):
    __slots__=('_SIOVersion')
    
    
    def __init__(self):
        self._SIOVersion=str(self.GetSIOVersion())
        
    
    def IsolateSIOParameters(self,argc,argv):
        """
        The function IsolateSIOParameters is necessary to prepare all connection parameters for the connection to a target.
        """
        parameters=argv.split(" ") #split argv string in its parameters to create a pointer in the next step --> ['TEngConnect', '/REMOTE=UDA1,12,1,10,36864,VDBUS_FLAG_9600_BAUD,VDBUS_FLAG_NO_COLLISION']
        myargv = (ctypes.c_char_p * 2)(parameters[0].encode('unicode-escape'), parameters[1].encode('unicode-escape')) #pointer-array of argv is built (points to each member of 'parameters'
        
        print(argv)
        
        return DLL.TEngSIO.Tcl_IsolateSIOParameters(ctypes.c_int(argc),myargv)  #at this point connection via D-Bus is established.
    
    def IsSerialConnection(self):
        """
        The method IsSerialConnection can be used to check if a connection to any target is already established. (Not target specific!!!). In current state of development TEngSIO only allows one connection simultaneous.
        """
        return DLL.TEngSIO.IsSerialConnection()
    
    
    def GetSIOVersion(self):
        version=str(  ctypes.c_char_p(   DLL.TEngSIO.GetSIOVersion()   ) )
        version=version[10:(len(version)-2)] #cut "c_char_p(" ... ")
        return  version #return string 
    
    def GetLibVersion(self):
        version=str(  ctypes.c_char_p(   DLL.TEngSIO.GetLibVersion()   ) )
        version=version[10:(len(version)-2)] #cut "c_char_p(" ... ")
        return  version #return string

    def GetVendor(self):
        vendor=str(  ctypes.c_char_p(   DLL.TEngSIO.GetVendor()   ) )
        vendor=vendor[10:(len(vendor)-2)] #cut "c_char_p(" ... ")
        return  vendor #return string
    
    def GetCpuType(self):
        cpu=str(  ctypes.c_char_p(   DLL.TEngSIO.GetCpuType()   ) )
        cpu=cpu[10:(len(cpu)-2)] #cut "c_char_p(" ... ")
        return  cpu #return string
    
    def GetDataModel(self):
        model=  str(ctypes.c_ubyte( DLL.TEngSIO.GetDataModel())) #return unsigned char
        model=  model[8:(len(model)-1)]
        return model
    
    def GetDataModelString(self):
        string=str(  ctypes.c_char_p(   DLL.TEngSIO.GetDataModelString() ) ) 
        string=string[10:(len(string)-2)]    
        return string #return const char *
    
    def CloseSerialConnection(self):
        return DLL.TEngSIO.CloseSerialConnection();
