"""
Copyright (c) 2023 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : sends GoOffline message to make node with DBusCAN chip stop communicating over DBus
"""

sys.path.append(".\\common\\tools\\psflasher\\Externals")

from ps_util import PsArgumentParser
from pyUda   import *
import sys


def setup_dbus_com(udaVersion=None, udaName=None, baudrate=9600):
    rxFilter = [0xFFFF] * 16 #receive filter = all
    try:
        global uda
        uda = PyUda(udaVersion, udaName)
        uda.setDBusMode(DBusMode.DBus2)
        uda.setBaudrate(baudrate)
        uda.setSendRetry(3)
        uda.setSimulatedDevices(0xC)
        uda.setDBus2ReceiveFilter(rxFilter)
    except:
        sys.exit("Unable to establish connection with UDA")

# Makes node with DBusCAN chip stop communicating over DBus (by setting it offline via GoOffline message)
def reset_dbuscan(targetAddr):
    goOfflineMsgId   = 0xFDFF
    goOfflineMsgData = [0, 0, 0] # target address, reset delay, offline delay
    print("Sending GoOffline message...")
    txDict = uda.sendDBus2(targetAddr, dbus2Message(goOfflineMsgId, goOfflineMsgData), expectedAck=None)
    if (txDict['ackType'] != 0xA):
        print("Resetting node with DBusCAN chip failed")


# Initialize argument parser
argP = PsArgumentParser()
argP.add_argument("node", required=True, help="DBus node address")
argP.add_argument("std_baud", default="9600", required=False, help="DBus baudrate")
argP.add_argument("uda_version", required=True, help="UDA version")
argP.add_argument("uda_name", required=True, help="UDA name")
args = argP.parse_args(cmdParameters)

# Initialize variables
dbusNode   = int(args["node"], 0)
baudrate   = int(args["std_baud"])
udaVersion = int(args["uda_version"])
udaName    = str(args["uda_name"])

# Initialize UDA connection
setup_dbus_com(udaVersion, udaName, baudrate)

# Reset (node with) DBusCAN chip
reset_dbuscan(dbusNode)
