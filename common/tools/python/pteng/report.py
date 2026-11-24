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
Description      : Offers the scripts to report testexecutins 
                   and results, also pdf-reporting is supported.
"""


import getpass
import subprocess
import os
import sys
import time
import traceback
import yaml

from globals.constants import exec_const
from globals.folder_utils import PathFormer
from globals.file_loader import FileLoader
from globals.html_writer import HTML_Writer
from globals.xls_writer import XLS_Writer

import logging

import shutil

#from gcovr_wrapper import GcovCoverageInfo

EngineLogger = logging.getLogger("global")


template_line_projectname   = 26-1 # calculating the array_index, therefore minus 1
template_line_author        = 29-1 # calculating the array_index, therefore minus 1
template_line_for_filling   = 108-1 # calculating the array_index, therefore minus 1
       

class report():

    path_report_template = PathFormer.prj_path+"common"+os.sep+"tools"+os.sep+"Templates"+os.sep+"Template_tex2pdf_ut_report.tex"

    path_TestSequenceXSL = PathFormer.prj_path+"common"+os.sep+"tools"+os.sep+"python"+os.sep+"pteng"+os.sep+"TestSequence.xsl"
    
    def __init__(self,file_loader, proj_kpi, cvr_line_info):
        
        self.project = file_loader.test_desc.project
        
        self.file_loader = file_loader
        self.proj_kpi = proj_kpi
        self.cvr_line_info = cvr_line_info
        
        self.template = open(report.path_report_template,"r").readlines()    
        self.template[template_line_projectname]   = self.clean_for_latex("%s\n\r"%self.project)
        self.template[template_line_author]        = self.clean_for_latex("%s\n\r"%getpass.getuser())
        
        self.filling_information  = ""
        
        self.created_Html_Reports = []
        
        
    def generateReports(self, std, pdf):
        for fl in self.file_loader.ToReport:
            fl.createResultdir()
            if fl.loaded_type == FileLoader.TSEQ:
                self.create_COVRReport_from_Description(fl)
                if std:
                    self.create_HTMLReport_from_TestDescriptor(fl)
                    self.create_YMLReport_from_TestDescriptor(fl)
            elif fl.loaded_type == FileLoader.INT:
                xlsp = XLS_Writer(fl.xls_filepath)
                fl.test_desc.print_xls(xlsp)
                xlsp.close()
        
        if fl.loaded_type in [FileLoader.TSEQ, FileLoader.TSUITE]:
            if pdf:
                self.create_PDFReport_from_Description()

    def writeHTMLResultFiles(self):
        
        if not os.path.exists(PathFormer.ci_logs_results):
            os.makedirs(PathFormer.ci_logs_results)
            
        for pic in HTML_Writer.pic_list_s:
            shutil.copyfile(PathFormer.pics_path+os.sep+pic, PathFormer.ci_logs_results+pic)
            
        for pic in HTML_Writer.pic_list_s:
            shutil.copyfile(PathFormer.pics_path+os.sep+pic, PathFormer.ci_logs_path+pic)
        
        TSuite_html_name = time.strftime("%Y_%m_%d-%H_%M_%S_")+self.file_loader.basename
        hwr = HTML_Writer(PathFormer.ci_logs_path+TSuite_html_name+PathFormer.html_ext)
        
        hwr.print_doctype()
        
        hwr.start_html()
        hwr.enter_title_in_head(self.file_loader.basename)
        
        hwr.open_body()
        hwr.define_heading_with_time(self.file_loader.test_desc.project)
        
        self.file_loader.test_desc.html_summary(hwr, pdf_report=self.file_loader.copy_and_link_pdf())
        
        hwr.start_center()
        hwr.start_table()
        header_list = ["Sequence", "Report", "Validation", "Coverage"]
        hwr.start_tr()
        for header in header_list:
            hwr.insert_th(header)
        hwr.end_tr()
        for fl in self.file_loader.ToReport:
                
            if not fl.test_desc == None:
                hwr.start_tr()
                hwr.insert_td(fl.basename)
                hwr.insert_td(fl.copy_and_link())
                hwr.start_td()
                hwr.insert_pic_by_validation(fl.test_desc.validation)
                hwr.end_td()
                hwr.insert_td(fl.test_desc.covr_string)
                hwr.end_tr()
                
        hwr.end_table()
        hwr.end_center()
        hwr.close_body()
        hwr.end_html()
        hwr.close_html()
        
        print("PTEng:TSuite:"+TSuite_html_name) 
    
    def create_COVRReport_from_Description(self, file_loader):
        
        file_loader.setCVRFile()
        
        if self.proj_kpi == None:
           EngineLogger.error("Have no KPI-dict for: {file_}".format(file_ = file_loader.cvr_filepath)) 
           return
        if self.cvr_line_info == None:
           EngineLogger.error("Have no COVR_lineInfo for: {file_}".format(file_ = file_loader.cvr_filepath)) 
           return
        
        html_output = ""
        for test_seq in file_loader.test_desc.test_sequences_list:
            if test_seq.unit in self.proj_kpi and test_seq.function in self.cvr_line_info:
                function_line_list = self.cvr_line_info[test_seq.function]
                code_lines = function_line_list[0]
                cvr_index_lines = function_line_list[1]
                covr_vars_of_module = self.proj_kpi[test_seq.unit]
                
                covr_var_tpl = test_seq.function+"_Line[_Q_]"
                
                html_output += "\n<br/>\n<table border=\"1\" cellspacing=\"0\" cellpadding=\"3\"><tr><td colspan=\"3\"><b>"+test_seq.function+"</b></td></tr>\n"
                html_output += "<tr><td>"+"CVR-Index"+"</td><td>"+"Count"+"</td><td>"+"Code"+"</td></tr>\n"
                
                for index in range(0, len(cvr_index_lines)):
                    
                    list_counts = []
                    for cvr_index in cvr_index_lines[index]:
                        list_counts.append(covr_vars_of_module[covr_var_tpl.replace("_Q_", str(cvr_index))])
                    str_lst_counts = None
                    if len(list_counts) == 0:
                        str_lst_counts = "-"
                    else:
                        str_lst_counts = str(list_counts)[1:-1]
                    html_output += "<tr><td>"+str(cvr_index_lines[index])+"</td><td>"+str(str_lst_counts)+"</td><td><pre>"+code_lines[index]+"</pre></td></tr>\n"
                
                html_output += "</table>\n"    
            else:
                html_output += "\n<br/>\n"+"Could not retrieve Coverage for function {func_} of module: {module_}<br/>\n".format(func_ = test_seq.function, module_ = test_seq.unit)
        
        with open(file_loader.cvr_filepath, "w") as f:
            f.write(html_output)
            
        if os.path.isfile(file_loader.cvr_filepath):
            pass
        else:
            EngineLogger.error("No COVR-report created : {file_}".format(file_ = file_loader.cvr_filepath)) 
    
    
    def create_HTMLReport_from_TestDescriptor(self, file_loader):
        file_loader.setHTMLFile()
        
        hwr = HTML_Writer(file_loader.html_filepath)
        hwr.print_doctype()
        
        hwr.start_html()
        hwr.enter_title_in_head(self.file_loader.basename)
        
        hwr.open_body()
        hwr.define_heading(self.file_loader.basename) 
        hwr.start_center()
        hwr.start_table(border=0)       
        file_loader.test_desc.print_html(hwr)   
        hwr.end_table()
        hwr.end_center()
        hwr.close_body()
        hwr.end_html()
        hwr.close_html()        
  
    def create_YMLReport_from_TestDescriptor(self, file_loader):
        file_loader.setYMLFile()
        
        with open(file_loader.yml_filepath, "w+") as f:
            yaml.dump(file_loader.test_desc.getDict(), f) 
               
    """
    Create a PDF-Report from a XML-report-File.
    Change save path of pdf-report
    """       
    def create_PDFReport_from_Description(self):
        
        target_information = ""
        for target_information_part in self.file_loader.test_desc.target_information.split("\n"):
            target_information  += self.clean_for_latex(target_information_part)+"\\\\"
        
        if (target_information):
            self.insert_section("TEngSIO Info", "Information about the target:")
            self.insert_at_current_without_cleaning("(received from TEngSIO layer)\\\\ \\\\")
            self.insert_at_current_without_cleaning(target_information)
        else:
            self.insert_at_current("No Target-Information available.")
        
        self.insert_at_current_without_cleaning("\nConnection: "+self.file_loader.test_desc.conn_flags +"\n")
        
        self.insert_section("Test Summary", "")
        test_summary = "Reported on {dmy_} at {hms_}.\n".format(dmy_=time.strftime("%d.%m.%Y"), hms_=time.strftime("%H:%M:%S"))
        self.insert_at_current_without_cleaning(test_summary)
        self.insert_at_current("Execution by: {exec_}\n".format(exec_=self.file_loader.test_desc.execution))
        self.insert_at_current_without_cleaning("\nValidation: {valid_}\n".format(valid_=exec_const.getValidationTexStringFromEnumerator(self.file_loader.test_desc.validation)))
        self.insert_at_current("Coverage: prj-{pcovr_} functions-{covr_}".format(pcovr_=self.file_loader.test_desc.prj_covr_string, covr_=self.file_loader.test_desc.covr_string))
        
        for file_loader in self.file_loader.ToReport:
            for test_seq in file_loader.test_desc.test_sequences_list:
                
                self.insert_subsection("Test Sequence: {id_}".format(id_=test_seq.identifier), "")
                self.insert_at_current_without_cleaning("Validation: {valid_}".format(valid_=exec_const.getValidationTexStringFromEnumerator(test_seq.validation)))
                self.insert_at_current("Description: {desc_}".format(desc_=test_seq.description))
                self.insert_at_current("Unit: {unit_}".format(unit_=test_seq.unit)) 
                self.insert_at_current("Function: {func_}".format(func_=test_seq.function))
                self.insert_at_current("Testcases: {count_}".format(count_=str(len(test_seq.test_cases_list)))) 
                self.insert_at_current("Signature:".format(desc_=test_seq.function_sig))
                self.insert_at_current_without_cleaning("\\small\n")
                self.insert_at_current("{sig_}".format(sig_=test_seq.function_sig))
                self.insert_at_current_without_cleaning("\\normalsize\n")
                
                self.insert_at_current("SequenceCoverage: {covr_}".format(covr_= test_seq.covr_string))

                self.creatSeqTexTableOfCoverage(test_seq)
                
                if len(test_seq.exe_info):
                    self.insert_at_current("Exe-Info: {info_}".format(info_ = test_seq.exe_info))
                
                for testcase in test_seq.test_cases_list:
                    
                    self.insert_subsubsection("TestCase: {id_}".format(id_ = testcase.identifier), "") 
                      
                    self.insert_at_current_without_cleaning("Validation: {valid_}".format(valid_ = exec_const.getValidationTexStringFromEnumerator(testcase.validation) ))
                    
                    self.insert_at_current("Description: {desc_}".format(desc_ = testcase.description))
                    
                    if len(testcase.exe_info):
                        self.insert_at_current("Exe-Info:") 
                        for splitter in testcase.exe_info.split("\n"):
                            self.insert_at_current("{add_info_}".format(add_info_ = splitter)) 
                            
                    self.insert_at_current("Start-Time: {start_time_}".format(start_time_ = str(testcase.start_time)))
                    self.insert_at_current("Execution-Time: {delta_time_}s".format(delta_time_ = str(testcase.delta_time)))   
                    
                    if not testcase.set_up_function == None:
                        self.insert_at_current("Set-Up-Function: {setup_}".format(setup_ = testcase.set_up_function))
                    if not testcase.tear_down_function == None:
                        self.insert_at_current("Tear-Down-Function: {teardown_}".format(teardown_ = testcase.tear_down_function))
                    
                    self.insert_at_current_without_cleaning("\\small\n")
                    
                    if len(testcase.parameter_list):
                        self.centering("Parameters:")
                        
                        self.insert_table_beginning_at_current("|c|c|", "In & In-solved")
                        
                        for prm in testcase.parameter_list:
                            self.insert_multicolum_data_at_current(2, " |l| ", prm.identifier, adds="", no_hl=False)
                            self.insert_table_data_at_current(prm.getAsList())
                           
                        self.insert_table_ending_at_current()
                    
                    if len(testcase.return_list):
                        self.centering("Return:")
                        self.insert_table_beginning_at_current("|c|c|c|c|", "Exp & Exp-solved & Operator & Out")
                    
                        for ret in testcase.return_list:
                            self.insert_multicolum_data_at_current(3, " |l| ", ret.identifier, adds=exec_const.getValidationColumnTexStringFromEnumerator(ret.validation), no_hl=False) 
                            self.insert_table_data_at_current(ret.getAsList())
                    
                        self.insert_table_ending_at_current()
                    
                    if len(testcase.variables_list):
                        self.centering("Variables:")
                        
                        self.insert_table_beginning_at_current("|c|c||c|c|c|c|", "In & In-solved & Exp & Exp-solved & Operator & Out")
                    
                        for var in testcase.variables_list:                          
                            self.insert_multicolum_data_at_current(5, " |l| ", var.identifier, adds=exec_const.getValidationColumnTexStringFromEnumerator(var.validation), no_hl=False) 
                            self.insert_table_data_at_current(var.getAsList())
                            
                            if len(var.exe_info):  
                                self.insert_multicolum_data_at_current(6, " |l| ", var.exe_info, no_hl=False) 
                        
                        self.insert_table_ending_at_current()
                    
                    self.insert_at_current_without_cleaning("\\normalsize\n")
        
        self.run_pdflatex(self.file_loader)                      

    def creatSeqTexTableOfCoverage(self, test_seq):
        if test_seq.unit in self.proj_kpi and test_seq.function in self.cvr_line_info:
            self.centering("Coverage per Line:")
            function_line_list = self.cvr_line_info[test_seq.function]
            code_lines = function_line_list[0]
            cvr_index_lines = function_line_list[1]
            covr_vars_of_module = self.proj_kpi[test_seq.unit]
            
            covr_var_tpl = test_seq.function+"_Line[_Q_]"
            
            self.insert_table_beginning_at_current("|c|c|l|", "CVR-Index & Count & Code")
            
            for index in range(0, len(cvr_index_lines)):
                
                list_counts = []
                for cvr_index in cvr_index_lines[index]:
                    list_counts.append(covr_vars_of_module[covr_var_tpl.replace("_Q_", str(cvr_index))])
                str_lst_counts = None
                if len(list_counts) == 0:
                    str_lst_counts = "-"
                else:
                    str_lst_counts = str(list_counts)[1:-1]
                self.insert_table_data_at_current([str(cvr_index_lines[index]), str(str_lst_counts), str(code_lines[index])])
            
            self.insert_table_ending_at_current()   
        else:
            self.centering("Could not retrieve per Line Coverage.")        
         
        
    def run_pdflatex(self, file_loader):
        dummy = os.getcwd()

        file_loader.setTexFile() 
        file_loader.createResultdir()
        
        self.template[template_line_for_filling] = self.filling_information
        self.filling_information    = ""
        
        creation_file = open(file_loader.tex_filepath,"w+")
        for line in self.template:
            creation_file.write(line)
        creation_file.close()
        
        os.chdir(file_loader.result_dir)
        
        os.environ["path"] = os.environ["path"]+";M:/SwTools/TeXLive2012/bin/win32/" 
        
        pdflatex_out1=""
        pdflatex_out2=""  
        
        print("-"*150) 

        try:

            pdflatex_out1 = subprocess.check_output("pdflatex --interaction=nonstopmode {tex_file_}".format(tex_file_ = file_loader.tex_file))   
            pdflatex_out2 = subprocess.check_output("pdflatex --interaction=nonstopmode {tex_file_}".format(tex_file_ = file_loader.tex_file)) # 2 times to resolve references in LaTeX
            
            pdflatex_out2 = pdflatex_out2.decode("utf8")
            
            splitted_output = str(pdflatex_out2).split("\n")
            
            out_file = splitted_output[-4].replace("\r", "")
            bytes_count = splitted_output[-3].replace("\r", "")[:-1]
            tex_info = splitted_output[0].replace("\r", "")
            
            EngineLogger.info("{f1_}{f2_}".format(f1_ = out_file, f2_ = bytes_count))

            try:
                subprocess.check_output("latexmk -c -silent {tex_file_}".format(tex_file_ = file_loader.tex_file)) # clean pdflatex auxiliary files, check_output to supress stdout output of process
            except:
                exc_info_ = sys.exc_info()
                formatted_exc = traceback.format_exception(exc_info_[0], exc_info_[1], exc_info_[2])
                EngineLogger.error("Cleaning of auxiliary files for {pdf_name} caused: {exc_val_}".format(pdf_name = file_loader.tex_file, exc_val_ = formatted_exc[-1]))
            
                
        except Exception as e:                  #traceback modul
            exc_info_ = sys.exc_info()
            formatted_exc = traceback.format_exception(exc_info_[0], exc_info_[1], exc_info_[2])
            EngineLogger.error("Creation of PDF-report {pdf_name} caused: {exc_val_}".format(pdf_name = file_loader.tex_file, exc_val_ = formatted_exc[-1]))
            EngineLogger.error("Further Info: The PDF is created in most cases.")
            os.chdir(dummy)
            return None
        
        if os.path.isfile(file_loader.pdf_filepath):
            print("--> PDF-Report successfully created: {file_}".format(file_ = file_loader.pdf_file))
        else:
            print("--> No PDF-Report created: {file_}".format(file_ = file_loader.pdf_file))
        
        os.chdir(dummy)
        
    
        
    def insert_table_beginning_at_current(self, cols, columns):
        table = """\\begin{center}
{
\\fontsize{7}{10}
\\tablefirsthead{
\\hline
columns \\\\\\hline}
\\tablehead{
\\hline\\multicolumn{len_col} {|l|}{\\scriptsize\\sl continued from previous page}\\\\
\\hline    
columns \\\\\\hline}
\\tabletail{
\\hline
\\multicolumn{len_col}{|l|}{\\scriptsize\\sl continued on next page}\\\\
\\hline}
\\tablelasttail{\\hline}
\\begin {supertabular}{ cols } \\hline
"""

        table = table.replace("cols", cols)
        table = table.replace("columns", columns)
        table = table.replace("len_col", str(columns.count("&")+1))
        
        self.filling_information += table
        
    def insert_table_ending_at_current(self):
        table = """\end {supertabular}
}
\end{center}
"""

        self.filling_information += table
        
    def insert_table_data_at_current(self,table_row_list, clean_col=True):
        table_row = ""
        for column in table_row_list:
            if clean_col:
                table_row += self.clean_for_latex(column) + "& "
            else:
                table_row += str(column) + "& "
        table_row = table_row[:-2]+" \\\\\\hline\n" #remove last "& "
        self.insert_at_current_without_cleaning(table_row)
        
    def insert_multicolum_data_at_current(self, len_col, col, data, adds="", no_hl=False):
        data = self.clean_for_latex(data)
        adds = self.clean_for_latex(adds)
        
        if no_hl:
            hline = ""
        else:
            hline = "\\hline"
            
        len_col = "{"+str(len_col)+"}"
        col = "{"+col+"}"
        data = "{"+data+"}"
        
        if len(adds):
            adds = "&"+adds
            
        self.filling_information += "\\multicolumn{len_col_}{col_}{data_}{adds_} \\\\ {hline_}\n".format(len_col_=len_col, col_=col, data_=data, adds_=adds, hline_=hline)
        
    
        
    def insert_at_current(self,content):
        self.filling_information += "\n%s\n"%self.clean_for_latex(content)
    
    def insert_at_current_without_cleaning(self,content):
        self.filling_information += "%s\n"%content
    
    def insert_section(self,name,content):
        template="""
\\newpage
\section{%s}

%s


"""
        clean_content = self.clean_for_latex(content)
        clean_name    = self.clean_for_latex(name)
        self.filling_information += template%(clean_name,clean_content)
    
    def insert_subsection(self,name,content):
        template="""
\subsection{%s}

%s

"""
        clean_content = self.clean_for_latex(content)
        clean_name    = self.clean_for_latex(name)
        self.filling_information += template%(clean_name,clean_content)

    def insert_subsubsection(self,name,content):
        template="""
\subsubsection{%s}

%s

"""
        clean_content = self.clean_for_latex(content)
        clean_name    = self.clean_for_latex(name)
        self.filling_information += template%(clean_name,clean_content)
    
    def centering(self,content):
        template="""
\\begin{center}
%s
\\end{center}
"""
        self.filling_information +=  template%(content)
        
    def clean_for_latex(self,dirty):
        #valid_ord_list=[10,161,163,167,169,170,171,174,182,183,184,186,187,191,192,193,194,195,196,197,198,199,200,201,202,203,204,205,206,207,209,210,211,212,213,214,216,217,218,219,220,221,223,224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,241,242,243,244,245,246,248,249,250,251,252,253,255]
        if dirty in ["\\PAS","\\FAI","\\PASSED","\\FAILED","\\cb", "\\EXC", "\\EXCEPTION", "\\True", "\\False"]:
            return dirty
        clean=str(dirty)
        clean_str = ""
        for character in clean:
            if (32 <= ord(character) <= 126): # or (ord(character) in valid_ord_list):
                clean_str=clean_str+character

        clean = clean_str
        clean=clean.replace("\\","\\textbackslash ")
        clean=clean.replace("%","\%")
        clean=clean.replace("^","\^")
        clean=clean.replace("_","\_")
        clean=clean.replace("&","\&")
        clean=clean.replace("$","\$")
        clean=clean.replace("#","\#")
        clean=clean.replace("~","\\textasciitilde ") #\~ makes tilde over character 
        clean=clean.replace("{","\{")
        clean=clean.replace("}","\}")
        clean=clean.replace("*","\**")
        
        return clean
    
    
 