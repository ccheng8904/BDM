#Phyton 2.7 script
import re, os, sys
INFO="Calculate Sum over all data\n"
INFO+="Script for python 2.7\n"
INFO+="call example: python27.exe script.py LC_UIM_O_MAIN.hex"

File = ""
memorySize = 0
memoryStart = None
memoryEnd = None

TypeDefinition = ["Data Record", "End of File Record", "Extended Segment Address Record", "Start Segment Address Record", "Extended Linear Address Record", "Start Linear Address Record"]

def GetMemorySize():
  global File
  global memorySize
  global memoryStart
  global memoryEnd
  
  f = open(File, 'r')
  
  for line in f: #run through .hex file
    line = line.strip()
    if ":" == line[0]:
      Size = int(line[1:3],16)
      Type = int(line[7:9],16)
      Address = int(line[3:7],16)
      
      if memoryStart is None:
        memoryStart = Address
      if (TypeDefinition[Type]=="Extended Linear Address Record"): #break operation in case flash area is done
        Data = int(line[9:-2])
        if Data>8191:
          break
      else:
        memorySize += Size
  memoryEnd = memorySize + memoryStart


def CalculateCheckSum():
  global File
  global memoryEnd
  ChipID = None
  f = open(File, 'r')
  count = 0
  memCount = 0
  SpecialAreaFound = False
  
  for line in f:
  
    line = line.strip()
    if ":" == line[0]:
    
      Size = int(line[1:3],16)
      Type = int(line[7:9],16)
      Address = int(line[3:7],16)
      Checksum = int(line[-2:],16)
      Data = line[9:-2]
      
      ValueList = re.findall('..',line[9:-2])
      
      if (len(ValueList) == Size):
      
        if ((False != SpecialAreaFound)&(memCount >= (memorySize-1))):
          if ("Data Record" == TypeDefinition[Type]):
            if (12 <= Size):
              ChipID = ''.join(str(i) for i in ValueList[2:6])
              if ("00000000" != ChipID):
                ChipID = int(ChipID,16)
        else:
          if ("Data Record" == TypeDefinition[Type]):
            for value in ValueList:
              value = int(value, 16)
              count += value
              memCount += 1
              SpecialAreaFound = False
          else:
            if ("Extended Linear Address Record" == TypeDefinition[Type]):
              SpecialAreaFound = True
          
      else:
        print("Invalid address line found - size inequal to definition")
        print(line)
  f.close()
  if ChipID is None:
    print("Skipping PSoC checksum, no metadata found")
  elif ("0" != ChipID):
    Checksum = count % 65536 #modulo hex block size

    ChecksumHex = format('%04X' % Checksum)
    CustomChecksum = count + ChipID
    CustomChecksumHex = format('%08X' % CustomChecksum)
    print("     Binary Sum: "          + format('0x%08X' % count) + " - " + format('%dd' % count))
    print(" 16Bit Checksum: " + "    " + format('0x%04X' % Checksum) + " - " + format('%dd' % Checksum))
    print("        Chip ID: "          + format('0x%08X' % ChipID))
    print("Custom Checksum: "          + format('0x%08X' % CustomChecksum) + " - " + format('%dd' % CustomChecksum))

    CheckSumFill = " -F(0x90300000):0x" + ChecksumHex[0:2] + " -F(0x90300001):0x" + ChecksumHex[-2:]
    CustomCheckSumFill = " -F(0x90500008):0x" + CustomChecksumHex[0:2]
    CustomCheckSumFill += " -F(0x90500009):0x" + CustomChecksumHex[2:4]
    CustomCheckSumFill += " -F(0x9050000A):0x" + CustomChecksumHex[4:6]
    CustomCheckSumFill += " -F(0x9050000B):0x" + CustomChecksumHex[-2:]
    
    if memoryEnd % 0x80 is not 0:
      memoryEndAligned = int((memoryEnd / 0x80) + 1) * 0x80
      CustomCheckSumFill += " -F(0x{:08x},0x{:08x}):0x00".format(memoryEnd, memoryEndAligned - 1)
    
    os.system(Patchbin + " -silent0 -ih(0,0x90600000)" + File + CheckSumFill + CustomCheckSumFill +" -oh()" + File)
  else:
    print("ChipID not found - deleted hex file to identify that error immediately")
    DeleteFile="del " + File
    print(DeleteFile)
    os.system(DeleteFile)
  pass

if (2 < len(sys.argv)):
  Patchbin = sys.argv[1]
  File = sys.argv[2]
  GetMemorySize()
  CalculateCheckSum()
else:
  print(INFO)
  print("\nParameter is missing!")