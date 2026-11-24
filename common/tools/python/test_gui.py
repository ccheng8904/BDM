"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : test_gui.py
Date of creation : 4.03.2020
Author Name/Dept : Nikolai Ehrhardt
Organization     : 
Description      : GUI to write testcases.
"""

from tkinter import Tk

from pteng.test_gui.mainframe import MainFrame
import os

if __name__ == "__main__":
    root = Tk()
    app = MainFrame(root)
    root.title("PTEng Editor")
    root.iconbitmap(os.path.dirname(os.path.abspath("."+os.sep)+os.sep+__file__)+os.sep+"setup.ico")
    root.mainloop()
