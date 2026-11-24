"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

Project          : Generic SW
Description      : Execute test of production messages.
                   - the check starts with sending message SilentModeTransitionRequest
                   - reads production data first - rules:
                        - check if data with fixed length is empty, the response should read "FFFFFF...."
                        - check return status if it is ok
                        - check if appliance data with flexible length have a data response return nothing
                        - check return status if it is "Item_Empty" error.
                   - switch to the programmer
                   - write production data - fist attempt should pass
                   - read production data after first write
                   - write production data - second attempt should fail
                   - read again production data after second write and compare them against first read


"""
# import binascii to calculate CRC-Ethernet (32bit) and CRC-CCITT (16bit)
import binascii
import json
sys.path.append(".\\common\\tools\\psflasher\\Externals")

from ps_util import *

# calculate aligned size of the items
def aligned_size(write_block_size, size_of_item):
    return ((int((size_of_item - 1) / write_block_size) + 1) * write_block_size)


# Read JSON file containing test data paterns
def get_data_from_json(json_file):
    # Opening JSON file
    try:
        with open(json_file, 'r') as fd:
            data = json.load(fd)
            fd.close()
            return data
    except OSError:
        print("Could not open/read a JSON File '{}'. ".format(json_file))


# Test of repsonse time on Silent Mode Request message
def test_silent_mode(max_response_time):
    response_time, status = node.silent_mode_check(max_response_time)
    print_header("Starting the Silent Mode Request test")
    print("Response time has to be less then {:.3f}s".format(max_response_time))
    print("Actual response time is           {:.3f}s".format(response_time))
    if status is True:
        print("The status is OK")
    else:
        print("The status is NOT OK")
    print("\n")


# four functions for formated printing to console
def print_header(header_text):
    print("\n")
    print(header_text.center(80,'-'))


def print_footer(footer_text):
    print(footer_text.center(80, '-'))


def print_production_data(production_data, descriptive_name):
    print("{:15}: {}".format(descriptive_name, bytes(production_data).hex().upper()))


def print_all_prod_data(round, data_dict):
    print_header(" " + str(round) + ". reading of production data")
    for key in sorted(data_dict):
        print_production_data(data_dict[key], key)


# Read all items from FW ID table by using ReadRequest32 message
def read_fwid_table(production_data_dict):
    #crc 0-3. byte, 1-st module number 4-5. byte, 1-st FWID 6-13.byte, 2-nd module number 14-15. byte, 2-nd FWID 16-23.byte ...
    #TODO: import these sizes from syc file, for now - they are defined here
    #TODO: create computed crc and match with readout crc
    crc_size = 4
    module_number_size = 2
    fwid_item_size = 8
    fwid_item_max_count = 10

    readout_crc_value = node.read_data(production_data_dict['ProdFwIdAddr'], crc_size)
    crc_value = int(''.join(format(x,'02X') for x in readout_crc_value[::-1]),16)
    print("{:15}: 0x{:X}".format("FWID CRC:", crc_value))
    print(25 * '-')
    # read all 10 items from FWID table and print them to console
    for fwid_item in range(fwid_item_max_count):
        readout_value = []
        address_offset = crc_size + ((module_number_size + fwid_item_size) * fwid_item)
        readout_value = node.read_data(production_data_dict['ProdFwIdAddr'] + address_offset, module_number_size + fwid_item_size)
        readout_module_number_value = readout_value[0:2]
        readout_fwid_value = readout_value[2:10]
        module_number_id = int(''.join(format(x,'02X') for x in readout_module_number_value[::-1]),16)
        print("{:15}#{:2}: 0x{:X}".format("Module ID:", fwid_item + 1, module_number_id))
        print("{:15}#{:2}: {}".format("FWID:", fwid_item + 1, bytes(readout_fwid_value).hex().upper()))
        print(36 * '-')


# Read all production data by using ReadRequest32 message
def read_all_prod_data(production_data_dict):
    readout_value1 = node.read_data(production_data_dict['ProdTimeAddr'], production_data_dict['ProdTimeSize'])
    readout_value2 = node.read_data(production_data_dict['ProdTrIdAddr'], production_data_dict['ProdTrIdSize'])
    readout_value3 = node.read_data(production_data_dict['ProdHwIdAddr'], production_data_dict['ProdHwIdSize'])
    return {'Production Time': readout_value1, 'Tracing ID': readout_value2, 'HW ID': readout_value3}


# function prints all appliance data to the console
def print_all_appliance_data(round, data_dict):
    print_header(" " + str(round) + ". reading of appliance data")
    print("{:2}".format("ID"), "{:^3}".format("Len"), "{:^18s}".format("Response status"), "{:<64s}".format("The data of response message"))
    for key in sorted(data_dict):
        print("{:^2}".format(data_dict[key]["ID"]), "{:^3}".format(data_dict[key]["LengthType"]), "{:^18s}".format(data_dict[key]["ResponseStatus"]), "{:128s}".format(bytes(data_dict[key]["ReturnedData"]).hex().upper()))


# funtion reads all kind of appliance data
def read_all_appliance_data():
    returned_readout_data = {}
    for key in sorted(appl_data["1"]):
        appliance_item_number = int(key)
        status, readout_data = node.read_appliance_data(int(appliance_item_number))
        item_type = get_appliance_data_item_type(appliance_item_number)
        returned_readout_data[key] = {"ID":appliance_item_number, "LengthType": item_type, "ResponseStatus": status, "ReturnedData":readout_data}
    return returned_readout_data


# funtion returns type of appliance data - string or fixed length item
def get_appliance_data_item_type(appliance_item_id):
    if appliance_item_id in fixed_length_items:
        return "F"
    else:
        return "S"

# Test all production data by executing 3 testing stages, after each stage is checking data by
# reading them again and comparing with previous values
def test_all_prod_data():
    # read data at first, the production area should be empty
    # 1st Read, store and print production data
    empty_prod_data = read_all_prod_data(ProductionDataArea)
    print_all_prod_data(1,empty_prod_data)
    # check if data cell are empty
    for key in sorted(empty_prod_data):
        for item in empty_prod_data[key]:
            if item != empty_cell_value:
                prod_test_results[1]['Passed'] = False

    # 1st write production time
    print_header(" " + str(1) + ". writing of production data")
    print("Production time to be written: {}".format(prod_data["1"]["ProductionTime"]))
    status, written_data = node.write_prod_data(node.msg_id["WriteProdTimeReq"], node.msg_id["ProdTimeResp"], str_to_data(prod_data["1"]["ProductionTime"],"bcd"))
    print("Response status is {}.\nThe response message returns the data written: {}".format(status, bytes(written_data).hex().upper()))

    print("Production TR ID to be written: {}".format(prod_data["1"]["TracingID"]))
    status, written_data = node.write_prod_data(node.msg_id["WriteTracingIDReq"], node.msg_id["WriteTracingIDResp"], str_to_data(prod_data["1"]["TracingID"], "bcd"))
    print("Response status is {}.\nThe response message does not return any data.".format(status))

    print("Production HW ID to be written: {}".format(prod_data["1"]["HWID"]))
    status, written_data = node.write_prod_data(node.msg_id["WriteEcuConfHWReq"], node.msg_id["WriteEcuConfHWResp"], str_to_data(prod_data["1"]["HWID"], "hex"))
    print("Response status is {}.\nThe response message does not return any data.".format(status))
    crc_computed = binascii.crc32(bytes(str_to_data(prod_data["1"]["HWID"], "hex")))
    print("Expected CRC32 value for HW ID is: 0x{:X}".format(crc_computed))

    # 2nd Read and print production data
    written_prod_data = read_all_prod_data(ProductionDataArea)
    print_all_prod_data(2, written_prod_data)
    for key in sorted(written_prod_data):
        if empty_prod_data[key] == written_prod_data[key]:
            prod_test_results[2]['Passed'] = False

    crc_readout_part = written_prod_data['HW ID'][0:4]
    crc_value = int(''.join(format(x,'02X') for x in crc_readout_part[::-1]),16)
    print("CRC32 value readout: 0x{:X}".format(crc_value))
    print("CRC's are {}".format("matching." if crc_computed == crc_value else "different."))

    # 2nd write production time
    print_header(" " + str(2) + ". writing of production data")
    print("Production time to be written: {}".format(prod_data["2"]["ProductionTime"]))
    status, written_data = node.write_prod_data(node.msg_id["WriteProdTimeReq"], node.msg_id["ProdTimeResp"], str_to_data(prod_data["2"]["ProductionTime"],"bcd"))
    print("Response status is {}.\nThe response message returns the data written: {}".format(status, bytes(written_data).hex().upper()))

    print("Production TR ID to be written: {}".format(prod_data["2"]["TracingID"]))
    status, written_data = node.write_prod_data(node.msg_id["WriteTracingIDReq"], node.msg_id["WriteTracingIDResp"], str_to_data(prod_data["2"]["TracingID"], "bcd"))
    print("Response status is {}.\nThe response message does not return any data.".format(status))

    print("Production HW ID to be written: {}".format(prod_data["2"]["HWID"]))
    status, written_data = node.write_prod_data(node.msg_id["WriteEcuConfHWReq"], node.msg_id["WriteEcuConfHWResp"], str_to_data(prod_data["2"]["HWID"], "hex"))
    print("Response status is {}.\nThe response message does not return any data.".format(status))
    crc_computed = binascii.crc32(bytes(str_to_data(prod_data["2"]["HWID"], "hex")))
    print("Expected CRC32 value for HW ID is: 0x{:X}".format(crc_computed))

    # 3rd Read and print production data
    checked_prod_data = read_all_prod_data(ProductionDataArea)
    print_all_prod_data(3, checked_prod_data)
    for key in sorted(checked_prod_data):
       if written_prod_data[key] != checked_prod_data[key]:
            prod_test_results[3]['Passed'] = False

    crc_readout_part = checked_prod_data['HW ID'][0:4]
    crc_value = int(''.join(format(x,'02X') for x in crc_readout_part[::-1]),16)
    print("CRC32 value readout: 0x{:X}".format(crc_value))
    print("CRC's are {}".format("matching." if crc_computed == crc_value else "different."))

    return True


# Function calls 3 production messages to get production data while application is running
def call_prod_msg_in_app():
    status, readout_prod_time = node.read_prod_data(request_message_id=node.msg_id["ReadProdTimeReq"], response_message_id=node.msg_id["ProdTimeResp"])
    print_production_data(readout_prod_time, "Production Time")
    status, readout_tr_id = node.read_prod_data(request_message_id=node.msg_id["EcuUniqIdReq"], response_message_id=node.msg_id["EcuUniqIdResp"])
    print_production_data(readout_tr_id[2:], "Tracing ID")
    status, readout_ecu = node.read_prod_data(request_message_id=node.msg_id["EcuConfReadReq"], response_message_id=node.msg_id["EcuConfReadResp"])
    print_production_data(readout_ecu[3:], "ECU Config")


# Test all appliance data by executing 3 testing stages, after each stage is checking data by
# reading them again and comparing with previous values
def test_appliance_data():
    # 1st Read of appliance data
    empty_appliance_data = read_all_appliance_data()
    print_all_appliance_data(1,empty_appliance_data)
    # check if appliance data are empty
    for key in sorted(empty_appliance_data):
        # checking through all appliance items ID: 0,1,2...9
        # for 'F' - fixed length items will be returned content of empty cells that is compared one by one
        # for 'S' - variable length /string/ items will be returned content no data
        if empty_appliance_data[key]["LengthType"] == "F":
            for item in empty_appliance_data[key]["ReturnedData"]:
                if item != empty_cell_value:
                    appliance_test_results[1]['Passed'] = False
        else:
            if empty_appliance_data[key]["ReturnedData"] != []:
                appliance_test_results[1]['Passed'] = False


    # 1st Write of appliance data
    print_header(" " + str(1) + ". writing of appliance data")
    print("{:2}".format("ID"), "{:^3}".format("Len"), "{:^18s}".format("Response status"), "{:<64s}".format("The written data"))
    for key in sorted(appl_data["1"]):
        appliance_item_number = int(key)
        item_type = get_appliance_data_item_type(appliance_item_number)
        if not int(key) in string_items:
            prepared_data = str_to_data(appl_data["1"][key], "hex")
        else:
            prepared_data = str_to_data(appl_data["1"][key], "str")
        status = node.write_appliance_data(appliance_item_number, prepared_data)
        print("{:2}".format(key), "{:^3}".format(item_type), "{:^18s}".format(status), "{:<64s}".format(appl_data["1"][key]))

    # 2nd Read after 1st write of appliance data
    written_appliance_data = read_all_appliance_data()
    print_all_appliance_data(2, written_appliance_data)
    for key in sorted(written_appliance_data):
        if empty_appliance_data[key] == written_appliance_data[key]:
            appliance_test_results[2]['Passed'] = False

   # 2nd Write  of appliance data
    print_header(" " + str(2) + ". writing of appliance data")
    print("{:2}".format("ID"), "{:^3}".format("Len"), "{:^18s}".format("Response status"), "{:<64s}".format("The written data"))
    for key in sorted(appl_data["2"]):
        appliance_item_number = int(key)
        item_type = get_appliance_data_item_type(appliance_item_number)
        if not int(key) in string_items:
            prepared_data = str_to_data(appl_data["2"][key], "hex")
        else:
            prepared_data = str_to_data(appl_data["2"][key], "str")
        status = node.write_appliance_data(appliance_item_number, prepared_data)
        print("{:2}".format(key), "{:^3}".format(item_type), "{:^18s}".format(status), "{:<64s}".format(appl_data["2"][key]))

    # 3rd Read after 2nd write of appliance data
    checked_appliance_data = read_all_appliance_data()
    print_all_appliance_data(3, checked_appliance_data)
    for key in sorted(checked_appliance_data):
       if written_appliance_data[key] != checked_appliance_data[key]:
            appliance_test_results[3]['Passed'] = False
    return True

# funtion do all tests while programmer is running in a MCU, then it resets a MCU
def execute_tests_in_programmer():
    print_header("Starting test of production data. The FWU3 build is used.")
    # Perform test of production data in FWU3 build while a Programmer is running, returns True
    status = test_all_prod_data()
    # Reset MCU after test and read production data while an application is running
    print_header(" Resetting the MCU ")
    node.reset()
    node.ping(timeout)
    print(node.read_id_string('\n'))
    print_header("Starting test of FWID data. The FWU3 build is used.")
    read_fwid_table(ProductionDataArea)
    #TODO: check if this call is necessary
    #if production_messages_in_app == True:
    #    call_prod_msg_in_app()
    return status


# Show/print input parameters and calculated variables - important when checking the output
def print_debug_parameters():
    print("Platform : ", mcu_platform)
    print("The smallest writable block size specfic for the platform: ", "{:8X}H".format(write_block_size))
    print("Is FWU3 build? : ",hasProgrammer)
    print("Is FWU1 build? : ",production_messages_in_app)
    print("Is FWU1 build + appliance data in application (EFR32 only)? : ",appliance_messages_in_app)
    print("Maximum Silent mode time : ",max_silent_mode_time)
    print("Timeout : ",timeout)
    print("Prod. data Timestamp size:".ljust(40," "),"{:>8X}H".format(production_time_size))
    print("Prod. data TR ID size:".ljust(40," "),"{:>8X}H".format(production_tracing_id_size))
    print("Prod. data HW ID size:".ljust(40," "),"{:>8X}H".format(production_hw_id_size))
    if hasProgrammer == True:
        print("Prod. data FW ID table size:".ljust(40," "),"{:>8X}H".format(production_fwid_table_size))
    print("Aligned prod. data Timestamp size:".ljust(40," "),"{:>8X}H".format(aligned_size(write_block_size,production_time_size)))
    print("Aligned prod. data TR ID size:".ljust(40," "),"{:>8X}H".format(aligned_size(write_block_size,production_tracing_id_size)))
    print("Aligned prod. data HW ID size:".ljust(40," "),"{:>8X}H".format(aligned_size(write_block_size,production_hw_id_size)))
    if hasProgrammer == True:
        print("Aligned prod. data FW ID table size:".ljust(40," "),"{:>8X}H".format(aligned_size(write_block_size,production_fwid_table_size)))
    # Printing start addresses
    print("Production data Timestamp address:".ljust(40," "),"{:>08X}H".format(production_time_address))
    print("Production data TR ID address:".ljust(40," "),"{:>08X}H".format(production_tracing_id_address))
    print("Production data HW ID address:".ljust(40," "),"{:>08X}H".format(production_hw_id_address))
    if hasProgrammer == True:
        print("Production data FW ID table address:".ljust(40," "),"{:>08X}H".format(production_fwid_table_address))
    if appliance_messages_in_app == True:
        print("Appliance data address:".ljust(40," "),"{:>08X}H".format(appliance_data_address))

    print("ECUstatus     : ", ECUstatus)
    print("ECU read_data : ", bytes(read_data).hex().upper())
    print("ECUobjCount   : ",ECUobjCount)
    print("Testing data will be used (from JSON file):")
    for key in prod_data["1"]:
        print("{:14s} : {}".format(key, prod_data["1"][key]))
    if appliance_messages_in_app == True:
        for key in appl_data["1"]:
            print("{} : {}".format(key, appl_data["1"][key]))


# Initialize argument parser
argP = PsArgumentParser(description='Test of production data')
argP.add_argument("node", required=True, help="DBus node address")
argP.add_argument("std_baud", default="9600", required=False, help="DBus baudrate")
argP.add_argument("programmer", default=False, action="set_true", help="Set only in case of FWU3 with programmer included")
argP.add_argument("proddata", default=False, action="set_true", help="Set only in case of FWU1 with production messages enabled")
argP.add_argument("pdjf", dest="production_data_json_file", default=None, required=True, help="A default production data JSON file.")
argP.add_argument("appldata", default=False, action="set_true", help="Set only in case of FWU1 and EFR32 platform with appliance messages enabled")
argP.add_argument("adjf", dest="appliance_data_json_file", default=None, required=False, help="A default appliance data JSON file.")
argP.add_argument("timeout", default="5", help="Timeout for checking connection with MCU")
argP.add_argument("max_silent_mode_time", default="0.1", help="Maximum time of silent mode transition")
argP.add_argument("fwu_otp_start_addr", default="0", help="The start address of OTP area in the flash in Hex)")
argP.add_argument("prod_time_size", default="0", help="The size of production time in the flash in Hex)")
argP.add_argument("prod_trid_size", default="0", help="The size of production data Unique Id (Tracing ID) in the flash in Hex)")
argP.add_argument("prod_hwid_size", default="0", help="The size of production data HW ID in the flash in Hex)")
#argP.add_argument("prod_time_addr", default="0", help="The address of production time in the flash in Hex)")
#argP.add_argument("prod_trid_addr", default="0", help="The address of production data Unique Id (Tracing ID) in the flash in Hex)")
#argP.add_argument("prod_hwid_addr", default="0", help="The address of production data HW ID in the flash in Hex)")
argP.add_argument("prod_fwid_table_size", default="0", help="The size of production data FW ID in the flash in Hex)")
argP.add_argument("platform", required=False, help="MCU platform.")
argP.add_argument("write_block_size", required=True, help="The smallest writable block size specfic for platform.")
args = argP.parse_args(cmdParameters)

# Exit codes
# OK = 0 not needed to specify
PROD_DATA_AREA_IS_NOT_EMPTY = 1
PROD_DATA_AREA_WRITE_FAILED = 2
PROD_DATA_AREA_IS_REWRITTEN = 3
APPLICATION_IS_RUNNING      = 4
NO_PROD_MSG_IN_APPLICATION  = 5
APPLIANCE_DATA_AREA_IS_NOT_EMPTY = 6
APPLIANCE_DATA_AREA_WRITE_FAILED = 7
APPLIANCE_DATA_AREA_IS_REWRITTEN = 8

# Initialize variables, convert those they are passed as string to numbers
udaNode = 0xC0
dbusNode = int(args["node"], 0)
stdBaud = int(args["std_baud"])
timeout = int(args["timeout"])
max_silent_mode_time = float(args["max_silent_mode_time"])
hasProgrammer = args["programmer"]
production_messages_in_app = args["proddata"]
production_data_json_file = args["production_data_json_file"]
appliance_messages_in_app = args["appldata"]
appliance_data_json_file = args["appliance_data_json_file"]
#production_time_address = args["prod_time_addr"]
production_time_size = int(args["prod_time_size"],16)
#production_tracing_id_address = args["prod_trid_addr"]
production_tracing_id_size = int(args["prod_trid_size"],16)
#production_hw_id_address = args["prod_hwid_addr"]
production_hw_id_size = int(args["prod_hwid_size"],16)
production_fwid_table_size = int(args["prod_fwid_table_size"],16)
fwu_otp_start_addr = int(args["fwu_otp_start_addr"],16)
mcu_platform = args["platform"]
write_block_size = int(args["write_block_size"],16)

# calculated variables: start address of each production data item is needed for read message ReadRequest32
# the order of calculation matches with the order of placement variables in file fwu_opt_memory.c
production_time_address = fwu_otp_start_addr
production_tracing_id_address = production_time_address + aligned_size(write_block_size,production_time_size)
production_hw_id_address = production_tracing_id_address + aligned_size(write_block_size,production_tracing_id_size)
# the FWID start address is calculated only in case of FWU3
if hasProgrammer == True:
    production_fwid_table_address = production_hw_id_address + aligned_size(write_block_size,production_hw_id_size)
# the Appliance data start address is calculated only in case of FWU1 and appliance messages in application enabled (EFR32)
if appliance_messages_in_app == True:
    appliance_data_address = production_hw_id_address + aligned_size(write_block_size,production_hw_id_size)

# to sign if test was performed or not
test_performed = False
test_appliance_msg_performed = False

# The list of type id number of Appliance data items with fixed length only
fixed_length_items = [0, 3, 6, 9, 10]
# The list of type id number of Appliance data items with string type data only
string_items = [1, 2, 3, 4, 5, 6, 7, 8]

# define empty cell value. It is platform depending.
if (mcu_platform == 'psoc4') or (mcu_platform == 'sh30f'):
    empty_cell_value = 0
else:
    empty_cell_value = 255

# Initialize testing data from JSON files
prod_data = get_data_from_json(production_data_json_file)
if appliance_messages_in_app == True:
    appl_data = get_data_from_json(appliance_data_json_file)

# Define production data area variable from input parameters and calculated variables
if hasProgrammer == True:
    ProductionDataArea = {
        'ProdTimeAddr':production_time_address,
        'ProdTimeSize':production_time_size,
        'ProdTrIdAddr':production_tracing_id_address,
        'ProdTrIdSize':production_tracing_id_size,
        'ProdHwIdAddr':production_hw_id_address,
        'ProdHwIdSize':production_hw_id_size,
        'ProdFwIdAddr':production_fwid_table_address,
        'ProdFwIdSize':production_fwid_table_size
        }
else:
    ProductionDataArea = {
        'ProdTimeAddr':production_time_address,
        'ProdTimeSize':production_time_size,
        'ProdTrIdAddr':production_tracing_id_address,
        'ProdTrIdSize':production_tracing_id_size,
        'ProdHwIdAddr':production_hw_id_address,
        'ProdHwIdSize':production_hw_id_size,
        }

# Initialize values of results as a dictionary
prod_test_results = dict({1:{'Passed': True, 'Name': '1-st read of PD: data shall be empty (0xFF values).'},
                          2:{'Passed': True, 'Name': '2-nd read of PD (after 1-st write): data shall be different from 1-st read.'},
                          3:{'Passed': True, 'Name': '3-rd read of PD (after 2-nd write): data shall match 2-nd read.'}
                         })

appliance_test_results =  dict({1:{'Passed': True, 'Name': '1-st read of AD: data shall be empty (0xFF values or empty string).'},
                                2:{'Passed': True, 'Name': '2-nd read of AD (after 1-st write): data shall be different from 1-st read.'},
                                3:{'Passed': True, 'Name': '3-rd read of AD (after 2-nd write): data shall match 2-nd read.'}
                                })

# Initialize UDA connection
setup_dbus_com(stdBaud)

# Initialize DBus node
node = DBusNode(dbusNode, udaNode)

# Inspect run mode
ECUstatus, read_data = node.read_prod_data(request_message_id=node.msg_id["EcuConfReadReq"], response_message_id=node.msg_id["EcuConfReadResp"])
ECUobjCount = read_data[2]

#print debug info
print_debug_parameters()

# test silent mode transition request and response messages independently from a build type.
test_silent_mode(max_silent_mode_time)


# checking parameters
if hasProgrammer == True:
    # FWU3 build
    if ECUstatus == 1:
        # ECUstatus = DBPL_EcuStatusUpdateModeActive => a Programmer is running, the desired condition
        test_performed = execute_tests_in_programmer()
    else:
        print("\nAn Application is running. Switching to Programmer...\n")
        node.switch_to_programmer()
        node.ping()
        # Inspect run mode again
        ECUstatus, read_data = node.read_prod_data(request_message_id=node.msg_id["EcuConfReadReq"], response_message_id=node.msg_id["EcuConfReadResp"])
        ECUobjCount = read_data[2]
        if ECUstatus == 1:
            test_performed = execute_tests_in_programmer()
        else:
            # ECUstatus = DBPL_EcuStatusOK => application is running, undesirable condition
            print("\nAn Application is running. To test production data the Programmer has to be runnning.\nMaybe the flashed binary is not a RELEASE binary.\n")
            sys.exit(APPLICATION_IS_RUNNING)
else:
    # FWU1 build
    if production_messages_in_app == True:
        # FWU1 build + production messages are in application
        print_header("Starting the test of production data messages. The FWU1 build is used.")
        # Perform test of production data in FWU1 build
        test_performed = test_all_prod_data()
        print_header(" Read out all production data by production messages in application.")
        call_prod_msg_in_app()

        #if ECUobjCount == 1:
        if appliance_messages_in_app == True:
            # FWU1 build + production + appliance messages are in application
            print_header("Starting the test of appliance data messages. The FWU1 build is used.")
            # Perform test of appliance data in FWU1 build
            test_appliance_msg_performed = test_appliance_data()
    else:
        print("\nProduction messages are not enabled in application. Nothing to test.\n")
        sys.exit(NO_PROD_MSG_IN_APPLICATION)



# Testing final results if tests was performed
if test_performed:
    # Result summary for production data test
    print("\n")
    print_header("Production Data (PD) Tests Results Summary")
    print("{:5s}".format("Stage"), "{:7s}".format("Passed"), "{:20s}".format("Description"))
    for stage in prod_test_results:
        print("{:5s}".format(str(stage)), "{:7s}".format(str(prod_test_results[stage].get('Passed'))), "{:20s}".format(prod_test_results[stage].get('Name')))
    print_footer("Summary table end.")
    print("\n")

    if test_appliance_msg_performed:
        # Result summary for appliance data test
        print_header("Appliance Data (AD) Tests Results Summary")
        print("{:5s}".format("Stage"), "{:7s}".format("Passed"), "{:20s}".format("Description"))
        for stage in appliance_test_results:
            print("{:5s}".format(str(stage)), "{:7s}".format(str(appliance_test_results[stage].get('Passed'))), "{:20s}".format(appliance_test_results[stage].get('Name')))
        print_footer("Summary table end.")

    # Testing all 3 stages of production data message test
    if prod_test_results[1].get('Passed') == False:
        sys.exit(PROD_DATA_AREA_IS_NOT_EMPTY)

    if prod_test_results[2].get('Passed') == False:
        sys.exit(PROD_DATA_AREA_WRITE_FAILED)

    if prod_test_results[3].get('Passed') == False:
        sys.exit(PROD_DATA_AREA_IS_REWRITTEN)

    # Testing all 3 stages of appliance data message test
    if appliance_test_results[1].get('Passed') == False:
        sys.exit(APPLIANCE_DATA_AREA_IS_NOT_EMPTY)

    if appliance_test_results[2].get('Passed') == False:
        sys.exit(APPLIANCE_DATA_AREA_WRITE_FAILED)

    if appliance_test_results[3].get('Passed') == False:
        sys.exit(APPLIANCE_DATA_AREA_IS_REWRITTEN)

# End of code
