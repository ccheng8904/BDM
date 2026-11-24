"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : setup_cli.py
Date of creation : 7.8.2018
Author Name/Dept : Nikolai Ehrhardt
Organization     : 
Description      : Initializes a GUI to configure the Build-Process and create Project in app-Folder.
"""


from argparse import ArgumentParser
from setup_gui.setup import * 
import os


argP = ArgumentParser(description='Creates a Project of the next Generation')

#basic setup
argP.add_argument("-project", dest="project", required=False, help="Define the Name of the Project to create.")
argP.add_argument("-variant", dest="variant", required=False, help="Define the Variant of the Project to create.")
argP.add_argument("-platform", dest="platform", required=False, help="Define the Platform of the Project to create.")
argP.add_argument("-derivat", dest="derivat", required=False, help="Define the Derivative of the Project to create.")
argP.add_argument("-fwu", dest="fwu", default="FWU3", type=str, required=False, help="Define the FirmwareUpdateType: FWU1/FWU3.")

#arguments for setup
argP.add_argument("-initArgs", dest="initArgs", default="", required=False, help="Additional arguments for target init (executing the setup).")

#basic configuration
argP.add_argument("-config", dest="config", default=None, type=str, nargs="*", required=False, help="Add list of paths to json-files to configure setup.")
argP.add_argument("-force", dest="force", default=False, action="store_true", required=False, help="Switch to force overwriting of files.")

#compiler
argP.add_argument("-compiler", dest="compiler", default=None, type=str, required=False, help="Define the compiler to build application: iarRx/keilArm/armclang/gccArm.")

#communication
argP.add_argument("-node", dest="node", default=None, type=str, required=False, help="Define applications node-address: 1-12.")
argP.add_argument("-uart", dest="uart", default=None, type=str, required=False, help="Define applications dbus-uart-channel.")
argP.add_argument("-baud", dest="baud", default=96, type=int, required=False, help="Define applications default uart-baudrate.")
argP.add_argument("-dbus_tx", dest="dbus_tx", default=None, type=str, required=False, help="Define receive pin for dbus.")
argP.add_argument("-dbus_rx", dest="dbus_rx", default=None, type=str, required=False, help="Define transmit pin for dbus.")


#component choice
argP.add_argument("-omit_dbal", dest="omit_dbal", default=False, action="store_true", required=False, help="Switch to omit DBal component.")
argP.add_argument("-comp", dest="comps", default="", type=str, nargs="*", required=False, help="Add components to application build.")
argP.add_argument("-ext_comp", dest="ext_comps", default="", type=str, nargs="*", required=False, help="Add external components to application build.")
argP.add_argument("-vcomp", dest="vcomp", default=False, action="store_true", required=False, help="Switch to enable component validation.")

argP.add_argument("-allcomp", dest="allcomp", default=False, action="store_true", required=False, help="Add all platform-suitable components to the setup.")

#OS choice
argP.add_argument("-scheduler", dest="scheduler", default="new", type=str, required=False, help="Define scheduler:old/new/rtos.")

#basic features
argP.add_argument("-funcsafe", dest="funcsafe", default=False, action="store_true", required=False, help="Switch to include functional safety component.")
argP.add_argument("-flexpart", dest="flexpart", default=False, action="store_true", required=False, help="Switch to enable/disable flexible partition table.")

#macros to add
argP.add_argument("-macros", dest="macros", default="", type=str, nargs="*", required=False, help="Add macros to application build, design list like following: macro=value macro2?=value2 ...")

#path for axivion db-files
argP.add_argument("-ax_db", dest="ax_db", default=None, type=str, required=False, help="Specify folder, where to put axivion-db-files.")


args = argP.parse_args()

if __name__ == "__main__":
    app = Setup()
    app.parseArgs(args)


