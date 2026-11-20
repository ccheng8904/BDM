"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : cibuild_excludes.py
Date of creation : 29.11.2021
Author Name/Dept : UAT Team, RBEI
Organization     :
Description      : This Python script is a generic solution for Exclusion/Masking
                   external library files for dashboard(V6.9.3)
"""

import os
import xml.etree.ElementTree as ET
import argparse

DIRECTORY = os.path.dirname(os.path.abspath(__file__)).replace('common\\build\\axivion\\config', '')
os.chdir(DIRECTORY)
XML_FILE_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'build.conf')

#Parsing the no_code_analysis variable
NO_CODE_PARSER = argparse.ArgumentParser()
NO_CODE_PARSER.add_argument(
    "--no_code_analysis",
    help="Locate the config path location.",
    nargs="+",
    action="store",
    dest="no_code_analysis",
    default=[""],
)
ARGS, _ = NO_CODE_PARSER.parse_known_args()
EXCLUDE_LIST = ARGS.no_code_analysis

# For preserving of XML comments
class CommentedTreeBuilder(ET.TreeBuilder):
    '''For preserving of XML comments 1 '''
    def __init__(self, *args, **kwargs):
        super(CommentedTreeBuilder, self).__init__(*args, **kwargs)

    def comment(self, data):
        '''Variables to preserve XML comments '''
        self.start(ET.Comment, {})
        self.data(data)
        self.end(ET.Comment)
CPARSER = ET.XMLParser(target=CommentedTreeBuilder())
TREE = ET.parse(XML_FILE_PATH, CPARSER)
ROOT = TREE.getroot()

for a in ROOT.iter('action'):
    if a.attrib['tool'] == 'stylecheck':
        for b in a.findall('input'):
            if b.attrib['name'] == 'excludes':
                l = b.findall('element')
                for element in l:
                    b.remove(element)
                for i in EXCLUDE_LIST:
                    F = ET.SubElement(b, 'element', value=str(i+"/*"))
                    F.tail = "\n\t\t\t"
                F.tail = '\n\t\t'

TREE.write(XML_FILE_PATH)
NEW_HEADER = '<?xml version="1.0" encoding="ISO-8859-1"?>\n' \
                 '<!DOCTYPE config SYSTEM "continuous.dtd">\n\n\n'

with open(XML_FILE_PATH, 'r') as contents:
    SAVE = contents.read()
with open(XML_FILE_PATH, 'w') as contents:
    contents.write(NEW_HEADER)
    contents.write(SAVE)
