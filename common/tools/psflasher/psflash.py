"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : script for flashing of modules using programmer
"""

sys.path.append(".\\common\\tools\\psflasher\\Externals")

from ps_util import *
from map_protocol2 import *


# Function for reading data from binary file
def get_bytes_from_file(filename):
    if not os.path.isfile(filename):
        sys.exit("Data file \"{}\" could not be found".format(filename))
    bdata = open(filename, "rb").read()
    data=[]
    for x in bdata:
        data.append(x)
    return data


# Initialize argument parser
argP = PsArgumentParser(description='Flashing of remote firmware update modules')
argP.add_argument("node", required=True, help="DBus node address")
argP.add_argument("std_baud", default="9600", help="Standard application baudrate")
argP.add_argument("high_baud", default="125000", help="High baudrate used for flashing")
argP.add_argument("module", required=True, action="append", help="List of modules to flash")
argP.add_argument("noreset", dest="reset", default=True, action="set_false", help="Disable reset after flashing")
args = argP.parse_args(cmdParameters)


# Initialize variables
udaNode  = 0xC0
dbusNode = int(args["node"], 0)
stdBaud = int(args["std_baud"])
highBaud = int(args["high_baud"])
modules = args["module"]
doReset = args["reset"]


# Initialize UDA connection
setup_dbus_com(stdBaud)

# Initialize DBus node and map device
node = DBusNode(dbusNode)
mapDevice = MAP_Device(udaNode,dbusNode)

# Try to ping board
node.ping()

# Switch to programmer
print("Switching to programmer...")
node.switch_to_programmer()

# Change baudrate to highBaud
print("Changing baudrate to {} baud".format(highBaud))
node.change_baudrate(highBaud)

# Set bausdrate retrigger every 20s
mapDevice.SetBaudTrigerfreq(highBaud,35,20)

# Switch UDA baud rate
setDBus2HwUart(highBaud)

# Loop trough modules and flash them
for module in modules:
    moduleID, moduleFile = module.split(':')
    moduleID = int(moduleID, 0)

    # read data from binary file into list
    data = get_bytes_from_file(moduleFile)

    # get module info and size of module
    mapDevice.GetDeviceInfo()
    mapDevice.ModuleInfo(moduleID)
    partSize = mapDevice.ModuleInfo(moduleID)['ModuleSize']
    modSize = len(data)
    
    print("Partition writable size: 0x{0:0X}".format(partSize))
    print("Module file size: 0x{0:0X}".format(modSize))
    
    if modSize > partSize:
        raise Exception("Size of module exceeds writable size of partition")
    
    #setup and write module
    mapDevice.SetupModule(moduleID)
    mapDevice.WriteModule(moduleID,data)

# Switch back to normal baudrate
print("Changing baudrate to {} baud".format(stdBaud))
node.change_baudrate(stdBaud)

# Switch UDA baud rate
setDBus2HwUart(stdBaud)

# Try to ping board
node.ping()

# If requested, reset board
if doReset:
    print("Resetting node...")
    node.reset()
else:
    print("Reset is not performed!")

killConnection()
