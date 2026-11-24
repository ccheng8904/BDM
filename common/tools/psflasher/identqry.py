"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : reads out the idString of application and programmer module
"""

sys.path.append(".\\common\\tools\\psflasher\\Externals")

from ps_util import *


def print_id_string(idString):
    print("Reading ID string:")
    print("------------------")
    print(idString)
    print("------------------")


# Initialize argument parser
argP = PsArgumentParser(description='Identity request for DBus node')
argP.add_argument("node", required=True, help="DBus node address")
argP.add_argument("std_baud", default="9600", required=False, help="DBus baudrate")
argP.add_argument("dbuscan", default=False, action="set_true", help="Optional whether DBusCAN chip is used")
argP.add_argument("programmer", default=False, action="set_true", help="Optional whether to query also programmer")
argP.add_argument("timeout", default="5", help="Timeout for checking connection with MCU")
args = argP.parse_args(cmdParameters)


# Initialize variables
dbusNode      = int(args["node"], 0)
stdBaud       = int(args["std_baud"])
hasDbuscan    = args["dbuscan"]
hasProgrammer = args["programmer"]
timeout       = int(args["timeout"])

if hasProgrammer:
    resetMsg = "ResetTriggerReq"
else:
    resetMsg = "ResetExecute"

# Initialize UDA connection
setup_dbus_com(stdBaud)

# Initialize DBus node
node = DBusNode(dbusNode)

# Initial ping - should be short
node.ping(timeout)

# Switch to app by reset and ping the board
# After flashing with psflash expecting longer timeout
print("Executing reset command...")
node.reset(reset_msg=resetMsg)
if hasDbuscan:
    responseTime = node.ping_by_ident_req(timeout)
else:
    responseTime = node.ping(timeout)
print("Boot time:{:.3f}s".format(responseTime))

# Read and print ID string - should be application
print_id_string(node.read_id_string('\n'))

if hasProgrammer:
    # Switch to programmer and wait for ping response
    print("Switching to programmer...")
    node.switch_to_programmer()
    node.ping()

    # Read and print ID string - should be programmer
    print_id_string(node.read_id_string('\n'))

    # Reset - jump back to application
    print("Switching to application...")
    node.reset(reset_msg=resetMsg)
