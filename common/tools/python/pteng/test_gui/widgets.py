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

from tkinter import *
from tkinter import filedialog
from tkinter import ttk

        
class Frame_SelectPath(ttk.Frame):
    def __init__(self, master, text, **kw):
        super().__init__(master, **kw)
        self.master = master
        self.label_text = text
        self.filepath = ""
        
        self.Label     = ttk.Label(self, text=self.label_text, width=8)
        self.FileEntry = ttk.Entry(self, textvariable=self.filepath, width=30)
        self.Button_browse = ttk.Button(self, text="...", width=4, command=self.selectFile)
        
        self.FileEntry["state"] = "readonly"
        
        self.Label.grid(column=0, row=0, sticky=(N, S, W), padx=2, pady=1)
        self.FileEntry.grid(column=1, row=0, sticky=(N, S, E, W), padx=2, pady=1)
        self.Button_browse.grid(column=2, row=0, sticky=(N, S, E), padx=2, pady=1)
        self.columnconfigure(0, weight=2)
        self.columnconfigure(1, weight=6)
        self.columnconfigure(2, weight=1)
        
    def selectFile(self):
        pass
        
    def setFilePath(self, fpath):
        self.filepath = fpath
        self.FileEntry["state"] = "normal"
        self.FileEntry.insert('0', self.filepath)
        self.FileEntry["state"] = "readonly"
  
class Frame_SelectFilePath(Frame_SelectPath):
    def __init__(self, master, text, **kw):
        super().__init__(master, text, **kw)
       
    def selectFile(self):
        self.filepath = filedialog.askopenfilename(defaultextension='.xml', filetypes=[("all", "*.*")])
        self.FileEntry["state"] = "normal"
        self.FileEntry.insert('0', self.filepath)
        self.FileEntry["state"] = "readonly"
        
class Frame_SelectDirPath(Frame_SelectPath):
    def __init__(self, master, text, **kw):
        super().__init__(master, text, **kw)

    def selectFile(self):
        self.filepath = filedialog.askdirectory()
        self.FileEntry["state"] = "normal"
        self.FileEntry.insert('0', self.filepath)
        self.FileEntry["state"] =  "readonly"
        

class LabelEntry(ttk.Frame):
    def __init__(self, master, text, wentry, **kw):
        super().__init__(master, **kw)
        self.master = master
        self.label_text = text
        self.entry_var = StringVar()
        
        self.Label  = ttk.Label(self, text=self.label_text, width=12)
        self.Entry  = ttk.Entry(self, textvariable=self.entry_var, width=wentry)
        self.Label.grid(column=0, row=0, sticky=(E), padx=1, pady=2)
        self.Entry.grid(column=1, row=0, sticky=(W), padx=1, pady=2)
        
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=3)
        
class LabelEntry_savedIndicated(LabelEntry):
    def __init__(self, master, text, wentry, **kw):
        super().__init__(master, text, wentry, **kw)
        self.Entry.bind("<Key>", self.setUnsaved)
        
    def setUnsaved(self, event):
        print ("pressed", repr(event.char))
        self.master.context.context.setCurrentTabIndexUnsaved()
        
class SocoSearchButton(ttk.Frame):
    def __init__(self, master, text, wentry, **kw):
        super().__init__(master.Settings_Frame, **kw)
        self.master = master
        self.label_text = text
        self.entry_var = StringVar()
        
        self.Button  = Button(self, text=self.label_text, command=self.search, width=12)
        self.Entry  = ttk.Entry(self, textvariable=self.entry_var, width=wentry)
        self.Button.grid(column=0, row=0, sticky=(E), padx=1, pady=2)
        self.Entry.grid(column=1, row=0, sticky=(W), padx=1, pady=2)
        
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=3)

    def search(self):
        self.master.soco_adapter.search(self.entry_var.get())
        
class FilterEntryCheck(ttk.Frame):
    def __init__(self, master, text, wentry, **kw):
        super().__init__(master.Settings_Frame, **kw)
        self.master = master
        self.label_text = text
        self.entry_var = StringVar()
        self.check_text = StringVar()
        self.check_text.set(text)
        self.check_var = BooleanVar()
        self.check_var.set(False)
        self.spin_var = StringVar()
        self.spin_var.set("unit")
        
        self.CheckButton    = ttk.Checkbutton(self, text=self.label_text, variable=self.check_var, textvariable=self.check_text, command=self.applyFilter)
        self.Entry          = ttk.Entry(self, textvariable=self.entry_var, width=wentry)
        self.Spinbox        = Spinbox(self, values=["unit"], width=wentry)
        self.CheckButton.grid(column=0, row=0, sticky=(E), padx=1, pady=2)
        self.Entry.grid(column=1, row=0, sticky=(W), padx=1, pady=2)
        self.Spinbox.grid(column=3, row=0, sticky=(W), padx=1, pady=2) 
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=3)
        self.columnconfigure(2, weight=2)
        
    def applyFilter(self):
        if self.check_var.get():
            if self.master.soco_adapter:
                self.master.soco_adapter.applyFilter(self.entry_var.get(), type=self.spin_var.get())
            else:
                messagebox.showinfo(title="Error", message="SourceCodeTree not loaded.", icon="error")
                self.check_var.set(False)
        else:
            self.master.soco_adapter.refill()
        
             

        