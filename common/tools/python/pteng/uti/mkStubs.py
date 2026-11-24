#!/usr/bin/env python
"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : mkStubs.py
Date of creation : 7.8.2018
Author Name/Dept : Nikolai Ehrhardt
Organization     : 
Description      : Reads information about the Stub-Process from XML and generates 
                   the stubs, the all_stub_rd.h and the generic_stub.c.
"""

import sys
import os
import re
import json

from ..soco import SourceCodeDescriptor, INDENTTATION
from ..globals.folder_utils import split_filepath
from ..globals.rplTmplt import TemplateFileReplace

#USE_NEW_TEMPLATES = False
GENERATE_WRAPPER = True 
    
def removeListFromList(list_in, list_ex):
    for ex in list_ex:
        list_in.remove(ex)
        
    return list_in
    
def sort_func_desc(func_desc):
    return func_desc.function_name
    
class TS_creater():
    def __init__(self, func_list):
        self.funcs_to_rep = func_list
        
    def replacer(self, m):
        for func1 in self.funcs_to_rep:
            if(func1 == m.group(0)):
                #print("match12345")
                return "TESTUB("+func1+")"
        
        return m.group(0)
    
class KW_coverer():
    def __init__(self, var, vars, func):
        self.covr_var = var
        self.func = func
        self.covr_vars = vars
        self.placed_covr_vars = []
        
    def replacer(self, m):
        self.placed_covr_vars.append(self.covr_vars[self.func])
        rest = m.groupdict()["tabs"]+self.covr_var+"["+str(self.covr_vars[self.func])+"]++; "+m.groupdict()["kw"]+" "+m.groupdict()["cl"]
        self.covr_vars[self.func] += 1
        return rest
        

class mkInstrument():
    def __init__(self, soco, tmpl_folder, stub_path, work_with_testframes=False):
        self.source_description = soco 
        self.Replacements = Replacements = {"PROJECT_NAME": self.source_description.project_name}
        self.tmplFolder = tmpl_folder        
        self.stubPath = stub_path
        self.abs_tf_path = os.path.abspath("./"+self.stubPath+"/Testframes")
        self.work_with_testframes = work_with_testframes
        
        self.local_exclude_dict = {}
        
        self.reg_coverer = re.compile(r"(?P<tabs>\s+)(?P<kw>break|return|continue)(?P<cl>\s+|\W+)")
        self.alpha_num = re.compile(r"(\w+)")
        
        self.coverage_vars = dict()
        self.COVR_OfFunctionInfo = {}
        self.giveWarningForCVRLI = False
        
	
    def mkWrap(self):
            
        wrapper_replacements = {
                                    "ExtWrapperDeclaration"     : "",
                                    "ExtWrapperDefinition"      : "",
                                    "ExtWrapperDataDefinition"  : "",
                                    "WrapperByPass"             : ""
                                }
        
        self.Replacements.update(wrapper_replacements)
        
        Replacer = TemplateFileReplace(self.Replacements) 
        
        # prepare unit iteration      
        All_Wrappers = list([])
                      
        # unit iteration
        for unit_description in list(self.source_description.unit_dictionary.values()):                                    
            
            if unit_description.unit_source_name:
                if not unit_description.unit_source_name.upper() in self.source_description.matching_units_upper:
                    continue
            else:
                continue       
            
            #reset values for new iteration
            unit_wrapper_func_ptr_list = ""
            unit_wrapper_func_ptr_list_local = ""
            unit_wrapper_func_ptr_list_driver = ""
            
            #Wrapper strings for replacement
            unit_wrapper_func_def_all = ""
            unit_wrapper_func_decl_all = ""
            unit_wrapper_func_arg_defines = ""
           
            unit_defined_func_list_global   = []
            unit_declared_func_list_global  = []
            unit_defined_func_list_local    = []
            unit_declared_func_list_local   = []
            
            self.Replacements["ModuleName"] = unit_description.unit_name
            self.Replacements["UnitName"] = unit_description.unit_name
            self.Replacements["MODULE_NAME"] = unit_description.unit_name.upper()
            self.Replacements["UNIT_NAME"] = unit_description.unit_name.upper()
            self.Replacements["MODULE_PREFIX"] = unit_description.unit_prefix    
            self.Replacements["UNIT_PREFIX"] = unit_description.unit_prefix  
            
            func_list = []
            func_list.extend(list(unit_description.function_descriptions_global_dict.values()))
            func_list.extend(list(unit_description.function_descriptions_local_dict.values()))
            
                   
            # function iteration:  
            func_list.sort(key=(lambda func_descr: func_descr.function_name)) 
                
            for function_description in func_list:         
                
                if function_description.is_static == True:
                    # extend function list
                    if function_description.define_unit:
                        if not function_description.function_name in All_Wrappers:
                            unit_defined_func_list_local.append(function_description.function_name)
                    unit_declared_func_list_local.append(function_description.function_name)
                else:
                    # extend function list
                    if function_description.define_unit:
                        if not function_description.function_name in All_Wrappers:
                            unit_defined_func_list_global.append(function_description.function_name)
                    unit_declared_func_list_global.append(function_description.function_name)
                
                # generate wrapper symbols for defined functions
                if not function_description.define_unit == None:
                    #print "func_name in mkstub: {name2}".format(name2 = function_description.function_name)
                    if not function_description.function_name in All_Wrappers:
                        All_Wrappers.append(function_description.function_name)
                        (wrapper_decl, wrapper_def, wrapper_args) = function_description.mk_func_ut_wrapper(self.source_description.type_system_ID)
                        unit_wrapper_func_def_all += wrapper_def
                        unit_wrapper_func_decl_all += wrapper_decl
                        unit_wrapper_func_arg_defines += wrapper_args
                    else:
                        print("The function: {func_}, was already wrapped, concerning unit: {unit_}.".format(func_ = str(function_description.function_name), unit_ = str(unit_description.unit_name)))

                else:
                    print("No Wrapper was generated for function: {func_}, because it is not defined.".format(func_ = str(function_description.function_name)))  
                    

            unit_wrapper_func_ptr_list = "const T_TM_Anchor {MODULE_NAME}_atTestFuncPtr[] = ".format(MODULE_NAME=unit_description.unit_name.upper())
            unit_wrapper_func_ptr_list += "\n{\n" 
            if len(unit_defined_func_list_global) > 0:                               
                unit_wrapper_func_ptr_list += INDENTTATION + "(T_TM_Anchor) "+ ("_ut_wrapper,\n" + INDENTTATION + "(T_TM_Anchor) ").join(unit_defined_func_list_global)
                unit_wrapper_func_ptr_list += '_ut_wrapper,\n' 
            unit_wrapper_func_ptr_list += INDENTTATION + "(T_TM_Anchor) " + unit_description.unit_name.upper() + "_use_of_pointer_argument_vars"
            unit_wrapper_func_ptr_list += '};\n'

            unit_wrapper_func_ptr_list_local = "const T_TM_Anchor {MODULE_NAME}_atTestFuncPtr_Local[] = ".format(MODULE_NAME=unit_description.unit_name.upper())
            unit_wrapper_func_ptr_list_local += "\n{\n"  
            if len(unit_defined_func_list_local) > 0:              
                unit_wrapper_func_ptr_list_local += INDENTTATION + "(T_TM_Anchor) "+ ("_ut_wrapper,\n" + INDENTTATION + "(T_TM_Anchor) ").join(unit_defined_func_list_local) + '_ut_wrapper,\n'
            unit_wrapper_func_ptr_list_local += INDENTTATION + "(T_TM_Anchor) " + unit_description.unit_name.upper() + "_use_of_pointer_argument_vars_local"
            unit_wrapper_func_ptr_list_local += "};\n"
                          
                
            self.Replacements["WrapperFunctionPtrLists"] =  unit_wrapper_func_ptr_list + '\n' + unit_wrapper_func_ptr_list_local 
            
            # update Wrapper specific self.Replacements
            self.Replacements["WrapperDeclaration"] = unit_wrapper_func_decl_all
            self.Replacements["WrapperDefinition"]  = unit_wrapper_func_def_all
            self.Replacements["WrapperArgumentDefines"] = unit_wrapper_func_arg_defines

            
            Replacer.addReplacement(self.Replacements)   
                                    
            if not os.path.exists(os.path.join(self.stubPath, "AutoDefault")):
                os.makedirs(os.path.join(self.stubPath, "AutoDefault"))
       
            # Wrapper-File generation        
            Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_Wrapper.c"), os.path.join(self.stubPath, "AutoDefault", unit_description.unit_name + "_ut_Wrapper.c"), True) 
            Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_Wrapper.h"), os.path.join(self.stubPath, "AutoDefault", unit_description.unit_name + "_ut_Wrapper.h"), True)  
            Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_UserWrapper.c"), os.path.join(self.stubPath, "AutoUser", unit_description.unit_name + "_ut_UserWrapper.c"), True, overwrite=False)


    def mkGeneric(self):

        
        Replacer = TemplateFileReplace(self.Replacements) 
        

        general_TP_includes = ""
        general_TP_enumeration = ""   
        general_anchor_declaration = ""
        general_anchor_enumeration = ""


        # unit iteration
        for unit_description in list(self.source_description.unit_dictionary.values()):  
            
      
            
            if unit_description.unit_source_name:
                if not unit_description.unit_source_name.upper() in self.source_description.matching_units_upper:
                    continue
            else:
                continue   
            
            unit_defined_func_list_global   = []
            unit_declared_func_list_global  = []
            unit_defined_func_list_local    = []
            unit_declared_func_list_local   = []
            
            func_list = []
            func_list.extend(list(unit_description.function_descriptions_global_dict.values()))
            func_list.extend(list(unit_description.function_descriptions_local_dict.values()))
            
                   
            # function iteration:  
            func_list.sort(key=(lambda func_descr: func_descr.function_name)) 
                
            for function_description in func_list:
                    

                    
                if function_description.is_static == True:
                    # extend function list
                    if function_description.define_unit:
                        unit_defined_func_list_local.append(function_description.function_name)
                    unit_declared_func_list_local.append(function_description.function_name)
                else:
                    # extend function list
                    if function_description.define_unit:
                         unit_defined_func_list_global.append(function_description.function_name)
                    unit_declared_func_list_global.append(function_description.function_name)
     
            #ToDo NiEH: discuss this if.
            #if (len(unit_declared_func_list_local) > 0) or (len(unit_declared_func_list_global) > 0):     
            # add to general external anchor declaration
                
            general_TP_enumeration += "\n"
            

            general_anchor_declaration += "#if defined(" + unit_description.unit_name.upper() + "_UNDER_TEST)\n" 
            
            #workaround function declared but not defined
            general_anchor_declaration += INDENTTATION + "extern const T_TM_Anchor " + unit_description.unit_name.upper() + "_atTestFuncPtr[];\n"  
            general_anchor_declaration += INDENTTATION + "extern const T_TM_Anchor " + unit_description.unit_name.upper() + "_atTestFuncPtr_Local[];\n"
            
            general_anchor_declaration += "#endif\n" 
            
            # add to general external anchor enumeration
            general_anchor_enumeration += INDENTTATION + "#if defined(" + unit_description.unit_name.upper() + "_UNDER_TEST)\n" 
            
            
            general_anchor_enumeration += 2*INDENTTATION + unit_description.unit_name.upper() + "_atTestFuncPtr,\n"
            general_anchor_enumeration += 2*INDENTTATION + unit_description.unit_name.upper() + "_atTestFuncPtr_Local,\n"
            
            general_anchor_enumeration += INDENTTATION + "#endif\n"
            
            
            general_anchor_declaration += "#if defined(" + unit_description.unit_name.upper() + "_UT_TEST_DRIVER)\n"
            general_anchor_declaration += INDENTTATION + "extern const T_TM_Anchor " + unit_description.unit_name.upper() + "_atUserTestDriverFuncPtr[];\n"
            general_anchor_declaration += "#endif\n" 
            
            general_anchor_enumeration += INDENTTATION + "#if defined(" + unit_description.unit_name.upper() + "_UT_TEST_DRIVER)\n"  
            general_anchor_enumeration += 2*INDENTTATION + unit_description.unit_name.upper() + "_atUserTestDriverFuncPtr,\n"
            general_anchor_enumeration += INDENTTATION + "#endif\n" 
            
            Replacer.addReplacement(self.Replacements)                                  
            
        
        #Add the anchor for TestSupport
        if GENERATE_WRAPPER:
            general_anchor_declaration += "#if defined(UNIT_TEST)\n"
            general_anchor_declaration += INDENTTATION + "extern const T_TM_Anchor TM_TEST_SupportFuncPtr[];\n"
            general_anchor_declaration += "#endif\n" 
        
            general_anchor_enumeration += INDENTTATION + "#if defined(UNIT_TEST)\n"  
            general_anchor_enumeration += 2*INDENTTATION + "TM_TEST_SupportFuncPtr,\n"
            general_anchor_enumeration += INDENTTATION + "#endif\n"

        
        general_anchor_declaration += INDENTTATION + "extern const T_TM_Anchor COMMONSTUB_atTestFuncPtr[];\n" 
        
        general_anchor_enumeration += 2*INDENTTATION + "COMMONSTUB" + "_atTestFuncPtr,\n"

        # generate general files
        self.Replacements["AllExtAnchorDeclarations"] = general_anchor_declaration
        self.Replacements["GlobalAnchor"] = general_anchor_enumeration 

           
        Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_GenericStub.c"), os.path.join(self.stubPath, "GenericStub.c"), True) 

    def mkCommon(self, add_stubfuncs):
        
        add_stubfuncs.sort()
        all_undefined_functions = {}
        add_undefined_functions = {}
        all_extern_variables = []
        
        all_headers = []
        
        if not os.path.exists(os.path.join(self.stubPath, "Common")):
            os.makedirs(os.path.join(self.stubPath, "Common"))
            
        #reset values for new iteration
        unit_defined_func_list_global = []
        unit_declared_func_list_global = []
        unit_func_ptr_list = ""
        unit_use_of_ptr_arg_function = ''
        unit_stub_TPs_global = ""
        unit_stub_bypass = ""
        unit_stub_vars_global = ""
        unit_stub_func_decl_global = ""
        unit_stub_func_defs_global = ""

        unit_pointer_argument_names = []    
            
        # unit iteration
        for unit_description in list(self.source_description.unit_dictionary.values()):  
            
            for function in add_stubfuncs:
                if not ":" in function:
                    if function in unit_description.function_descriptions_global_dict:
                        add_undefined_functions[function] = unit_description   
             
            if unit_description.unit_source_name:
                if not unit_description.unit_source_name.upper() in self.source_description.matching_units_upper:       
                    continue
            else:
                continue  
            
            #collect all external variables, and included headers
            for ex_var in (unit_description.external_variables):
                if not ex_var in all_extern_variables:
                    all_extern_variables.append(ex_var)
                    
            for header in unit_description.included_headers:
                if not header in all_headers:
                    all_headers.append(header)
                pure_h = split_filepath(header)[1]
                if pure_h in self.source_description.unit_dictionary:
                    inc_unit = self.source_description.unit_dictionary[pure_h]
                    #print(inc_unit.external_variables)
                    for ex_var in (inc_unit.external_variables):
                        if not ex_var in all_extern_variables:
                            all_extern_variables.append(ex_var)
            
            func_list = []
            func_list.extend(list(unit_description.function_descriptions_global_dict.values()))
            func_list.extend(list(unit_description.function_descriptions_local_dict.values()))
            
            for function_description in func_list: 
                all_undefined_functions.update(function_description.callee_dict)
         
        
        all_ext_undefined_functions = []
        
        #add additional stubfunctions
        all_undefined_functions.update(add_undefined_functions)

        #create stubs for al undefined functions    
        for function in sorted(all_undefined_functions):
            
            function_descriptor = None
            
            unit_descriptor = all_undefined_functions[function]
            
            #skip if called function is declared/defined in a unit_under_test
            if(unit_descriptor.unit_name.upper() in self.source_description.matching_units_upper):
                for h in unit_descriptor.included_headers:
                    if not h in all_headers:
                        all_headers.append(h)
                continue
            
            if function in unit_descriptor.function_descriptions_global_dict:
                function_descriptor = unit_descriptor.function_descriptions_global_dict[function]
                    
            
            if function_descriptor == None:
                print("While creation of CommonStubs no function_descriptor found for: {func_}".format(func_ = function))
            else: 
                
                all_ext_undefined_functions.append(function)
                
                for h in unit_descriptor.included_headers:
                    if not h in all_headers:
                        all_headers.append(h)

                           
                (stub_func_decl, stub_func_def, stub_var_decls, pointer_argument_names) = function_descriptor.get_stub_stuff()

                unit_stub_bypass += "\t\t#define {func_name} {func_name}_Stub\n".format(func_name=function) 

                unit_defined_func_list_global.append(function_description.function_name)
                unit_declared_func_list_global.append(function_description.function_name)

                unit_stub_vars_global += "".join(stub_var_decls)
                if len(stub_var_decls) > 0:
                    unit_stub_vars_global += "\n"
                unit_stub_func_decl_global += stub_func_decl + "\n"
                unit_stub_func_defs_global += stub_func_def + "\n"
                unit_pointer_argument_names.extend(pointer_argument_names)
        
        
        all_extern_variables_filtered = list()  
        
        for ex_var in all_extern_variables:
            ex_var_is_declared = False
            for unit_description in list(self.source_description.unit_dictionary.values()):
                if (unit_description.unit_name.upper() in self.source_description.matching_units_upper):
                    if ex_var in unit_description.variable_descriptions_dict:
                        ex_var_is_declared = True
                        break
            if not ex_var_is_declared:
                all_extern_variables_filtered.append(ex_var)
           
        for var in sorted(all_extern_variables_filtered):
            stubbed_ex_var = False
            for unit_description in list(self.source_description.unit_dictionary.values()):
                if var in unit_description.variable_descriptions_dict and not stubbed_ex_var:
                    
                    for h in unit_description.included_headers:
                        if not h in all_headers:
                            all_headers.append(h)
                    
                    var_string = unit_description.variable_descriptions_dict[var].definition_description.get_source_code_type(var)+";\n"
                    if "[]" in var_string:
                        unit_stub_vars_global += var_string.replace("[]", "[STUBVAR_ARRAY_SIZE]").replace(";", "={};")
                    else:
                        unit_stub_vars_global += var_string
                    stubbed_ex_var = True
                    break
                elif var in unit_description.ext_variable_descriptions_dict and not stubbed_ex_var: 
                    
                    for h in unit_description.included_headers:
                        if not h in all_headers:
                            all_headers.append(h)
                    
                    var_string = unit_description.ext_variable_descriptions_dict[var].definition_description.get_source_code_type(var)+";\n"
                    if "[]" in var_string:
                        unit_stub_vars_global += var_string.replace("[]", "[STUBVAR_ARRAY_SIZE]").replace(";", "={};")
                    else:
                        unit_stub_vars_global += var_string
                    stubbed_ex_var = True
                    break
            if not stubbed_ex_var:
                print("Variable {var_} is declared extern in a unit under test and no stub-variable has been added to CommonStub.".format(var_ = var))
            
                
        # generate function pointer list  
        unit_func_ptr_list = "const T_TM_Anchor COMMONSTUB_atTestFuncPtr[] = "
        unit_func_ptr_list += "\n{\n"                                                  
        unit_func_ptr_list += INDENTTATION + "(T_TM_Anchor) " + "COMMONSTUB" + "_use_of_pointer_argument_vars"
        unit_func_ptr_list += "\n};\n"
        
        
        unit_use_of_ptr_arg_function = 'static int ' + "COMMONSTUB" + '_use_of_pointer_argument_vars (void)\n{\nint help = 0;\n'      
        if unit_pointer_argument_names:
            unit_use_of_ptr_arg_function += 'help += (int) &' + ';\n\thelp += (int) &'.join(unit_pointer_argument_names) + ';\n'        
        unit_use_of_ptr_arg_function += '\nreturn help;\n}\n' 
        
        for func in add_stubfuncs:
            if ":" in func:
                (stub_func_decl, stub_func_def, stub_var_decls, unit_rd) = SourceCodeDescriptor.FunctionDescriptor.get_stub_stuff_of_functiondescriptorstring(func)

                unit_stub_vars_global += "".join(stub_var_decls)
                if len(stub_var_decls) > 0:
                    unit_stub_vars_global += "\n"
                unit_stub_func_decl_global += stub_func_decl + "\n"
                unit_stub_func_defs_global += stub_func_def + "\n"
                unit_stub_bypass += unit_rd
            
        
        # assign self.Replacements
        self.Replacements["StubByPass"] = unit_stub_bypass 
        self.Replacements["ExtStubDeclaration"] = unit_stub_func_decl_global    
        self.Replacements["FunctionPtrList"] = unit_func_ptr_list
        self.Replacements["UseOfPtrArgumentFunction"] = unit_use_of_ptr_arg_function    
        self.Replacements["ExtStubDataDefinition"] = unit_stub_vars_global
        self.Replacements["ExtStubDefinition"] = unit_stub_func_defs_global
        self.Replacements["StubRedirection"]  = unit_stub_bypass
                 
        
        all_included_headers = ""
        all_headers.sort()
        for h in all_headers:
            if not h.lower() in ["bsh_stdinc.h", "testsupport.h", "processor.h"]:
                all_included_headers += "#include \""+h+"\"\n"
                
        
        self.Replacements["UnitName_Stub_header_include"] = all_included_headers
        
        Replacer = TemplateFileReplace(self.Replacements) 
        
        Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_Common_Stub.c"), os.path.join(self.stubPath, "Common", "CommonStub.c"), True, overwrite=False)
        Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_Common_rd_Stub.h"), os.path.join(self.stubPath, "Common", "CommonStub_rd.h"), True, overwrite=False)
        Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_Common_dcl_Stub.h"), os.path.join(self.stubPath,  "Common", "CommonStub_dcl.h"), True, overwrite=False)
    
            

    def mkStubs(self, cFiles_with_provided_stubs, provided_RD_Headers, Header_with_TI=[]):
        
        """ - generate the according stub and wrapper files  
            - generate a stub function for every declared and/or defined function and redirect function call in the according ..._Stub.h file
            - generate a wrapper function for every defined function
            - place every defined function in the unit specific anchor array  """           
        
        Replacer = TemplateFileReplace(self.Replacements) 
        All_TPs = list([])
        
        all_rd_inclusions = ""
        
        if not os.path.exists(os.path.join(self.stubPath, "AutoUser")):
            os.makedirs(os.path.join(self.stubPath, "AutoUser"))
        if not os.path.exists(os.path.join(self.stubPath, "AutoDefault")):
            os.makedirs(os.path.join(self.stubPath, "AutoDefault"))

        
        for header in Header_with_TI:
                 
            ResultFile = open(os.path.join(self.stubPath, "AutoDefault", header +"_Stub.h"),"w")
            ResultFile.close()               

        # unit iteration
        for unit_description in list(self.source_description.unit_dictionary.values()):  
            
            #print('processing ', unit_description.unit_name)    
            if unit_description.unit_source_name:
                if not unit_description.unit_source_name.upper() in self.source_description.matching_units_upper:
                    #print("No matching for: ", unit_description.unit_name)
                    continue
            else:
                #print("No source for: ", unit_description.unit_name)
                if unit_description.unit_name.upper() in self.source_description.matching_units_upper:
                    print("Generated no Stub-Files for {unit_}, because the unit source name is not available.".format(unit_ = unit_description.unit_name))
                continue  
            
            #reset values for new iteration
            ModulePrefix = ''
            unit_defined_func_list_global = []
            unit_declared_func_list_global = []
            unit_defined_func_list_local = []
            unit_declared_func_list_local = []
            unit_func_ptr_list = ""
            unit_func_ptr_list_local = ""
            unit_use_of_ptr_arg_function = ''
            unit_use_of_local_ptr_arg_function = ''
            unit_stub_bypass = ""
            unit_stub_vars_global = ""
            unit_stab_vars_local = ""
            unit_stub_func_decl_global = ""
            unit_stub_func_decl_local = ""
            unit_stub_func_defs_global = ""
            unit_stub_func_defs_local = ""
            unit_pointer_argument_names = []
            unit_pointer_argument_names_local = []
            
            unit_stub_local_typedef = ""
            
            for typedef in list(unit_description.typedef_container.get_all_typedefinitions_dict().values()):
                if typedef.is_local:
                    #print(typedef.get_forward_declaration())
                    unit_stub_local_typedef += typedef.get_forward_declaration() +"\n"
                    
                          
            self.Replacements["ModuleName"] = unit_description.unit_name
            self.Replacements["UnitName"] = unit_description.unit_name
            self.Replacements["MODULE_NAME"] = unit_description.unit_name.upper()
            self.Replacements["UNIT_NAME"] = unit_description.unit_name.upper()
            self.Replacements["MODULE_PREFIX"] = unit_description.unit_prefix    
            self.Replacements["UNIT_PREFIX"] = unit_description.unit_prefix  
            

            func_list = []
            func_list.extend(list(unit_description.function_descriptions_global_dict.values()))
            func_list.extend(list(unit_description.function_descriptions_local_dict.values()))
            
                   
            # function iteration:  
            func_list.sort(key=(lambda func_descr: func_descr.function_name)) 
                
            for function_description in func_list:
                
                # for an XS file: stub only functions that are defined in this unit and declared in XS.h (or McXS.h)
                #nur ohne tf
                if not self.work_with_testframes:
                    pass
                else:
                    if not id(function_description.define_unit) == id(unit_description):
                        continue
                    
                    
                # generate stub function declaration and definition and stub variable declaration            
                (stub_func_decl, stub_func_def, stub_var_decls, pointer_argument_names) = function_description.get_stub_stuff()
                  
                # todo functionality UlBr 2016-08-23: handle correctly if declared in A and defined in B           
                           
                # extend according unit variables with function declarations and definitions and stub variable declarations   
                if function_description.is_static == True:
                    # extend function list
                    if function_description.define_unit:
                        unit_defined_func_list_local.append(function_description.function_name)
                    unit_declared_func_list_local.append(function_description.function_name)
                    
                    if (function_description.define_unit and self.work_with_testframes) or (not self.work_with_testframes):
                        unit_stab_vars_local += "".join(stub_var_decls)
                        if len(stub_var_decls) > 0:
                            unit_stab_vars_local += "\n"
                        unit_stub_func_decl_local += stub_func_decl + "\n"
                        unit_stub_func_defs_local += stub_func_def + "\n"
                        unit_pointer_argument_names_local.extend(pointer_argument_names)

                else:
                    unit_stub_bypass += "\t\t#define {func_name} {func_name}_Stub\n".format(func_name=function_description.function_name) 
                    # extend function list
                    if function_description.define_unit:
                         unit_defined_func_list_global.append(function_description.function_name)
                    unit_declared_func_list_global.append(function_description.function_name)
                    
                    if (function_description.define_unit and self.work_with_testframes) or (not self.work_with_testframes):
                        unit_stub_vars_global += "".join(stub_var_decls)
                        if len(stub_var_decls) > 0:
                            unit_stub_vars_global += "\n"
                        unit_stub_func_decl_global += stub_func_decl + "\n"
                        unit_stub_func_defs_global += stub_func_def + "\n"
                        unit_pointer_argument_names.extend(pointer_argument_names)
              
            
       
                # generate function pointer list  
                unit_func_ptr_list = "const T_TM_Anchor {MODULE_NAME}_atTestFuncPtr[] = ".format(MODULE_NAME=unit_description.unit_name.upper())
                unit_func_ptr_list += "\n{\n" 
                if len(unit_defined_func_list_global) > 0:                                
                    unit_func_ptr_list += INDENTTATION + "(T_TM_Anchor) "+ (",\n" + INDENTTATION + "(T_TM_Anchor) ").join(unit_defined_func_list_global)
                    unit_func_ptr_list += ',' + INDENTTATION + "\n"                               
                unit_func_ptr_list += INDENTTATION + "(T_TM_Anchor) " + unit_description.unit_name.upper() + "_use_of_pointer_argument_vars"
                unit_func_ptr_list += "\n};\n"
                       
                unit_func_ptr_list_local = "const T_TM_Anchor {MODULE_NAME}_atTestFuncPtr_Local[] = ".format(MODULE_NAME=unit_description.unit_name.upper())
                unit_func_ptr_list_local += "\n{\n"
                if len(unit_defined_func_list_local) > 0:
                    unit_func_ptr_list_local += INDENTTATION + "(T_TM_Anchor) "+ (",\n" + INDENTTATION + "(T_TM_Anchor) ").join(unit_defined_func_list_local) + ',\n'             
                unit_func_ptr_list_local += INDENTTATION + "(T_TM_Anchor) " + unit_description.unit_name.upper() + "_use_of_pointer_argument_vars_local\n"
                unit_func_ptr_list_local += "};\n"  

            
            unit_use_of_ptr_arg_function = 'static int ' + unit_description.unit_name.upper() + '_use_of_pointer_argument_vars (void)\n{\nint help = 0;\n'      
            if unit_pointer_argument_names:
                unit_use_of_ptr_arg_function += 'help += (int) &' + ';\n\thelp += (int) &'.join(unit_pointer_argument_names) + ';\n'        
            unit_use_of_ptr_arg_function += '\nreturn help;\n}\n' 
            
            unit_use_of_local_ptr_arg_function = 'static int ' + unit_description.unit_name.upper() + '_use_of_pointer_argument_vars_local (void)\n{\nint help = 0;\n'      
            if unit_pointer_argument_names_local:
                unit_use_of_local_ptr_arg_function += 'help += (int) &' + ';\n\thelp += (int) &'.join(unit_pointer_argument_names_local) + ';\n'        
            unit_use_of_local_ptr_arg_function += '\nreturn help;\n}\n'           
            
                 
            # assign self.Replacements
            self.Replacements["StubByPass"] = unit_stub_bypass 
            self.Replacements["ExtStubDeclaration"] = unit_stub_func_decl_global    
                
            self.Replacements["FunctionPtrList"] = unit_func_ptr_list
            self.Replacements["FunctionPtrListLocal"] = unit_func_ptr_list_local
            self.Replacements["UseOfPtrArgumentFunction"] = unit_use_of_ptr_arg_function  
            self.Replacements["UseOfPtrArgumentFunctionLocal"] = unit_use_of_local_ptr_arg_function        
            self.Replacements["ExtStubDataDefinition"] = unit_stub_vars_global
            self.Replacements["ExtStubDefinition"] = unit_stub_func_defs_global
            self.Replacements["IntStubDeclaration"] = unit_stub_func_decl_local
            self.Replacements["LocalTypedefs"] = unit_stub_local_typedef
            self.Replacements["IntStubDataDefinition"] =  unit_stab_vars_local 
            self.Replacements["IntStubDefinition"] = unit_stub_func_defs_local
            
            self.Replacements["StubRedirection"]     = unit_stub_bypass
            

            self.Replacements["UnitName_Stub_header_include"] = "#include \"" + unit_description.unit_name + "_Stub_dcl.h\""
            
            #unit_description.readout()    
                
            #all_included_headers nutzen    
            if (unit_description.unit_header_name + ".h" in unit_description.included_headers):
                self.Replacements["UnitName_header_include"] = "#include \"" + unit_description.unit_header_name + ".h\""
            else:
                self.Replacements["UnitName_header_include"] = "" 
                
            cov_dcls = ""
            cov_vars = ""
            unit_covr_vars = self.coverage_vars[unit_description.unit_name]
            for var in sorted(unit_covr_vars):
                if unit_covr_vars[var] > 0:
                    cvar = "uint8_t " + var+"_Line"+"["+str(unit_covr_vars[var])+"]"
                    cov_vars += cvar+" = {0};\n"
                    cov_dcls += "extern "+cvar+";\n"
                    
            self.Replacements["COVERAGE_VARS_DCLS"] = cov_dcls
            self.Replacements["COVERAGE_VARS"] = cov_vars  
            
            Replacer.addReplacement(self.Replacements)   
                                                
            # generate unit files
            if not self.work_with_testframes:
                Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_Stub.h"), os.path.join(self.stubPath, "AutoDefault", unit_description.unit_name +"_Stub.h"), True)
                        
               
            Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_Stub.c"), os.path.join(self.stubPath, "AutoDefault", unit_description.unit_name + "_Stub.c"), True)
            
            Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_IntStub.h"), os.path.join(self.stubPath, "AutoDefault", unit_description.unit_name +"_IntStub.h"), True)

            Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_IntStub.c"), os.path.join(self.stubPath, "AutoDefault", unit_description.unit_name +"_IntStub.c"), True)  

            Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_UserStub.c"), os.path.join(self.stubPath, "AutoUser", unit_description.unit_name + "_UserStub.c"), True, overwrite=False) 
                              
                 
            Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_UserIntStub.c"), os.path.join(self.stubPath, "AutoUser", unit_description.unit_name + "_UserIntStub.c"), True, overwrite=False)                
            
            
            if self.work_with_testframes:
                
                all_rd_inclusions += "#include \"./AutoDefault/"+ unit_description.unit_name + "_Stub_rd.h\"\n"
                
                Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_rd_Stub.h"), os.path.join(self.stubPath, "AutoDefault", unit_description.unit_name + "_Stub_rd.h"), True)
                Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_dcl_Stub.h"), os.path.join(self.stubPath,  "AutoDefault", unit_description.unit_name + "_Stub_dcl.h"), True)
        
        if self.work_with_testframes:
                
            for header in provided_RD_Headers:
                all_rd_inclusions += "#include \"./Provided/"+ header + "_Stub_rd.h\"\n"
                
            for cfile in cFiles_with_provided_stubs:
                all_rd_inclusions += "#include \"./Provided/"+ cfile + "_Stub_rd.h" + "\"\n"
            
            all_rd_inclusions += "#include \"./Common/CommonStub_rd.h\"\n" 
                
            self.Replacements["IncludeStubRDFiles"] =  all_rd_inclusions  
            
             
            Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_All_Stub_rd.h"), os.path.join(self.stubPath,  "All_Stub_rd.h"), True)         
    

    def counterInCode(self, no, code_positions):
        for func in code_positions:
            if(no >= code_positions[func][0] and  no < code_positions[func][1]):
                return func
        return ""
        
    def stubLocals(self, l, funcs):
        
        TS_repper = TS_creater(funcs)       
        
        lrep = self.alpha_num.sub(TS_repper.replacer, l)
        
        return lrep
        
    def formLocalExclude(self, Exclude_local_Stub):
        local_exclude_dict = {}
        for ex in Exclude_local_Stub:
            func, excludes = ex.split(":", 1)
            if not func in local_exclude_dict:
                local_exclude_dict[func] = excludes.split("-")
            else:
                local_exclude_dict[func].extend(excludes)
                
        return local_exclude_dict
        
    def placeCOVR(self, ls, func, cvars):
            
        c_keywords= {
            "for": 2,
            "continue": 1,
            "break": 1,
            "return":1
            }
        
        cvar = func+"_Line"
        
        l = ls[0]
        ds_comment = "//" in l
        string_to_reattach = ["", ""]
        c_start = "/*" in l
        c_end =  "*/" in l

        in_comment = ls[2]
        if ds_comment and not in_comment:
            cc_split = l.split("//", 1)
            l = cc_split[0] 
            string_to_reattach[1] = "//"+cc_split[1]             
        else:
            # code /* comment */ code # code /* comment # comment */ code 
            start_c_split = l.split("/*", 1)
            end_c_split = l.split("*/", 1)
            if in_comment:
                if c_end:
                    l = end_c_split[1]
                    in_comment = False
                    string_to_reattach[0] = end_c_split[0]+"*/"
                else:
                    l = ""
                    string_to_reattach[0] = end_c_split[0]
            
            else:
                if c_start and not c_end:
                    l = start_c_split[0]
                    in_comment = True
                    string_to_reattach[1] = "/*"+start_c_split[1]
                    
                elif c_start and c_end:
                    l = start_c_split[0]+"####"+end_c_split[1]
                    string_to_reattach[0] = "/*"+start_c_split[1].split("*/", 1)[0]+"*/"
                    in_comment = False
                else: #not starting a comment, and not in a comment, means there can not end any comment
                    pass
                    
        
        #if not l.endswith("\r\n"):
            #l += "\r\n"

        
        missing = ls[1]       
        
        i = 0
        last_i = 0
        
        found_index = []
        placed_covr_vars = []
        
        
        while(i > -1):
            i = l.find(";", i)
            if(i == last_i):
                break
            if(i > -1):
                if missing > 0:
                    missing -= 1
                else:
                    found_index.append(i)
                        
            last_i = i
         
        ret_ind = ["", 0, in_comment]#nochmal gucken
        
        if(len(found_index) == 0):
            ret_ind[0] = l
            ret_ind[1] = 0
            for kw in c_keywords:
                if(kw in l):
                    ret_ind[1] = c_keywords[kw]
                    break
        
        else:  
            last_k = 0    
            needed_index = []
            #for_found = False
                 
            for k in found_index:
                
                part = l[last_k:k]
                last_k = k
                
                for kw in c_keywords:
                    if(kw in part):
                        missing = c_keywords[kw]
                        break
                
                if(missing > 0):
                    missing -= 1
                    continue
                else:
                    needed_index.append(k)
                
                       
            l_covered = l
            counter = 0
                
            for k in needed_index:
                placed_covr_vars.append(cvars[func])
                toReplace = l_covered[k:k+1].replace(";", "; "+cvar+"["+str(cvars[func])+"]++;")
                cvars[func] += 1
                l_covered = l_covered[0:k]+toReplace+l_covered[k+1:]
            
            
                
            ret_ind[0] = l_covered
            ret_ind[1] = missing
            
        l_covered = ret_ind[0]  
        KW_coverer_instance = KW_coverer(cvar, cvars, func)       
        l_covered = self.reg_coverer.sub(KW_coverer_instance.replacer , l_covered)
        if len(KW_coverer_instance.placed_covr_vars):
            placed_covr_vars.extend(KW_coverer_instance.placed_covr_vars)
        ret_ind[0] = l_covered
        
        if c_start and c_end and not in_comment:
            ret_ind[0] = ret_ind[0].replace("####", string_to_reattach[0])
        else:
            ret_ind[0] = string_to_reattach[0] + ret_ind[0] + string_to_reattach[1]
            
        return ret_ind, placed_covr_vars
    
    def copyUnit(self, unitdesc, func_list, stub_locals, place_covr):       
        
        abs_unit_path = os.path.abspath("./"+unitdesc.source_filepath)
        
        tu = open(abs_unit_path, "r")
        uf = None
        
        if not os.path.isfile(self.abs_tf_path+"/"+unitdesc.unit_source_name+"_framed.c"):
            uf = open(self.abs_tf_path+"/"+unitdesc.unit_source_name+"_framed.c", "w")
        else:
            self.giveWarningForCVRLI = True
            print("Frame for {unit_} exists, it is not generated.".format(unit_ = self.abs_tf_path+"/"+unitdesc.unit_source_name+"_framed.c"))
            
            
        loc_funcs = list(unitdesc.function_descriptions_local_dict.keys())
        loc_funcs.extend(unitdesc.function_descriptions_global_dict.keys())
        covr_vars = {}
        code_positions = {}
        
        loc_funcs.sort(key=len, reverse=True)
        
        if "global" in self.local_exclude_dict:
            loc_funcs = removeListFromList(loc_funcs, self.local_exclude_dict["global"])
            
        for fdesc in func_list:
            if(id(fdesc.define_unit) == id(unitdesc)):
                code_positions[fdesc.function_name] = fdesc.code_position
                covr_vars[fdesc.function_name] = 0
                self.COVR_OfFunctionInfo[fdesc.function_name] =[["{"], [[]]]
                
        counter = 0
        ret_l_m = ["", 0, False]
        
        for line in tu:
            candidate = self.counterInCode(counter, code_positions)
            if(len(candidate)):
                loc_funcs_ex = list(loc_funcs)
                if candidate in self.local_exclude_dict:
                    loc_funcs_ex = removeListFromList(loc_funcs_ex, self.local_exclude_dict[candidate])
                ret_l_m[0] = line
                if (stub_locals):
                    ret_l_m[0]  = self.stubLocals(ret_l_m[0], loc_funcs_ex)
                if (place_covr):
                    ret_l_m , placed_covr_vars = self.placeCOVR(ret_l_m, candidate, covr_vars)
                    
                    current_line_list = self.COVR_OfFunctionInfo[candidate]
                    current_line_list[0].append(bytes(line)) 
                    current_line_list[1].append(placed_covr_vars)
                
                #covr_vars[fdesc.function_name] += l_t[1]
                try:
                    uf.write(ret_l_m[0])
                except:
                    pass
            else:
                try:
                    uf.write(line)
                except:
                    pass
                #print(str(counter)+":"+line)
            counter+=1
        try:
            uf.close()
        except:
            pass
            
        tu.close()
        
        return covr_vars

               
    def mkTestframe(self, List_TFUnits, List_Units_with_TI, stub_locals, place_covr, Exclude_local_Stub):
        
        self.local_exclude_dict = self.formLocalExclude(Exclude_local_Stub)
        
        List_TFUnits_upper = [unit.upper() for unit in List_TFUnits]
             
        List_Units_with_TI_upper = [unit.upper() for unit in List_Units_with_TI]
            
        Replacer = TemplateFileReplace(self.Replacements) 
        
        all_by_testframed_included_files_with_TI = list()
        
        all_in_source_included_header = []
        
        #collect all headers
        for unit_description in self.source_description.unit_dictionary.values(): 
            
            if unit_description.unit_name.upper() in List_TFUnits_upper:
                for h in unit_description.included_headers:
                    if not h in all_in_source_included_header:
                        all_in_source_included_header.append(h)
        
        #collect dcl for all units under test 
        dcl_list_asString = str()
        for matching_unit in self.source_description.matching_units:          
            dcl_list_asString += "#include \"./../AutoDefault/" + matching_unit + "_Stub_dcl.h\"\n"
                
        includes = ""
        for h in all_in_source_included_header:
            if not h.lower() in ["BSH_stdinc.h", "bsh_stdinc.h", "TestSupport.h", "testsupport.h", "inttbl.h"]:
                includes += "#include \""+h+"\"\n"        
        
        for unit_description in self.source_description.unit_dictionary.values(): 
            
            if unit_description.unit_name.upper() in List_TFUnits_upper:
                
                self.Replacements["UNIT_NAME"]           = unit_description.unit_name.upper()
                self.Replacements["UnitName"]            = unit_description.unit_name
                self.Replacements["UNIT_PREFIX"]         = unit_description.unit_prefix 
                
                all_callees = dict()
                func_list = []
                func_list.extend(list(unit_description.function_descriptions_global_dict.values()))
                func_list.extend(list(unit_description.function_descriptions_local_dict.values()))
                for func in func_list:
                    all_callees.update(func.callee_dict)
                
                #places also TESTUB for locals and gcov vars
                cov_vars = self.copyUnit(unit_description, func_list, stub_locals, place_covr)
                #necessary to write common_stub-file
                self.coverage_vars[unit_description.unit_name] = cov_vars
                
                unit_includes = includes    
                        
                for tf_dcl in list(self.source_description.unit_dictionary.keys()):
                    callee_found = False
                    if not (unit_description.unit_name.upper() == tf_dcl.upper()):
                        if(tf_dcl.upper() in List_TFUnits or tf_dcl in List_TFUnits ):
                            for callee in all_callees:
                                if id(self.source_description.unit_dictionary[tf_dcl]) == id(all_callees[callee]):
                                    callee_found = True
                    if callee_found:
                        unit_includes += "#include \"./../AutoDefault/"+tf_dcl+"_Stub_dcl.h\"\n"
                        
                self.Replacements["INCLUDES"]            = unit_includes
                
                #Unit dcl include
                self.Replacements["UnitName_Stub_header_include"] = dcl_list_asString
                
                for inc_unit in unit_description.cfile_includes_cfile_of_unit:
                    if inc_unit in List_Units_with_TI:
                        if inc_unit not in all_by_testframed_included_files_with_TI:
                            all_by_testframed_included_files_with_TI.append(inc_unit)
                
                
                Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_testframe.c"), os.path.join(self.stubPath, "Testframes", unit_description.unit_name + "_testframe.c"), True)
                Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_ut_config_Stub.h"), os.path.join(self.stubPath,  "TestConfig/AutoDefault", unit_description.unit_name + "_ut_config.h"), True, overwrite=False)
            
        for unit_description in self.source_description.unit_dictionary.values(): 
            
            if unit_description.unit_name.upper() in List_Units_with_TI_upper:
                
                self.Replacements["UNIT_NAME"]           = unit_description.unit_name.upper()
                self.Replacements["UnitName"]            = unit_description.unit_name
                self.Replacements["UNIT_PREFIX"]         = unit_description.unit_prefix  
                
                Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_testframe_for_TI.c"), os.path.join(self.stubPath, "Testframes", unit_description.unit_name + "_testframe.c"), True)
                Replacer.replaceTemplate(os.path.join(self.tmplFolder, "Template_ut_config_Stub.h"), os.path.join(self.stubPath,  "TestConfig/AutoDefault", unit_description.unit_name + "_ut_config.h"), True)

        #print(self.COVR_OfFunctionInfo)
        COVR_perLineInfo = self.stubPath+"/"+"COVR_perLineInfo.json"
        with open(COVR_perLineInfo, "w+") as f:
            #ensure ascii = False, allows also non-ascii characters to put to json, 
            #here no problem because lines are decoded to bytes.
            data = json.dumps(self.COVR_OfFunctionInfo, ensure_ascii=False)
            
            f.write(data)
         

