import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
import os
import multiprocessing


class build_window(tk.Frame):

    def __init__(self):
        tk.Frame.__init__(self, tk.Tk())
        self.master.bind("<Escape>", self.close_window)
        
        # Project Selection
        self.frm_proj = tk.Frame(master=self)
        self.lbl_proj = tk.Label(master=self.frm_proj, text="Select Project:")
        self.cmbbx_proj = tk.ttk.Combobox(master=self.frm_proj, values=self.parse_projects(), state='readonly')
        
        # Variant Selection
        self.frm_vars = tk.Frame(master=self)
        self.lbl_vars = tk.Label(master=self.frm_vars, text="Select Variant:")
        self.cmbbx_vars = tk.ttk.Combobox(master=self.frm_vars, postcommand=self.parse_variants, state='readonly')
        self.lbl_vars_fw = tk.Label(master=self.frm_vars, relief=tk.GROOVE, text="FWU_")
        self.lbl_vars_fw2 = tk.Label(master=self.frm_vars, text="Firmware:")
    
        # Target Selection
        self.frm_trgt = tk.Frame(master=self)
        self.lbl_trgt = tk.Label(master=self.frm_trgt, text="Select Target:")
        self.target_list = ["clean", "clean_all", "link", "relink", "all", "lint", "release", "flash", "flash_only", "flashST", "flashST_only", "flashGD", "flashGD_only", "JLink", "JLink_only", "flashE1",
        "flashE1_only", "psflash", "psflash_only", "axstyle", "doxy", "help", "datasheet", "refman", "errata"]
        self.cmbbx_trgt = ttk.Combobox(master=self.frm_trgt, values=self.target_list, state='readonly')
        
        # Separator
        self.sep_vert = ttk.Separator(master=self, orient='vertical')
        self.sep_hor = ttk.Separator(master=self, orient='horizontal')
        
        # Build options
        self.frm_opt = tk.Frame(master=self)
        self.lbl_opt = tk.Label(master=self.frm_opt, text="Build options:")
        self.chk_opt_B_val = tk.BooleanVar()
        self.chk_opt_B_val.set(False)
        self.chk_opt_B = tk.Checkbutton(master=self.frm_opt, text="ignore timestamps (-B)", var=self.chk_opt_B_val)
        self.chk_opt_j_val = tk.BooleanVar()
        self.chk_opt_j_val.set(False)
        self.chk_opt_j = tk.Checkbutton(master=self.frm_opt, text="use all cores (-j)", var=self.chk_opt_j_val)
        self.chk_opt_preproc_val = tk.BooleanVar()
        self.chk_opt_preproc_val.set(False)
        self.chk_opt_preproc = tk.Checkbutton(master=self.frm_opt, text="keep preprocessor files", var=self.chk_opt_preproc_val)
        self.chk_opt_dbg_val = tk.BooleanVar()
        self.chk_opt_dbg_val.set(False)
        self.chk_opt_dbg = tk.Checkbutton(master=self.frm_opt, text="debugging information", var=self.chk_opt_dbg_val)
        
        # Additional commands
        
        self.frm_add = tk.Frame(master=self)
        self.lbl_add = tk.Label(master=self.frm_add, text="Additional commands:")
        self.ent_add = tk.Entry(master=self.frm_add, width=40)
        
        # Command preview
        self.frm_prv = tk.Frame(master=self)
        self.lbl_prv = tk.Label(master=self.frm_prv, text="Command:")
        self.cmbbx_prv = ttk.Combobox(master=self.frm_prv, width=100)
        self.btn_prv = tk.Button(master=self.frm_prv, height=1, width=2, text=u"\u21bb", command=self.get_command)
        
        # Run command
        self.frm_run = tk.Frame(master=self)
        self.btn_run = tk.Button(master=self.frm_run, text="Run", command=self.run_command)
        
        self.configure_layout()
        
    def parse_projects(self):
        var_path = os.path.join(os.path.abspath(os.getcwd()), "../app/")
        projects = [d for d in os.listdir(var_path) if os.path.isdir(os.path.join(var_path, d))]
        projects.insert(0, "default project")
        return projects
    
    def parse_variants(self):
        curr_proj = self.cmbbx_proj.get()
        if curr_proj == '':
            self.cmbbx_vars['values'] = ["Please select a Project"]
            return
        elif curr_proj == "default project":
            cfg_path = "../app/build/common_config.mak"
        else:
            cfg_path = "../app/" + curr_proj + "/build/common_config.mak"
        file_cnfg = open(cfg_path).readlines()
        variants_list = []
        a = len(file_cnfg)
        i = 0
        while i < a:
            while not file_cnfg[i] == "\n" and file_cnfg[i][-2] == "\\":
                file_cnfg[i] = file_cnfg[i][0:-2] + file_cnfg[i+1]
                file_cnfg.pop(i+1)
                a -= 1
            i += 1
        firmware = 0
        for line in file_cnfg:
            if "app_variants" in line:
                variants_list += line.replace("app_variants", '').replace("+=", '').replace("=", '').split()
        firmware = self.lbl_vars_fw["text"]
        if firmware == "FWU1":
            variants_list.extend(["ramLoader", "romLoader"])
        elif firmware == "FWU3":
            variants_list.extend(["BootManager", "programmer", "lromloader", "ramLoader"])
        variants_list.insert(0, "default variant")
        self.cmbbx_vars['values'] = variants_list
    
    def parse_firmware(self, event):
        curr_proj = self.cmbbx_proj.get()
        if curr_proj == '':
            return
        elif curr_proj == "default project":
            cfg_path = "../app/build/common_config.mak"
        else:
            cfg_path = "../app/" + curr_proj + "/build/common_config.mak"
        file_cnfg = open(cfg_path).readlines()
        firmware = 0
        for line in file_cnfg:
            if "FW_update_type" in line:
                if "FWU1" in line:
                    firmware = 1
                elif "FWU3" in line:
                    firmware = 3
        if firmware == 1:
            self.lbl_vars_fw['text'] = "FWU1"
        elif firmware == 3:
            self.lbl_vars_fw['text'] = "FWU3"
        else:
            self.lbl_vars_fw['text'] = "FWU_"
            messagebox.showwarning(title="Error", message="Couldn't find firmware update type!")
    
    # parse the make command from the combo- and checkboxes
    def get_command(self):
        cmd = "make"
        if self.cmbbx_proj.get() and not self.cmbbx_proj.get() == "default project":
            cmd += " project=" + self.cmbbx_proj.get()
        if self.cmbbx_vars.get() and not self.cmbbx_vars.get() == "default variant":
            cmd += " variant=" + self.cmbbx_vars.get()
        if self.ent_add.get():
            cmd += " " + self.ent_add.get()
        if self.cmbbx_trgt.get():
            cmd += " " + self.cmbbx_trgt.get()
            
        if self.chk_opt_j_val.get():
            cmd += " -j" + str(multiprocessing.cpu_count())
        if self.chk_opt_B_val.get():
            cmd += " -B"
        if self.chk_opt_preproc_val.get():
            cmd += " preproc=1"
        if self.chk_opt_dbg_val.get():
            cmd += " -d"
        
        self.cmbbx_prv.delete(0, "end")
        self.cmbbx_prv.insert(0, cmd)
        
    # run the make command
    def run_command(self):
        if self.cmbbx_prv.get():
            if self.cmbbx_prv.get() not in self.cmbbx_prv["values"]:
                self.cmbbx_prv["values"] = (*self.cmbbx_prv["values"], self.cmbbx_prv.get())
            os.system("..\\" + self.cmbbx_prv.get())
        
    def close_window(self, event):
        if messagebox.askokcancel("Close", "Do you want to quit?"):
            self.master.destroy()
    # layout the widgets
    def configure_layout(self):
        self.master.minsize(650, 250)
        self.columnconfigure([0, 1], weight=1, minsize=325)
        
        self.pack(fill="both")
        
        # Project Selection
        self.lbl_proj.pack(side=tk.LEFT)
        self.cmbbx_proj.pack(side=tk.LEFT)
        self.cmbbx_proj.bind("<<ComboboxSelected>>", self.parse_firmware)
        self.frm_proj.grid(row=0, column=0, padx=5, pady=5, sticky="w")
        
        # Variant Selection
        self.lbl_vars.pack(side=tk.LEFT)
        self.cmbbx_vars.pack(side=tk.LEFT)
        self.lbl_vars_fw2.pack(side=tk.LEFT, padx=(5, 0))
        self.lbl_vars_fw.pack(side=tk.LEFT, padx=5)
        self.frm_vars.grid(row=1, column=0, padx=5, pady=5, sticky="w")
    
        # Target Selection
        self.frm_trgt.grid(row=2, column=0, padx=5, pady=5, sticky="w")
        self.lbl_trgt.pack(side=tk.LEFT)
        self.cmbbx_trgt.pack(side=tk.LEFT, padx=(4, 0))
        
        # Separator
        self.sep_vert.grid(row=0, column=0, rowspan=3, sticky="ens")
        self.sep_hor.grid(row=2, column=0, columnspan=2, sticky="ews")
        
        # Build options
        self.lbl_opt.grid(row=0, column=0, sticky="w")
        self.chk_opt_j.grid(row=1, column=0, sticky="w")
        self.chk_opt_B.grid(row=2, column=0, sticky="w")
        self.chk_opt_preproc.grid(row=3, column=0, sticky="w")
        self.chk_opt_dbg.grid(row=4, column=0, sticky="w")
        self.frm_opt.grid(row=0, column=1, rowspan=3, padx=5, pady=(10, 5), sticky="w")
        
        #Additional commands
        self.lbl_add.grid(row=0, column=0, sticky="w")
        self.ent_add.grid(row=0, column=1, sticky="w")
        self.frm_add.grid(row=3, column=0, columnspan=2)
        
        # Command preview
        self.frm_prv.grid(row=4, column=0, columnspan=2, padx=5, pady=5, sticky="we")
        self.lbl_prv.grid(row=0, column=0, columnspan=2)
        self.cmbbx_prv.grid(row=1, column=0, sticky="we")
        self.btn_prv.grid(row=1, column=1, padx=5)
        self.frm_prv.columnconfigure(0, weight=1)

        # Run command
        self.frm_run.grid(row=5, column=0, columnspan=2, padx=5, pady=5, sticky="e")
        self.btn_run.grid(row=0, column=1, padx=5)
        
    