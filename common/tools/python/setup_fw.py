"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : setup_fw.py
Date of creation : 7.8.2018
Author Name/Dept : Jens Lehmann, Nikolai Ehrhardt, Michal Kopcik
Organization     : 
Description      : Initializes a GUI to configure the Build-Process and create Project in app-Folder.
"""


from setup_gui.fw_gui import * 
import os
import platform


if __name__ == "__main__":
    app = fwUpdate()
    app.master.title("Framework Setup Utility")
    if platform.system() == "Windows":
        icon_path=os.path.dirname(__file__) + os.sep + "setup.ico"
        app.master.iconbitmap(icon_path)
    app.mainloop()

