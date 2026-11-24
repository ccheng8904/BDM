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

import yaml

from tkinter import ttk
from tkinter import filedialog
from tkinter import messagebox

from .soco_adapter import SocoAdapter

from .menues import RightClick_TestSuiteTree, RightClick_TCTree_SocoAdapter
from ..globals.file_loader import TestCaseDesc, TestSequenceDesc, TestSequences, TestSuite, checkIOStringForList, FileLoader
from ..globals.folder_utils import PathFormer
from ..globals.constants import tree_const, exec_const

class VScrolledTreeview(ttk.Treeview):

    end = 'end'

    def __init__(self, master, **kw):
        super().__init__(master, **kw)
        self.context = master
        self.target = master.target
        self.vsb = ttk.Scrollbar(master, orient="vertical", command=self.yview)
        
        self.configure(yscrollcommand=self.vsb.set)
        self.saved = True
        
    def gridScrollBar(self, **kw):
        self.vsb.grid(**kw)
        
    def setSaved(self, item):
        self.saved=True
        text = self.item(item)["text"]
        if text.endswith("*"):
            self.item(item, text=text[:-1])
        
    def setUnsaved(self, item):
        self.saved=False
        text = self.item(item)["text"]
        if not text.endswith("*"):
            self.item(item, text=text+"*")

class TestSuiteTreeView(VScrolledTreeview):
    def __init__(self, master, **kw):
        super().__init__(master, **kw)
        self.bind("<1>", self.edit)
        
        self.rightclick_menu = RightClick_TestSuiteTree(self)
        self.bind('<3>', self.rightclick_menu.popup)
        
        self.last_added_ts = 0
        self.loaded_fileloader = {}

    #parent split is: [tc_id, one of tc_key_tuple, ]
    def isEntryField(self, item_split, col):
        if len(item_split) == 1:
            used_split = 0
        else:
            used_split = item_split[1]
        if tree_const.tsuite_index_column_map[col] in tree_const.tsuite_entry_cols_dict[used_split]:
            return True
        else:
            return False

    def edit(self, event):
        region = self.identify_region(event.x, event.y)
        if region == 'cell':

            def enter(event):
                act_entry = self.set(item, column)
                if not act_entry == entry.get():
                    self.setUnsaved(self.item_split[0])
                    self.set(item, column, entry.get())
                entry.destroy()
                

            column = self.identify_column(event.x)  
            item = self.identify_row(event.y)  
            self.item_split = item.split("__", 2)
            if not self.isEntryField(self.item_split, column) or len(self.get_children(item)):
                return
                
            x, y, width, height = self.bbox(item, column) 
            value = self.set(item, column)
            entry = None

            entry = ttk.Entry(self) 
            entry.insert(0, value) 
            entry.bind('<Return>', enter)
            entry.bind('<FocusOut>', enter)    

            entry.place(x=x, y=y, width=width, height=height,
                        anchor='nw')  
            entry.focus_set()

        elif region == 'heading': 
            return
        elif region == 'nothing': 
            return
        else:
            return
    
    #refactoren
    def addTestSequenceDescToTSNB(self, seq_item):
        seq_path = self.set(seq_item, tree_const.col_loc)
        suite_path = self.set(seq_item.split("__", 1)[0], tree_const.col_loc)
        path_former = PathFormer.fromSeqOfSuite(seq_path, suite_path)
        if path_former == None:
            messagebox.showinfo(title="File-Error", message="The Sequence path is corrupt!", icon="error")
        else:
            self.context.addTestSequenceDescToTSNB(path_former.abs_path)
            
    def newTSuite(self):
        #fileloader mit tsuite initialisieren
        self.getYamlSavePath()
        if len(self.save_path):
            with open(self.save_path, "w+") as f:
                yaml.dump({}, f)
            path_former = PathFormer(self.save_path)
            parent_txt = path_former.basename
            self.insert("", VScrolledTreeview.end, str(self.last_added_ts), text=parent_txt, values=("", path_former.abs_path))
            self.last_added_ts += 1
            
    def addFromTestDesc(self, fileloader):
        parent_txt = fileloader.basename
        self.loaded_fileloader[fileloader.abs_path]=fileloader
        parent_id = self.insert("", VScrolledTreeview.end, str(self.last_added_ts), text=parent_txt, value=("", fileloader.abs_path))
        self.last_added_ts += 1
        for index in range(0, len(fileloader.test_desc.file_loader_list)):
            self.insert(parent_id, VScrolledTreeview.end, parent_id+"__"+tree_const.tsuite_tseq+"__"+str(index), text="testsequence", values=fileloader.test_desc.getSeqDataTuple(index))
    
    def addTSeq(self, tsuite):
        self.getYamlOpenPath()
        if self.open_path:
            path_former = PathFormer(self.open_path)
            data = (path_former.basename, path_former.getFromTestRootPath(self.set(tsuite, column=tree_const.col_loc)))
            self.insert(tsuite, VScrolledTreeview.end, tsuite+"__"+tree_const.tsuite_tseq+"__"+path_former.basename, text="testsequence", values=data)        
            self.setUnsaved(tsuite)
            
    def removeTSeq(self, item):
        self.delete(item)
        self.setUnsaved(item.split("__", 1)[0])
    
    def getTestSuiteDict(self, tsuite):
        tseq_dict = {}
        for tseq in self.get_children(tsuite):
                tseq_dict[self.set(tseq, column=tree_const.col_id)] = self.set(tseq, column=tree_const.col_loc)
                
        tsuite_dict = {"testsuite": tseq_dict}
        return tsuite_dict
        
    def getYamlSavePath(self, ini_file=""):
        self.save_path = filedialog.asksaveasfilename(defaultextension='.yaml', initialfile=ini_file, filetypes=[("all", "*.*")])
        
    def getYamlOpenPath(self, ini_file=""):
        self.open_path = filedialog.askopenfilename(defaultextension='.yaml', initialfile=ini_file, filetypes=[("all", "*.*")])  
    
    def saveAll(self):
        for ts in self.get_children():
            self.save(ts)
    
    def save(self, tsuite):
        self.save_path = self.set(tsuite, tree_const.col_loc)
        if not len(self.save_path):
            self.saveAs(tsuite)
        else:
            self.saveYML(tsuite)
            
    def saveAs(self, tsuite):
        self.getYamlSavePath(tsuite)
        self.set(tsuite, tree_const.col_loc, str(self.save_path))
        self.saveYML(tsuite)
        
    def saveYML(self, tsuite):
        if len(self.save_path):
            with open(self.save_path, "w+") as f:
                yaml.dump(self.getTestSuiteDict(tsuite), f)  
            self.setSaved(tsuite)
                
    def close(self, item):
        abs_path = self.set(item, tree_const.col_loc)
        if abs_path in self.loaded_fileloader:
            del self.loaded_fileloader[abs_path]
        self.detach(item)
        
    def convertToYaml(self, item):
        abs_path = self.set(item, tree_const.col_loc)
        tsuite_fl = self.loaded_fileloader.get(abs_path, None)
        if not tsuite_fl:
            messagebox.showinfo(title="File-Error", message="Can't convert testsuite!", icon="error")
            return
        if tsuite_fl.isXML():
            self.close(item)
            for fl in tsuite_fl.test_desc.file_loader_list:
                fl.convertPath()
                fl.save()
            tsuite_fl.convertPath()
            tsuite_fl.save()
            self.addFromTestDesc(tsuite_fl)
            
    def execute(self, item):
        abs_path = self.set(item, tree_const.col_loc)
        self.execution = True
        
        fl = FileLoader(abs_path)
        fl.analyseFile()

        self.target.execute_unknown(fl)
        ts = fl.test_desc
        
        self.execution = False   

        valid_string = ts.getValidationTreeTag()
        self.item(item, tags=(valid_string, ))
        self.set(item, tree_const.col_valid, valid_string)
        for index, child in enumerate(self.get_children(item)):
            valid_string = ts.file_loader_list[index].test_desc.getValidationTreeTag()
            self.item(child, tags=(valid_string, ))
            self.set(child, tree_const.col_valid, valid_string)            

        #self.fillTCParentFromTestCaseDesc(tc_parent, test_case)
        self.tag_configure(exec_const.passed_str, background="#7CFC00")
        self.tag_configure(exec_const.failed_str, background="#FF6347")
        self.tag_configure(exec_const.exception_str, background="#C71585")
            
        
           
            
            
class TestCaseTreeView(VScrolledTreeview):
    def __init__(self, master, **kw):
        super().__init__(master, **kw)
        self.bind("<1>", self.edit)
        
        self.last_added_tc = 0
        
        self.func_desc = None
        self.added_desc = {}
        
        self.execution = False
        
    def setTseqsTabUnsaved(self):
        self.context.context.context.context.setCurrentTabIndexUnsaved()
        
    def setFuncDesc(self, func_desc):
        self.func_desc = func_desc
        self.sig_id_type_ist = self.func_desc.getSignatureIdentifierTypeList()
        self.prm_id_index_list, self.ret_id_index_list = self.func_desc.getSignatureIDIndexList()
        #needs access to self.func_desc, therefore its not in constructor
        self.rightclick_menu = RightClick_TCTree_SocoAdapter(self)
        self.bind('<3>', self.rightclick_menu.popup)
        
        
    def edit(self, event):
        region = self.identify_region(event.x, event.y)
        if region == 'cell':

            def enter(event):
                act_entry = self.set(item, column)
                if not act_entry == entry.get():
                    self.set(item, column, entry.get())
                    if not tree_const.tc_iset in item:
                        self.setTseqsTabUnsaved()
                entry.destroy()

            column = self.identify_column(event.x)  
            item = self.identify_row(event.y)  
            self.item_split = item.split("__", 2)
            print(column, item, self.item_split)
            if not (tree_const.isEntryField(self.item_split, column) or tree_const.isReadField(self.item_split, column)): #or len(self.get_children(item)):
                return
                
            x, y, width, height = self.bbox(item, column) 
            value = self.set(item, column)
            entry = None
            #ToDo NiEh: check if entryfield and not settings or prm/ret/var
            got_col = tree_const.tc_index_column_map[column]
            if got_col == tree_const.col_op:
                entry = ttk.Combobox(self, state="readonly", values=tree_const.all_ops)
                entry.bind('<<ComboboxSelected>>', enter) 
                entry.set(value)
            elif got_col == tree_const.col_rep:
                entry = ttk.Combobox(self, state="readonly", values=tree_const.combo_reps)
                entry.bind('<<ComboboxSelected>>', enter) 
                entry.set(value)
            elif got_col in tree_const.tc_read_cols:
                entry = ttk.Entry(self) 
                entry.insert(0, value) 
                entry.config(state="readonly")
                entry.bind('<Return>', enter) 
            else:
                entry = ttk.Entry(self) 
                entry.insert(0, value) 
                entry.bind('<Return>', enter)     
                    
            entry.bind('<FocusOut>', enter)    
            entry.place(x=x, y=y, width=width, height=height, anchor='nw')  
            entry.focus_set()
            #destroy entry on scroll or change of window-size

        elif region == 'heading': 
            return
        elif region == 'nothing': 
            return
        else:
            return 

    def addVariable(self, var_desc, values=None, tc_id=None):
        selected = self.selection()
        if len(selected) == 0 and tc_id == None:
            return False
        else:
            if tc_id == None:
                tc_id = selected[0].split("__", 1)[0]
            else:
                pass
            id_type_list = var_desc.getIdentifierTypeList()
            id_settings_list = var_desc.getIdentifierIndexList()
            return self.addVarList_to_tc(tc_id, id_type_list, id_settings_list, values=values)
            
    def addVariableToAllTC(self, var_desc, values=None):
        id_type_list = var_desc.getIdentifierTypeList()
        id_settings_list = var_desc.getIdentifierIndexList()
        for tc_id in self.get_children():
            self.addVarList_to_tc(tc_id, id_type_list, id_settings_list, values=values)
            
    def addVarList_to_tc(self, tc_id, var_id_type_list, var_index_settings, values=None):
        tc_vars_id = tc_id+"__"+tree_const.tc_vars
        tc_vars_id_info_set = None
        if values == None:
            values = tree_const.var_value_std
        try:
            self.insert(tc_id, VScrolledTreeview.end, tc_id+'__'+tree_const.tc_vars, text='Variables')
        except:
            pass
        try:
            tc_vars_id_info_set = self.insert(tc_vars_id, VScrolledTreeview.end, tc_vars_id+"__"+tree_const.tc_iset, text="Settings", values=())
        except:
            pass
        
        if tc_vars_id_info_set == None:
            tc_vars_id_info_set = tc_vars_id+"__"+tree_const.tc_iset
           
        self.addSettingsRecursive(tc_vars_id_info_set, var_index_settings)
        for id_type_tuple in var_id_type_list:
            self.insert(tc_vars_id, VScrolledTreeview.end, tc_vars_id+"__"+id_type_tuple[0], text=id_type_tuple[0], values=values)
            self.added_desc[id_type_tuple[0]] = id_type_tuple[1]
        return True
    
    def deleteItem(self, item):
        parent = self.parent(item)
        self.delete(item)
        if len(self.get_children(parent)) == 0:
            self.delete(parent)
        self.setTseqsTabUnsaved()
            
    def toggleSettingsItem(self, item):
        value = self.set(item, column=tree_const.col_in)
        new_value = tree_const.val_pos
        if value == tree_const.val_pos:
            new_value = tree_const.val_neg
        self.set(item, tree_const.col_in, new_value)
            
    def addTC(self):
        self.last_added_tc += 1
        self.last_added_tc_str = str(self.last_added_tc)
        act_tc = self.insert('', VScrolledTreeview.end, self.last_added_tc_str, text=self.last_added_tc_str)
        tc_desc = self.insert(act_tc, VScrolledTreeview.end, self.last_added_tc_str+'__'+tree_const.tc_desc, text='Description')
        self.set(tc_desc, tree_const.col_in, "Description")
        self.fillFromFuncDesc()
        self.setTseqsTabUnsaved()
            
    def fillFromFuncDesc(self):
        ret_id = ""
        if not self.func_desc.returnsVoid():
            ret_id = self.insert(self.last_added_tc_str, VScrolledTreeview.end, self.last_added_tc_str+'__'+tree_const.tc_ret, text='Return')
            ret_id_info_set = self.insert(ret_id, VScrolledTreeview.end, ret_id+"__"+tree_const.tc_iset, text="Settings", values=())
            self.addSettingsRecursive(ret_id_info_set, self.ret_id_index_list)
            if len(self.sig_id_type_ist[1]) == 1:
                self.insert(ret_id, VScrolledTreeview.end, ret_id+"__"+"return_type", text="return_type", values=tree_const.ret_value_std)
                self.added_desc["return_type"] = self.sig_id_type_ist[1][0][1]
            else:
                for returner in self.sig_id_type_ist[1]:
                    self.insert(ret_id, VScrolledTreeview.end, ret_id+"__"+"return_type."+returner[0], text="return_type."+returner[0])
                    self.added_desc["return_type."+returner[0]] = returner[1]
        
        prm_id = ""
        if not self.func_desc.paramsVoid():
            prm_id = self.insert(self.last_added_tc_str, VScrolledTreeview.end, self.last_added_tc_str+'__'+tree_const.tc_prms, text='Parameters')
            prm_id_info_set = self.insert(prm_id, VScrolledTreeview.end, prm_id+"__"+tree_const.tc_iset, text="Settings", values=())
            self.addSettingsRecursive(prm_id_info_set, self.prm_id_index_list)
            for prm_tuple in self.sig_id_type_ist[0]:
                values_toset, argvars = self.rightclick_menu.getTCValuesToSet(prm_tuple[0])
                for i, av in enumerate(argvars):
                    var_values = list(tree_const.var_value_std)
                    if not len(argvars) == (i+1):
                        var_values[0] = arg_vars[i+1]+".&"
                    self.addVariable(self.rightclick_menu.var_desc_dict[av], values=var_values, tc_id=str(self.last_added_tc))
                self.insert(prm_id, VScrolledTreeview.end, prm_id+"__"+prm_tuple[0], text=prm_tuple[0], values=values_toset) 
                self.added_desc[prm_tuple[0]] = prm_tuple[1]

    def addFromSequence(self, sequence):
        for test_case in sequence.test_cases_list:
            self.last_added_tc += 1
            self.last_added_tc_str = str(self.last_added_tc)
            act_tc = self.insert('', VScrolledTreeview.end, self.last_added_tc_str, text=self.last_added_tc_str)
            self.insert(act_tc, VScrolledTreeview.end, self.last_added_tc_str+'__'+tree_const.tc_desc, text='Description', values=(test_case.description,))
            self.fillTCParentFromTestCaseDesc(act_tc, test_case)

    def fillTCParentWithExecutedTestCase(self, tc_parent, test_case):
        for child in self.get_children(tc_parent):
            self.delete(child)
            
        #self.insert('', VScrolledTreeview.end, text="egal", tags=("Passed"))
        valid_string = test_case.getValidationTreeTag()
        self.item(tc_parent, tags=(valid_string, ))
        self.set(tc_parent, tree_const.col_valid, valid_string)
        self.fillTCParentFromTestCaseDesc(tc_parent, test_case)
        self.tag_configure(exec_const.passed_str, background="#7CFC00")
        self.tag_configure(exec_const.failed_str, background="#FF6347")
        self.tag_configure(exec_const.exception_str, background="#C71585")
        self.tag_configure(exec_const.resolved_str, background="#FFA500")
            
    def fillTCParentFromTestCaseDesc(self, tc_parent, test_case):
        self.fillVariableSettingsOfIO(self.fillParentWithIOList(tc_parent, test_case, 'Variables', tree_const.tc_vars), test_case.variables_list)
        self.fillSettingsOfIO(self.fillParentWithIOList(tc_parent, test_case, 'Return', tree_const.tc_ret), self.ret_id_index_list)
        self.fillSettingsOfIO(self.fillParentWithIOList(tc_parent, test_case, 'Parameters', tree_const.tc_prms), self.prm_id_index_list)
    
    
    def fillParentWithIOList(self, tc_parent, test_case, text, tree_spec):
        io_list = test_case.tree_spec_list_dict[tree_spec]
        if len(io_list):
            io_parent = self.insert(tc_parent, VScrolledTreeview.end, tc_parent+'__'+tree_spec, text=text)
            id_info_set = self.insert(io_parent, VScrolledTreeview.end, io_parent+"__"+tree_const.tc_iset, text="Settings", values=())
            for io in io_list:
                io_id = self.insert(io_parent, VScrolledTreeview.end, io_parent+'__'+io.identifier, text=io.identifier, values=io.getDataTuple()) 
                if self.execution:
                    self.see(io_id)
                    tag_string = io.getValidationTreeTag()
                    if tag_string:
                        self.item(io_id, tags=(tag_string, ))
                    
                    
                
            return id_info_set
        return None
        
    def fillVariableSettingsOfIO(self, id_info_set, variables_list):
        temp_var_dict = {}
        for var in variables_list:
            rootvar = var.identifier.replace("[", ".[", 1).split(".", 1)[0]
            if not rootvar in temp_var_dict:
                var_desc = SocoAdapter.Global.getVariableDesc(rootvar)
                temp_var_dict[rootvar] = ""
                var_index_settings = var_desc.getIdentifierIndexList()
                self.addSettingsRecursive(id_info_set, var_index_settings)
                

    def fillSettingsOfIO(self, id_info_set, id_index_list):
        if id_info_set:
            self.addSettingsRecursive(id_info_set, id_index_list)

    #ToDo NiEh: at this moment prm-ret-var idstrings are still filled from descriptors, you can fill with applyIDStringsForItem
    def addSettingsRecursive(self, insert_id, id_index_list):
        for id_info in id_index_list:
            print(id_info)
            value_tuple = tree_const.tuple_pos
            if "[" in id_info[0]:
                id_info_split = id_info[0].replace("[", "").replace("]", "").split("-")
                if int(id_info_split[0]) > 0:
                    value_tuple = tree_const.tuple_neg
                
            new_id = self.insert(insert_id, VScrolledTreeview.end, insert_id+"#"+id_info[0], text=id_info[0], values=value_tuple)
            if len(id_info[1]):
                self.addSettingsRecursive(new_id, id_info[1])
                
    def applyIDStringsForItem(self, item):
        item_split = item.split("__", 3)
        final_id_string_list = []
        for child in self.get_children(item):
            final_id_string_list.extend(self.getSettingsFinalIDListOfItem(child))
        self.setTseqsTabUnsaved()
        
        parent_branch_to_detach = item_split[0]+"__"+item_split[1]
        self.detach(item)
        for det_child in self.get_children(parent_branch_to_detach):
            self.detach(det_child)
        
        self.reattach(item, parent_branch_to_detach, VScrolledTreeview.end)
        value_tuple = tree_const.getValueTupleForKind(item_split[1])
        col_value_list = list(value_tuple)
        for id_string in final_id_string_list:
            item_to_set = parent_branch_to_detach+"__"+id_string[0]
            try:
                self.move(item_to_set, parent_branch_to_detach, VScrolledTreeview.end) #remove [1] = "."
                if self.set(item_to_set, column=tree_const.col_rep) == "" and not id_string[1] == "":
                    self.set(item_to_set, tree_const.col_rep, id_string[1])
            except:
                try:
                    self.insert(parent_branch_to_detach, VScrolledTreeview.end, item_to_set, text=id_string[0], values=value_tuple) 
                    if not id_string[1] == "":
                        self.set(item_to_set, tree_const.col_rep, id_string[1])
                except:
                    pass
                    

    def getIndexRangesOfSplit(self, split):
        mo = tree_const.array_range_regexp.findall(split)
        if len(mo):
            mo.reverse()
            index_ranges = []
            for index_tuple in mo:
                start = int(index_tuple[1])
                end = int(index_tuple[2])
                indexes = []
                
                for i in range(start, end+1):
                    indexes.append("["+str(i)+"]")
                index_ranges.append(indexes) 

            return self.combineArrayIndexesRecursive([""], index_ranges, 0)    
        else:
            return [split]
                        
    
    def combineArrayIndexesRecursive(self, lastindex_str_list, index_list, current_key):
        current_list = []
        
        if current_key == len(index_list):
            return lastindex_str_list
        
        for index in index_list[current_key]:
            for last_index in lastindex_str_list:
                current_list.append(last_index+index)

        return self.combineArrayIndexesRecursive(current_list, index_list, current_key+1)            
            
    
    def getSettingsFinalIDListOfItem(self, item, current_indexed_members=[], array_index_string=""):
        no_child_list = []
        new_array_index_string = str(array_index_string)
        new_current_indexed_members = list(current_indexed_members)
        value = self.set(item, column=tree_const.col_in)   
        value_index = not (tree_const.array_range_regexp.match(value) == None)
        if value == tree_const.val_pos or value_index:
            irep = self.set(item, column=tree_const.col_rep)
                
            childs = self.get_children(item)  
            len_childs_zero = (len(childs) == 0)            
            act_mem = item.rsplit("#", 1)[1]
            is_index_mem = act_mem.startswith("[")
                
            if len(new_current_indexed_members) == 0:
                new_current_indexed_members.append([act_mem, irep])
            else:
                if not is_index_mem:
                    for index in range(0, len(new_current_indexed_members)):
                        crep = new_current_indexed_members[index][1]
                        if crep == "":
                            crep = irep                
                        new_current_indexed_members[index] = [new_current_indexed_members[index][0]+"."+act_mem, crep]
                else:
                    if value_index:
                        new_array_index_string += value
                    else:
                        new_array_index_string += act_mem
            
            if len(new_array_index_string) and (len_childs_zero or not is_index_mem):
                indexed_mem_list = self.getIndexRangesOfSplit(new_array_index_string)
                next_list = []
                for index in range(0, len(new_current_indexed_members)):
                    crep = new_current_indexed_members[index][1]
                    if crep == "":
                        crep = irep
                    for indexes in indexed_mem_list:
                        id_string = [new_current_indexed_members[index][0]+indexes, crep]
                        if len_childs_zero:
                            next_list.append(id_string)
                        else:
                            for child in childs:
                                next_list.extend(self.getSettingsFinalIDListOfItem(child, current_indexed_members=[id_string], array_index_string="")) 
                return next_list                
            
            if len_childs_zero:
                no_child_list.extend(new_current_indexed_members)
            else:
                for child in childs:
                    no_child_list.extend(self.getSettingsFinalIDListOfItem(child, current_indexed_members=new_current_indexed_members, array_index_string=new_array_index_string)) 

        return no_child_list
        
    
    def deleteTC(self, item):
        self.delete(item)
        self.setTseqsTabUnsaved()
        
    def execute(self, item):
        self.execution = True
        tc_id_str = item.split("__", 1)[0]
        
        tc_dict = self.getTestCaseDict(tc_id_str)
        tc_desc = TestCaseDesc.getFromDict(tc_id_str, tc_dict)
        tseq_dict = self.context.context.getYAMLDict(no_tc=True)
        tseq_desc = TestSequenceDesc.getFromDict("Single Testcase", tseq_dict)
        tseq_desc.function_desc = self.func_desc
        tseq_desc.test_cases_list = [tc_desc]
        tseqs = TestSequences()
        tseqs.test_sequences_list = [tseq_desc]
        self.target.execute_test_by_descriptor(tseqs)
        
        tseqs.test_sequences_list[0].print_summary(v=True)
        
        self.fillTCParentWithExecutedTestCase(tc_id_str, tc_desc)
        self.execution = False
    
    #funktioniert seit settings nicht mehr
    def copyTC(self, item):
        self.last_added_tc += 1
        self.last_added_tc_str = str(self.last_added_tc)
        self.insert('', VScrolledTreeview.end, self.last_added_tc_str, text=self.last_added_tc_str)
        self.copyChildsToFather(self.last_added_tc_str, item, 1)
        self.setTseqsTabUnsaved()
        
    def copyChildsToFather(self, father, child, text_index):
        for new_child in self.get_children(child):
            child_splitter = new_child.split("__",2)
            print(text_index, child_splitter)
            if text_index == 1:
                item_text = tree_const.tc_text_id_map[child_splitter[text_index]]
            elif text_index == 2 and child_splitter[2] == tree_const.tc_iset:
                item_text = "Settings"
            elif text_index > 2:
                item_text = child_splitter[2].split("#")[-1]
            else:
                item_text = child_splitter[text_index]
            act_set = self.insert(father, VScrolledTreeview.end, new_child.replace(child_splitter[0], self.last_added_tc_str, 1), text=item_text, values=self.getValueTuple_ofItem(new_child))
            self.copyChildsToFather(act_set, new_child, text_index+1)
    
        
    def getValueTuple_ofItem(self, item):
        col_values = list()
        for col in tree_const.tc_header_tuple:
            col_values.append(self.set(item, column=col))
            
        return tuple(col_values)
            
    def getTestCasesAsDict(self):
        tc_tree_dict = {}
        for tc in self.get_children():
            tc_tree_dict[tc] = self.getTestCaseDict(tc)
        return tc_tree_dict
        
    def getTestCaseDict(self, tc):
        tc_dict = {}
        for tc_info in self.get_children(tc):
            if tc_info.endswith(tree_const.tc_desc):
                tc_dict["description"] = self.set(tc_info, column=tree_const.col_in)
            elif tc_info.endswith(tree_const.tc_prms):
                tc_dict["parameter"] = self.getDictFromChildsOfCols(tc_info, tree_const.tc_entry_cols_dict[tree_const.tc_prms])
            elif tc_info.endswith(tree_const.tc_vars):
                tc_dict["variable"] = self.getDictFromChildsOfCols(tc_info, tree_const.tc_entry_cols_dict[tree_const.tc_vars])
            elif tc_info.endswith(tree_const.tc_ret):
                tc_dict["return"] = self.getDictFromChildsOfCols(tc_info, tree_const.tc_entry_cols_dict[tree_const.tc_ret])
        return tc_dict
        
    def getDictFromChildsOfCols(self, child, cols):
        id_info_dict = {}
        Settings_id = child+"__"+tree_const.tc_iset
        try:
            self.detach(Settings_id)
        except:
            pass
        
        for identifier in self.get_children(child):
            
            info_dict = {}
            for col in cols:
                if col == tree_const.col_dc:
                    continue
                    
                if col in tree_const.io_cols:
                    info_dict[col] = str(self.set(identifier, column=col)).strip()
                else:
                    info_dict[col] = str(self.set(identifier, column=col)).strip()
            
            if len(self.set(identifier, column=tree_const.col_dc)):
                if tree_const.col_exp in info_dict:
                    info_dict[tree_const.col_exp] = exec_const.dont_care
                else:
                    info_dict = {}
            
            id_info_dict[identifier.split("__", 2)[2]] = info_dict

        try:
            self.reattach(Settings_id, child, 0)
        except:
            pass
        
        return id_info_dict
        
        

