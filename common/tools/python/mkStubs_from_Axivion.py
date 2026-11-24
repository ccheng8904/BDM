#!/usr/bin/env python
"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : mkStubs_from_Axivion.py
Date of creation : 7.8.2018
Author Name/Dept : Nikolai Ehrhardt, Ulrike Brueggemann
Organization     : 
Description      : Loads the SourceCodeDescriptor and genrates the stub-files and testframes.
"""
import os
from argparse import ArgumentParser
import xml.dom.minidom as dom

from pteng.soco.SourceCodeDescriptor import SourceCodeDescriptor
from pteng.uti.mkStubs import mkInstrument
from pteng.globals.folder_utils import create_folder

def main():

    argP = ArgumentParser(description='Creates the stubs.')

    argP.add_argument("-cfg_file", dest="cfg_file", required=True, help="filepath of config file")
  
    args = argP.parse_args()
    
    cfg_xml = dom.parse(os.path.abspath("./"+args.cfg_file))
    
    IncludeFiles    = list([])
    ExcludeFolders  = list([])
    ExcludeFiles    = list([])
    Units_ToStub    = list([])
    Header_with_TI  = list([])
    work_with_testframes = True
    folder_name     = ""
    gen_wrapper = True
    Units_under_Test = list([])
    Units_with_TI_to_TF = list([])
    Units_to_TF = list([])
    CFILES_not_to_Stub = list([])
    provided_rd_Headers = list([])
    Addtional_Stubfunctions = list([])
    Exclude_local_Stub = list([])
    stub_locals = True
    place_covr = True
    
    for node in cfg_xml.childNodes:
        if node.localName == "config":
            
            for tag in node.childNodes:
                if tag.localName == "WORK_WITH_TESTFRAMES":
                    if str(tag.childNodes[0].data).upper() == "TRUE":
                        work_with_testframes = True
                if tag.localName == "template_folder":
                    tmplFolder = str(tag.childNodes[0].data)
                if tag.localName == "stub_rootfolder":
                    stubPath = str(tag.childNodes[0].data)
                if tag.localName == "TET_testscript_rootfolder":
                    scriptPath = str(tag.childNodes[0].data)               
                if tag.localName == "TET_cfg_output_folder":
                    TETConfigPath = str(tag.childNodes[0].data) 
                if tag.localName == "Axivion_IR_filepath":
                    graphFile = str(tag.childNodes[0].data)  
                if tag.localName == "GENERATE_STUBS_FOR_UNITS":
                    Units_ToStub.append(str(tag.childNodes[0].data))
                if tag.localName == "HEADER_WITH_TI":
                    Header_with_TI.append(str(tag.childNodes[0].data))    
                if tag.localName == "project":
                    project_name = str(tag.childNodes[0].data) 
                if tag.localName == "folder":
                    folder_name = str(tag.childNodes[0].data) 
                if tag.localName == "type_system_key":
                    type_system_ID = str(tag.childNodes[0].data)     
                if tag.localName == "UNITS_TO_PUT_UNDER_TEST":
                    Units_under_Test.append(str(tag.childNodes[0].data))
                if tag.localName == "Units_to_TF":
                    Units_to_TF.append(str(tag.childNodes[0].data))
                if tag.localName == "Units_with_TI":
                    Units_with_TI_to_TF.append(str(tag.childNodes[0].data)) 
                if tag.localName == "CFILES_WITH_PROVIDED_STUB_FILES":
                    CFILES_not_to_Stub.append(str(tag.childNodes[0].data))
                if tag.localName == "provided_rd_Header":
                    provided_rd_Headers.append(str(tag.childNodes[0].data))
                if tag.localName == "ADDITIONAL_STUBFUNCTIONS":
                    Addtional_Stubfunctions.append(str(tag.childNodes[0].data))
                if tag.localName == "EXCLUDE_LOCAL_STUB":
                    Exclude_local_Stub.append(str(tag.childNodes[0].data))
                if tag.localName == "bauhaus_py_modules":
                    sys.path.append(str(tag.childNodes[0].data))
                    sys.path.append(str(tag.childNodes[0].data)+"/bauhaus")
                if tag.localName == "stub_locals":
                    stub_locals = str(tag.childNodes[0].data)
                    if stub_locals.lower() == "true":
                        stub_locals = True
                    else:
                        stub_locals = False
                if tag.localName == "place_covr":
                    place_covr = str(tag.childNodes[0].data)
                    if place_covr.lower() == "true":
                        place_covr = True
                    else:
                        place_covr = False


    
    abs_stub_path = os.path.abspath("./"+stubPath)
    
    if not os.path.exists(abs_stub_path):
        try:
            os.makedirs(abs_stub_path)  
        except:
            errorstring = "Creation failed: " + abs_stub_path
            print(errorstring)
            return


    create_folder(stubPath+"\\Testframes")
    create_folder(stubPath+"\\TestConfig\\AutoDefault")
    create_folder(stubPath+"\\TestConfig\\User")
    
    
    source_description = SourceCodeDescriptor.CreateFrom_AxivionGraphFilepath(graphFile, project_name, folder_name, type_system_ID)

    source_description.matching_units = []
    

    source_description.matching_units.extend(Units_ToStub)
    
    for unit in Units_ToStub:
        source_description.matching_units_upper.append(str(unit.upper()))
    
    for unit in CFILES_not_to_Stub:
        if unit in source_description.matching_units:
            for match_unit in source_description.matching_units:
                if unit == match_unit:
                    source_description.matching_units.remove(unit)
                    
            for match_unit in source_description.matching_units_upper:
                if unit.upper() == match_unit:
                    source_description.matching_units_upper.remove(unit.upper())
                        
    print("StubPath: ", [stubPath]) 
        
    if __debug__:
       units_as_string_list = list()
       for unit in source_description.matching_units:
           units_as_string_list.append(str(unit))
       print("UnitsToBeStubbed: ", units_as_string_list)

    Available_Unit_names_upper  = [name.upper() for name in list(source_description.unit_dictionary.keys())]   

    all_units = list([])
    for unit_desc in list(source_description.unit_dictionary.values()):
        all_units.append(unit_desc.unit_name.upper())
    
    not_found_unit = False 
    
    for m_unit in source_description.matching_units_upper:
        if not m_unit in Available_Unit_names_upper:
            print("{m_unit_} shall be stubbed, but was not linked with Axivion.".format(m_unit_ = m_unit))
    
    
    for unit in Units_to_TF:
        if not unit.upper() in all_units:    
            print("{unit_} shall be testframed but is not found in SourceCodeDescriptor, but was not linked with Axivion.".format(unit_ = unit))
            not_found_unit = True
            

    print("UnitsToBeTestFramed: ", Units_to_TF)
    
    if(not_found_unit):
        print("AvailableUnits: ", all_units)


    instrument = mkInstrument(source_description, tmplFolder, stubPath, work_with_testframes)

    instrument.mkGeneric()
    instrument.mkTestframe(Units_to_TF, Units_with_TI_to_TF, stub_locals, place_covr, Exclude_local_Stub) 
    instrument.mkStubs(CFILES_not_to_Stub, provided_rd_Headers, Header_with_TI)
    instrument.mkWrap() 
    instrument.mkCommon(Addtional_Stubfunctions)     
    

if __name__ == '__main__':
    main()




