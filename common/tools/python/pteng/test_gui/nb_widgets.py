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
Description      : wraps some basic widgets for customization
"""

import yaml

from pteng.public_layer import *

from tkinter import ttk
from tkinter import filedialog
from tkinter import messagebox

from ..globals.folder_utils import split_filepath
from ..globals.file_loader import FileLoader, TestSequences, TestSuite
from ..globals.constants import tree_const

from .menues import RightClick_TSequencesNoteBook, RightClick_TSeqNotebook
from .tree_widgets import TestSuiteTreeView
from .frame_widgets import Frame_Soco, TabFrame_TestSequence

import threading

class TargetThread(threading.Thread):
    def __init__(self, test_desc):
        threading.Thread.__init__(self)
        self.test_desc = test_desc
        
    def run(self):
        default_comconfig = r"C:/Data/Data/CI-Jobs/GIT_Framework/out/PEDFW/Intel_gccIntel/testClient_ped_fw__utility/testClient_ped_fw__utility_ComConfig.xml"
        pstd_filepath = r"C:/Data/Data/CI-Jobs/GIT_Framework/out/PEDFW/Intel_gccIntel/testClient_ped_fw__utility/bin/testClient_ped_fw__utility_pstd.xml" 
        psyd_filepath = r"C:/Data/Data/CI-Jobs/GIT_Framework/out/PEDFW/Intel_gccIntel/testClient_ped_fw__utility/bin/testClient_ped_fw__utility_psyd.xml"
        cvrli_filepath = r"C:/Data/Data/CI-Jobs/GIT_Framework/common/prog/ped_fw/utility/Intel/Stubs/COVR_perLineInfo.json"

        print("The used instance of TargetDscr() is named \'target1\', so you can use this instance for further commands.")
        target1 = TargetDscr()
        print("Started import of PSTD and PSYD.")
        target1.import_pstd(pstd_filepath)
        target1.append_psyd(psyd_filepath)
        target1.set_comconfig_from_file(default_comconfig)
        target1.set_cvrli(cvrli_filepath)
        target1.connect()        
        print("Starting TSeq")
        target1.target.execute_test_by_descriptor(self.test_desc)
        print(self.test_desc.getDict())
        target1.disconnect()
        

class NoteBook_base(ttk.Notebook):
    def __init__(self, context, **kw):
        super().__init__(context, **kw)
        self.saved=True
   
    def getWidgetFromTabIndex(self, tab_index):
        self.tab_list = self.tabs()
        return self.children[self.tab_list[tab_index].rsplit(".",1)[1]] 
        
    def getCurrentTab(self):
        self.tab_list = self.tabs()
        return self.children[self.tab_list[self.index('current')].rsplit(".",1)[1]] 
        
    def changeTabTextOfCurrent(self, new_text):
        tab_index = self.index('current')
        text = self.tab(self.tab_list[tab_index])["text"]
        if text.endswith("*"):
            self.tab(self.tab_list[tab_index], text=new_text+"*")
        else:
            self.tab(self.tab_list[tab_index], text=new_text)
    
    def setTabIndexSaved(self, tab_index):
        self.saved=True
        self.tab_list = self.tabs()
        text = self.tab(self.tab_list[tab_index])["text"]
        if text.endswith("*"):
            self.tab(self.tab_list[tab_index], text=text[:-1])
            
    def setTabIndexUnsaved(self, tab_index):
        self.saved=False
        self.tab_list = self.tabs()
        text = self.tab(self.tab_list[tab_index])["text"]
        if not text.endswith("*"):
            self.tab(self.tab_list[tab_index], text=text+"*")
            
    def setCurrentTabIndexUnsaved(self):
        self.tab_list = self.tabs()
        try:
            self.setTabIndexUnsaved(self.index('current'))
        except:
            print(self.tab_list)
        
    def setCurrentTabIndexSaved(self):
        self.tab_list = self.tabs()
        try:
            self.setTabIndexSaved(self.index('current'))
        except:
            print(self.tab_list)
                
    def forgetAllTabs(self):
        for tid in self.tabs():
            self.forget(tid)
            
class TabNoteBook_base(NoteBook_base):
    def __init__(self, context, **kw):
        super().__init__(context, **kw)
        self.context = context
        self.target = context.target
        
class MainNoteBook_base(NoteBook_base):
    def __init__(self, context, **kw):
        super().__init__(context.contentFrame, **kw)
        self.context = context
        self.target = context.target
        
class Notebook_Navi(MainNoteBook_base):
    def __init__(self, context, **kw):
        super().__init__(context, **kw)
        
        self.add(Frame_Soco(self), state="normal", text="SymbolBrowser") 
        self.tsuite_tree = TestSuiteTreeView(self)
        self.tsuite_tree['columns'] = tree_const.tsuite_header_tuple
        self.tsuite_tree.heading('#0', text='TestSuites')
        self.tsuite_tree.heading(tree_const.col_id, text='Identifier')
        self.tsuite_tree.heading(tree_const.col_loc, text='Location')
        self.tsuite_tree.heading(tree_const.col_valid, text='Validation')
        self.tsuite_tree.column('#0', width=5)
        self.tsuite_tree.column(tree_const.col_id, width=10)
        self.add(self.tsuite_tree, state="normal", text="TestSuites") 
        
    def addTestSuiteDesc(self, fl):
        testsuite_tab = self.getWidgetFromTabIndex(1)
        self.select(1)
        testsuite_tab.addFromTestDesc(fl)
        
    def addTestSequenceDescToTSNB(self, path):
        file_loader = FileLoader(path)
        file_loader.analyseFile()
        if file_loader.loaded_type == FileLoader.TSEQ:
            self.context.tseqs_nb.addTestSequenceDesc(file_loader)
        else:
            messagebox.showinfo(title= "File-Error", message="File was not interpreted correctly!", icon="error")


class NoteBook_TSeq(TabNoteBook_base):
    def __init__(self, context, **kw):
        super().__init__(context, **kw)
        self.tsnotebook_rclick = RightClick_TSeqNotebook(self)
        self.bind('<3>', self.tsnotebook_rclick.popup)
        
        self.unit_name = ""
        self.save_path = ""
    
    def getYamlSavePath(self):
        self.save_path = filedialog.asksaveasfilename(defaultextension='.yaml', initialfile=self.unit_name, filetypes=[("all", "*.*")])
        self.context.changeTabTextOfCurrent(split_filepath(self.save_path)[1])
    
    def save(self):
        if not len(self.save_path):
            self.getYamlSavePath()

        self.saveYML()
            
    def saveAs(self):
        self.getYamlSavePath()
        self.saveYML()

    def saveYML(self):
        print("SavePath:"+self.save_path)
        if len(self.save_path):
            with open(self.save_path, "w+") as f:
                yaml.dump(self.getTSeqYAMLDict(), f)
                self.context.setCurrentTabIndexSaved()
                
        
    def execute(self):
        tseq_dict = self.getTSeqYAMLDict()
        #print(tseq_dict)
        tseq = TestSequences.getFromDict(tseq_dict)
        fl = FileLoader.getFromInstance(self.save_path, tseq, FileLoader.TSEQ)
        
        self.target.execute_unknown(fl)
        self.forgetAllTabs()
        #print(fl.test_desc.getDict())
        self.addTestSequenceDesc(fl)
        
    def getTSeqYAMLDict(self):
        all_ts_dict = {}
        #ToDo NiEh: normally check if ts-name exist in all_ts_dict
        for ts_frame in list(self.children.values()):
            #for safety, removed menu parents (that caused menu attachment to self.children)
            #normally this check is not necessary anymore
            if isinstance(ts_frame, TabFrame_TestSequence):
                all_ts_dict[ts_frame.TestSequence.entry_var.get()] = ts_frame.getYAMLDict()   
        return {"testsequences": all_ts_dict}
    
    def addTestSequenceFromFuncDesc(self, func_desc):
        self.unit_name = func_desc.define_unit.unit_name
        NewFrame = TabFrame_TestSequence(self)
        NewFrame.addFuncDesc(func_desc)
        self.add(NewFrame, state="normal", text=func_desc.function_name)
        

    def addVariable(self, var_desc):
        self.context.setCurrentTabIndexUnsaved()
        act_Frame = self.getCurrentTab()
        return act_Frame.addVarDesc(var_desc)
    
    def addVariableToAllTC(self, var_desc):
        self.context.setCurrentTabIndexUnsaved()
        act_Frame = self.getCurrentTab()
        return act_Frame.addVariableToAllTC(var_desc)
        
    def addTestSequenceDesc(self, fl):
        self.save_path = fl.abs_path
        for seq in fl.test_desc.test_sequences_list:
            NewFrame = TabFrame_TestSequence(self)
            NewFrame.addFromSequenceDesc(seq)
            self.add(NewFrame, state="normal", text=seq.function)             
            
class NoteBook_TSequences(MainNoteBook_base):
    def __init__(self, context, **kw):
        super().__init__(context, **kw)
        self.context = context
        self.seqsnotebook_rclick = RightClick_TSequencesNoteBook(self)
        self.bind('<3>', self.seqsnotebook_rclick.popup)
        
    def saveAll(self):
        for tseqs_frame in list(self.children.values()):
            tseqs_frame.save()

    def addTestSequenceDesc(self, fl):
        NewFrame = NoteBook_TSeq(self)
        NewFrame.addTestSequenceDesc(fl)
        self.add(NewFrame, state="normal", text=fl.basename) 
        
    def addTestSuiteDesc(self, fileloader):
        for fl in fileloader.test_desc.file_loader_list:
            self.addTestSequenceDesc(fl)
            
    def newTestSequenceFromFuncDesc(self, func_desc):
        NewTabFrame = NoteBook_TSeq(self)
        self.add(NewTabFrame, state="normal", text=func_desc.define_unit.unit_name) 
        self.select(self.tabs()[self.index('end')-1])
        NewTabFrame.addTestSequenceFromFuncDesc(func_desc)
        
    def addTestSequenceFromFuncDesc(self, func_desc):
        CurrentTab = self.getCurrentTab()
        CurrentTab.addTestSequenceFromFuncDesc(func_desc)
        self.setCurrentTabIndexUnsaved()
        
