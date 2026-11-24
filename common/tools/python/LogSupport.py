"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : LogSupport.py
Date of creation : 7.8.2018
Author Name/Dept : Nikolai Ehrhardt 
Organization     : 
Description      : A class, that wraps the Py-Logger and does some folder-creation.
"""

import logging
import logging.config
import sys
import os
import traceback

class LoggerClass():

    def __init__(self, prm_logger, prm_logfile=False):
        self.Logger             = logging.getLogger(prm_logger)
        self.filestream_handler = None
        self.outstream_handler  = logging.StreamHandler(stream = sys.stdout)
        self.log_formatter      = logging.Formatter('[%(module)s#%(lineno)s] %(asctime)s, Level: %(levelname)s: %(message)s') 
        if prm_logfile:
            logfile          = self.provide_folder(prm_logfile)
            self.filestream_handler = logging.FileHandler(logfile, mode="a")
            self.filestream_handler.setFormatter(self.log_formatter)
            self.Logger.addHandler(self.filestream_handler)
            
        self.outstream_handler.setFormatter(self.log_formatter)
        
        
        self.Logger.addHandler(self.outstream_handler)
        self.Logger.setLevel(logging.WARNING)
    
    def provide_folder(self, prm_logfile):
        logpath = ""
        filepath, file = os.path.split(__file__)
        filepath = filepath.rsplit("\\", 1)[0]
        if filepath.endswith("Tools"):
        
            try:
                if not os.path.exists(filepath+"\\Logs"):
                    os.mkdir(filepath+"\\Logs")
            except:
                exc_info_ = sys.exc_info()
                formatted_exc = traceback.format_exception(exc_info_[0], exc_info_[1], exc_info_[2])
                print("{exc_val_}".format(exc_val_ = formatted_exc[-1]))
            else:
                pass
            
            
            if os.path.exists(filepath+"\\Logs"):
                logpath = filepath+"\\Logs"
                print(("Info: Created folder and log-file: .Tools\\Logs\\"+ prm_logfile + "_logfile.log"))
        
        if logpath == "":
            path =  os.path.abspath("./")
             
            if os.path.exists(path+"\\Logs"):
                logpath = path+"\\Logs"
                print(("Info: Created folder and log-file: .\\Logs\\"+ prm_logfile + "_logfile.log"))


            else:
                try:
                    os.mkdir(path+"\\Logs")
                except:
                    logpath = os.abspath("./")
                    print(("Were not able to create the folder for logging. {log_}_logfile.log is created in the folder: {folder_}.".format(log_ = prm_logfile, folder_ = logpath)))                    
                
                if os.path.exists(path+"\\Logs"):
                    logpath = path+"\\Logs"
                    print(("Info: Created folder and log-file: .\\Logs\\"+ prm_logfile + "_logfile.log"))
                
        return logpath + "\\" + prm_logfile+"_logfile.log"
        
    
        
    def change_level_filestream(self, prm_level):
        try:
            self.filestream_handler.setLevel(prm_level)
        except:
            pass
        else:
            pass
        
    def change_level_outstream(self, prm_level):
        self.outstream_handler.setLevel(prm_level)
    
    def set_logger_level(self, prm_level):
        self.Logger.setLevel(prm_level)
        
    def set_propagation(self, prm_bool):
        self.Logger.propagate = prm_bool
        
    def add_handler(self, prm_stream, prm_level= logging.DEBUG):
        
        path, name = os.path.split(prm_stream)
        
        if (os.path.isdir(path)):
            new_handler = logging.FileHandler(prm_stream, mode="a")
            new_handler.setFormatter(self.log_formatter)
            new_handler.setLevel(prm_level)
            self.Logger.addHandler(new_handler)
            
        else:
            print("Error LoggerClass.add_handler: You tried to add a log-handler, but the given path is invalid. This Error is printed not logged.")
            
    def close_all_handler(self):
        try:
            self.filestream_handler.close()
        except:
            pass
        else:
            pass
        
        self.outstream_handler.close()
        
    def close_filestreamhandler(self):
        try:
            self.filestream_handler.close()
        except:
            pass
        else:
            pass
        
    def close_outstreamhandler(self):
        self.outstream_handler.close()
        
    