"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : mainframe.py
Date of creation : 04.03.2020
Author Name/Dept : Nikolai Ehrhardt
Organization     : SDD-4
Description      : defines the mainframe for test-gui
"""

from tkinter import *
from tkinter import ttk, messagebox

from .nb_widgets import NoteBook_TSequences, Notebook_Navi
from .frame_widgets import Frame_Explorer
from .menues import MainMenu

#print(TclVersion)
#print(TkVersion)
class MainFrame():
    def __init__(self, context, target=None): 
        self.context = context
        
        if not target==None:
            self.target=target.target
        else:
            self.target=None
        self.context.option_add('*tearOff', False)
        self.menubar = MainMenu(self)
        self.context.config(menu=self.menubar)
        
        self.createWidgets()
        self.context.columnconfigure(0, weight=1)
        self.context.rowconfigure(0, weight=1)
        self.context.protocol('WM_DELETE_WINDOW', self.checkSaves)
        
    
    def createWidgets(self):
        self.contentFrame = ttk.Frame(self.context, padding=(3,3,12,12))
        self.contentFrame.grid(column=0, row=0, sticky=(N, S, E, W))
        
        FrameFoldertree = Frame_Explorer(self)
        FrameFoldertree.grid(column=0, row=0, sticky=(N, S, E, W), padx=2, pady=2)    
        self.tseqs_nb = NoteBook_TSequences(self)
        self.tseqs_nb.grid(column=1, row=0, sticky=(N, S, E, W), padx=2, pady=2)
        self.navi_nb = Notebook_Navi(self)
        self.navi_nb.grid(column=0, row=1, columnspan=2, sticky=(N, S, E, W), padx=2, pady=2)
        
        self.contentFrame.columnconfigure(0, weight=12)
        self.contentFrame.columnconfigure(1, weight=88)
        self.contentFrame.rowconfigure(0, weight=64)
        self.contentFrame.rowconfigure(1, weight=33)
        
    def checkSaves(self):
        if not self.tseqs_nb.saved or not self.navi_nb.tsuite_tree.saved:
            reply = messagebox.askyesnocancel("Exit", "There are unsaved changes, do you want to save them?", icon='question')
            
            if reply:
                self.navi_nb.tsuite_tree.saveAll()
                self.tseqs_nb.saveAll()
            elif reply is None:
                return
            else:
                pass 
        self.context.destroy()

    def _quit(self):
        self.quit()
        sys.exit(0)
    

if __name__ == "__main__":
    pass

    
