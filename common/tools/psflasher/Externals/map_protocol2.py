'''
Created on 12. 4. 2018

@author: dolinsky
'''

__Version__ = '1.3.1'

#import time

from FlashGordon import core
#from FlashGordon import bin_file_handler
from FlashGordon import error_codes
from  FlashGordon.script_api import *
#import coolib
import threading


#===============================================================================
# All in BigEndian
#===============================================================================
def int_to_List_LittleEnd(myInt,nBytes):return [(myInt >> (8 * i)) & 0xFF for i in range(nBytes)]
def int_to_List_BigEndian(myInt,nBytes):return [(myInt >> (8 * i)) & 0xFF for i in range(nBytes - 1, -1, -1)]


class MapStatus:
    MAP_ST_OK = 0x0  #Request OK and processed without errors
    MAP_ST_WRONG_PARAM = 0x01  #At  least one parameter has a wrong value
    MAP_ST_MISMATCH_PARAM = 0x02  #Request has wrong number of parameters
    MAP_ST_UNKNOWN_REQ = 0x03  #Request ID is unknown
    MAP_ST_SEQUENCE_CNT_ERROR = 0x04  #Sequence counter is wrong
    MAP_ST_PAYLOAD_SIZE_EXCEEDED = 0x05  #Not enough buffer to send response
    MAP_ST_SETUP_MODULE_ERROR = 0x06  #Fail to setting up module or wrong module set up
    MAP_ST_ACTION_PROHIBITED_ERROR = 0x07  #E.g. write module with reading access only
    MAP_ST_OUT_OF_RANGE_ERROR = 0x08  #Read or write request exceeds boarders of module
    MAP_ST_ERASE_ERROR = 0x09  #Attempt to erase module was not successful
    MAP_ST_WRITE_ERROR = 0x0A  #Error in attempting to write into erased module
    MAP_ST_ERROR = 0xFF  #Any error not covered by other statuses

    
class RequestID:
    GetDeviceInfo = 1
    SetupModule = 3
    ReadModule = 5
    StartWrite = 7
    WriteModule = 8
    EndWriteModule = 10

    
class ResponseID:
    GetDeviceInfo = 2
    SetupModule = 4
    ReadModule = 6
    WriteModule = 9


class ModuleAttribute():

    def __init__(self,):
        self.ModuleNumber = None
        self.RxPayload = None
        self.MaxTimeout = 1000


def make_Hsi_Header(protocolType,payLoad):
    totalLength = 8 + len(payLoad)
    hsiProtVerMj = 2
    statusCode = 0
    reserved = 0
    dataList = int_to_List_LittleEnd(totalLength,4)
    dataList.extend([hsiProtVerMj,protocolType,statusCode,reserved])
    return dataList


class DBus22Interface:
    NUM_OF_TARGETS = 16
    Stop = False
    baud = 9600
    Counter = 0
    duration = 35000
    Trigering = True

    def __init__(self,MasterNode = 0x0C,SlaveNode = 0x11):
        self.sapAddress = MasterNode
        self.device = SlaveNode
    
    def BTrigger(self,Node,Time = 35000):
    
        #print("############################## BTrigger ###########################")
        Time = int(Time / 10)
        Time = [(Time >> (8 * i)) & 0xFF for i in range(2 - 1, -1, -1)]
        sendDBus2(Node,[0xF5,0x0A],Time)
        return  
    
    def Triggerring(self,freq):
        while self.Trigering:
            self.Stop = True
            time.sleep(freq)

    def executeCommand(self,protocolType,payLoad,responseBuffSize,responseTimeout):
        if self.Stop:
            setDBus2HwUart(self.baud)
            self.BTrigger(self.device,self.duration)
            self.Stop = False
        dataList = make_Hsi_Header(protocolType,payLoad)
        dataList.extend(payLoad)
        MessageID = [0xF5,0x0C]
        message = [self.sapAddress]
        message.extend(dataList)
        self.Counter += len(message)
        sendDBus2(self.device,MessageID,message)
        #print('>> [HSI] ' + ' '.join([format(d,'02X') for d in message[1:9]]) + ' [MAP] ' + ' '.join([format(d,'02X') for d in payLoad[:]]))        
        respDict = core.getInterface().receiveDBus2(responseTimeout)
        if respDict == None:
            raise Exception(9999,'No response receive.')
        elif respDict['ackType'] == 0x13:
            #print('<< DBus2AckCorruptFrame  ' + ' '.join([format(d,'02X') for d in respDict['data']]))
            #print('ACK:',hex(respDict['ackType']))
            sendDBus2(self.device,MessageID,message)
            respDict = core.getInterface().receiveDBus2(responseTimeout)
            if respDict['ackType'] == 0x13:
                print('ACK:',hex(respDict['ackType']))
                raise Exception(9999,'<< DBus2AckCorruptFrame  ' + ' '.join([format(d,'02X') for d in respDict['data']]))
            elif respDict['ackType'] == 0xA:
                pass
            else:
                raise Exception(9999,'Problem with receive PROTOCOL message.')
        
        data = respDict['data'][2:]
        #print('<< [HSI] ' + ' '.join([format(d,'02X') for d in data[:8]]) + ' [MAP] ' + ' '.join([format(d,'02X') for d in data[8:]]))
        hsiHeader = data[:8]
        statusCode = hsiHeader[6]
        response = data[8:]        
        return 0,statusCode,response


class MAP_Device:
    protocolIdentifier = 0x11
    mapHeaderReqSize = 2
    mapHeaderResSize = 3
    ModAtt = ModuleAttribute()
    lastOffset = 0
    printStep = 0x400
    
    def __init__(self,MasterNode,SlaveNode,interface = None):
        self.commandSeqCnt = 0
        self.timeout = 1000
        self.DeviceInfo = {}
        self._logger = core.get_logger(self)
        if interface == None:
            self.interface = DBus22Interface(MasterNode,SlaveNode)
        else:
            self.interface = interface
        
        self.sapAddress = MasterNode
        self.device = SlaveNode
       
    def _getInt16(self,data):
        return (data[0] << 8) + data[1]

    def _getInt32(self,data):
        return (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3]
    
    def _addDataToList(self,value,bytecount,data):
        return [(value >> (8 * i)) & 0xFF for i in range(bytecount - 1, -1, -1)] + data

    def _addHeaderToList(self,commandId,data):
        
        data = self._addDataToList(self.commandSeqCnt,1,data)
        data = self._addDataToList(commandId,1,data)
        #print('_addHeaderToList',data)
        return data
    
    def _executeCommand(self,commandId,data,responseBufferSize,timeout = 4000):
        
        #print('data _executeCommand',data)
        
        status,responseStatusCode,response = self.interface.executeCommand(self.protocolIdentifier,  #data,
                                                                             self._addHeaderToList(commandId,data),
                                                                             responseBufferSize,timeout)
        
        #response=responseStatusCode['response']
        # StatusCode from interface (dll)
        if status != 0:
            status &= 0xFFFFFFFF
            msg = 'ExecuteCommandTransportStatusCodeError: 0x{}'.format(format(status,'08X'))
            core.warning(msg)
            self._logger.error(msg)
            core.LastErrorStatus = status
            raise Exception(*error_codes.Error.ExecuteCommandTransportStatusCodeError)
        # StatusCode from HSI-Header
        if responseStatusCode != 0:
            msg = 'HSI-StatusCode: 0x{}'.format(format(responseStatusCode,'02X'))
            core.warning(msg)
            self._logger.error(msg)
            core.LastErrorStatus = responseStatusCode
            raise  Exception(*error_codes.Error.ExecuteCommandResponseStatusCodeError)
        # MAP-Response header #
        
        if len(response) < 3:
            raise Exception(666,'Len of MAP header error')
            
        if self.commandSeqCnt + 1 != response[1]:
            msg = 'WrongComandSequenceCounter: 0x{}, expected: 0x{}'.format(format(response[1],'02X'),format(self.commandSeqCnt + 1,'02X'))
            core.warning(msg)
            self._logger.error(msg)
            raise Exception(0x0303,"Wrong Command Sequence Counter")  # Wrong Sequence counter 
        # /MAP-Response Header #
        # return mapStatus, cmd specific data
        self.commandSeqCnt = response[1]
        if self.commandSeqCnt == 0xFF:
            self.commandSeqCnt = -1
        
        return response[2],response[self.mapHeaderResSize:]
    
    def SetBaudTrigerfreq(self,baud,duration,fequenci):
        
        self.interface.baud = baud
        self.interface.duration = duration * 1000
        TH = threading.Thread(target = self.interface.Triggerring,args = (fequenci,),daemon = True).start()
    
    def get_device_info(self,index):
        
        """
        The device info request asks for information about the slaves memory modules.
        """
        print("[get_device_info]")
        data = self._addDataToList(index,1,[])
        #print('DATA GET DEVICED',data)
        try:
            mapStatus,response = self._executeCommand(RequestID.GetDeviceInfo,data,2)
        except:
            print("Get Device Info Fail")
            raise Exception("Get Device Info Fail")
        self.commandSeqCnt += 1
        return mapStatus,response
    
    def setup_module(self,memoryModule):
        
        """
        The setup module request asks the slave to prepare a memory module to work on.
        This setup is needed to perform read and write operations on modules.
        """
        
        print('[SETUP MODULE ]')
        data = self._addDataToList(memoryModule,2,[])
        try:
            mapStatus,response = self._executeCommand(RequestID.SetupModule,data,self.mapHeaderReqSize)
        except:
            print('setup_module Fail')
            raise
        MaxRxPayload = self._getInt16(response[:2])
        MaxTimeout = response[2]
        if response[3:] != []:
            print(response[2:])
            raise Exception('Len of data to match')
        self.commandSeqCnt += 1
        self.ModAtt.ModuleNumber = memoryModule
        self.ModAtt.MaxTimeout = 2 ** MaxTimeout 
        self.ModAtt.RxPayload = MaxRxPayload
        #print('self.ModAtt.MaxTimeout',self.ModAtt.MaxTimeout)
        #time.sleep(self.ModAtt.MaxTimeout)
        return mapStatus,self.ModAtt.RxPayload,self.ModAtt.MaxTimeout
    
    def read_module(self,memoryModule,dataLengthReq,addressOffset):
        print("[read_module]")
        data = self._addDataToList(addressOffset,4,[])
        data = self._addDataToList(dataLengthReq,2,data)
        data = self._addDataToList(memoryModule,2,data)
        try:
            mapStatus,response = self._executeCommand(RequestID.ReadModule,data,self.mapHeaderReqSize)
        except:
            print('read_module Fail')
            raise
        #mapStatus,response = 0,[x for x in range(100)]  # FFor Testing 
        
        if len(response) > dataLengthReq:
            raise Exception('Len of data to match')
        self.commandSeqCnt += 1
        return mapStatus,response
    
    def start_write_module(self,memoryModule,numOfBytes,firsByte):
        #print("[start_write_module]")
        data = firsByte
        data = self._addDataToList(numOfBytes,4,data)
        data = self._addDataToList(memoryModule,2,data)
        
        try:
            mapStatus,response = self._executeCommand(RequestID.StartWrite,data,self.mapHeaderReqSize,self.ModAtt.MaxTimeout * 1000)
        except:
            print('start_write_module Fail')
            raise
        #mapStatus,response = 0,[]  # FFor Testing 
        
        if response != []:
            raise Exception('Len of data to match')
        self.commandSeqCnt += 1
        return mapStatus
    
    def append_write_module(self,memoryModule,BytesToWrite):
        #print("[append_write_module]")
        data = BytesToWrite
        data = self._addDataToList(memoryModule,2,data)
        try:
            mapStatus,response = self._executeCommand(RequestID.WriteModule,data,self.mapHeaderReqSize)
        except:
            print('append_write_module Fail')
            raise
        #mapStatus,response = 0,[]  # FFor Testing 
        if response != []:
            raise Exception('Len of data to match')
        self.commandSeqCnt += 1
        return mapStatus
    
    def end_write_module(self,memoryModule,writtenbytes):
        #print('[end_write_module]')
        """
        This command gives the possibility, to finish the writing of a module after a smaller amount of data,
        than declared in start write module request. The overall number of bytes declared in this messages must be
        identical to the number bytes that the slave received so far to write.
        """
        data = self._addDataToList(writtenbytes,4,[])
        data = self._addDataToList(memoryModule,2,data)
        try:
            mapStatus,response = self._executeCommand(RequestID.EndWriteModule,data,self.mapHeaderReqSize)
        except:
            print('end_write_module Fail')
            raise
        self.commandSeqCnt += 1
        return mapStatus
    def printProgress(self,offset):
        printOffset= (offset + self.printStep-1) // self.printStep * self.printStep
        if printOffset != self.lastOffset:
            self.lastOffset = printOffset
            print(hex(printOffset), end='\r')
    #===========================================================================
    # Check Error
    #===========================================================================
    
    def FindError(self,ErrNum):
  
        att = vars(MapStatus().__class__)
        try:
            ErrMessage = list(att.keys())[list(att.values()).index(ErrNum)]
        except ValueError:
            return(0x9100,"General Interface Error") 
        return(ErrNum,ErrMessage)
    
    #===========================================================================
    # Comfort zone
    #===========================================================================
    
    def SetupModule(self,memoryModule):
        mapStatus,RxPayload,MaxTimeout = self.setup_module(memoryModule)
        if mapStatus != 0:
            raise Exception(*self.FindError(mapStatus))
        return mapStatus,RxPayload,MaxTimeout
    
    def WriteModule(self,memoryModule,data,MaxRxPayload = None):
        print('[WRITE MODULE COMMAND]')
        if MaxRxPayload == None:
           
            if self.ModAtt.ModuleNumber == memoryModule:
                #print('self.ModAtt.RxPayload',self.ModAtt.RxPayload)
                MaxRxPayload = self.ModAtt.RxPayload - 8  # -8 Because: header -2b, Module num -2b, Num of b -4b
            else:
                raise Exception("Not set MaxRxPayload")  
        
        #print('MAX DATA LEN',len(data[:MaxRxPayload]))
        mapStatus = self.start_write_module(memoryModule,len(data),data[:MaxRxPayload])
        
        if mapStatus != 0:
            raise Exception(*self.FindError(mapStatus))
        #print('mapStatus WRITE ',mapStatus)
        
        for offset in range(MaxRxPayload,len(data),MaxRxPayload + 4):
            #print('[Offset]',hex(offset),'[Len]',hex(len(data[offset:offset + MaxRxPayload + 4])))
            self.printProgress(offset)
            mapStatus = self.append_write_module(memoryModule,data[offset:offset + MaxRxPayload + 4])
            if mapStatus != 0:
                raise Exception(*self.FindError(mapStatus))
        return mapStatus
    def UnknownDataLenWrite(self,memoryModule,data,MaxRxPayload = None):
        print('[WRITE MODULE WITH UNKNOWN LEN OF DATA]')
        dataLen = len(data)
        if MaxRxPayload == None:
           
            if self.ModAtt.ModuleNumber == memoryModule:
                print('self.ModAtt.RxPayload',self.ModAtt.RxPayload)
                MaxRxPayload = self.ModAtt.RxPayload - 8  # -8 Because: header -2b, Module num -2b, Num of b -4b
            else:
                raise Exception("Not set MaxRxPayload")  
        
        mapStatus = self.start_write_module(memoryModule,self.ModuleInfo(memoryModule)['ModuleSize'],data[:MaxRxPayload])
        if mapStatus != 0:
            raise Exception(*self.FindError(mapStatus))
        del data[:MaxRxPayload]
        
        while data != []:
            mapStatus = self.append_write_module(memoryModule,data[:MaxRxPayload + 4])
            del data[:MaxRxPayload + 4]
            if mapStatus != 0:
                raise Exception(*self.FindError(mapStatus))
            #print('mapStatus WRITE ',mapStatus)
        mapStatus = self.end_write_module(memoryModule,dataLen)
        if mapStatus != 0:
            raise Exception(*self.FindError(mapStatus))
        return mapStatus
    
    def ReadModule(self,memoryModule,count,offset = 0):
        print('[READ MODULE COMMAND]')
        ModuleData = []
        if count > 0XFF - 1:
            frRead = 0XFF - 1
        else:
            frRead = count
        mapStatus,response = self.read_module(memoryModule,frRead,offset)
        if mapStatus != 0:
            raise Exception(*self.FindError(mapStatus))
        ModuleData.extend(response)
        maxread = len(ModuleData)
        while len(ModuleData) < count:
            if maxread > count - len(ModuleData):
                maxread = count - len(ModuleData)
            mapStatus,response = self.read_module(memoryModule,maxread,len(ModuleData))
            if mapStatus != 0:
                raise Exception(*self.FindError(mapStatus))
            ModuleData.extend(response)

        return ModuleData
    
    def GetDeviceInfo(self):
        print('[GET DEVICE INFO COMMAND]')
        self.DeviceInfo = {}
        StartIndex = 0
        NextIndex = None
        while NextIndex != 0:
            mapStatus,response = self.get_device_info(StartIndex)
            if mapStatus != 0:
                raise Exception(*self.FindError(mapStatus))
            #print(response)
            NextIndex = response[0]
            StartIndex = NextIndex
            del response[0]
            print('NextIndex:',NextIndex)
            while response != [] :
                if len(response) < 8:
                    raise Exception(444,'In response are not complete data')
                #print('response:',' '.join([format(d,'02X') for d in response]))
                #print('ModuleNumber',response[:2])
                ModuleNumber = self._getInt16(response[:2])
                ModuleSize = self._getInt32(response[ 2: 6])
                NameLenght = response[6]
                try:
                    Name = ''.join([chr(x) for x in response[7: 7 + NameLenght]])
                except:
                    Name = response[7: 7 + NameLenght]
                    print(response[7: 7 + NameLenght])
                    print('################ ERROR IN CODEC #############################')
                del response[:7 + NameLenght]
                self.DeviceInfo[ModuleNumber] = {'ModuleSize':ModuleSize,
                                               'NameLenght':NameLenght,
                                               'Name':Name
                                               }
                
        return mapStatus

    def ModuleInfo(self,ModuleNumber = None):
        
        print('\n#### Module Info ####')
        if ModuleNumber == None:
            return [module for module in self.DeviceInfo ]
        else:
            if ModuleNumber not in [module for module in self.DeviceInfo ]:
                raise Exception(9999,'Set wrong Module Number')
            else:
                print("Module:",hex(ModuleNumber))
                print("ModuleSize:",hex(self.DeviceInfo[ModuleNumber]['ModuleSize']))
                print("NameLenght:",self.DeviceInfo[ModuleNumber]['NameLenght'])
                print("Name:",self.DeviceInfo[ModuleNumber]['Name'])
                return self.DeviceInfo[ModuleNumber]

    
if __name__ == '__main__':
    pass