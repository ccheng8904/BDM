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
Description      : custom menues on right/left-click
"""

from tkinter import Menu
from tkinter import messagebox
from tkinter import filedialog
from tkinter import ttk, BooleanVar

from .soco_adapter import SocoAdapter

from ..globals.constants import tree_const
from ..globals.file_loader import FileLoader

class RightClick_base():
    def __init__(self, context):
        self.context = context
        self.target = context.target        

class SoCoExtension(RightClick_base):
    def __init__(self, context):
        super().__init__(context)
        self.soco_A = None
        
    def setSocoA(self, soco_a):
        self.soco_A = soco_a

class RightClick_TestSuiteTree(RightClick_base):
    def __init__(self, context):
        super().__init__(context)
        
    def popup(self, event):
        column = self.context.identify_column(event.x)
        item = self.context.identify_row(event.y)  
        region = self.context.identify_region(event.x, event.y)
        print(region, column, item)
        show_menu = False
        self.aMenu = Menu(None, tearoff=0)
        
        if len(item):
            self.context.focus(item)
            self.context.selection_set(item)
            self.tree_item = item  
            
        self.item_split = item.split("__", 2)
        if region == 'tree':
            if len(self.item_split) == 1:
                self.aMenu.add_command(label='Save', command=self.save)
                self.aMenu.add_command(label='SaveAs', command=self.saveAs)

                self.aMenu.add_command(label='Add TestSequence', command=self.addTSeq)
                xml=self.context.set(self.item_split[0], tree_const.col_loc).endswith("xml")
                if xml:
                    self.aMenu.add_command(label='Convert to yaml', command=self.convertToYaml)
                if self.target:
                    self.aMenu.add_command(label='Execute', command=self.execute)
                self.aMenu.add_separator()
                self.aMenu.add_command(label='Close', command=self.close)
                show_menu = True
            elif len(self.item_split) == 3:
                self.aMenu.add_command(label='Load', command=self.load)
                self.aMenu.add_separator()
                self.aMenu.add_command(label='Remove', command=self.removeTSeq)
                show_menu = True
        
        if region == 'nothing':
            self.aMenu.add_command(label='New TestSuite', command=self.createNewTSuite)
            show_menu = True  
            
        if show_menu:
            self.aMenu.post(event.x_root, event.y_root) 
            
    def execute(self):
        self.context.execute(self.tree_item)
     
    def load(self):
        self.context.addTestSequenceDescToTSNB(self.tree_item)
        
    def removeTSeq(self):
        self.context.removeTSeq(self.tree_item)
     
    def save(self):
        self.context.save(self.item_split[0])

    def saveAs(self):
        self.context.saveAs(self.item_split[0])        
    
    def addTSeq(self):
        self.context.addTSeq(self.item_split[0])
        
    def close(self):
        self.context.close(self.item_split[0])
        
    def convertToYaml(self):
        self.context.convertToYaml(self.item_split[0])
        
    def createNewTSuite(self):
        self.context.newTSuite()
            
class RightClick_TCTree_SocoAdapter(SoCoExtension):
    def __init__(self, context):
        super().__init__(context)
        self.soco_A = None
        
        self.tree_item = ''
        self.arg_var_list = None
        self.prm_arg_dict = None
        self.arg_var_list, self.prm_arg_dict = self.context.func_desc.get_arg_vars_as_list()
        
        self.stub_var_list = []
        self.calle_stubvar_dict = {"order": []}
        
        self.var_desc_dict = {}
        
        for callee in self.context.func_desc.callee_dict:
            func_desc = SocoAdapter.Global.getFuncDesc(callee)
            #inline function can cause func_desc=None
            if func_desc:
                stub_var_list = func_desc.get_stub_vars_as_list()
                self.calle_stubvar_dict[callee] = stub_var_list
                self.calle_stubvar_dict["order"].append(callee)
                self.stub_var_list.extend(stub_var_list)
        
        for var in self.stub_var_list+self.arg_var_list:
            var_desc = SocoAdapter.Global.getVariableDesc(var)
            if not var_desc == None:
                self.var_desc_dict[var] = var_desc     

    def getTCValuesToSet(self, prm):
        if MainMenu.argvar_check:
            arg_vars = self.prm_arg_dict.get(prm, None)
            if arg_vars == None:
                return tree_const.prm_value_std, []
            else:
                prm_values = list(tree_const.prm_value_std)
                prm_values[0] = arg_vars[0]+".&"
                return prm_values, arg_vars
        else:
            return tree_const.prm_value_std, []
    
    def createNewTC(self):
        self.context.addTC()
        
    def deleteTC(self):
        self.context.deleteTC(self.tree_item)
        
    def copyTC(self):
        self.context.copyTC(self.tree_item)
        
    def deleteItem(self):
        self.context.deleteItem(self.tree_item)
        
    def toggleSettingsItem(self):
        self.context.toggleSettingsItem(self.tree_item)
        
    def applyIDStringsForItem(self):
        self.context.applyIDStringsForItem(self.tree_item)
    

    def getStubVarMenu(self, var_dict, all_var_list):
        self.StubMenu = Menu(self.aMenu, tearoff=0)

        for decl in var_dict["order"]:
            decl_list = var_dict[decl]
            if len(decl_list):
                DeclMenu = Menu(self.StubMenu, tearoff=0)
                for var in decl_list:
                    DeclMenu.add_command(label=var, command=lambda v=var: self.addStubVar(v, decl_list)) #lambda function
                DeclMenu.add_command(label="all", command=lambda v="all": self.addStubVar(v, decl_list))
                self.StubMenu.add_cascade(label=decl, menu=DeclMenu)
        if len(var_dict["order"]):
            self.StubMenu.add_command(label="all", command=lambda v="all": self.addStubVar(v, all_var_list))
            return self.StubMenu
        else:
            return None
            
    def getStubVarMenu_allTC(self, var_dict, all_var_list):
        self.StubMenu = Menu(self.aMenu, tearoff=0)

        for decl in var_dict["order"]:
            decl_list = var_dict[decl]
            if len(decl_list):
                DeclMenu = Menu(self.StubMenu, tearoff=0)
                for var in decl_list:
                    DeclMenu.add_command(label=var, command=lambda v=var: self.addStubVar_allTC(v, decl_list)) #lambda function
                DeclMenu.add_command(label="all", command=lambda v="all": self.addStubVar_allTC(v, decl_list))
            self.StubMenu.add_cascade(label=decl, menu=DeclMenu)
        if len(var_dict["order"]):
            self.StubMenu.add_command(label="all", command=lambda v="all": self.addStubVar_allTC(v, all_var_list))
            return self.StubMenu
        else:
            return None
            
    def addStubVar(self, var, var_list):
        
        if var == "all":
            for var_f in var_list:
                self.context.addVariable(self.var_desc_dict[var_f])
        else:
            vd = self.var_desc_dict.get(var, None)
            if vd==None:
                messagebox.showinfo(title="Stub-Variable", message="Not Found: {var_}".format(var_=var), icon="error")
                return
            self.context.addVariableToAllTC(vd)
            
    def addStubVar_allTC(self, var, var_list):
        
        if var == "all":
            for var_f in var_list:
                self.context.addVariableToAllTC(self.var_desc_dict[var_f])
        else:
            vd = self.var_desc_dict.get(var, None)
            if vd==None:
                messagebox.showinfo(title="Stub-Variable", message="Not Found: {var_}".format(var_=var), icon="error")
                return
            self.context.addVariableToAllTC(vd)
            
    def execute(self):
        self.context.execute(self.tree_item)
        
    
    def popup(self, event):
        column = self.context.identify_column(event.x)
        item = self.context.identify_row(event.y)  
        region = self.context.identify_region(event.x, event.y)
        print(region, column, item)
        show_menu = False
        self.aMenu = Menu(None, tearoff=0)
        
        if len(item):
            self.context.focus(item)
            self.context.selection_set(item)
            self.tree_item = item  
            
        self.item_split = item.split("__", 2)
        if region == 'tree':
            if item.isdigit():
                stub_menu = self.getStubVarMenu(self.calle_stubvar_dict, self.stub_var_list)
                arg_menu = self.getStubVarMenu(self.prm_arg_dict, self.arg_var_list)
                if not stub_menu == None:
                    self.aMenu.add_cascade(label="Add StubVariables", menu=stub_menu)
                if not arg_menu == None:
                    self.aMenu.add_cascade(label="Add ArgVariables", menu=arg_menu)
                self.aMenu.add_separator()
                self.aMenu.add_command(label='Copy', command=self.copyTC)
                if self.target:
                    self.aMenu.add_command(label='Execute', command=self.execute)
                self.aMenu.add_command(label='Delete', command=self.deleteTC)
                
                show_menu = True
            elif len(self.item_split) == 3:
                if self.item_split[-1].startswith(tree_const.tc_iset) and not self.item_split[-1] == tree_const.tc_iset:
                    self.aMenu.add_command(label='Toggle', command=self.toggleSettingsItem)
                elif self.item_split[-1] == tree_const.tc_iset:
                    self.aMenu.add_command(label='Apply', command=self.applyIDStringsForItem)
                else:
                    self.aMenu.add_command(label='Delete', command=self.deleteItem)
                show_menu = True
            elif len(self.item_split) == 2:
                if (self.item_split[-1] in tree_const.tc_key_tuple[:3]):
                    self.aMenu.add_command(label='Delete', command=self.deleteItem)
                    show_menu = True
                else:
                    print(self.item_split)
            else:
                print(self.item_split)

            
        if region == 'nothing':
            stub_menu = self.getStubVarMenu_allTC(self.calle_stubvar_dict, self.stub_var_list)
            arg_menu = self.getStubVarMenu_allTC(self.prm_arg_dict, self.arg_var_list)
            if not stub_menu == None:
                self.aMenu.add_cascade(label="Add StubVariables", menu=stub_menu)
            if not arg_menu == None:
                self.aMenu.add_cascade(label="Add ArgVariables", menu=arg_menu)
            self.aMenu.add_separator()
            self.aMenu.add_command(label='New TestCase', command=self.createNewTC)
            show_menu = True  
            
        if show_menu:
            self.aMenu.post(event.x_root, event.y_root)               


class MainMenu(Menu):

    argvar_check = None

    def __init__(self, context):
        super().__init__(context.context)
        self.context = context
        self.target = context.target
        
        MainMenu.argvar_check = BooleanVar()
        MainMenu.argvar_check.set(True)
        
        self.std_reporting = BooleanVar()
        self.std_reporting.set(False)
        self.pdf_reporting = BooleanVar()
        self.pdf_reporting.set(False)
        
        self.setTargetReporting()
        
        self.menu_file = Menu(self)
        #self.menu_edit = Menu(self)
        self.menu_config = Menu(self)
        self.menu_reporting = Menu(self)
        self.add_cascade(menu=self.menu_file, label="File")
        #self.add_cascade(menu=self.menu_edit, label="Edit")
        self.add_cascade(menu=self.menu_config, label="Config")
        
        self.menu_file.add_command(label="Open", command=self.openFile)
        self.menu_config.add_checkbutton(label="Insert ArgVars", variable=MainMenu.argvar_check)
        self.menu_config.add_cascade(menu=self.menu_reporting, label="Reporting")
        
        self.menu_reporting.add_checkbutton(label="Standard", variable=self.std_reporting, command=self.setTargetReporting)
        self.menu_reporting.add_checkbutton(label="PDF", variable=self.pdf_reporting, command=self.setTargetReporting)
        
    def setTargetReporting(self):
        if self.target:
            self.target.reporting = self.std_reporting.get()
            self.target.pdf_reporting = self.pdf_reporting.get()
        
    def openFile(self):
        self.filepath = filedialog.askopenfilename(filetypes=[("all", "*.*")])
        if not len(self.filepath):
            return 
        file_loader = FileLoader(self.filepath)
        if not file_loader.valid:
            print("Not valid: "+ file_loader.abs_path)
            messagebox.showinfo(title= "File-Error", message="File was not valid!", icon="error")
            return
        file_loader.analyseFile()
        if not file_loader.test_desc == None:
            if file_loader.loaded_type == FileLoader.TSEQ:
                self.context.tseqs_nb.addTestSequenceDesc(file_loader)
            elif file_loader.loaded_type == FileLoader.TSUITE:
                self.context.navi_nb.addTestSuiteDesc(file_loader)
        else:
            messagebox.showinfo(title= "File-Error", message="File was not interpreted correctly!", icon="error")
        

class RightClick_Explorer(RightClick_base):
    def __init__(self, context):
        super().__init__(context)
        
        self.tree_item = ''

    def delete(self):
        if self.tree_item:
            self.context.detach(self.tree_item)


    def popup(self, event):
        column = self.context.identify_column(event.x)
        item = self.context.identify_row(event.y) 
        region = self.context.identify_region(event.x, event.y)
        print(region, column, item)
        if region == 'tree':
            if len(item):
                self.context.focus(item)
                self.context.selection_set(item)
                self.tree_item = item
            self.aMenu = Menu(None, tearoff=0)
            self.aMenu.add_command(label='Delete', command=self.delete)
            self.aMenu.post(event.x_root, event.y_root)

        else:
            return

            
class RightClick_SymbolBrowser(SoCoExtension):
    def __init__(self, context, tseqs_notebook):
        super().__init__(context)
        self.tseqs_nb = tseqs_notebook
        
        self.tree_item = ''
    
    def createTS(self):
        self.tseqs_nb.newTestSequenceFromFuncDesc(self.soco_A.getFuncDescOfUnit(self.parent_split[1], self.tree_item))
        self.tseqs_nb.setCurrentTabIndexUnsaved()
        
    def addTS(self):
        self.tseqs_nb.addTestSequenceFromFuncDesc(self.soco_A.getFuncDescOfUnit(self.parent_split[1], self.tree_item))
        self.tseqs_nb.setCurrentTabIndexUnsaved()      
    
    
    def addToTC(self):
        if self.tseqs_nb.getCurrentTab().addVariable(self.soco_A.getVariableDescOfUnit(self.parent_split[1], self.tree_item)):
            self.tseqs_nb.setCurrentTabIndexUnsaved()
            return
        messagebox.showerror("Error", "No TestCase selected!")
        
    def addToAllTc(self):
        self.tseqs_nb.getCurrentTab().addVariableToAllTC(self.soco_A.getVariableDescOfUnit(self.parent_split[1], self.tree_item))
        self.tseqs_nb.setCurrentTabIndexUnsaved()

        
    def popup(self, event):
        column = self.context.identify_column(event.x)
        item = self.context.identify_row(event.y)
        region = self.context.identify_region(event.x, event.y)
        print(region, column, item)

        if len(item):
            self.context.focus(item)
            self.context.selection_set(item)
            self.tree_item = item        
 
        if region == 'tree':
            self.aMenu = Menu(None, tearoff=0)
            self.parent_split = self.context.parent(item).split("__")
            print(self.parent_split)
            if self.parent_split[0].startswith("functions") :
                self.aMenu.add_command(label='New Sequence', command=self.createTS)
                self.aMenu.add_command(label='Add Sequence', command=self.addTS)
            if self.parent_split[0].startswith("variables") :
                self.aMenu.add_command(label='Add to selected', command=self.addToTC)
                self.aMenu.add_command(label='Add to all', command=self.addToAllTc)
            self.aMenu.post(event.x_root, event.y_root)
        else:
            return  


class RightClick_Notebook():   
    def __init__(self, context):
        self.context = context
        self.target = context.target
        self.tab_index = ''
        
                
class RightClick_TSeqNotebook(RightClick_Notebook):
    def __init__(self, context):
        super().__init__(context)
        self.aMenu = None
        
        
    def remove(self):
        self.context.forget(self.tab_index)
        self.context.context.setCurrentTabIndexUnsaved()
        if self.aMenu:
            self.aMenu.destroy()
        return
        
        
    def popup(self, event):
    
        comp = self.context.identify(event.x, event.y)

        if comp == "label":
            self.tab_index = self.context.index("@"+str(event.x)+","+str(event.y))
            self.context.select(self.tab_index)
            self.aMenu = Menu(None, tearoff=0)
            self.aMenu.add_command(label='Remove', command=self.remove)
            self.aMenu.post(event.x_root, event.y_root)
        else:
            return


class RightClick_TSequencesNoteBook(RightClick_Notebook):
    def __init__(self, context):
        super().__init__(context)
        
    def save(self):
        TS_Frame = self.context.getWidgetFromTabIndex(self.tab_index)
        TS_Frame.save()
        #self.context.setTabIndexSaved(self.tab_index)
        return
        
    def saveAs(self):
        TS_Frame = self.context.getWidgetFromTabIndex(self.tab_index)
        TS_Frame.saveAs()
        #self.context.setTabIndexSaved(self.tab_index)
        return
    
    def close(self):
        if not self.context.saved:
            if messagebox.askyesno(icon='question', message='Save before closing?', title='Save'):
                TS_Frame = self.context.getWidgetFromTabIndex(self.tab_index)
                TS_Frame.save()
        self.context.forget(self.tab_index)
        return
    
    def execute(self):
        TS_Frame = self.context.getWidgetFromTabIndex(self.tab_index)
        TS_Frame.execute()
        
    def popup(self, event):
    
        comp = self.context.identify(event.x, event.y)

        if comp == "label":
            self.tab_index = self.context.index("@"+str(event.x)+","+str(event.y))
            self.context.select(self.tab_index)
            self.aMenu = Menu(None, tearoff=0)
            self.aMenu.add_command(label='Save', command=self.save)
            self.aMenu.add_command(label='SaveAs', command=self.saveAs)
            if self.context.target:
                self.aMenu.add_command(label='Execute', command=self.execute)
            self.aMenu.add_separator()
            self.aMenu.add_command(label='Close', command=self.close)
            self.aMenu.post(event.x_root, event.y_root)
        else:
            return        
        
    