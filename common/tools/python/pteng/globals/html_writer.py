"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : HTML_Writer.py
Date of creation : 26.01.2018
Author Name/Dept : Nikolai Ehrhardt, Santosh Pulasani
Organization     : RBG-DESO
Description      : A Python Class, that enables you to generate HTML-Files.
"""

import time
import os


class HTML_Writer:

    thunder, star, kot_, kot, kot_ex, green, red = "Warning.png", "Hook.png", "Mandatory.png", "Required.png", "Error.png", "Green.png", "Red.png"
    star_s, kot__s, kot_ex_s = "Hook_small.png", "Mandatory_small.png", "Error_small.png"
    pic_list = [thunder, star, kot_, kot, kot_ex, green, red]
    pic_list_s = [star_s, kot__s, kot_ex_s]

    def __init__(self, file):
        try:
            self.fo = open(file, "w")
        except:
            print("Failed to open file:{file_}".format(file_=file))
            
    def close_html(self):
        self.fo.close()
        
    def print_doctype(self):
        self.fo.write("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n")
        
    def start_html(self):
        self.fo.write("<html>\n")
        
    def end_html(self):
        self.fo.write("</html>\n")
        
    def open_head(self):
        self.fo.write("\t<head>\n")
        
    def close_head(self):
        self.fo.write("\t</head>\n")
        
    def enter_title(self, string, tabs=2):
        self.fo.write("\t"*tabs+"<title>"+string+"</title>\n")
        
    def enter_title_in_head(self, string):
        self.open_head()
        self.enter_title(string)
        self.close_head()
        
    def open_body(self):
        self.fo.write("\t"+"<body>\n")
        
    def close_body(self):
        self.fo.write("\t"+"</body>\n")
        
    def define_heading(self, string, level=1, color="#FFFFFF", ul=False, tabs=0):
        string_to_write = "<center><h{level_} color={color_}>{string_}</h{level_}></center>\n".format(level_ = str(level), color_=color, string_ = string)
        self.fo.write("\t"*tabs + string_to_write)
        
    def define_heading_with_time(self, string, level=1, color="#FFFFFF", ul=False, tabs=0):
        string_to_write = "<center><h{level_} color={color_}>{string_} - {time_}</h{level_}></center>\n".format(level_ = str(level), color_=color, string_ = string, time_ = time.strftime('%A - %d.%m.%Y - %H:%M:%S - Berlin'))
        self.fo.write("\t"*tabs + string_to_write)
        
    def simple_print(self, string):
        self.fo.write(str(string)+"\n")
        
    def start_div(self, alignment, tabs=1):
        string_to_write="\t"*(tabs)+"<div align={align_}>\n".format(align_ = alignment)
        self.fo.write(string_to_write)
    
    def end_div(self):    
        self.fo.write("</div>\n")
    
    def start_pre(self, tabs=1):
        self.fo.write("\t"*(tabs)+"<pre>\n")
        
    def end_pre(self, tabs=1):
        self.fo.write("\t"*(tabs)+"</pre>\n")
        
    def insert_pre(self, inputList ,tabs=0):
        string_to_write="\t"*(tabs)+"<pre>\n"
        for line in inputList:
            string_to_write += "\t"*(tabs)+line
        string_to_write += "\n"+"\t"*(tabs)+"</pre>\n"
        self.fo.write(string_to_write)
    def define_colgroup(self, cols=list(), tabs=1):
        string_to_write = "\t"*tabs+"<colgroup>\n"
        for width in cols:
            string_to_write += "\t"*(tabs+1)+"<col width={w_}>\n".format(w_ = str(width))
            
        string_to_write += "\t"*tabs+"</colgroup>\n"
        
        self.fo.write(string_to_write)

    def start_table(self, border=1, cellspacing=0, cellpadding=2, bgcolor="", attributes="", tabs=1):
        if bgcolor=="":
            bgcolor = "\"AliceBlue\"" 
        string_to_write="\t"*(tabs)+"<table border={border_} cellspacing={cellsp_} cellpadding={cellp_} bgcolor={bgcolor_} bordercolor=\"darkblue\" {attr_}>\n".format(border_= str(border), cellsp_=str(cellspacing), cellp_=str(cellpadding), bgcolor_=bgcolor, attr_=attributes)
        self.fo.write(string_to_write)
    
    def end_table(self, tabs=1):
        self.fo.write("\t"*(tabs)+"</table>\n")
        
    def start_tr(self, tabs=1):
        self.fo.write("\t"*tabs+"<tr>\n")
        
    def end_tr(self, tabs=1):
        self.fo.write("\t"*tabs+"</tr>\n")
    
    """
    Inserts a table down, same parameters as in real html tag are allowed.
    """    
    def insert_th(self, string, align="", valign="", width="", height="", tabs=1):
        self.fo.write("\t"*tabs+"<th align={al_} valign={val_}>".format(al_=align,val_=valign, w_=width, h_=height)+str(string)+"</th>\n")
        
    def insert_th_attr(self, string, attributes="", tabs=1):
        self.fo.write("\t"*tabs+"<th {att_}>".format(att_=attributes)+str(string)+"</th>\n")   
            
    def insert_td(self, string, align="", valign="", width="", height="", tabs=1):
        self.fo.write("\t"*tabs+"<td align={al_} valign={val_}>".format(al_ = align,val_ = valign, w_ = width, h_ = height)+str(string)+"</td>\n")
        
    def insert_td_attr(self, string, attributes="", align="", valign="", tabs=1):
        self.fo.write("\t"*tabs+"<td {attr_} align={al_} valign={val_}>".format(attr_=attributes, al_ = align,val_ = valign)+str(string)+"</td>\n")

    def insert_td_center(self, string, valign="", width="", height="", tabs=1):
        self.fo.write("\t"*tabs+"<td align=\"center\" valign={val_}>".format(val_ = valign, w_ = width, h_ = height)+str(string)+"</td>\n")
        
    def insert_td_w_and_h(self, string, align="", valign="", width="", height="", tabs=1):
        self.fo.write("\t"*tabs+"<td align={al_} valign={val_} width={w_} height={h_}>".format(al_ = align,val_ = valign, w_ = width, h_ = height)+str(string)+"</td>\n")
    
    def start_td(self, attributes="", tabs=1):
        self.fo.write("\t"*tabs+"<td {att_}>\n".format(att_=attributes))
        
    def start_td_center(self, attributes="", tabs=1):
        self.fo.write("\t"*tabs+"<td align=\"center\" {att_}>\n".format(att_=attributes))
    
    def end_td(self, tabs=1):
        self.fo.write("\t"*tabs+"</td>\n")
        
    def start_center(self, tabs=1):
        self.fo.write("\t"*tabs+"<center>\n")
        
    def end_center(self, tabs=1):
        self.fo.write("\t"*tabs+"</center>\n")
    
    def insert_link(self,link,mask,tabs=0):
        string_to_write= ("\t"*tabs + "<a href="+link+">"+mask+"</a>\n")
        self.fo.write(string_to_write)
        
    def start_link(self, link, tabs=0):
        string_to_write= ("\t"*tabs + "<a href="+link+">")
        self.fo.write(string_to_write)
        
    def end_link(self,tabs=0):
        self.fo.write("</a>\n")

    def insert_green_bullet(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.green+"\" alt=\"&radic;\" border=\"0\">"
        self.fo.write(string_to_write) 
        
    def insert_red_bullet(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.red+"\" alt=\"X\" border=\"0\">"
        self.fo.write(string_to_write)  
        
    def insert_star(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.star+"\" alt=\"X\" border=\"0\">"
        self.fo.write(string_to_write)  
        
    def insert_thunder(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.thunder+"\" alt=\"X\" border=\"0\">"
        self.fo.write(string_to_write)  
        
    def insert_kot(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.kot+"\" alt=\"X\" border=\"0\">"
        self.fo.write(string_to_write)  
        
    def insert_kot_(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.kot_+"\" alt=\"X\" border=\"0\">"
        self.fo.write(string_to_write)  
        
    def insert_kot_ex(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.kot_ex+"\" alt=\"X\" border=\"0\">"
        self.fo.write(string_to_write) 

    def insert_star_s(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.star_s+"\" alt=\"X\" border=\"0\">"
        self.fo.write(string_to_write)  
        
    def insert_kot__s(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.kot__s+"\" alt=\"X\" border=\"0\">"
        self.fo.write(string_to_write)  
        
    def insert_kot_ex_s(self, tabs=4):
        string_to_write="<img src=\"."+os.sep+HTML_Writer.kot_ex_s+"\" alt=\"X\" border=\"0\">"
        self.fo.write(string_to_write) 
        
    def insert_pic_by_index(self, index, tabs=4):
        if index == 0:
            self.insert_kot_ex()
        elif index == 1:
            self.insert_kot_()
        elif index == 2:
            self.insert_kot()
        elif index == 3:
            self.insert_thunder()
        else:
            self.insert_star()
            
    def insert_pic_by_validation(self, valid, tabs=4):
        if valid == 0:
            self.insert_star_s()
        elif valid == 1:
            self.insert_kot__s()
        elif valid == 2:
            self.insert_kot_ex_s()
       
    def insert_simpletext(self, text, tabs=4):
        string_to_write= ("\t"*tabs +""+ text)
        self.fo.write(string_to_write)    
        
    def insert_bad_button(self, name, tabs=0): 
        string_to_write = "\t"*tabs + "<button class=\"collapsible_bad\">"+name+"</button>\n"
        self.fo.write(string_to_write) 
        
    def insert_good_button(self, name, tabs=0): 
        string_to_write = "\t"*tabs + "<button class=\"collapsible_good\">"+name+"</button>\n"
        self.fo.write(string_to_write) 
        
    def open_div_content(self, tabs=0):
        string_to_write = "<div class=\"content\">\n"
        self.fo.write(string_to_write) 
        
    def end_div(self):
        string_to_write = "</div>\n"
        self.fo.write(string_to_write) 
        
    def insert_br(self):
        string_to_write = "<br/>\n"
        self.fo.write(string_to_write)
               
    def insert_meta(self):
        string_to_write = "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n"
        self.fo.write(string_to_write) 
        
    def insert_style(self):
        string_to_write = """
        <style>
        .collapsible_good {
          background-color: #008000;
          color: white;
          cursor: pointer;
          padding: 18px;
          width: 100%;
          border: none;
          text-align: left;
          outline: none;
          font-size: 15px;
        }
        
        .collapsible_bad {
          background-color: #B22222;
          color: white;
          cursor: pointer;
          padding: 18px;
          width: 100%;
          border: none;
          text-align: left;
          outline: none;
          font-size: 15px;
        }
        
        .collapsible_good:hover {
          background-color: #006400;
        }
        
        .collapsible_bad:hover {
          background-color: #8B0000;
        }
        
        .content {
          padding: 0 18px;
          max-height: 0;
          overflow: auto;
          transition: max-height 0.2s ease-out;
        }
        
        
        </style>
        """ 
        self.fo.write(string_to_write)     
            
    def insertTableJava(self):
        string_to_write = """
        <script>
        var coll = document.getElementsByClassName("collapsible_bad");
        var i;
        
        for (i = 0; i < coll.length; i++) {
          coll[i].addEventListener("click", function() {
            this.classList.toggle("active");
            var content = this.nextElementSibling;
            if (content.style.maxHeight){
              content.style.maxHeight = null;
            } else {
              content.style.maxHeight = content.scrollHeight + "px";
            } 
          });
        }
        </script>
        <script>
        var coll = document.getElementsByClassName("collapsible_good");
        var i;
        
        for (i = 0; i < coll.length; i++) {
          coll[i].addEventListener("click", function() {
            this.classList.toggle("active");
            var content = this.nextElementSibling;
            if (content.style.maxHeight){
              content.style.maxHeight = null;
            } else {
              content.style.maxHeight = content.scrollHeight + "px";
            } 
          });
        }
        </script>
        """
        self.fo.write(string_to_write)
        
    def insert_bullet(self, checker):
        if checker:
            self.insert_green_bullet(tabs=5)
        else:
            self.insert_red_bullet(tabs=5)

    def write_KPITable(self, files, files_att, languages, languages_att, KPI_per_Unit, kpi):
    
        def name_of_file(file):
            return file["name"]
                   
        files.sort(key=name_of_file)
     
        self.start_table(1, 0, 10, tabs=2)
    
        self.start_tr(tabs=3)
        
        for att in languages_att:
            self.insert_th(str(att), align="center", valign="middle", tabs=4)
        
        self.insert_th("Instrumented", align="center", valign="middle", tabs=4)
        self.insert_th("Tested", align="center", valign="middle", tabs=4)
        self.insert_th("Percentage", align="center", valign="middle", tabs=4)
        self.end_tr(tabs=3)
        
        for l_dict in languages:
            self.start_tr(tabs=3)
            for att in languages_att:
                self.insert_td(str(l_dict[att]), align="left", valign="middle", tabs=4)
            
            if l_dict["name"] == "C":                
                self.insert_td(str(kpi[0]), align="left", valign="middle", tabs=4)
                self.insert_td(str(kpi[1]), align="left", valign="middle", tabs=4)
                self.insert_td(str(kpi[2]), align="left", valign="middle", tabs=4)
            else:
                self.insert_td("-", align="left", valign="middle", tabs=4)
                self.insert_td("-", align="left", valign="middle", tabs=4)
                self.insert_td("-", align="left", valign="middle", tabs=4)
                
                
            self.end_tr(tabs=3)
        self.end_table()
        self.insert_simpletext("<br/>", tabs=5)
        self.start_table(1, 0, 10, tabs=2)
    
        self.start_tr(tabs=3)
        
        for att in files_att:
            self.insert_th(str(att), align="center", valign="middle", tabs=4)
        
        self.insert_th("Testable", align="center", valign="middle", tabs=4)
        self.insert_th("Tested", align="center", valign="middle", tabs=4)
        self.insert_th("Percentage", align="center", valign="middle", tabs=4)
        self.end_tr(tabs=3)
        
        for f_dict in files:
        
            dir, basename = os.path.split(f_dict["name"]) 
            unit, extension = os.path.splitext(basename) 
            if (extension in [".cpp", ".c", "hpp", ".h"]):
                self.start_tr(tabs=3)
                for att in files_att:
                    self.insert_td(str(f_dict[att]), align="left", valign="middle", tabs=4)
                
                if unit in KPI_per_Unit and (extension == ".c" or extension == ".cpp"):
                    self.insert_td(str(KPI_per_Unit[unit][0]), align="left", valign="middle", tabs=4)
                    self.insert_td(str(KPI_per_Unit[unit][1]), align="left", valign="middle", tabs=4)
                    self.insert_td(str(KPI_per_Unit[unit][2]), align="left", valign="middle", tabs=4)
                    KPI_per_Unit.pop(unit)
                else:
                    self.insert_td("-", align="left", valign="middle", tabs=4)
                    self.insert_td("-", align="left", valign="middle", tabs=4)
                    self.insert_td("-", align="left", valign="middle", tabs=4)
                    
                    
                self.end_tr(tabs=3)
                
        self.end_table()
        self.insert_simpletext("<br/>", tabs=5)
        
    def writeProjectTable(self, bproject):
        self.start_table(1, 0, 10, tabs=2)
    
        self.start_tr(tabs=3)
        self.insert_th(bproject.name, align="center", valign="middle", tabs=4)
        for target in bproject.target_list:
            self.insert_th(str(target), align="center", valign="middle", tabs=4) 
        self.end_tr(tabs=3)
        
        for citxt_restname in bproject.citxt_restnames:
            
            self.start_tr(tabs=3)
            self.insert_td(str(citxt_restname), align="center", valign="middle", tabs=4)
            
            target_rest_dict = bproject.getTargetDictOfRestNames(citxt_restname)
            
            for target in bproject.target_list:
                
                if target in target_rest_dict:
                    citxt = target_rest_dict[target]
                    self.start_td(attributes="align=\"center\" valign=\"middle\"", tabs=4)
                    link_string = "."+os.sep+citxt.filename
                    self.start_link(link_string, tabs=5)
                    self.insert_bullet(citxt.validation)
                    self.end_link(tabs=5)
                    self.insert_simpletext(citxt.validator.getTargetString(), tabs=5)
                    self.end_td(tabs=4)
                    
                else:
                    self.insert_td(str("N/A"), align="center", valign="middle", tabs=4)
                    
            self.end_tr(tabs=3)
        
        self.end_table()
