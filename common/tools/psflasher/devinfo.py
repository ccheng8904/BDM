"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : Reads information about modules from DBus node
"""

sys.path.append(".\\common\\tools\\psflasher\\Externals")

from ps_util import *
from map_protocol2 import *


# Initialize argument parser
argP = PsArgumentParser(description='Reads information about modules from DBus node')
argP.add_argument("node", required=True, help="DBus node address")
argP.add_argument("std_baud", default="9600", help="DBus baudrate")
args = argP.parse_args(cmdParameters)


# Initialize variables
udaNode = 0xC0
dbusNode = int(args["node"], 0)
stdBaud = int(args["std_baud"])


# Initialize UDA connection
setup_dbus_com(stdBaud)

# Initialize DBus node
node = DBusNode(dbusNode)

# Try to ping board
node.ping()

# Switch to programmer
print("Switching to programmer...")
node.switch_to_programmer()

# Wait for programmer to start
node.ping()

# Get info for all modules
mapDevice = MAP_Device(udaNode,dbusNode)
mapDevice.GetDeviceInfo() 
for module in mapDevice.ModuleInfo():
    print(mapDevice.ModuleInfo(module))

# Switch back to application
print("Executing reset...")
node.reset()
