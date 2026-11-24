"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : folder_utils.py
Date of creation : 4.03.2020
Author Name/Dept : Nikolai Ehrhardt
Organization     : 
Description      : Class to form filepathes, handle testcase result-dir.
"""

import os
import os.path
import time
import shutil

class PathFormer():

    test_folder = "testfiles"
    test_xml_folder = "TestXML"
    sequences_folder = "TestSequences"
    sequences_folder_small = "testsequences"
    suites_folder = "TestSuites"
    suites_folder_small = "testsuites"
    results_folder = "TestResults"
    results_folder_small = "testresults"
    
    test_folder_tuple = (test_folder, test_xml_folder)
    
    prj_path = os.path.dirname(__file__).rsplit("common")[0] + os.sep
    
    python_path = prj_path+"common"+os.sep+"tools"+os.sep+"python"+os.sep
    setup_icon = python_path+"setup.ico"
    
    pics_path = prj_path+"ci"+os.sep+"ci_tools"+os.sep+"pics"

    ci_logs_path = prj_path + "out" + os.sep + "_ci_logs" + os.sep 
    
    ci_logs_results = ci_logs_path + results_folder + os.sep
    
    testresults_rel = "."+os.sep+results_folder
    
    yml_ext = [".yml", ".yaml"]
    xml_ext = [".xml"]
    tex_ext = ".tex"
    pdf_ext = ".pdf"
    html_ext = ".html"
    xls_ext = ".xlsx"

    def __init__(self, path):
        self.path           = path
        self.abs_path       = None
        self.result_dir     = None
        
        self.valid          = False
        self.ext            = None
        self.basename       = None
        self.identifier     = None
        self.dir            = None
        
        self.suffix         = None
        self.tex_file       = None
        self.pdf_file       = None
        self.cvr_file       = None
        self.html_file      = None
        self.yml_file       = None
        self.xls_file       = None
        self.tex_filepath   = None
        self.pdf_filepath   = None
        self.cvr_filepath   = None  
        self.html_filepath  = None   
        self.yml_filepath   = None
        self.xls_filepath   = None
        
        if self.isString():
            self.getAbsFilePath()
            self.split_filepath()
            self.getResultPath()
    
    @classmethod
    def fromSeqOfSuite(cls, seq_path, suite_path):
        if os.path.isfile(seq_path):
            return cls(seq_path)
        elif os.path.isfile(suite_path):
            for tfolder in PathFormer.test_folder_tuple:
                if tfolder in seq_path and tfolder in suite_path:
                    root_path, test_path = suite_path.rsplit(tfolder, 1)
                    pf = cls(root_path+seq_path)
                    pf.path = seq_path
                    return pf
            
            return cls(seq_path)
        else:
            return None
    
    def split_filepath(self): 
        self.dir, self.basename, self.ext = split_filepath(self.abs_path)
        self.identifier = self.basename
    
    def isString(self):
        return (type(self.path) == str)
        
    def isXML(self):
        return self.ext in PathFormer.xml_ext
             
            
    def isYML(self):
        return self.ext in PathFormer.yml_ext
        
    def getFromTestRootPath(self, comparison_path):
        root_path = ""
        test_path = ""
        for tfolder in PathFormer.test_folder_tuple:
            if tfolder in self.abs_path and tfolder in comparison_path:
                root_path, test_path = self.abs_path.rsplit(tfolder, 1)
                comp_path_split = comparison_path.rsplit(tfolder, 1)
                if root_path == comp_path_split[0]:
                    return tfolder+test_path
        
        return self.abs_path 
            
        
    def getAbsFilePath(self):
        
        self.abs_path = os.path.abspath(self.path)
        if os.path.isfile(self.abs_path):
            self.valid = True      
        
        if not self.valid:
            self.abs_path   = os.path.normpath(PathFormer.prj_path + self.path)
            if os.path.isfile(self.abs_path):
                self.valid = True
                
    def getResultPath(self):   
        if self.valid:
            for folder in (PathFormer.sequences_folder_small,  PathFormer.suites_folder_small):
                if folder in self.abs_path:
                    abs_splitted = self.abs_path.rsplit(folder, 1)
                    self.result_dir = abs_splitted[0] + PathFormer.results_folder + abs_splitted[1]
            if self.result_dir == None:
                self.result_dir = self.dir
                
            self.result_dir = os.path.split(self.result_dir)[0]
            
    def createResultdir(self):
        create_folder(self.result_dir)
        
    def convertPath(self):
        if PathFormer.test_xml_folder in self.abs_path:
            self.ext = PathFormer.yml_ext[1]
            self.abs_path = self.abs_path.replace(PathFormer.test_xml_folder, PathFormer.test_folder, 1).replace(PathFormer.suites_folder, PathFormer.suites_folder_small).replace(PathFormer.sequences_folder, PathFormer.sequences_folder_small)
            dir, basename, ext = split_filepath(self.abs_path)
            self.abs_path = dir+os.sep+basename+self.ext
            self.dir = dir
            self.path = self.path.replace(PathFormer.test_xml_folder, PathFormer.test_folder, 1).replace(PathFormer.suites_folder, PathFormer.suites_folder_small).replace(PathFormer.sequences_folder, PathFormer.sequences_folder_small)
            dir, basename, ext = split_filepath(self.path)
            self.path = dir+os.sep+basename+self.ext
            self.result_dir = self.result_dir.replace(PathFormer.test_xml_folder, PathFormer.test_folder, 1).replace(PathFormer.results_folder, PathFormer.results_folder_small)
            create_folder(self.result_dir)
            create_folder(self.dir)
            
    
    def getSuffix(self, const_rslt_name, prm_suffix):
        self.suffix = ""
        if const_rslt_name:
            self.suffix = ""
        elif prm_suffix == None:
            self.suffix = "TU_"+time.strftime("%Y_%m_%d-%H_%M_%S_")
        else:
            self.suffix = prm_suffix
        
        print("-"*150)    
        print("Report: [fileprefix: {suffix_}]".format(suffix_=self.suffix))
        print("-"*150)

    def setTexFile(self):
        self.getSuffix(False, time.strftime("PDF_Report_%Y%m%d_II_%H%M%S_"))
        self.tex_file = self.suffix+self.basename+PathFormer.tex_ext
        self.tex_filepath = self.result_dir+os.sep+self.tex_file
        self.pdf_file = self.suffix+self.basename+PathFormer.pdf_ext
        self.pdf_filepath = self.result_dir+os.sep+self.pdf_file
        
    def setCVRFile(self):
        self.cvr_file = self.suffix+self.basename+"_cvr"+PathFormer.html_ext
        self.cvr_filepath = self.result_dir+os.sep+self.cvr_file
        
    def setHTMLFile(self):
        self.html_file = self.suffix+self.basename+PathFormer.html_ext
        self.html_filepath = self.result_dir+os.sep+self.html_file
        
    def setYMLFile(self):
        self.yml_file = self.suffix+self.basename+PathFormer.yml_ext[1]
        self.yml_filepath = self.result_dir+os.sep+self.yml_file
        
    def setXLSFile(self):
        self.xls_file = self.basename+PathFormer.xls_ext
        self.xls_filepath = self.result_dir+os.sep+self.xls_file    
        
    def copy_and_link(self):
        link = ""
        
        link += self.checkcopy_link(self.yml_filepath, "yml")
        link += self.checkcopy_link(self.html_filepath, "html")
        link += self.checkcopy_link(self.cvr_filepath, "cvr")
        link += self.checkcopy_link(self.xls_filepath, "xls")
           
        return link 

    def copy_and_link_pdf(self):
        link = ""
        
        link += self.checkcopy_link(self.pdf_filepath, "PDF-Report")
        if len(link):
            link += "<br/>\n"
            
        return link         
            
    def checkcopy_link(self, filepath, tag):
        if filepath == None:
            return ""
        if os.path.isfile(filepath):
            shutil.copy(filepath, PathFormer.ci_logs_results)
            link_file = os.path.split(filepath)[1]
            return "<a href=\""+PathFormer.testresults_rel+os.sep+link_file+"\">"+tag+"</a> "
        return ""
        
        
                
def split_filepath(filepath): 
    dir, basename = os.path.split(filepath) 
    name, extension = os.path.splitext(basename)   
    return (dir, name, extension) 
    
def create_folder(folderpath_to_create):
    
    if not os.path.exists(folderpath_to_create):
        try: 
            os.makedirs(folderpath_to_create)  
        except:
            print("Could not create folder: {folder_}".format(folder_ = folderpath_to_create))
            return

                