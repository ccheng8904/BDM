"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : widgets.py
Date of creation : 2020-03-04
Author Name/Dept : Nikolai Ehrhardt
Organization     : SDD-4
Description      : Adapter for filling treeview from soco
"""

from ..soco.SourceCodeDescriptor import SourceCodeDescriptor

class SocoAdapter():

    Global = None
    
    all = "all"
    func_prefix = "functions__"
    var_prefix = "variables__"
    all_func = func_prefix+all
    all_var = var_prefix+all

    def __init__(self, pstd_filepath="", psyd_filepath="", treeview=None, soco= None):
        if soco == None:
            self.soco = SourceCodeDescriptor()
            self.soco.SetFrom_PSTD_Filepath(pstd_filepath)
            self.soco.AddFrom_PSYD_Filepath(psyd_filepath)
        else:
            self.soco = soco
        SocoAdapter.Global = self
        self.tv = treeview
        
        self.unit_desc = None
        self.function_tv_id = None
        self.variable_tv_id = None
        self.type_tv_id = None
        
        self.refill()
        
    """
    @classmethod    
    def fromSoco(cls, soco, treeview):
        sa = cls(soco=soco, treeview=treeview)
        sa.soco = soco
        SocoAdapter.Global = sa
        sa.tv = treeview
        
        sa.unit_desc = None
        sa.function_tv_id = None
        sa.variable_tv_id = None
        sa.type_tv_id = None
        
        sa.refill()
        return sa
    """    
    def refill(self):
        self.deleteAll()
        self.fillTreeview()        

    def deleteAll(self):
        for child in self.tv.get_children():
            self.tv.delete(child)        

    def fillTreeview(self):
        unit_names = list(self.soco.unit_dictionary.keys())
        unit_names.sort()
        
        for unit in unit_names:
            self.addUnit(unit)

            
    def addUnit(self, unit):
        unit_tv_id = self.tv.insert('', 'end', unit, text=unit)
        
        self.unit_desc = self.soco.unit_dictionary[unit]
        unit_function_item = SocoAdapter.func_prefix+unit
        unit_var_item = SocoAdapter.var_prefix+unit
        self.function_tv_id = self.tv.insert(unit_tv_id, 'end', unit_function_item, text='functions')
        self.variable_tv_id = self.tv.insert(unit_tv_id, 'end', unit_var_item, text='variables')
        #self.type_tv_id = self.tv.insert(unit_tv_id, 'end', 'types'+"__"+unit, text='types')
        
        self.fillFunctions(self.unit_desc.function_descriptions_local_dict)
        self.fillFunctions(self.unit_desc.function_descriptions_global_dict)
 
        self.fillVariables(self.unit_desc.variable_descriptions_dict)
        self.fillVariables(self.unit_desc.ext_variable_descriptions_dict)
        
        self.checkAndDelete(unit_function_item)
        self.checkAndDelete(unit_var_item)
        self.checkAndDelete(unit)
                
    def checkAndDelete(self, item):
        if not len(self.tv.get_children(item)):
            self.tv.delete(item)        
                
    
    def search(self, text, item=None):
    
        if text=="":
            self.refill()
            return
        self.tv.insert('', 'end', SocoAdapter.all_func, text="functions")
        self.tv.insert('', 'end', SocoAdapter.all_var, text="variables")
        
        self.getCompareFuncOfText(text)
        for unit_item in self.tv.get_children():
            if unit_item in [SocoAdapter.all_func, SocoAdapter.all_var]:
                continue
            self.search_rec(unit_item)
            self.tv.delete(unit_item)
            
        self.checkAndDelete(SocoAdapter.all_func)
        self.checkAndDelete(SocoAdapter.all_var)    
            
    def search_rec(self, item):
        for rec_item in self.tv.get_children(item):
            
            if len(self.tv.get_children(rec_item)) == 0:
                if self.compare_func(rec_item):
                    moveto = ""
                    if item.startswith("f"):
                        moveto = SocoAdapter.all_func
                    elif item.startswith("v"):
                        moveto = SocoAdapter.all_var
                    else:
                        continue
                    self.tv.move(rec_item, moveto, 1)
            else:
                self.search_rec(rec_item)
                
                
    
    def applyFilter(self, text, type="unit"):
        self.getCompareFuncOfText(text)
            
        self.deleteAll()
        if type == "unit":
            unit_names = list(self.soco.unit_dictionary.keys())
            unit_names.sort()
            
            for unit in unit_names:
                if self.compare_func(unit):
                    self.addUnit(unit)            
        

    def getCompareFuncOfText(self, text):
        self.compare_func = None
        self.ctext = ""
        if text.startswith("*"):
            self.compare_func = self.endswith
            self.ctext = text[1:]
        elif text.endswith("*"): 
            self.compare_func = self.startswith
            self.ctext = text[:-1]
        else:
            self.compare_func = self.isIn
            self.ctext = text
         
        
    def isIn(self, unit):
        return self.ctext in unit
        
    def startswith(self, unit):
        return unit.startswith(self.ctext)
        
    def endswith(self, unit):
        return unit.endswith(self.ctext)
        
                
    def fillFunctions(self, func_dict):
        func_names = list(func_dict.keys())
        func_names.sort()
        for func in func_names:
            func_desc = func_dict[func]
            if not func_desc.define_unit:
                continue
            addr_location = "N/A"
            if func_desc.SymbolLocation:
                addr_location = func_desc.SymbolLocation.location_str
            data = (func_desc.getSignature(funcname_add="#"),addr_location,"-",func_desc.getQualifier())
            try:
                for_func_tv_id = self.tv.insert(self.function_tv_id, 'end', func, text=func, values=data)
            except:
                print("Function already inserted: "+func)
            
    def fillVariables(self, var_dict):
        var_names = list(var_dict.keys())
        var_names.sort()
        for var in var_names:
            var_desc = var_dict[var]
            quali = var_desc.getStatic()+var_desc.definition_description.get_qualifier_string()
            type = var_desc.definition_description.get_source_code_type("#")
            data = (type, var_desc.definition_description.symbol_location_descr.location_str, "-", quali)
            for_var_tv_id = self.tv.insert(self.variable_tv_id, 'end', var, text=var, values=data)
            
    def getVariableDescOfUnit(self, unit, var):
    
        if unit == SocoAdapter.all:
            return self.getVariableDesc(var)
        
        unit_desc = self.soco.unit_dictionary[unit]
        
        desc = unit_desc.variable_descriptions_dict.get(var, None)
        if desc:
            return desc
        
        desc = unit_desc.ext_variable_descriptions_dict.get(var, None)
        if desc:
            return desc
        
        return
        
    def getFuncDescOfUnit(self, unit, func):
    
        if unit == SocoAdapter.all:
            return self.getFuncDesc(func)
    
        unit_desc = self.soco.unit_dictionary[unit]
        desc = unit_desc.function_descriptions_local_dict.get(func, None)
        if desc:
            return desc
        
        desc = unit_desc.function_descriptions_global_dict.get(func, None)
        if desc:
            return desc
        
        return
        
    def getFuncDesc(self, func):
        for unit in self.soco.unit_dictionary:
            func_desc = self.getFuncDescOfUnit(unit, func)
            if not func_desc == None:
                return func_desc
            
            
    def getVariableDesc(self, var):
        for unit in self.soco.unit_dictionary:
            var_desc = self.getVariableDescOfUnit(unit, var)
            if not var_desc == None:
                return var_desc        
            
            
            
            
            
            
            
            