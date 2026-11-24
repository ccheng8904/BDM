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
Description      : wraps some tree widgets for customization
"""

import os
import os.path

from tkinter import *
from tkinter import ttk

from .widgets import Frame_SelectDirPath, Frame_SelectFilePath, LabelEntry, SocoSearchButton, FilterEntryCheck, LabelEntry_savedIndicated
from .tree_widgets import TestCaseTreeView, VScrolledTreeview
from .menues import RightClick_Explorer, RightClick_SymbolBrowser, MainMenu
from .soco_adapter import SocoAdapter
from ..globals.constants import tree_const

class TabFrame_Base(ttk.Frame):
    def __init__(self, context, **kw):
        super().__init__(context, **kw)
        self.context = context
        self.target = context.target

class Frame_Base(ttk.Frame):
    def __init__(self, context, **kw):
        super().__init__(context.contentFrame, **kw)
        self.context = context
        self.target = context.target          

class TabFrame_TestSequence(TabFrame_Base):
    def __init__(self, context, **kw):
        super().__init__(context, **kw)
        
        self.TestSequence = LabelEntry_savedIndicated(self, "Sequence: ", 60)
        self.Function     = LabelEntry(self, "Function: ", 60)
        self.Function.Entry["state"] = "readonly"
        self.Signature     = LabelEntry(self, "Signature: ", 60)
        self.Signature.Entry["state"] = "readonly"
        self.Description  = LabelEntry_savedIndicated(self, "Description: ", 60)
        
        self.function = ""
        
        self.TestSequence.grid(column=0, row=0, columnspan=2, sticky=(W))
        self.Function.grid(column=0, row=1, columnspan=2, sticky=(W))
        self.Signature.grid(column=0, row=2, columnspan=2, sticky=(W))
        self.Description.grid(column=0, row=3, columnspan=2, sticky=(W))
        
        self.TestCase_Frame = TabFrame_TestCase(self)
        self.TestCase_Frame.grid(column=0, row=4, sticky=(N, S, E, W), padx=0, pady=0)
        
        self.columnconfigure(0, weight=1)
        self.rowconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)
        self.rowconfigure(2, weight=1)
        self.rowconfigure(3, weight=1)
        self.rowconfigure(4, weight=30)
        
    def addFuncDesc(self, func_desc):
        self.func_desc = func_desc
        self.function = self.func_desc.function_name
        self.TestSequence.entry_var.set("TS_"+self.function)
        self.Function.entry_var.set(self.function)
        self.Signature.entry_var.set(self.func_desc.getSignature(funcname_add="#"))
        self.TestCase_Frame.tc_info_tree.setFuncDesc(self.func_desc)
        self.TestCase_Frame.tc_info_tree.addTC()
        
    def addVarDesc(self, var_desc):
        return self.TestCase_Frame.tc_info_tree.addVariable(var_desc)
      
    def addVariableToAllTC(self, var_desc):
        self.TestCase_Frame.tc_info_tree.addVariableToAllTC(var_desc)
        
    def addFromSequenceDesc(self, sequence):
        if sequence.function_desc == None:
            sequence.function_desc = SocoAdapter.Global.getFuncDesc(sequence.function)
        self.func_desc = sequence.function_desc
        self.function = sequence.function
        self.TestSequence.entry_var.set(sequence.identifier)
        self.Function.entry_var.set(self.function)
        self.Description.entry_var.set(sequence.description)
        if not self.func_desc == None:
            self.Signature.entry_var.set(self.func_desc.getSignature(funcname_add="#"))
            self.TestCase_Frame.tc_info_tree.setFuncDesc(self.func_desc)
        self.TestCase_Frame.tc_info_tree.addFromSequence(sequence)
                 
    def getYAMLDict(self, no_tc=False):
        ts_dict = {} 
        ts_dict["function"] = self.Function.entry_var.get()
        ts_dict["description"] = self.Description.entry_var.get()
        if no_tc:
            ts_dict["testcases"] = {}
        else:
            ts_dict["testcases"] = self.TestCase_Frame.getTestCasesAsDict()
        return ts_dict
        
        

class TabFrame_TestCase(TabFrame_Base):
    def __init__(self, context, **kw):
        super().__init__(context, **kw)
        
        self.tc_info_tree = TestCaseTreeView(self, height=3, selectmode="browse")
        
        self.tc_info_tree['columns'] = tree_const.tc_header_tuple
        self.tc_info_tree.heading('#0', text='TestCase')
        for index in range(0, len(tree_const.tc_header_tuple)):
            self.tc_info_tree.heading(tree_const.tc_header_tuple[index], text=tree_const.tc_heading_tuple[index])
        
        self.tc_info_tree.column(tree_const.col_dc, width=2)
        #self.tc_info_tree.column(tree_const.col_op, width=5)

        self.tc_info_tree.grid(column=0, row=0, sticky=(N,S, E, W))
        self.tc_info_tree.gridScrollBar(column=1, row=0, sticky=(N,S,W))
        
        self.columnconfigure(0, weight=50)
        self.rowconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        
    def getTestCasesAsDict(self):
        return self.tc_info_tree.getTestCasesAsDict()
        
        
class Frame_Soco(TabFrame_Base):
    def __init__(self, context, **kw):
        super().__init__(context, **kw)
        self.soco_adapter = None
        
        self.Settings_Frame = ttk.Frame(self)
        SymbolSearch = SocoSearchButton(self, "Search", 20)
        SymbolSearch.grid(column=1, row=0, sticky=(W), pady=2)
        SymbolFilter = FilterEntryCheck(self, "Filter", 20)
        SymbolFilter.grid(column=3, row=0, sticky=(W), pady=2)
        self.Settings_Frame.grid(column=0, row=1, columnspan=2, sticky=(W), pady=2)
        
        self.socotree = VScrolledTreeview(self, selectmode="browse")
        self.socotree_rclick = RightClick_SymbolBrowser(self.socotree, self.context.context.tseqs_nb)
        self.socotree.bind('<3>', self.socotree_rclick.popup)
        self.socotree['columns'] = tree_const.soco_header_tuple
        self.socotree.heading('#0', text='Units')
        self.socotree.heading(tree_const.col_type, text='Type')
        self.socotree.heading(tree_const.col_address, text='Address')
        self.socotree.heading(tree_const.col_linkage, text='Linkage')
        self.socotree.heading(tree_const.col_qualifier, text='Qualifier')
        
        self.socotree.grid(column=0, row=2, columnspan=2, sticky=(N, S, E, W))
        self.socotree.gridScrollBar(column=2, row=2, sticky=(N, S, W))
        
        Load_Frame = ttk.Frame(self)
        self.pstd_frame = Frame_SelectFilePath(Load_Frame, "PSTD: ")
        self.pstd_frame.grid(column=0, row=0, sticky=(S, E, W))
        self.psyd_frame = Frame_SelectFilePath(Load_Frame, "PSYD: ")
        self.psyd_frame.grid(column=1, row=0, sticky=(S, E, W))
        self.soco_loadbutton = Button(Load_Frame, text="Load", command=self.loadSoco)
        self.soco_loadbutton.grid(column=2, row=0, sticky=(W), pady=2)
        Load_Frame.grid(column=0, row=0, columnspan=2, sticky=(W), pady=2)
        
        self.columnconfigure(0, weight=28)
        self.columnconfigure(1, weight=1)
        self.rowconfigure(0, weight=1)
        self.rowconfigure(1, weight=1)
        self.rowconfigure(2, weight=8)
        
        if not self.target == None:
            self.soco_adapter = SocoAdapter(soco=self.target.source_description, treeview=self.socotree)
            self.pstd_frame.setFilePath(self.target.source_description.pstd_file)
            self.psyd_frame.setFilePath(self.target.source_description.psyd_file)
            self.socotree_rclick.setSocoA(self.soco_adapter)           

    def loadSoco(self):
    
        if not os.path.isfile(self.pstd_frame.filepath):
            return     
        if not os.path.isfile(self.psyd_frame.filepath):
            return 
        
        self.soco_adapter = SocoAdapter(self.pstd_frame.filepath, self.psyd_frame.filepath, self.socotree)
        self.socotree_rclick.setSocoA(self.soco_adapter)

class Frame_Explorer(Frame_Base):
    def __init__(self, context, **kw):
        super().__init__(context, **kw) 

        self.foldertree = VScrolledTreeview(self, selectmode="browse")
        self.foldertree_rclick = RightClick_Explorer(self.foldertree)
        self.foldertree.bind('<3>', self.foldertree_rclick.popup)
        self.foldertree.heading('#0', text='Workspace')
        
        self.foldertree.insert('', 'end', 'folders', text='Folders')
        
        self.foldertree.grid(column=0, row=0, sticky=(N, S, E, W))
        self.foldertree.gridScrollBar(column=1, row=0, sticky=(N, S, W))
        
        FrameSetFolder = Frame_SelectDirPath(self, "root: ")
        FrameSetFolder.grid(column=0, row=1, sticky=(N, E, W))
        
        self.columnconfigure(0, weight=9)
        self.rowconfigure(0, weight=1)
        self.columnconfigure(1, weight=1)
        self.rowconfigure(0, weight=1)    
        