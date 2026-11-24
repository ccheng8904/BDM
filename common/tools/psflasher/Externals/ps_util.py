"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : Support utilities for psflash
"""

from  pyUda import *
from  FlashGordon.script_api import *
import time
import sys

# Custom argument parser, because default one can not be used in psflash scripts :-(
class PsArgumentParser:
    
    def __init__(self, description="No description"):
        self.argDict = dict()
        
    def add_argument(self, argName, dest=None, default=None, required=False, action="replace", help="No help provided"):
        argItem = dict()
        
        if dest is None:
            dest = argName
        
        argItem["dest"] = dest
        argItem["value"] = default
        argItem["required"] = required
        argItem["help"] = help
        argItem["action"] = action
        
        if action == "append":
            argItem["value"] = list()
        else:
            argItem["value"] = default
        
        self.argDict[argName] = argItem
        
    def parse_args(self, args):
        # Parse incoming arguments
        for arg in args:
            if '=' in arg:
                [argName, argValue] = arg.split("=", 1)
            else:
                argName = arg
            
            if argName in self.argDict:
                action = self.argDict[argName]["action"]
                value = self.argDict[argName]["value"]
                
                # Switch action to do with value
                if action == "set_true":
                    value = True
                elif action == "set_false":
                    value = False
                elif action == "append":
                    value.append(argValue)
                else:
                    value = argValue
                
                self.argDict[argName]["value"] = value
                
            else:
                sys.exit("Error, Unknown argument \"{}\"".format(argName))
        
        retDict = dict()
        # Create return dictionary
        for key in self.argDict:
            argItem = self.argDict[key]
            if argItem["required"] and argItem["value"] is None:
                sys.exit("Error, Argument \"{}\" is required".format(key))
            
            retDict[argItem["dest"]] = argItem["value"]
        
        return retDict


# Establish connection with UDA
def setup_dbus_com(baudrate=9600):
    try:
        establishConnection(UDA1 | DBUS2,"UDA-02")
        setDBus2HwUart(baudrate)
        setDBus2TransmissionRetries(3)
        setDBus2AckFilter(0xC)
        setDBus2RcvFilter('ALL')
    except:
        sys.exit("Unable to establish connection with UDA")

# convert integer to array of n-bytes
def num_to_data(value, number_of_bytes):
    return list(value.to_bytes(number_of_bytes, byteorder = 'big'))

# convert 16-bit integer to array of bytes
def int16_to_data(value):
    return num_to_data(value, 2)

# convert 32-bit integer to array of bytes
def int32_to_data(value):
    return num_to_data(value, 4)


def str_to_data(string_value, string_type):
    TWO_DIGITS = 2
    return_value = list()
    if string_type == "str":
        for character in range(len(string_value)):
            return_value.append(ord(string_value[character]))
        return return_value
    else:
        if string_type == "hex":
            for i in range(0, len(string_value),TWO_DIGITS):
                two_chararacters = string_value[i : i+TWO_DIGITS]
                return_value.append(int(two_chararacters,16))
        elif string_type == "bcd":
            for i in range(0, len(string_value),TWO_DIGITS):
                two_chararacters = string_value[i : i+TWO_DIGITS]
                return_value.append(int(two_chararacters,10))
        else:
            sys.exit("Wrong type '{}'".format(string_type))
        return return_value

# convert array of bytes to integer
def data_to_int(data):
    value = 0
    byteShift = 0
    for byte in reversed(data):
        value += byte << byteShift
        byteShift += 8
    return value


# Convert array of integers to string, until terminator character
def data_to_str(data, terminator='\0'):
    retStr = ""
    strLen = 0
    for i in data:
        ch = chr(i)
        if ch == terminator:
            return retStr, strLen
        else:
            strLen += 1
            retStr += ch
    return None, 0


class DBusNode:

    msg_id = {
        "Ping": 0xF080,
        "ReadReq32": 0xF001,
        "ReadResp32": 0xF101,
        "IdentReq": 0xFF00,
        "IdentResp": 0xFE00,
        "EcuUniqIdReq": 0xF308,
        "EcuUniqIdResp": 0xF309,
        "WriteEcuConfHWReq": 0xF312,
        "WriteEcuConfHWResp": 0xF313,
        "WriteTracingIDReq": 0xF314,
        "WriteTracingIDResp": 0xF315,
        "WriteProdTimeReq": 0xF316,
        "ProdTimeResp": 0xF317,
        "ReadProdTimeReq": 0xF318,
        "WriteAppDataReq": 0xF31C,
        "WriteAppDataResp": 0xF31D,
        "ReadAppDataReq":0xF31E,
        "ReadAppDataResp":0xF31F,
        "ResetTriggerReq": 0xF50B,
        "SilentModeTransReq": 0xF501,
        "SilentModeTransResp": 0xF503,
        "UpdateModeVerifyReq": 0xF504,
        "UpdateModeVerifyResp": 0xF505,
        "UpdateModeTransReq": 0xF506,
        "BaudVerifyReq": 0xF507,
        "BaudVerifyResp": 0xF508,
        "BaudTransReq": 0xF509,
        "EcuConfReadReq": 0xF510,
        "EcuConfReadResp": 0xF511,
        "ResetExecute": 0xFD00
    }
    return_status_dict = {
        160: "OK",
        177: "Parameter 1 Error",
        178: "Parameter 2 Error",
        179: "Parameter 3 Error",
        192: "Length Error",
        193: "Unknown Message Error",
        208: "Function Error",
        209: "Empty Item Error",
        224: "Unknown Item Error"
    }

    def status_id_to_msg(self, status_id):
        if status_id in self.return_status_dict:
            return self.return_status_dict[status_id]
        else:
            return "Not defined status"


    def __init__(self, targetNode, udaNode=0xC0):
        self.Node = targetNode
        self.UdaNode = udaNode


    def write(self, message_id, data=[]):
        try:
            sendDBus2(self.Node, message_id, data)
        except:
            sys.exit("DBus message 0x{0:04X} failed".format(message_id))


    def read(self, message_id, timeout=None):
        try:
            return receiveDBus2(message_id, timeout=timeout)
        except:
            sys.exit("DBus message 0x{0:04X} failed".format(message_id))


    # Try to ping MCU multiple times if necessary
    def ping(self, timeout=2):
        startTime = time.time()
        pingCounter = 0
        while True:
            try:
                # Send ping message
                retVal = self.write(self.msg_id["Ping"])
                
                # Calculate response time
                actualTime = time.time() - startTime
                return actualTime
                
            except:
                # Calculate actual time
                actualTime = time.time() - startTime
                
                # Print timeout message every second
                if pingCounter is not int(actualTime):
                    pingCounter = int(actualTime)
                    print("Ping timeout {}".format(pingCounter))
                
                # Check if timeout has been reached
                if actualTime > timeout:
                    sys.exit("Connection timeout after {} seconds".format(pingCounter))


    # Try to "ping" MCU multiple times if necessary by using IdentityRequest message
    def ping_by_ident_req(self, timeout=2):
        readTimeout = 0.02
        pingCounter = 0
        startTime = time.time()
        while True:
            try:
                # Send IdentityRequest and wait for IdentityResponse
                self.write(self.msg_id["IdentReq"], [self.UdaNode])
                self.read(self.msg_id["IdentResp"], readTimeout)
                # Calculate response time
                actualTime = time.time() - startTime
                return actualTime
            except:
                # Calculate actual time
                actualTime = time.time() - startTime
                # Print timeout message every second
                if pingCounter is not int(actualTime):
                    pingCounter = int(actualTime)
                    print("Ping timeout {}".format(pingCounter))
                
                # Check if timeout has been reached
                if actualTime > timeout:
                    sys.exit("Connection timeout after {} seconds".format(pingCounter))


    # Read data from Node
    def read_data(self, address, size):
        addrArray = int32_to_data(address)
        self.write(self.msg_id["ReadReq32"], [self.UdaNode, size] + addrArray)
        data = self.read(self.msg_id["ReadResp32"])[2:]
        return data


    # Read data from Node
    def read_string(self, address, max_size=128, read_size=32):
        data = []
        retStr = ""
        index = 0
        
        while index < max_size:
            # Read data read_size bytes at a time
            data += self.read_data(address + index, read_size)
            index += read_size
            retStr, strLen = data_to_str(data)
            
            if retStr is not None:
                break
        
        return retStr, strLen


    # Read ID string of actual module
    def read_id_string(self, separator=" "):
        retStr = ""
        index = 0
        
        # Read address of ID string
        self.write(self.msg_id["IdentReq"], [self.UdaNode])
        addrData = data_to_int(self.read(self.msg_id["IdentResp"])[2:])
        
        # Read 4 individual strings: variant, build time, module width and ID string
        for n in range(4):
            idStr, strLen = self.read_string(addrData + index)
            
            if idStr is None:
                sys.exit("Unable to read ID string")
            
            retStr += idStr + separator
            index += strLen + 1
        
        return retStr[:-1]


    # Command to reset node
    # Timeout is in milliseconds
    def reset(self, time_ms=0, reset_msg="ResetTriggerReq"):
        t = int(time_ms / 10)
        try:
            self.write(self.msg_id[reset_msg],[t])
        except:
            sys.exit("Reset of board failed")


    # Command to change baudrate
    def change_baudrate(self, newBaudrate):
        baudData = int16_to_data(int(newBaudrate / 100))
        try:
            # Baud Rate Verify Request
            self.write(self.msg_id["BaudVerifyReq"], [self.UdaNode] + baudData)
            # Baud Rate Verify Response
            transitionDelay = data_to_int(self.read(self.msg_id["BaudVerifyResp"])) / 100
            # Baud Rate Transition Request
            self.write(self.msg_id["BaudTransReq"])
            time.sleep(transitionDelay)
            
        except:
            sys.exit("Switch to new baudrate {} failed".format(newBaudrate))


    # Command to switch to Programmer
    def switch_to_programmer(self):
        try:
            # Update Mode Verify Request
            self.write(self.msg_id["UpdateModeVerifyReq"], [self.UdaNode])
            # Update Mode Verify Response
            switchTime = data_to_int(self.read(self.msg_id["UpdateModeVerifyResp"])) / 100
            # Update Mode Transition Request
            self.write(self.msg_id["UpdateModeTransReq"])
            time.sleep(switchTime)
            
        except:
            sys.exit("Switch to module failed")


    # Read production data by production messages from an actual node
    def read_prod_data(self, request_message_id, response_message_id):
        status = "Not OK"
        if request_message_id == self.msg_id["ReadProdTimeReq"]:
            # Read production time request and response
            self.write(request_message_id)
            Data = self.read(response_message_id)
            # set status and separate data
            status = self.status_id_to_msg(Data[0])
            Data = Data[1:]
        elif request_message_id == self.msg_id["EcuUniqIdReq"]:
            # Read ECU Unique ID
            self.write(request_message_id, [self.UdaNode])
            Data = self.read(response_message_id)[1:]
            status = Data[1]
        elif request_message_id == self.msg_id["EcuConfReadReq"]:
            # Read ECU Config (HW and SW information)
            self.write(request_message_id, [self.UdaNode, 0x1])
            Data = self.read(response_message_id)
            status = Data[1]
            Data = Data[1:]
        else:
            Data = []
        return status, Data


    # Write production data by production messages into an actual node
    def write_prod_data(self, request_message_id, response_message_id, data):
        self.write(request_message_id, data)
        Data = self.read(response_message_id)

        # evaluation of status
        status = self.status_id_to_msg(Data[0])
        if response_message_id == self.msg_id["WriteEcuConfHWResp"] or response_message_id == self.msg_id["WriteTracingIDResp"]:
            # no data is returned in these response messages
            Data = []
            return status, Data
        elif response_message_id == self.msg_id["ProdTimeResp"]:
            return status, Data[1:]

    # Check silent mode of actual node
    def silent_mode_check(self,max_response_time):
        startTime = time.time()
        ret_status = False
        # response variable initialized to any non empty value e.g. three stars ***
        # it will be empty after successful silent mode response
        response = [42] * 3
        try:
            # Send silent mode transition request
            self.write(self.msg_id["SilentModeTransReq"],[self.UdaNode])
            response = self.read(self.msg_id["SilentModeTransResp"])
            actualTime = time.time() - startTime
            # response has to be empty and time of response less then max allowed
            if actualTime < max_response_time:
                if response == []:
                    ret_status = True
            return actualTime, ret_status

        except:
            actualTime = time.time() - startTime
            if actualTime < max_response_time:
                sys.exit("Connection timeout. Time was {:.3f}s".format(actualTime))


    def read_appliance_data(self, appliance_item_number):
        status = ""
        try:
            self.write(self.msg_id["ReadAppDataReq"], [self.UdaNode, appliance_item_number])
            Data = self.read(self.msg_id["ReadAppDataResp"])
        except:
            sys.exit("DBus Read Appliance Data Message failed.\nProbably you run appliance data test on electronic without enabled appliance data messages.")
        # evaluation of status
        status = self.status_id_to_msg(Data[2])
        return status, Data[3:]


    def write_appliance_data(self, appliance_item_number, data):
        status = ""
        message = [self.UdaNode, appliance_item_number] + data
        try:
            self.write(self.msg_id["WriteAppDataReq"], message)
            Data = self.read(self.msg_id["WriteAppDataResp"])
        except:
            sys.exit("DBus Read Appliance Data Message failed.\nProbably you run appliance data test on electronic without enabled appliance data messages.")
        # evaluation of status
        status = self.status_id_to_msg(Data[1])
        return status
