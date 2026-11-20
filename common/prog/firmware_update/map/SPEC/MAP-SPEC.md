# MAP (Memory Access Protocol)

## Revision history

| Revision | Date       | Changes                                                                          | Author                           |
| -------- | ---------- | -------------------------------------------------------------------------------- | -------------------------------- |
| 0.1      | 01.03.2018 | Initial revision of draft                                                        | Przemyslaw Psonka                |
| 0.2      | 02.03.2018 | Review: Cleaner separation of logic into chapters                                | Przemyslaw Psonka, Simon Holesch |
| 0.3      | 20.03.2018 | Added behaviour of write start request with overall length equaling zero         | Przemyslaw Psonka                |
| 0.4      | 05.04.2018 | Added status MAP_ST_ERROR                                                        | Przemyslaw Psonka                |
| 0.5      | 11.04.2018 | Added invalid module number                                                      | Przemyslaw Psonka                |
| 1.0      | 16.04.2018 | First official version                                                           | Przemyslaw Psonka                |
| 1.1      | 18.09.2018 | More precise definition of module info name                                      | Przemyslaw Psonka                |
| 1.2      | 18.03.2019 | Added option to write module, if you don't know precise amount of data at start  | Przemyslaw Psonka                |
| 1.3      | 18.06.2021 | Added new error codes with values 11, 12 and 13                                  | Przemyslaw Psonka                |

## 1 Purpose of this document

This document is intended to give information about the logic and concept of
MAP. It is not intended to give any details about implementation, although it
is always strongly recommended to implement everything in the most generic and
cleanest way possible.


## 2 General information about MAP

In this chapter, general information about the Memory Access Protocol shall be
given.


### 2.1 Purpose and character of MAP

MAP is intended to be used to control the reading and modification of the
memory on micro controllers without a file system. More specifically, the aim
of MAP is the update of software and data on these controllers.

It is to be embedded into another communication protocol, because a MAP frame 
itself does not contain any properties, which would allow checking its
correctness or length. This information is to be deduced from parameters inside
the protocol, into which MAP is to be embedded.

Furthermore, it is a master-slave protocol. Each slave must be able to handle
the requests of one master correctly at a time. It is not allowed to issue
requests to a slave from several masters at a time, nor is any slave obliged to
handle such a situation without errors.

MAP is not intended to give the master any hardware specific information
concerning the slave to be updated. These issues have to be handled internally
by the slave.


### 2.2 Concept of memory modules

Internally, each slave to be updated by means of MAP has to intersect its
memory into so called "memory modules".

By using MAP requests, the master can read or modify the address space defined
in exactly one memory module at a time. Furthermore, only complete memory 
modules can be updated (which means, erased and newly written). These
restrictions have following consequences:

 - Memory modules are not allowed to overlap.
 - If the memory is divided into erase pages (smallest units, which can be 
   erased with one erase command), every memory module has to start exactly at
   the beginning of an erase page and end exactly at the end of such a page.
   
Each memory module can be identified by a name and a 16 bit module number. The
module number may have almost any value within the given range, with the
exception of FFFFh.


## 3 Composition of MAP

Each MAP frame consists of a header and a payload specific to that frame. The
default timeout for receiving a response after issuing a request is one second.

Within a MAP frame, all parameters are to be in network byte order
(big endian).


### 3.1 Header 

The header for a MAP request looks thus:

| Parameter        | Size     |
| ---------------- | -------- |
| Request ID       | one byte |
| Sequence counter | one byte |

The header for a MAP response looks thus:

| Parameter        | Size     |
| ---------------- | -------- |
| Response ID      | one byte |
| Sequence counter | one byte |
| Status code      | one byte |

The sequence counter should be incremented by both master and slave, for each
message, that is being sent. The master must always check the counter of a
response to filter duplicate messages. If not explicitly stated otherwise, the
slave updates the sequence counter without checking.


### 3.2 Status codes of MAP responses

| Name of status                 | Value | Explanation of meaning                                          |
| ------------------------------ | ----- | --------------------------------------------------------------- |
| MAP_ST_OK                      | 0     | Request OK and processed without errors                         |
| MAP_ST_WRONG_PARAM             | 1     | At least one parameter has a wrong value                        |
| MAP_ST_MISMATCH_PARAM          | 2     | Request has wrong number of parameters                          |
| MAP_ST_UNKNOWN_REQ             | 3     | Request ID is unknown                                           |
| MAP_ST_SEQUENCE_CNT_ERROR      | 4     | Sequence counter is wrong                                       |
| MAP_ST_PAYLOAD_SIZE_EXCEEDED   | 5     | Not enough buffer to send response                              |
| MAP_ST_SETUP_MODULE_ERROR      | 6     | Fail to setting up module or wrong module set up                |
| MAP_ST_ACTION_PROHIBITED_ERROR | 7     | E.g. write module with reading access only                      |
| MAP_ST_OUT_OF_RANGE_ERROR      | 8     | Read or write request exceeds boarders of module                |
| MAP_ST_ERASE_ERROR             | 9     | Attempt to erase module was not successful                      |
| MAP_ST_WRITE_ERROR             | 10    | Error in attempting to write into erased module                 |
| MAP_ST_READ_ERROR              | 11    | Error in attempting to read module                              |
| MAP_ST_WRITE_DATA_LEN_MISMATCH | 12    | Data received to write did not have the declared overall length |
| MAP_ST_WRITE_BUFFER_DATA_LEFT  | 13    | Overall length of data to write is not correctly aligned        |
| MAP_ST_ERROR                   | 255   | Any error not covered by other statuses                         |

The first five values in the table above are an indication for an incorrect MAP
frame, while the others indicate problems during the attempt to process the
request.


### 3.3 Specific commands

The following sections give a description of the requests and responses, which
can be sent and received in MAP.


#### 3.3.1 Get device info request

The device info request asks for information about the slaves memory modules.

Request ID is 1.
The request has the following parameters:

| Parameter      | Size      |
| -------------- | --------- |
| Request Header | two bytes |
| Index          | one byte  |

The index is to be treated as an offset in the area, in which memory modules
are defined. 


#### 3.3.2 Get device info response

The device info response gives information about the slave's memory modules.

Response ID is 2.
The response has the following parameters:

| Parameter             | Size           |
| --------------------- | -------------- |
| Response Header       | three bytes    |
| Next Index            | one byte       |
| Array of Module Infos | see next table |

The index is marking the first module, for which no info has been put into the
response. 

The exact shape of each module info looks thus:

| Parameter       | Size                 |
| --------------- | -------------------- |
| Module Number   | two bytes            |
| Module Size     | four bytes           |
| Name Length     | one byte             |
| Name            | Value of Name Length |

The name must only contain letters, digits, underscores and hyphens. Every
module name must be unique on the given device and start with a letter. The
length of a module name must not be bigger than 50 characters. The module name
in the module info is not to be zero terminated.


#### 3.3.3 Setup module request

The setup module request asks the slave to prepare a memory module to work on.
This setup is needed to perform read and write operations on modules. 

Request ID is 3.
The request has the following parameters:

| Parameter      | Size      |
| -------------- | --------- |
| Request Header | two bytes |
| Module Number  | two bytes |


#### 3.3.4 Setup module response

The setup Module response confirms that the module requested had been prepared
for read and write operations. 

Response ID is 4.
The response has the following parameters:

| Parameter                                     | Size        | Info  |
| --------------------------------------------- | ----------- | ----- |
| Response Header                               | three bytes |       |
| Max Rx Payload                                | two bytes   |       |
| Max Timeout in seconds(exponent of power 2^x) | one byte    | x<=10 |

"Max Rx Payload" gives the maximum length of MAP frames (including header),
that the slave can receive. The maximum timeout is the worst case time that a
master has to wait for a response.


#### 3.3.5 Read module request

Request ID is 5.
The request has the following parameters:

| Parameter       | Size       |
| --------------- | ---------- |
| Request Header  | two bytes  |
| Module Number   | two bytes  |
| Number of bytes | two bytes  |
| Offset          | four bytes |

The first byte will be read from address (StartOfModule+Offset). The end of the
area to be read must not exceed the boarders of the module.


#### 3.3.6 Read module response

Response ID is 6.
The response has the following parameters:

| Parameter       | Size                                                               |
| --------------- | ------------------------------------------------------------------ |
| Response Header | three bytes                                                        |
| Data            | requested number of bytes, or as much as fits into transmit buffer |

The master receiving this response must calculate the length of the received
byte array from the length of the complete MAP frame.


#### 3.3.7 Start write module request

Request ID is 7.
The request has the following parameters:

| Parameter               | Size                         |
| ----------------------- | ---------------------------- |
| Request Header          | two bytes                    |
| Module Number           | two bytes                    |
| Overall number of bytes | four bytes                   |
| First bytes to write    | depending on length of frame |

The offset, at which a write operation in a module starts, is always zero. Each
portion of bytes, that has successfully been written, implicitly increases this
offset. The overall number of bytes must not exceed the module size. The slave
must calculate the length of the portion of bytes to write from the length of
the MAP frame.

In the specific case, in which the overall number of bytes given by the start
write module request is zero, the slave has to take the following action:

- If the module is located in a memory type, which does not require erasing
before writing new data, the slave should just send a positive write module
response.

- If the module is located in a memory type, which requires erasing before new
data can be written, the slave must erase the memory of the module.


#### 3.3.8 Append write module request

Request ID is 8.
The request has the following parameters:

| Parameter         | Size                         |
| ----------------- | ---------------------------- |
| Request Header    | two bytes                    |
| Module Number     | two bytes                    |
| Bytes to write    | depending on length of frame |

The slave must calculate the length of the data to write from the length of the
MAP frame. The offset for the next part of a write operation is implicitly
increased with each portion of bytes that has been successfully written.


#### 3.3.9 Write module response

This response is issued for all three types of write requests (chapters 3.3.7,
3.3.8 and 3.3.10).

Response ID is 9.
The response has no additional parameters:

| Parameter       | Size        |
| --------------- | ----------- |
| Response Header | three bytes |


#### 3.3.10 Prematurely end write module request

Request ID is 10.
The request has the following parameters:

| Parameter                       | Size                         |
| ------------------------------- | ---------------------------- |
| Request Header                  | two bytes                    |
| Module Number                   | two bytes                    |
| Overall number of bytes written | four bytes                   |

This command gives the possibility, to finish the writing of a module after
a smaller amount of data, than declared in the start write module request. The
overall number of bytes declared in this message must be identical to the
number of bytes that the slave received so far to write.


#### 3.3.11 Unknown command response

This response is issued for requests with unknown id.

Response ID is UINT8_MAX.
The response has no additional parameters:

| Parameter       | Size        |
| --------------- | ----------- |
| Response Header | three bytes |

The status of this message always must be "MAP_ST_UNKNOWN_REQ".


## 4 Usage of MAP

This chapter gives information about the usage of MAP.


### 4.1 How to get a device info

Any MAP masters wanting to perform read or write operations on its slave, may
request the full list of memory modules with "Get device info request" first.

The master sends the first get device info request with the index equaling
zero. If the slave cannot send the complete list of modules, the response will
return an index, which is not equal to zero, so that the next get device info
request has to be sent with this new index to get the information about the
next few modules. Once the index returned by the response is zero, the
information about all modules of the slave has been received.


### 4.2 How to prepare a module for reading and writing

Before starting any reading or writing operation, the correct module always has
to be set up. 

For that, the master has to send a setup module request. After receiving a 
setup module response, the timeout for read and write requests for that module
is to be adjusted to the received value.

If no response to a read/write request is received within this timeout, it must
be assumed, that an error has occurred during receiving or processing of the 
request.


### 4.3 How to read data from a module

After preparing the module, the master sends a read request. If less data,
than requested, is received, the master must send another request with adjusted
offset.


### 4.4 How to write data to a module

After setting up the module, the master has to split the data to write into
slices, which will fit into the maximum payload.

Each writing operation in a module must be started with a start write module
request and, if necessary, continued with append write module requests.

If the master does not know the exact amount of data to write at the beginning,
the master must declare the size of the module as the overall number of bytes
to be written. Should the real amount of data be smaller, than declared, the
master must stop the writing operation by the prematurely end write module
request.

On the slave's side, the sequence counter must be checked when processing the
append write request. If a message with the previous counter arrived (i.e. 
duplicate message), a positive response must be issued, although the message
must not be processed again. For the correct counter, the request must be both
processed and answered. For any other sequence counter, the response should
return "MAP_ST_SEQUENCE_CNT_ERROR".

If a response with an error is issued by the slave during the write process,
the master must not continue the write process.


### 4.5 Error handling

In case of a response timeout, the master must send its request again. After
three unsuccessful attempts, the master should stop the process and report the
error to the user.

If the master receives any response with an error code, it should stop and
report the error to the user.

