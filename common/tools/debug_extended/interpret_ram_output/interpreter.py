"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : interpreter.py
Date of creation : 2022-11-11
Author Name/Dept : Fabian Deml (GDE-EDS6)
Organization     : GDE-EDS
Description      : Python script to convert hex dump into ASCII string
"""

# This python script helps to interpret the debug data from the RAM interface of the debug extended component of the Framework.
# 
# The RAM interface of the debug_extended component saves the debug data in a global uint8_t array, which is used as a ring buffer.
# Using a debugger of your choice, you can save the RAM array into a file to analyze the data.
# This python script converts the debug data with a special format (HEX386 format) into a ordered string (ASCII)
#
# This program takes the file "DebugData_RingBuffer.hex" and "DebugData_RingBufferIndex.hex" as input.
# DebugData_RingBuffer.hex contains the buffer variable DBGI_RamRingBuffer in HEX386 format.
# DebugData_RingBufferIndex.hex contains the buffer index DBGI_ringBuff.read in HEX386 format.
# For a example, have a look at the initial files.
#
# After calling the interpreter.py, the file 'output.log' is filled.
# The data of the RAM interface can be read ASCII coded (string) and ordered.

# Read the raw input data from file
def readRingBufferRawData(file):
    with open(file, 'r') as inputFile:
        RB_RawData = inputFile.readlines()
    inputFile.close()
    return RB_RawData

# Remove overhead at the beginning and end of each line of the hex-format
def removeOverhead(OverheadedData): 
    CleanData = 0
    for Index in range(0, len(OverheadedData)):
        OverheadedData[Index] = OverheadedData[Index][9:-3]
        CleanData = ''.join(OverheadedData)
    return CleanData

# Divide the input data string into single parts with length DataLenInBytes and type int
def convertRingBuffer(DataString, DataLenInBytes):
    LineEnding = 11
    NullCharacter = 00
    Data = removeOverhead(DataString)
    DataDivided = [Data[i:i+DataLenInBytes] for i in range(0, len(Data), DataLenInBytes)]
    for Index in range(0, len(DataDivided)):
        DataDivided[Index] = int(DataDivided[Index], 16)
    # Remove hex file ending and beginning
    DataDivided.pop(0)
    try:
        DataDivided.remove(NullCharacter)
        DataDivided.remove(LineEnding)
    except:
        pass
    return DataDivided
    
# Put the data of the ring buffer in order, if it was once completely filled with data. Otherwise no change is necessary
def orderDataFromRingBuffer(RawData, RB_Index):
    RingInRingBuffer = 0
    RawDataInOrder = RawData

    # Check if the whole ring buffer was filled with data yet
    for Index in range(RB_Index, len(RawData)):
        if RawData[Index] != chr(0x00):
            RingInRingBuffer = 1

    if RingInRingBuffer == 1:
        RawDataInOrder = RawData[RB_Index:] + RawData[:RB_Index]
    return RawDataInOrder 

# Convert int data list to ASCII string (e.g. 65 --> A) 
def convertIntToAscii(RawDataInOrder):
    ConvertedData = [int(x) for x in RawDataInOrder]
    ConvertedData = ''.join(chr(i) for i in ConvertedData)
    return ConvertedData
    
# Write data into output file
def writeToOutputFile(ConvertedData, OutputFile):
    OutputFile = open(OutputFile, 'w')
    OutputFile.write(ConvertedData)
    OutputFile.close()

# Convert the index for the ring buffer
def convertRingBufferIndex(RB_RawDataIndex):
    Lines = removeOverhead(RB_RawDataIndex)
    Lines = str(Lines).zfill(10)          # Normalize the input to the length of 10. Then it can be handled equal for Keil and Ozone
    ArrayIndex = Lines [6:8] + Lines[4:6] # Turn data from little to big endianess
    return int(ArrayIndex, 16)

def removeHexEnding(IntegerData):
	CleanData = list(map(lambda x: x.replace("11", ""), ConvertedData))
	return CleanData


if __name__ == '__main__':
    # Read and convert input data from file
    RB_RawData = readRingBufferRawData('DebugData_RingBuffer.hex')
    RB_ConvertedRingBuffer = convertRingBuffer(RB_RawData, 2)
    RB_AsciiData = convertIntToAscii(RB_ConvertedRingBuffer)

    # Read and convert RB index from file
    RB_RawDataIndex = readRingBufferRawData('DebugData_RingBufferIndex.hex')
    RB_ConvertedRingBufferIndex = convertRingBufferIndex(RB_RawDataIndex)

    # Change the order of the output by using the RB_Index
    RB_Output = orderDataFromRingBuffer(RB_AsciiData, RB_ConvertedRingBufferIndex)
    print(RB_Output)

    # Save data in output file
    writeToOutputFile(RB_Output, 'output.log')

