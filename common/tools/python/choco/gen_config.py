#!/usr/bin/env python
"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : gen_config.py
Date of creation : 2021.05.04
Author Name/Dept : Nikolai Ehrhardt
Organization     : 
Description      : Will generate the config files (tool-download) for chocolatey-package-manager, and also consider cc-path if given.
"""

from argparse import ArgumentParser
import xml.dom.minidom as dom
import os.path, json

argP = ArgumentParser(description="Chocolatey-Config-Generator")

argP.add_argument("-tools", dest="tools", type=str, required=True, help="specify path to xml-file with tools-list")
argP.add_argument("-config", dest="config", type=str, required=True, help="specify path, where to put config files")
argP.add_argument("-cc", dest="cc", type=str, default=None, required=False, help="specify compiler")
argP.add_argument("-platform", dest="platform", type=str, default=None, required=False, help="specify platform")
args = argP.parse_args()

#const-tag-strings
c_element_tag = "element"
c_packages_tag = "packages"
c_package_tag = "package"

#const-key-strings
c_version_key = "version"
c_id_key = "id"

class ChocoConfig():

    #excluded package-id per cc_build_path
    ex_compiler_ids = {
        "keilArm": ["gcc_arm", "IAR_RX"],
        "armclang": ["gcc_arm", "IAR_RX"],
        "gccArm": ["IAR_RX", "keil"],
        "iarRx": ["gcc_arm", "keil"]
    }

    def __init__(self, args):
            
        self.platform = args.platform
        self.cc = args.cc
        
        self.ex_compi_ids = ChocoConfig.ex_compiler_ids.get(self.cc, [])
        if self.platform != "psoc4" and self.platform != None:
            self.ex_compi_ids.append("Psoc_Creator")
    
        self.tools = []
        tools_xml = dom.parse(args.tools)
        elements=tools_xml.getElementsByTagName(c_element_tag)
        for el in elements:
            self.tools.append(str(el.childNodes[0].data))
        
        self.config_xml = None
        self.package_tags = []
        self.packages_tag = None
        if os.path.isfile(args.config):
            ex_config_xml = dom.parse(args.config)
            self.package_tags = ex_config_xml.getElementsByTagName(c_package_tag)

        self.config_xml = dom.Document()
       
        self.packages_tag = self.config_xml.createElement(c_packages_tag)
        self.config_xml.appendChild(self.packages_tag)
        
        for package_tag in self.package_tags:
            self._append_foreign_tag(package_tag)
            
    def _get_relevant_attrs(self):
        relevant_attrs = []
        
        for t in self.tools:
            t_atts = t.split(";")
            current_atts = {}
            for att in t_atts:
                att_split = att.split("=")
                if att_split[0] == "Path" or att_split[0] == "make":
                    continue
                current_atts[att_split[0]] = att_split[1]
                
            if current_atts["id"] in self.ex_compi_ids:
                continue
            
            if len(self.package_tags) == 0 or self._mod_tag(current_atts):
                relevant_attrs.append(current_atts)
        
        return relevant_attrs
    
    #modify existing package, exg.: with newer version
    def _mod_tag(self, attrs):
        attrs_necessary = True
        for package_tag in self.package_tags:
            if package_tag.getAttribute(c_id_key) == attrs[c_id_key]:
                version = attrs.get(c_version_key, None)
                if version == None or package_tag.getAttribute(c_version_key) == version:
                    attrs_necessary = False
                    break
        return attrs_necessary 
        
    def _append_foreign_tag(self, package_tag):
        attrs = dict(package_tag.attributes.items())
        self._create_element_with_attributes(attrs)

    def _create_element_with_attributes(self, attrs):
        package_tag = self.config_xml.createElement(c_package_tag)    
        package_tag = self.packages_tag.appendChild(package_tag)
        for key in attrs:
            package_tag.setAttribute(key, attrs[key])
                    
    def create(self):
        for attrs in self._get_relevant_attrs():
            self._create_element_with_attributes(attrs)
        
        try:
            os.makedirs(os.path.split(args.config)[0])
        except:
            pass
        f = open(args.config, "wb+")
        f.write(self.config_xml.toprettyxml("\t", "\n", "UTF-8"))
        f.close()
        

def main(args):
    ChocoConfig(args).create()

if __name__ == "__main__":
    main(args)
    