"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : file_loader.py
Date of creation : 4.03.2020
Author Name/Dept : Nikolai Ehrhardt
Organization     : 
Description      : Class to load any editor-related files.
"""

import re
import yaml
import xml.dom.minidom as dom

from .constants import tree_const, exec_const

from .folder_utils import PathFormer

comparator_dict = {
    "equal_to":                    "==",
    "not_equal_to":                "!=",
    "less_than":                   "<",
    "greater_than":                ">",
    "less_than_or_equal_to":       "<=",
    "greater_than_or_equal_to":    ">=",
    "equal_to_range":              "=..",
    "not_equal_to_range":          "!=..",
}

    
def getSafeChildNodeText(node):
    if len(node.childNodes):
        return node.childNodes[0].data
    
    return ""
    
def evaluateList(valid_list):
    passed_list = list()
    
    if len(valid_list) == 0:
        return exec_const.passed_int
    
    for el in valid_list:
        if el.validation == None:
            el.validation = exec_const.exception_int
        
        passed_list.append(el.validation)
        
    return max(passed_list)
    
def makeValueMaxLen(max_len, value):

    if max_len == 1:
        return value

    if type(value) == list:
        if not len(value) == max_len:
            print("MaxLen Error: "+str(value))
            return None
        else:
            return value
            
    else:
        len_list = []
        for i in range(0, max_len):
            len_list.append(value)
        return len_list
        
def checkIOStringForList(io_string):
    if type(io_string) == str:
        if io_string.startswith("[") and io_string.endswith("]"):
            io_string = io_string[1:-1]
            if "-" in io_string and not "," in io_string:
                io_split = io_string.split("-")
                return list(range(int(io_split[0]),int(io_split[1])+1))
            else:
                new_list = []
                for g in io_string.split(","):
                    new_list.append(checkIOStringForList(g.strip()))
                return new_list
                
        else:
            return io_string
    else:
        return io_string
        
def check_makeStringFromList(io_list):
    if io_list == None:
        return io_list
    if type(io_list) in [tuple, list]:
        ret = "["
        for value in io_list:
            if type(value) in [tuple, list]:
                ret += makeStringFromList(io_list)
            else:    
                ret += str(value)
            ret += ", "
        ret = ret[:-2]+"]"
        return ret
    else:
        return str(io_list)

def id_sorter(io):
    return io.identifier
            

class VarID():

    #hex(x)oct(x)bin(x)
    rep_dict = {
        "hex": hex,
        "oct": oct,
        "bin": bin,
        tree_const.combo_reps[0]: int,
        "": int,
        "None": int
    }

    float_regexp = re.compile(r"\d+\.\d+")

    def __init__(self):
        self.identifier = None
        self.rep = tree_const.combo_reps[0]
        self.range_test_len = 1
        self.range_test_index = 0
        
    def getRangeTestLen(self):
        return self.range_test_len
        
    def setTCIndex(self, index):
        self.range_test_index = index
        
    def get_value_indexed(self, variable):
        if self.range_test_len == 1:
            return variable
        else:
            return variable[self.range_test_index]
        
    def set_value_indexed(self, variable, value):
        if self.range_test_len == 1:
            variable = value 
        else:
            variable[self.range_test_index] = value         
            
        
    def get_comparison_list(self, comparison_group_member):               # comparison_group_member is a xml-node
        sign=comparator_dict.get(comparison_group_member.localName, "==")
        if sign in tree_const.range_ops:
            value=[None,None]
            for range_kind in comparison_group_member.childNodes:
                if range_kind.localName == "from":
                    small = self.transform_uni(range_kind.childNodes[0].nodeValue)
                elif range_kind.localName == "to":
                    high = self.transform_uni(range_kind.childNodes[0].nodeValue)
                    
            if small<high:
                value=[small, high]
            else:
                value=[high, small]
                
        else:
            value=self.transform_uni(comparison_group_member.childNodes[0].nodeValue)   #removed int cast, to save strings also
        return [sign,value]         
    
    def transform_uni(self, prm_unicode):       #ToDo NiEh Not a complete exception handling.
        #print(prm_unicode)
        unicode = checkIOStringForList(prm_unicode)
        #print(unicode)
        if type(unicode) in [list,tuple]:
            
            new_list = []
            for uni in unicode:
                new_list.append(self.transform_uni(uni))
            
            self.range_test_len = max(self.range_test_len, len(new_list))
            return new_list
    
        elif type(unicode) == str:
            
            try :
                return int(unicode)
            except ValueError:
                if VarID.float_regexp.match(str(unicode)):
                    #print("regexp macth")
                    #print(unicode)
                    #print(float(unicode))
                    return float(unicode)
                else:
                    return self.makeIntFromRepr(unicode)
            else:
                #print("Unknown value type: "+self.identifier+" = "+unicode)
                return None
        else:
            #print("Transform of non-string {type_}".format(type_ = type(unicode)))
            return unicode

    
    def makeIntFromRepr(self, str_int):
        if str_int.startswith("0x"):
            self.rep = "hex"
            return int(str_int, 16)
        elif str_int.startswith("0b"):
            self.rep = "bin"
            return int(str_int, 2)
        elif str_int.startswith("0o"):
            self.rep = "oct"
            return int(str_int, 8)
        else:
            return str_int
            
    def getRepOfValue(self, value):
        if type(value) == tuple or type(value) == list:
            if len(value) == 1 and self.range_test_len == 1:    #normally this case is not existent
                return self.getRep(value[0])
            else:
                new_values = []
                for val in value:
                    new_values.append(self.getRep(val))
                return new_values
        else:
            return self.getRep(value)
            
    def getRep(self, value):
        if type(value) == str:
            return value
                
        if type(value) == float:
            return value
        if self.rep == None:
            self.rep = ""
        if value in [None, "None"]:
            #print("Returne rep None") kommt overall
            return str(None)
        else:
            try:
                return VarID.rep_dict[self.rep](value)
            except:
                print("No Represantation: {id_} - {value_}".format(id_=self.identifier, value_=value))
                return str(value)

class InValue(VarID):
    def __init__(self):
        super().__init__()
        self.in_value       = None
        self.solved_in      = None

    #you must consider the range_test_len, because you cant know if you have ..= or string as value, that also has a len
    def get_in_value_indexed(self): 
        return self.get_value_indexed(self.in_value)
    
        
    def get_solved_in_indexed(self):
        return self.get_value_indexed(self.solved_in)
        
    def set_solved_in_indexed(self, value):
        if self.range_test_len == 1:
            self.solved_in = value
        else:
            self.solved_in[self.range_test_index] = value
        
    def bringToRangeLen(self, max_len):
        self.in_value = makeValueMaxLen(max_len, self.in_value)
        self.solved_in = makeValueMaxLen(max_len, self.solved_in)
        
    def getValidationTreeTag(self):
        if self.solved_in:
            return exec_const.resolved_str
        return None
    
        
    def print_summary(self):
        val_list = self.getAsList()
        print("  -> {var_id}: {out_}".format(var_id = self.identifier, out_ = str(val_list)))    
    
    @classmethod
    def getFromXMLNode(cls, node):
        in_value = cls()
        in_value.identifier = node.attributes["identifier"].value
        for in_child in node.childNodes:  
            if in_child.localName == "in":
                in_value.setInFromXML(in_child)
        
        return in_value
        
    def setInFromXML(self, node):
        self.in_value = self.transform_uni(getSafeChildNodeText(node))
        self.rep = ""       
        xml_attribute = node.getAttributeNode("specified_in")
        if not xml_attribute == None:
            self.rep = xml_attribute.value[0:3]
        
    @classmethod
    def getFromDict(cls, id, idict):   
        in_value = cls()
        in_value.identifier = id   
        in_value.in_value   = in_value.transform_uni(idict[tree_const.col_in])
        in_value.rep        = idict.get(tree_const.col_rep, tree_const.combo_reps[0])
        return in_value
      
    def getAsList(self):
        return list(self.getDataTuple())[:2]
        
    def getDataTuple(self):
        return (check_makeStringFromList(self.in_value), 
                check_makeStringFromList(self.getRepOfValue(self.solved_in)), 
                "", 
                "", 
                "", 
                "", 
                "", 
                self.rep,
                "")
        
    def print_html(self, hwr):
        
        hwr.start_tr()
        hwr.insert_td(self.identifier)
        hwr.insert_td_center(self.in_value)
        hwr.insert_td_center(self.getRepOfValue(self.solved_in))
        hwr.end_tr()  

    def getDict(self):
        yml_dict = {}         
        
        dtuple = self.getDataTuple()
        yml_dict[tree_const.col_rep]            = self.rep
        yml_dict[tree_const.col_in]             = dtuple[0]
        yml_dict[tree_const.col_in_solved]      = dtuple[1]
        
        return yml_dict
        
        
class OutValue(VarID):
    def __init__(self):
        super().__init__()
        self.exp_value      = None
        self.solved_exp     = None
        self.operator       = None
        self.out_value      = None
        self.validation     = None
        
    def getRangeTestLen(self):
        if type(self.exp_value) in [list, tuple] and self.operator in tree_const.range_ops:
            if len(self.exp_value) == 2:
                if not type(self.exp_value[0]) in [list, tuple]:
                    self.range_test_len = 1
        return self.range_test_len

    def get_exp_value_indexed(self):  
        return self.get_value_indexed(self.exp_value)
        
        
    def get_solved_exp_indexed(self):
        return self.get_value_indexed(self.solved_exp)
        
    def set_solved_exp_indexed(self, value):
        if self.range_test_len == 1:
            self.solved_exp = value
        else:
            self.solved_exp[self.range_test_index] = value
        
    def get_out_value_indexed(self): 
        return self.get_value_indexed(self.out_value)
        
    def set_out_value_indexed(self, value): 
        if self.range_test_len == 1:
            self.out_value = value
        else:
            self.out_value[self.range_test_index] = value
        
    def bringToRangeLen(self, max_len):
        self.out_value = makeValueMaxLen(max_len, self.out_value)
        self.exp_value = makeValueMaxLen(max_len, self.exp_value)
        self.solved_exp = makeValueMaxLen(max_len, self.solved_exp)
        
    def getValidationTreeTag(self):
        return exec_const.getValidationStringFromEnumerator(self.validation)
        
    @classmethod
    def getFromXMLNode(cls, node):
        out_value = cls()
        out_value.identifier = node.attributes["identifier"].value
        for out_child in node.childNodes:
            if out_child.localName == "out":
                void_ = out_value.setOutFromXML(out_child)
                if void_ == 1:
                    return None
        return out_value
        
        
    def setOutFromXML(self, node):
        for out_child in node.childNodes:
            if out_child.localName in list(comparator_dict.keys()):
                self.operator, self.exp_value = self.get_comparison_list(out_child)
                if self.exp_value == "void":
                    return 1
                xml_attribute = out_child.getAttributeNode("specified_in")
                if not xml_attribute == None:
                    self.rep = xml_attribute.value[0:3]
                    
            if out_child.localName == "outcome":
                self.out_value  = self.transform_uni(getSafeChildNodeText(out_child))
            if out_child.localName == "validation":
                self.validation  = exec_const.getValidationEnumeratorFromString(getSafeChildNodeText(out_child))
                
    @classmethod
    def getFromDict(cls, id, odict):
        out_value = cls()    
        out_value.identifier = id
        out_value.operator  = odict.get(tree_const.col_op, tree_const.combo_ops[0])
        out_value.exp_value = out_value.transform_uni(odict.get(tree_const.col_exp, exec_const.dont_care))
        out_value.rep       = odict.get(tree_const.col_rep, tree_const.combo_reps[0])
        return out_value
        
    def validate(self):
        solved_exp = self.get_solved_exp_indexed()
        out_value = self.get_out_value_indexed()
        #print(solved_exp)
        #print(out_value)
        evaluation = False
        if out_value == None or solved_exp == None or out_value=="None":
            self.validation = exec_const.exception_int
            return
            
        if type(solved_exp) == list:
            if solved_exp[0] == None or solved_exp[1] == None:
                self.validation = exec_const.exception_int
                return 
        
        if (solved_exp == exec_const.dont_care):
            evaluation = True
        else:
            if self.operator in tree_const.combo_ops:
                eval_string = "{out_} {op_} {exp_}".format(out_=out_value, op_=self.operator, exp_=solved_exp)    
                #print(eval_string)
                evaluation = eval(eval_string)
            elif self.operator == "=..":
                evaluation = eval("{out_}>={exp0_} and {out_}<={exp1_}".format(out_=out_value, exp0_=solved_exp[0], exp1_=solved_exp[1]))
            elif self.operator == "!=..":
                evaluation = eval("not ({out_}>={exp0_} and {out_}<={exp1_})".format(out_=out_value, exp0_=solved_exp[0], exp1_=solved_exp[1]))
            else:
                self.validation = exec_const.exception_int
        #print(evaluation)    
        if not self.validation in [exec_const.exception_int, exec_const.failed_int]:
            if evaluation:
                self.validation = exec_const.passed_int
            else:
                self.validation = exec_const.failed_int
    
        
    def print_summary(self):
        if not (self.validation == exec_const.passed_int):
            val_list = self.getAsList()
            print("  -> [{validation}] {var_id}: {out_}".format(validation = exec_const.getValidationStringFromEnumerator(self.validation), var_id = self.identifier, out_ = str(val_list)))

    
    def getAsList(self):
        return list(self.getDataTuple())[2:-2]
        
    def getDataTuple(self):
        return ("", 
                "", 
                check_makeStringFromList(self.exp_value), 
                check_makeStringFromList(self.getRepOfValue(self.solved_exp)), 
                self.operator, 
                check_makeStringFromList(self.getRepOfValue(self.out_value)), 
                exec_const.getValidationStringFromEnumerator(self.validation), 
                self.rep)
        
    def print_html(self, hwr):
        
        hwr.start_tr()
        hwr.insert_td(self.identifier)
        hwr.insert_td_center(self.exp_value)
        hwr.insert_td_center(self.getRepOfValue(self.solved_exp))
        hwr.insert_td_center(self.operator)
        hwr.insert_td_center(self.getRepOfValue(self.out_value))
        hwr.start_td()
        hwr.insert_pic_by_validation(self.validation)
        hwr.end_td()
        hwr.end_tr() 
        

    def getDict(self):
        yml_dict = {}         
        yml_dict["validation"]  = exec_const.getValidationStringFromEnumerator(self.validation)
        
        dtuple = self.getDataTuple()
        yml_dict[tree_const.col_rep]            = self.rep
        yml_dict[tree_const.col_exp]            = dtuple[2]
        yml_dict[tree_const.col_exp_solved]     = dtuple[3]
        yml_dict[tree_const.col_op]             = self.operator
        yml_dict[tree_const.col_out]            = dtuple[5]     
        
        return yml_dict

class ExeInfo:
    def __init__(self):
        self.exe_info       = ""

    def print_exe_info(self):
        print("-"*100)
        if len(self.exe_info):
            print("Exception-Info:")
            print(self.exe_info)

    def add_info_string(self, info):
        if len(self.exe_info):
           self.exe_info += "\n" 
           
        self.exe_info += info             
        
        
class TestVariableDesc(InValue, OutValue, ExeInfo):
    def __init__(self):
        super().__init__()
        self.identifier     = None
        self.exe_info       = ""
        
    def getRangeTestLen(self):
        if type(self.exp_value) in [list, tuple] and self.operator in tree_const.range_ops:
            if len(self.exp_value) == 2:
                if not type(self.exp_value[0]) in [list, tuple]:
                    self.range_test_len = 1
        return self.range_test_len
        
    def bringToRangeLen(self, max_len):
        self.in_value = makeValueMaxLen(max_len, self.in_value)
        self.exp_value = makeValueMaxLen(max_len, self.exp_value)
        self.solved_in = makeValueMaxLen(max_len, self.solved_in)
        self.solved_exp = makeValueMaxLen(max_len, self.solved_exp)
        self.out_value = makeValueMaxLen(max_len, self.out_value)
        
    def getValidationTreeTag(self):
        return exec_const.getValidationStringFromEnumerator(self.validation)
        
    def print_summary(self):
    
        if not (self.validation == exec_const.passed_int):
            print("  -> [{validation}] {var_id}: ".format(validation = exec_const.getValidationStringFromEnumerator(self.validation), var_id = self.identifier))
            val_list = self.getAsList()
            print("  -> in:  {in_}".format(in_ = str(val_list[0:2])))
            print("  -> out: {out_}".format(out_ = str(val_list[3:])))
        if self.validation == exec_const.exception_int:
            self.print_exe_info()

    
    @classmethod
    def getFromXMLNode(cls, node):
        test_var_desc = cls()
        test_var_desc.identifier =  str(node.attributes["identifier"].value)
        
        for child in node.childNodes:  
            if child.localName == "in":
                test_var_desc.setInFromXML(child)
            if child.localName == "out": 
                test_var_desc.setOutFromXML(child)
            if child.localName == "add_info":
                test_var_desc.exe_info = str(getSafeChildNodeText(child)) 
        
        return test_var_desc    
     
    @classmethod     
    def getFromDict(cls, identifier, var_dict):
        test_var_desc = cls()
        test_var_desc.identifier    =  identifier
        test_var_desc.rep           = var_dict.get(tree_const.col_rep, tree_const.combo_reps[0]) 
        
        test_var_desc.in_value      = test_var_desc.transform_uni(var_dict.get(tree_const.col_in, "") )
        
        test_var_desc.exp_value     = test_var_desc.transform_uni(var_dict.get(tree_const.col_exp, exec_const.dont_care) )
        test_var_desc.operator      = var_dict.get(tree_const.col_op, tree_const.combo_ops[0])            
        test_var_desc.out_value     = test_var_desc.transform_uni(var_dict.get(tree_const.col_out, "") )
         
                
        test_var_desc.exe_info      = var_dict.get("exe_info", "") 
        test_var_desc.validation    = var_dict.get("validation", None)
        
        return test_var_desc
        
    def print_html(self, hwr):
        """hwr.start_tr()
        hwr.insert_td_attr(self.identifier, attributes="colspan=\"4\"")
        hwr.start_td(attributes="colspan=\"2\"")
        hwr.insert_pic_by_validation(self.validation)
        hwr.end_td()
        hwr.end_tr()""" 
        
        hwr.start_tr()
        hwr.insert_td(self.identifier)
        hwr.insert_td_center(str(self.in_value))
        hwr.insert_td_center(self.getRepOfValue(self.solved_in))
        hwr.insert_td_center(str(self.exp_value))
        hwr.insert_td_center(self.getRepOfValue(self.solved_exp))
        hwr.insert_td_center(self.operator)
        hwr.insert_td_center(self.getRepOfValue(self.out_value))
        hwr.start_td()
        hwr.insert_pic_by_validation(self.validation)
        hwr.end_td()
        hwr.end_tr() 
        
        if len(self.exe_info):
            hwr.start_tr()
            hwr.insert_td_attr(self.exe_info, attributes="colspan=\"8\"")
            hwr.end_tr()         

    def getAsList(self):
        return list(self.getDataTuple())[:-2]
        
    def getDataTuple(self):
        return (check_makeStringFromList(self.in_value), 
                check_makeStringFromList(self.getRepOfValue(self.solved_in)), 
                check_makeStringFromList(self.exp_value), 
                check_makeStringFromList(self.getRepOfValue(self.solved_exp)), 
                self.operator, 
                check_makeStringFromList(self.getRepOfValue(self.out_value)), 
                exec_const.getValidationStringFromEnumerator(self.validation), 
                self.rep)            
            
    def getDict(self):
        yml_dict = {}         
        yml_dict["validation"]  = exec_const.getValidationStringFromEnumerator(self.validation)
        
        if self.exe_info:
            yml_dict["exe_info"]    = self.exe_info
        
        dtuple = self.getDataTuple()
        yml_dict[tree_const.col_rep]            = self.rep
        yml_dict[tree_const.col_in]             = dtuple[0]
        yml_dict[tree_const.col_in_solved]      = dtuple[1]
        yml_dict[tree_const.col_exp]            = dtuple[2]
        yml_dict[tree_const.col_exp_solved]     = dtuple[3]
        yml_dict[tree_const.col_op]             = self.operator
        yml_dict[tree_const.col_out]            = dtuple[5]     
        
        return yml_dict
        
        

class TestCaseDesc(ExeInfo):

    io_classes = {
        "return": OutValue,
        "parameter": InValue,
        "variable": TestVariableDesc
    }

    def __init__(self):
        super().__init__()
        self.identifier                 = None
        self.description                = None
        
        self.parameter_list             = list()   #instances of InValue
        self.return_list                = list()   #instances of OutValue
        self.variables_list             = list()
        
        self.tree_spec_list_dict = {
            tree_const.tc_vars: self.variables_list,
            tree_const.tc_ret: self.return_list,
            tree_const.tc_prms: self.parameter_list
        }
        
        self.set_up_function            = None
        self.tear_down_function         = None
        
        self.validation                 = None
        
        self.start_time                 = int(0)
        self.delta_time                 = int(0)
        self.covr_vars                  = dict()
        
        self.range_test_index           = 0
        self.range_test_len             = 1
    
        
    def evaluate(self):
        ret_valid = evaluateList(self.return_list)
        var_valid = evaluateList(self.variables_list)
        if self.validation == None:
            self.validation = max([ret_valid, var_valid])
            
    def getValidationTreeTag(self):
        return exec_const.getValidationStringFromEnumerator(self.validation)
        
    def print_summary(self, v=False):
            
        if not (self.validation == exec_const.passed_int) or v:
            delta_string = format(self.delta_time, ".3f")
            print("   ->case {case_id} [{validation_}] [Timedelta: {delta_}] ".format(case_id = self.identifier, delta_=delta_string, validation_ = exec_const.getValidationStringFromEnumerator(self.validation)))
            
            for prm in self.parameter_list:
                prm.print_summary()
            
            for ret in self.return_list:
                ret.print_summary()
                
            for variable in self.variables_list:
                variable.print_summary()
        
        if self.validation == exec_const.exception_int:
            self.print_exe_info()
        
    def getPrmDataTuple(self, index):
        return self.parameter_list[index].getDataTuple()
    
    def getRetDataTuple(self, index):
        return self.return_list[index].getDataTuple()
        
    def get_resolved_prm_list(self):
        prm_list = []
        for prm in self.parameter_list:
            prm_list.append(prm.solved_in)  
        return prm_list
        
    @classmethod
    def getFromXMLNode(cls, node):
        test_case_desc    = cls()
        test_case_desc.identifier = node.attributes["identifier"].value      
                                        
        for child in node.childNodes:
            if child.localName == "test_variable":
                test_case_desc.append_variable(TestVariableDesc.getFromXMLNode(child))
            if child.localName == "description":
                test_case_desc.description       = getSafeChildNodeText(child)
            if child.localName == "add_info":
                test_case_desc.exe_info          = getSafeChildNodeText(child)
            if child.localName == "start_time":
                test_case_desc.start_time        = getSafeChildNodeText(child)
            if child.localName == "delta_time":
                test_case_desc.delta_time        = getSafeChildNodeText(child)
            if child.localName == "test_case_result":
                test_case_desc.validation        = exec_const.getValidationEnumeratorFromString(getSafeChildNodeText(child))
            if child.localName == "test_return":          # for structs this if is more than one time true
                out_value = OutValue.getFromXMLNode(child)
                if not out_value == None:
                    test_case_desc.return_list.append(out_value)       
            if child.localName == "test_parameter":
                test_case_desc.parameter_list.append(InValue.getFromXMLNode(child))
                        
            if child.localName == "set_up":
                for function_child in child.childNodes:
                    if function_child.localName == "function":
                        test_case_desc.set_up_function =  str(function_child.attributes["identifier"].value)  
                        
            if child.localName == "tear_down":             
                for function_child in child.childNodes:
                    if function_child.localName == "function":
                        test_case_desc.tear_down_function =  str(function_child.attributes["identifier"].value)
        
            
        return test_case_desc

    @classmethod
    def getFromDict(cls, identifier, tc_dict):       
        test_case_desc    = cls()
        test_case_desc.identifier = identifier   

        test_case_desc.description          = tc_dict.get("description", "") 
        test_case_desc.exe_info             = tc_dict.get("exe_info", "") 
        test_case_desc.start_time           = tc_dict.get("start_time", "") 
        test_case_desc.delta_time           = tc_dict.get("delta_time", "") 
        test_case_desc.validation           = exec_const.getValidationEnumeratorFromString(tc_dict.get("validation", None))
        test_case_desc.set_up_function      = tc_dict.get("set_up", None)
        test_case_desc.tear_down_function   = tc_dict.get("tear_down", None)
        
        test_case_desc.appendIOValues(tc_dict, "return", test_case_desc.return_list)
        test_case_desc.appendIOValues(tc_dict, "parameter", test_case_desc.parameter_list)
        test_case_desc.appendIOValues(tc_dict, "variable", test_case_desc.variables_list)
        
        
        test_case_desc.return_list.sort(key=id_sorter)
        test_case_desc.parameter_list.sort(key=id_sorter)
        test_case_desc.variables_list.sort(key=id_sorter)
        
        test_case_desc.getAndMakeRangeTestLen()
        
        return test_case_desc
        
    def getAndMakeRangeTestLen(self):
        all_io_list = self.parameter_list+self.variables_list+self.return_list
        self.range_test_len = 1
        for io_id in all_io_list:
            #getRangeTestLen corrects in case of "..="
            self.range_test_len = max(self.range_test_len, io_id.getRangeTestLen())
            
        for io in all_io_list:
            io.bringToRangeLen(self.range_test_len)
            
    def incrementRangeTestIndex(self):
        self.range_test_index += 1
        all_io_list = self.parameter_list+self.variables_list+self.return_list
        for io_id in all_io_list:
           io_id.range_test_index += 1

    def appendIOValues(self, tc_dict, keyword, iolist):
        io_dict = tc_dict.get(keyword, {})
        io_class = TestCaseDesc.io_classes[keyword]
        for io_key, io_op_dict in list(io_dict.items()):
            iolist.append(io_class.getFromDict(io_key, io_op_dict))         
            
    def append_variable(self, test_var_desc):
        self.variables_list.append(test_var_desc)
        
    def print_html(self, hwr):
        hwr.start_table()
        hwr.start_tr()
        hwr.insert_th_attr(self.identifier, attributes="align=\"center\" colspan=\"2\" textcolor=\"orange\"")
        hwr.end_tr()
        
        hwr.start_tr()
        hwr.insert_td("Description")
        hwr.insert_td(self.description)
        hwr.end_tr()
        
        hwr.start_tr()
        hwr.insert_td("Validation")
        hwr.start_td()
        hwr.insert_pic_by_validation(self.validation)
        hwr.end_td()
        hwr.end_tr()

        if self.exe_info:
            hwr.start_tr()
            hwr.insert_td("Exe-Info")
            hwr.insert_td(self.exe_info)
            hwr.end_tr()
        
        hwr.start_td(attributes="colspan=\"2\"")
        if len(self.parameter_list):
            hwr.start_center()
            hwr.start_table(attributes="width=\"100\"")
            hwr.start_tr()
            hwr.insert_th_attr("Parameter", attributes="colspan=\"3\"")
            hwr.end_tr()
            
            hwr.start_tr()
            hwr.insert_td("ID")
            hwr.insert_td("In")
            hwr.insert_td("In-Solved")
            hwr.end_tr()
            for prm in self.parameter_list:
                prm.print_html(hwr)
            hwr.end_table()
            hwr.end_center()
                
        if len(self.return_list):
            hwr.start_center()
            hwr.start_table(attributes="width=\"100\"")
            hwr.start_tr()
            hwr.insert_th_attr("Return", attributes="colspan=\"6\"")
            hwr.end_tr()
            
            hwr.start_tr()
            hwr.insert_td("ID")
            hwr.insert_td("Exp")
            hwr.insert_td("Exp-Solved")
            hwr.insert_td("Operator")
            hwr.insert_td("Out")
            hwr.insert_td("Validation")
            hwr.end_tr()
            for ret in self.return_list:
                ret.print_html(hwr)
            hwr.end_table()
            hwr.end_center()
                
        if len(self.variables_list):
            hwr.start_center()
            hwr.start_table(attributes="width=\"100\"")
            hwr.start_tr()
            hwr.insert_th_attr("Variable", attributes="colspan=\"8\"")
            hwr.end_tr()
            
            hwr.start_tr()
            hwr.insert_td("ID")
            hwr.insert_td("In")
            hwr.insert_td("In-Solved")
            hwr.insert_td("Exp")
            hwr.insert_td("Exp-Solved")
            hwr.insert_td("Operator")
            hwr.insert_td("Out")
            hwr.insert_td("Validation")
            hwr.end_tr()
            for var in self.variables_list:
                var.print_html(hwr)
            hwr.end_table()
            hwr.end_center()
                
        hwr.end_td()
        hwr.end_tr() 
        hwr.end_table()

    def getDict(self):
        yml_dict = {}
        yml_dict["description"] = self.description         
        yml_dict["validation"]  = exec_const.getValidationStringFromEnumerator(self.validation)
        yml_dict["start_time"] = self.start_time
        yml_dict["delta_time"] = self.delta_time          
        
        if self.set_up_function:
            yml_dict["set_up"] = self.set_up_function
        if self.tear_down_function:
            yml_dict["tear_down"] = self.tear_down_function  
        
        if self.exe_info:
            yml_dict["exe_info"]    = self.exe_info
        
        if len(self.return_list):
            sub_dict = {}
            for ret in self.return_list:
                sub_dict[ret.identifier] = ret.getDict()
            yml_dict["return"] = sub_dict
            
        if len(self.parameter_list):
            sub_dict = {}
            for prm in self.parameter_list:
                sub_dict[prm.identifier] = prm.getDict()
            yml_dict["parameter"] = sub_dict
            
        if len(self.variables_list):
            sub_dict = {}
            for var in self.variables_list:
                sub_dict[var.identifier] = var.getDict()
            yml_dict["variable"] = sub_dict

        return yml_dict
        
    
        
class TestSequenceDesc(ExeInfo):
    def __init__(self):
        super().__init__()
        self.identifier             = None
        self.description            = None
        
        self.function               = None
        self.function_wrapper       = None
        self.function_sig           = None
        self.function_desc          = None
        
        self.test_cases_list        = []
        
        self.validation             = None  
        
        self.covr_vars              = {}
        self.covr_string            = None
        self.covr_vars_exist        = False
        
        self.unit                   = None 
    
        
    def evaluate(self):
        tc_valid = evaluateList(self.test_cases_list)
        if self.validation == None:
            self.validation = tc_valid
        
    @classmethod
    def getFromXMLNode(cls, node):
        test_sequence = cls()
        test_sequence.identifier    = str(node.attributes["identifier"].value)
        
        for child in node.childNodes:
            if child.localName == "function":
                test_sequence.function          = str(child.attributes["identifier"].value)
                test_sequence.function_wrapper  = test_sequence.function+"_ut_wrapper"
            if child.localName == "description":
                test_sequence.description       = getSafeChildNodeText(child)
            if child.localName == "test_sequence_passed":
                test_sequence.validation        = exec_const.getValidationEnumeratorFromString(getSafeChildNodeText(child))
            if child.localName == "unitname":
                test_sequence.unit              = str(child.attributes["unit"].value)
            if child.localName == "test_cases":
                for test_case in child.childNodes:
                    if test_case.localName == "test_case":
                        test_sequence.append_test_case(TestCaseDesc.getFromXMLNode(test_case))
                        
        return test_sequence
        
    @classmethod
    def getFromDict(cls, identifier, ts_dict):
        test_sequence = cls()
        test_sequence.identifier        = str(identifier)
        test_sequence.function          = ts_dict.get("function", None)
        test_sequence.function_wrapper  = test_sequence.function+"_ut_wrapper"
            
        test_sequence.description       = ts_dict.get("description", None)  
        test_sequence.validation        = exec_const.getValidationEnumeratorFromString(ts_dict.get("validation", None))
        test_sequence.unit              = ts_dict.get("unit", None) 
        tc_dict = ts_dict["testcases"]
        for tc in tc_dict:
            test_sequence.append_test_case(TestCaseDesc.getFromDict(tc, tc_dict[tc]))  
            
        test_sequence.test_cases_list.sort(key=id_sorter)
            
        return test_sequence
        
    def append_test_case(self, test_case_desc):
        self.test_cases_list.append(test_case_desc)
        
    def setCOVRPercentageOfDict(self):
        size = len(self.covr_vars)
        
        counter = 0
        for key in self.covr_vars:
            if self.covr_vars[key] > 0:
                counter += 1
        
        if (size == 0):
            percentage = 0
        else:
            percentage = (counter/size) * 100
        self.covr_string = "{:.2f}%".format(percentage) 

    def print_summary(self, v=False):
        if not (self.validation == exec_const.passed_int) or v: 
            print("-"*150)
            print("---->Testsequence:  [id: {id_}] [unit: {module}] ==> {valid_}".format(module = self.unit,id_=str(self.identifier), valid_=exec_const.getValidationStringFromEnumerator(self.validation)))
            
            if not self.validation == exec_const.passed_int or v:
                for case in self.test_cases_list:
                    case.print_summary(v=v)
                    
        if self.validation == exec_const.exception_int:
            self.print_exe_info()
            
    def print_html(self, hwr):
        hwr.start_table()
        hwr.start_tr()
        hwr.insert_th_attr(self.identifier, attributes="colspan=\"2\"")
        hwr.end_tr()
        
        hwr.start_tr()
        hwr.insert_td("Function")
        hwr.insert_td(self.function)
        hwr.end_tr()
       
        hwr.start_tr()
        hwr.insert_td("Signature")
        hwr.insert_td(self.function_sig)
        hwr.end_tr()
        
        hwr.start_tr()
        hwr.insert_td("Unit")
        hwr.insert_td(self.unit)
        hwr.end_tr()
        
        hwr.start_tr()
        hwr.insert_td("Description")
        hwr.insert_td(self.description)
        hwr.end_tr()
        
        hwr.start_tr()
        hwr.insert_td("Validation")
        hwr.start_td()
        hwr.insert_pic_by_validation(self.validation)
        hwr.end_td()
        hwr.end_tr()
        if self.covr_string:
            hwr.start_tr()
            hwr.insert_td("Covr")
            hwr.insert_td(self.covr_string)
            hwr.end_tr()
        if self.exe_info:
            hwr.start_tr()
            hwr.insert_td("Exe-Info")
            hwr.insert_td(self.exe_info)
            hwr.end_tr()
        
        hwr.start_td(attributes="colspan=\"2\"")
        for case in self.test_cases_list:
            case.print_html(hwr) 
        hwr.end_td()
        hwr.end_tr() 
        hwr.end_table()

    def getDict(self):
        yml_dict = {}
        yml_dict["function"] = self.function  
        yml_dict["description"] = self.description         
        yml_dict["validation"] = exec_const.getValidationStringFromEnumerator(self.validation)
        yml_dict["unit"] = self.unit
        if self.covr_string:
            yml_dict["covr"] = self.covr_string
        if self.exe_info:
            yml_dict["exe_info"] = self.exe_info
        sub_dict = {}
        for case in self.test_cases_list:
            sub_dict[case.identifier] = case.getDict()
            
        yml_dict["testcases"] = sub_dict

        return yml_dict


class TestProject(): 
    def __init__(self):
        self.project                = None 
        self.validation             = None 

        self.kpi                    = {}
        self.covr_string            = None
        self.prj_covr_string        = None
        self.covr_vars_exist        = False
        
        self.execution              = None    
        self.timestamp              = None
        
        self.target_information     = None
        self.conn_flags             = None
        
    def getValidationTreeTag(self):
        return exec_const.getValidationStringFromEnumerator(self.validation)
        
    def prj_summary(self):
        print("-"*150)
        print("Project: {prj}".format(prj = self.project))
        print("Summary: [Timestamp: {tstamp}] ==> {bool_}".format(tstamp=self.timestamp, bool_=exec_const.getValidationStringFromEnumerator(self.validation)))
        if self.covr_vars_exist:
            print("Coverage: [complete: {cvr_prj_}] [testfunctions: {cvr_}]".format(cvr_prj_=self.prj_covr_string, cvr_=self.covr_string))
        if self.execution:
            print("Execution: [by: {exe_by_}]".format(exe_by_=self.execution))
        print("-"*150)
        
    def html_summary(self, hwr, pdf_report=""):
        target_information = ""
        for target_information_part in self.target_information.split("\n"):
            target_information  += target_information_part+"<br/>"
        
        summary_list = [["TengSIO", target_information],
                        ["Connection", self.conn_flags],
                        ["Execution by:", "{exec_}".format(exec_=self.execution)]]
        if self.covr_vars_exist:
            summary_list.append(["Coverage", "prj-{pcovr_}<br/> functions-{covr_}".format(pcovr_=self.prj_covr_string, covr_=self.covr_string)])
        
        hwr.start_center()
        hwr.start_table()
        hwr.start_tr()
        hwr.insert_th_attr("Test Summary", "colspan=\"2\"")
        hwr.end_tr()
         
        for sum in summary_list:
            hwr.start_tr()
            hwr.insert_td(sum[0])
            hwr.insert_td(sum[1])
            hwr.end_tr()
        
        hwr.start_tr() 
        hwr.insert_td("Validation:")        
        hwr.start_td()
        hwr.insert_pic_by_validation(self.validation)
        hwr.end_td()
        hwr.end_tr()   
        
        if len(pdf_report):
            hwr.start_tr()
            hwr.start_td("colspan=\"2\"")
            hwr.simple_print(pdf_report)
            hwr.end_td()
            hwr.end_tr()
            
        hwr.end_table()  
        hwr.end_center()
        hwr.simple_print("<br/>")
             

class TestSequences(TestProject):
    def __init__(self):
        super().__init__()
        self.test_sequences_list    = []

    def evaluate(self):
        ts_valid = evaluateList(self.test_sequences_list)
        if self.validation == None:
            self.validation = ts_valid
        
    def append_test_sequence(self, test_sequence_descriptor):
        self.test_sequences_list.append(test_sequence_descriptor)
    
    def search_sequence_by_name(self, prm_name):
        for sequence in self.test_sequences_list:
            if sequence.identifier == prm_name:
                return sequence.unit
            
    
    def getNodesExecutionInfo(self, node):
        for child in node.childNodes:
            if child.localName == "projectname":
                self.covr_string            = str(child.attributes["sequences_covr"].value)
                self.project                = str(child.attributes["project"].value)
                self.target_information     = str(child.attributes["connection"].value)
            if child.localName == "test_sequence_result":
                self.validation             = exec_const.getValidationEnumeratorFromString(getSafeChildNodeText(child))
            if child.localName == "test_timestamp":
                self.timestamp              = getSafeChildNodeText(child)
            if child.localName == "execution_by":
                self.execution           = getSafeChildNodeText(child)   
                
    @classmethod
    def create_from_xml(cls, abs_path):
        test_descriptor = cls()               

        test_case_xml=dom.parse(abs_path)    
        for test_sequences in test_case_xml.childNodes:
            if test_sequences.localName == "test_sequences":
                test_descriptor.getNodesExecutionInfo(test_sequences)
                for test_sequence in test_sequences.childNodes:
                    if test_sequence.localName == "test_sequence":
                        test_descriptor.append_test_sequence(TestSequenceDesc.getFromXMLNode(test_sequence))
        
        return test_descriptor, FileLoader.TSEQ
        
    @classmethod    
    def create_from_yml(cls, abs_path):
        test_descriptor = cls()

        seq_dict = {}
        with open(abs_path, "r") as f:
            seq_dict = yaml.safe_load(f)
        
        ts_dict = seq_dict["testsequences"]
        for ts in ts_dict:
            test_descriptor.append_test_sequence(TestSequenceDesc.getFromDict(ts, ts_dict[ts]))
            
        test_descriptor.test_sequences_list.sort(key=id_sorter)
        
        return test_descriptor, FileLoader.TSEQ
    
    @classmethod    
    def getFromDict(cls, tdict):
        test_descriptor = cls()
        ts_dict = tdict["testsequences"]
        for ts in ts_dict:
            test_descriptor.append_test_sequence(TestSequenceDesc.getFromDict(ts, ts_dict[ts]))
        
        test_descriptor.test_sequences_list.sort(key=id_sorter)
        
        return test_descriptor
        
    def print_summary(self, v=False):
        for seq in self.test_sequences_list:
            seq.print_summary(v=v)
            
    def print_html(self, hwr):
        hwr.start_tr()
        hwr.start_td()
        for seq in self.test_sequences_list:
            seq.print_html(hwr) 
        hwr.end_td()
        hwr.end_tr() 

    def getDict(self):
        yml_dict = {}
        sub_dict = {}
        
        for seq in self.test_sequences_list:
            sub_dict[seq.identifier] = seq.getDict()
            
        yml_dict["testsequences"] = sub_dict

        return yml_dict
            
         
class TestSuite(TestProject):
    
    def __init__(self):
        super().__init__()
        self.file_loader_list               = []

    
    def getSeqDataTuple(self, index):
        fl = self.file_loader_list[index]
        return (fl.basename, fl.path)
        
    def getDict(self):
        tseq_dict = {}
        for fl in self.file_loader_list:
                tseq_dict[fl.basename] = fl.path
                
        tsuite_dict = {"testsuite": tseq_dict}
        return tsuite_dict
    
    """
    prints a testsuite result.
    """    
    def print_summary(self, v=False):
        
        for fl in self.file_loader_list:
            fl.test_desc.print_summary(v=v)
        
        print("-"*150) 
        for fl in self.file_loader_list:
            print("["+ exec_const.getValidationStringFromEnumerator(fl.test_desc.validation) +"] : "+ fl.path)
        
    
    def evaluate(self):
        
        passed_list = list()
        
        for fl in self.file_loader_list:
            passed_list.append(fl.test_desc.validation)
            
        self.validation = max(passed_list)
    
       
    @classmethod 
    def create_from_xml(cls, abs_path):        
        
        test_suite_desc = cls()
        test_suite_xml = dom.parse(abs_path) 
        
        for test_suite in test_suite_xml.childNodes:
            for test_file in test_suite.childNodes:
                if test_file.localName == "test_sequence_file":
                    fl = FileLoader.fromSeqOfSuite(test_file.attributes["location"].value, abs_path)
                    fl.analyseFile()
                    test_suite_desc.file_loader_list.append(fl) 
                    
        return test_suite_desc, FileLoader.TSUITE
        
    @classmethod    
    def create_from_yml(cls, abs_path):
        test_suite_desc = cls()

        suite_dict = {}
        with open(abs_path, "r") as f:
            suite_dict = yaml.safe_load(f)
        
        ts_dict = suite_dict["testsuite"]
        for ts in ts_dict:
            fl = FileLoader.fromSeqOfSuite(ts_dict[ts], abs_path)
            fl.analyseFile()
            test_suite_desc.file_loader_list.append(fl) 
        test_suite_desc.file_loader_list.sort(key=id_sorter)
        return test_suite_desc, FileLoader.TSUITE 
    
    @classmethod
    def getFromDict(cls, suite_dict, abs_path):
        test_suite_desc = cls()
        ts_dict = suite_dict["testsuite"]
        for ts in ts_dict:
            fl = FileLoader.fromSeqOfSuite(ts_dict[ts], abs_path)
            fl.analyseFile()
            test_suite_desc.file_loader_list.append(fl) 
        test_suite_desc.file_loader_list.sort(key=id_sorter)
        return test_suite_desc        

class IntTestSuite(TestProject):
    def __init__(self):
        #super().__init__()
        self.file_loader_list = []
        
    @classmethod    
    def create_from_yml(cls, abs_path):
        int_testsuite = cls()

        suite_dict = {}
        with open(abs_path, "r") as f:
            suite_dict = yaml.safe_load(f)
        
        ts_dict = suite_dict["integrationtests"]
        for ts in ts_dict:
            fl = FileLoader(ts_dict[ts])
            fl.analyseFile()
            int_testsuite.file_loader_list.append(fl) 
        
        return int_testsuite, FileLoader.INTSUITE 
    
    def evaluate(self):
    
        passed_list = list()
        
        for fl in self.file_loader_list:
            passed_list.append(fl.test_desc.validation)
            
        self.validation = max(passed_list)
        
    def print_summary(self):
        
        for fl in self.file_loader_list:
            fl.test_desc.print_summary()
        
        print("-"*150) 
        for fl in self.file_loader_list:
            print("["+ exec_const.getValidationStringFromEnumerator(fl.test_desc.validation) +"] : "+ fl.path)
    
    
class IntTestDesc(TestProject):
    def __init__(self):
        #super().__init__()
        self.test_ids = []
        
    @classmethod    
    def create_from_yml(cls, abs_path):
        inttest_descriptor = cls()

        seq_dict = {}
        with open(abs_path, "r") as f:
            seq_dict = yaml.safe_load(f)
        
        ts_dict = seq_dict["integrationtest"]
        for ts in ts_dict:
            inttest_descriptor.append_test_id(IntTestID.getFromDict(ts, ts_dict[ts]))
        
        return inttest_descriptor, FileLoader.INT 
        
    def append_test_id(self, inttest_id):
        self.test_ids.append(inttest_id)
        
    def print_xls(self, xls):
        for tid in self.test_ids:
            tid.print_xls(xls)
            
    def evaluate(self):
        self.validation = evaluateList(self.test_ids)
        
    def print_summary(self):
        if not (self.validation == exec_const.passed_int): 
            print("-"*150)
            print("---->IntegrationTest: ==> {valid_}".format(valid_=exec_const.getValidationStringFromEnumerator(self.validation)))
            
            for tid in self.test_ids:
                tid.print_summary()
        
class IntTestID():
    def __init__(self):
        self.identifier = None
        self.cleanup = None
        self.result = None
        self.rep = ""
        self.exe = []
        self.validation = None
        self.delta_time = None
        
    def evaluate(self):
        if not self.validation == None:
            self.validation = max(self.result.validate(), self.validation)
        else:
            self.result.validate()
        
        
    @classmethod
    def getFromDict(cls, identifier, ts_dict):
        inttest_id = cls()
        inttest_id.identifier   = str(identifier)
        cleanup_dict = ts_dict.get("cleanup", None)
        if cleanup_dict:
            inttest_id.cleanup      = cleanup.getFromDict(cleanup_dict)
            
        result_dict = ts_dict.get("result", None)
        if result_dict:            
            inttest_id.result       = result.getFromDict(result_dict)
        
        inttest_id.rep          = ts_dict.get("rep", "")
        
        exe_dict = ts_dict.get("exe", None)
        if exe_dict: 
            for tc in range(1, len(exe_dict)+1):
                inttest_id.append_exe(execution.getFromDict(exe_dict[tc]))  
            
        return inttest_id
        
    def append_exe(self, exe):
        self.exe.append(exe)
        
    def print_xls(self, xls):
        
        xls.print_xls(self.identifier, "CPU")
        
        for exe in self.exe:
            xls.start_exe()
            exe.print_xls(xls)
            xls.end_exe()
            
        xls.apply_styles()
            
        if self.result:
            self.result.print_xls(xls)
            
    def print_summary(self):
            
        if not (self.validation == exec_const.passed_int):
            delta_string = format(self.delta_time, ".3f")
            print("   ->test {case_id} [{validation_}] [Timedelta: {delta_}] ".format(case_id = self.identifier, delta_=delta_string, validation_ = exec_const.getValidationStringFromEnumerator(self.validation)))
            
            if self.cleanup:
                self.cleanup.print_summary()
            
            for e in self.exe:
                e.print_summary()
                
            self.result.print_summary()
        
        
class execution():
    def __init__(self):
        self.set_array = []
        self.call_function = []
        self.get_variable = []
    
    def print_summary(self):
        for sa in self.set_array:
            sa.print_summary()
        for sa in self.call_function:
            sa.print_summary()
        for sa in self.get_variable:
            sa.print_summary()
    
    @classmethod
    def getFromDict(cls, exe_dict):
        exe = cls()
        
        call_dict = exe_dict.get("call_function", None)
        if call_dict:
            for tc in range(1, len(call_dict)+1):
                exe.call_function.append(call_function.getFromDict(call_dict[tc])) 
            
        set_array_dict = exe_dict.get("set_array", None)
        if set_array_dict:
            for tc in range(1, len(set_array_dict)+1):
                exe.set_array.append(set_array.getFromDict(set_array_dict[tc]))  
                
        get_var_dict = exe_dict.get("get_variable", None)
        if get_var_dict:
            for tc in range(1, len(get_var_dict)+1):
                exe.get_variable.append(get_variable.getFromDict(get_var_dict[tc])) 
                    
        return exe
                
    def print_xls(self, xls):
        for sa in self.set_array:
            sa.print_xls(xls)
        for cf in self.call_function:
            cf.print_xls(xls)            
        for gv in self.get_variable:
            gv.print_xls(xls)
    
class cleanup(execution):
    def __init__(self):
        super().__init__()

class get_variable(ExeInfo):
    def __init__(self):
        self.identifier = None
        self.value      = None
        
    @classmethod
    def getFromDict(cls, gv_dict):
        gv = cls()
        for key in gv_dict:
            gv.identifier = key
        
        return gv
        
    def print_xls(self, xls):
        xls.print_xls("GET: "+self.identifier, self.value) 
        
    def print_summary(self):
        print( "-> GET {var_}: {value_}".format(var_ = self.identifier, value_=str(self.value)))
        if self.value == None:
            self.print_exe_info()
        
class get_array(ExeInfo):
    def __init__(self):
        self.identifier     = None
        self.range          = None 
        self.values         = None
        
    @classmethod
    def getFromDict(cls, ga_dict):
        gv = cls()
        for key in ga_dict:
            gv.identifier     = key
            gv.range          = ga_dict[key][0] 
        
        return gv
        
    def print_xls(self, xls):
        for i in range(0, self.range):
            xls.print_xls("GET: "+self.identifier+"["+str(i)+"]", self.values[i])  

    def print_summary(self):
        print( "-> GET {var_}[0-{range_}]: {value_}".format(var_ = self.identifier,range_=str(self.range), value_=str(self.values))) 
        if None in self.values:
            self.print_exe_info()
            
        
class set_array(ExeInfo):
    def __init__(self):
        self.identifier = None
        self.range      = None 
        self.value      = None
        
    @classmethod
    def getFromDict(cls, sa_dict):
        sa = cls()
        for key in sa_dict:
            dval = sa_dict[key]
            sa.identifier = key
            sa.range = dval[0]
            sa.value = dval[1]
        
        return sa
        
    def print_xls(self, xls):
        for i in range(0, self.range):
            xls.print_xls("SET: "+self.identifier+"["+str(i)+"]", self.value)  
            
    def print_summary(self):
        print( "-> SET {var_}[0-{range_}]: {value_}".format(var_ = self.identifier,range_=str(self.range), value_=str(self.value)))

        
class call_function(ExeInfo):
    def __init__(self):
        self.identifier = None
        self.parameters = None 
        self.return_value = None
        
    @classmethod
    def getFromDict(cls, cf_dict):
        cf = cls()
        for key in cf_dict:
            cf.identifier = key
            cf.parameters = cf_dict[key]
        
        return cf
        
    def print_xls(self, xls):
        xls.print_xls("TH_FSF_Function", self.identifier+str(self.parameters))

    def print_summary(self):
        print( "-> CALL {var_}[{prm_}]: {value_}".format(var_ = self.identifier, prm_=str(self.parameters), value_=str(self.return_value))) 
        if None in self.return_value:
            self.print_exe_info()        

class result():
    def __init__(self):
        self.exp_string = None
        self.buffer = None
        self.validation = None
        self.res_string = None

    def print_summary(self):
        print( "-> Buffer [{valid_}]: {value_}".format(valid_=exec_const.getValidationStringFromEnumerator(self.validation), value_=str(self.res_string))) 
        if not self.validation == exec_const.passed_int:
            self.buffer.array.print_summary()
        
    def validate(self):
        self.res_string = self.buffer.getResStr()
        if self.res_string.strip() == self.exp_string.strip():
            self.validation = exec_const.passed_int
        else:
            self.validation = exec_const.failed_int
        
    @classmethod
    def getFromDict(cls, r_dict):
        r = cls()
        r.exp_string = r_dict.get("expectation", None)
        buf = r_dict.get("buffer", None)
        if buf:
            r.buf = buffer.getFromDict(buf)
        
        return r 

    def print_xls(self, xls):
        if self.validation:
            xls.print_xls("Success", self.res_string, style=xls_printer.OkStyle, color=False) 
        else:
            xls.print_xls("Failed", self.res_string, style=xls_printer.ErrStyle, color=False) 
        
class buffer(ExeInfo):
    def __init__(self):
        self.len_var = None
        self.array = None
        
    @classmethod
    def getFromDict(cls, b_dict):
        b = cls()
        b.len_var = get_variable.getFromDict(b_dict["len"])
        b.array = get_array.getFromDict(b_dict["get_array"])
        
        return b 

    def getResStr(self):
        resStr = ""
        for value in self.array.values:
            resStr = resStr + '{:02X} '.format(value)
        return resStr
        
class FileLoader(PathFormer):

    TSEQ = 0
    TSUITE = 1
    INT = 2
    INTSUITE = 3
    
    poly_xml_dict = {
        "test_sequences" : TestSequences,
        "test_suite" : TestSuite
    }
    
    poly_yml_dict = {
        "testsequences" : TestSequences,
        "testsuite" : TestSuite,
        "integrationtest" : IntTestDesc,
        "integrationtests" : IntTestSuite
    }
    
    
    def __init__(self, path):
        super().__init__(path)
        
        self.loaded_type = None
        self.test_desc = None
        
        self.ToReport = []
        
     
    @classmethod
    def getFromInstance(cls, path, desc, type):
        fl = cls(path)
        
        fl.test_desc = desc
        if type == FileLoader.TSEQ:
            fl.ToReport = [fl]
        elif type == FileLoader.TSuite:
            fl.ToReport = desc.file_loader_list
            
        fl.loaded_type = type
        return fl
        
    def analyseFile(self):
        if self.isXML():
            t_xml = dom.parse(self.abs_path)
            for node in t_xml.childNodes:
                test_class = FileLoader.poly_xml_dict.get(node.localName, None)
                if test_class:
                    self.test_desc, self.loaded_type = test_class.create_from_xml(self.abs_path)
                    break
        
        elif self.isYML(): 
            t_dict = {}
            with open(self.abs_path, "r") as f:
                t_dict = yaml.safe_load(f)
                
            test_class = FileLoader.poly_yml_dict.get(list(t_dict.keys())[0], None)
            if test_class:
                self.test_desc, self.loaded_type = test_class.create_from_yml(self.abs_path)
        
        if self.loaded_type in [FileLoader.TSEQ, FileLoader.INT]:
            self.ToReport.append(self)
        elif self.loaded_type in [FileLoader.TSUITE, FileLoader.INTSUITE]:
            self.ToReport = self.test_desc.file_loader_list
            
    def save(self):
         with open(self.abs_path, "w+") as f:
            yaml.dump(self.test_desc.getDict(), f)