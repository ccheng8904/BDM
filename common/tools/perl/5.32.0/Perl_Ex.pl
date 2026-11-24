#!/usr/bin/perl

#*******************************************************************************
#* © 2011-2017, Cypress Semiconductor Corporation
#* or a subsidiary of Cypress Semiconductor Corporation. All rights
#* reserved.
#*
#* This software, including source code, documentation and related
#* materials (“Software”), is owned by Cypress Semiconductor
#* Corporation or one of its subsidiaries (“Cypress”) and is protected by
#* and subject to worldwide patent protection (United States and foreign),
#* United States copyright laws and international treaty provisions.
#* Therefore, you may use this Software only as provided in the license
#* agreement accompanying the software package from which you
#* obtained this Software (“EULA”).
#*
#* If no EULA applies, Cypress hereby grants you a personal, non-
#* exclusive, non-transferable license to copy, modify, and compile the
#* Software source code solely for use in connection with Cypress’s
#* integrated circuit products. Any reproduction, modification, translation,
#* compilation, or representation of this Software except as specified
#* above is prohibited without the express written permission of Cypress.
#*
#* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO
#* WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING,
#* BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
#* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
#* PARTICULAR PURPOSE. Cypress reserves the right to make
#* changes to the Software without notice. Cypress does not assume any
#* liability arising out of the application or use of the Software or any
#* product or circuit described in the Software. Cypress does not
#* authorize its products for use in any products where a malfunction or
#* failure of the Cypress product may reasonably be expected to result in
#* significant property damage, injury or death (“High Risk Product”). By
#* including Cypress’s product in a High Risk Product, the manufacturer
#* of such system or application assumes all risk of such use and in doing
#* so agrees to indemnify Cypress against all liability.
#********************************************************************************
use lib './';

use Win32::OLE;
use Win32::OLE::Variant;
use Class::Struct;
use IO::Handle;
#The possible values for MiniProg3 in SWD mode are:
#  0 – 5-pin connector
#  1 – 10-pin connector
use constant $enumSWDConnectors = {
  FIVE_PINS => 0,
  TEN_PINS  => 1,
};
# include all required defined constants placed in the PPCOM.pl
require "PPCOM.pl";
# declare variables for default values of interface, frequency and connector type
my $interface;
my $frequency;
my $connector;
# declare customer variable for interface, frequency and connector type
my $custom_interface;
my $custom_frequency;
my $custom_connector;

$m_sLastError = "";
#set autoflush to true for printing on one line in loop
STDOUT->autoflush(1);
#File handling
my $filename         = $ARGV[0];
my $logfile          = $ARGV[1];
my $custom_interface = $ARGV[2];
my $custom_frequency = $ARGV[3];
my $custom_connector = $ARGV[4];
my $filenameWithDoubleBackSlash = $filename;
$interface = $PPCOM::enumInterfaces->{$custom_interface};
$frequency = $PPCOM::enumFrequencies->{$custom_frequency};
$connector = $enumSWDConnectors->{$custom_connector};
# checking values after parsing them from command line parameters
if (not defined $interface)
{
  print "interface value is not defined correctly, choose from possible values: JTAG, ISSP, I2C, SWD, SPI, SWD_SWV\n";
  print "Default interface value will be used: SWD\n";
  $interface = $PPCOM::enumInterfaces->{SWD};
}
if (not defined $frequency)
{
  print "Frequency value is not defined correctly, choose from possible values: FREQ_48_0, FREQ_24_0, FREQ_16_0, FREQ_03_2, FREQ_06_0, FREQ_12_0, FREQ_08_0, FREQ_01_6, FREQ_01_5, FREQ_03_0\n";
  print "Default connector value will be used: FREQ_03_0\n";
  $frequency = $PPCOM::enumFrequencies->{FREQ_03_0};
}
if (not defined $connector)
{
  print "Connector value is not defined correctly, choose from possible values: FIVE_PINS, TEN_PINS\n";
  print "Default connector value will be used: FIVE_PINS\n";
  $connector = $enumSWDConnectors->{FIVE_PINS};
}
# uncomment  for testing only:
# print "Used binary file: $filename \n";
#transform / to \\ in the pathname
$filenameWithDoubleBackSlash =~ s#/#\\\\#g;
print "Used binary file after backslash modification: $filenameWithDoubleBackSlash \n";
#open log file, if the file not exist, then create it
open(my $fh, '>', $logfile) or die "Could not open file '$logfile' $!";
print "Used communication values:\n";
print "\tInterface: $interface\n";
print "\tSpeed: $frequency\n";
print "\tConnector: $connector\n";

#Error constants
$S_OK = 0;
$E_FAIL = -1;

#Chip Level Protection constants
$CHIP_PROT_VIRGIN = 0x00;
$CHIP_PROT_OPEN = 0x01;
$CHIP_PROT_PROTECTED = 0x02;
$CHIP_PROT_KILL = 0x04;
$CHIP_PROT_MASK = 0x0F;

sub SUCCEEDED
{
  if ($_[0] >= 0) { return 1; }
  else { return 0; }
}

sub SplitArray
{
  my @chars = split //, $_[0];
  @swd_out = ();
  foreach my $val (@chars)
  {
    push @swd_out, ord($val);
  }
  return @swd_out;
}

sub OpenPort
{
  my $ports = Variant(VT_VARIANT | VT_BYREF, []);
     $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  my $hr = $pp->GetPorts($ports, $m_sLastError);
  my $array = $ports->Value();
  my $size = @$array;

  if (not SUCCEEDED($hr)) { return $hr; }

  if ($size <= 0)
  {
    printf("Connect any Programmer to PC\n");
    return -1;
  }

  my $bFound = 0;
  my $portName = "";
  for (my $i = 0; $i < $size; $i++)
  {
    my $sPort = $$array[$i];
    my $where1 = -1;
    my $where2 = -1;
	my $where3 = -1;
    my $word1 = "MiniProg3";
    my $word2 = "TrueTouchBridge";
	my $word3 = "KitProg";
    $where1 = index($sPort, $word1);
    $where2 = index($sPort, $word2);
	$where3 = index($sPort, $word3);
    if (($where1 != -1) || ($where2 != -1) || ($where3 != -1))
    {
      $portName = $sPort;
      $bFound = 1;
      last;
    }
  }

  if ($bFound == 0)
  {
    $m_sLastError = "Connect any MiniProg3/TrueTouchBridge/KitProg device to the PC";
    return -1;
  }

  #Port should be opened just once to connect Programmer device (MiniProg1/3,etc).
  #After that you can use Chip-/Programmer- specific APIs as long as you need.
  #No need to repoen port when you need to acquire chip 2nd time, just call Acquire() again.
  #This is true for all other APIs which get available once port is opened.
  #You have to call OpenPort() again if port was closed by ClosePort() method, or
  #when there is a need to connect to other programmer, or
  #if programmer was physically reconnected to USB-port.

  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->OpenPort($portName, $m_sLastError);
  return $hr;
}

sub ClosePort()
{
  #Close port when all operations are complete, so it will be available for other applications
  my $hr;

  my $strError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->ClosePort($strError);

  return $hr;
}

sub InitializePort
{
  my $hr;
  #Setup Power - "3.3V" and internal for reset mode
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $pp->SetPowerVoltage("3.3", $m_sLastError);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->PowerOn($m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }
  print "Power On and set voltage on 3.3V\n";
  print $fh "Power On and set voltage on 3.3V\n";
  #For MiniProg3 - Set protocol
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->SetProtocol($interface, $m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }
  print "Protocol was set successfully to SWD.\n";
  print $fh "Protocol was set successfully to SWD.\n";
  #For MiniProg3 - Set connector and frequency (do not check result, since it will fail for TTBridge - not supported)
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $pp->SetProtocolConnector($connector, $m_sLastError);
  print "Connector was set SWD to 5-pin connector.\n";
  print $fh "Connector was set SWD to 5-pin connector.\n";
  #For MiniProg3 - Set frequency
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $pp->SetProtocolClock($frequency, $m_sLastError);
  print "Bus frequency for the active protocol was set to 3MHz.\n";
  print $fh "Bus frequency for the active protocol was set to 3MHz.\n";
  return $hr;
}

sub CheckHexAndDeviceCompatibility
{
  my $hr;
  my $hexJtagID;
  my $chipJtagID;
  my $jtagID;
  my $result = 0;

  #Read silicon ID from chip
  $jtagID = Variant(VT_VARIANT | VT_BYREF, []);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->PSoC4_GetSiliconID($jtagID, $m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }
  $chipJtagID = $jtagID->Value();
  my @chipJtagID = SplitArray($chipJtagID);

  #Read silicon ID from hex
  $jtagID = Variant(VT_VARIANT | VT_BYREF, []);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->HEX_ReadJtagID($jtagID, $m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }
  $hexJtagID = $jtagID->Value();
  my @hexJtagID = SplitArray($hexJtagID);

  #Currently this cycle is commented, since the real silicon ID is not generated by PSoC Creator
  $result = 1;
  for (my $i = 0; $i < 4; $i++)
  {
    if ($i == 2) { next; } #ignore revision, 11(AA),12(AB),13(AC), etc
    if ($hexJtagID[$i] != $chipJtagID[$i])
    {
      $result = 0;
      last;
    }
  }
  return ($hr, $result);
}

sub PSoC4_IsChipNotProtected
{
  my $hr;

  #Chip Level Protection reliably can be read by below API (available in VIRGIN, OPEN, PROTECTED modes)
  #This API uses SROM call - to read current status of CPUSS_PROTECTION register (privileged)
  #This register contains current protection mode loaded from SFLASH during boot-up.
  my $flashProt = Variant(VT_VARIANT | VT_BYREF, []);
  my $chipProt = Variant(VT_VARIANT | VT_BYREF, []);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->PSoC4_ReadProtection($flashProt, $chipProt, $m_sLastError);
  if (not SUCCEEDED($hr)) { return $E_FAIL; } #consider chip as protected if any communication failure

  my @flashProt = SplitArray($flashProt->Value());
  my @chipProt = SplitArray($chipProt->Value());

  #Check Result
  if (($chipProt[0] & $CHIP_PROT_PROTECTED) == $CHIP_PROT_PROTECTED)
  {
    $m_sLastError = "Chip is in PROTECTED mode. Any access to Flash is suppressed.";
    return $E_FAIL;
  }

  return $S_OK;
}

sub PSoC4_EraseAll
{
  my $hr;

  #Check chip level protection here. If PROTECTED then move to OPEN by PSoC4_WriteProtection() API.
  #Otherwise use PSoC4_EraseAll() - in OPEN/VIRGIN modes.
  $hr = PSoC4_IsChipNotProtected();
  if (SUCCEEDED($hr)) #OPEN mode
  {
    #Erase All - Flash and Protection bits. Still be in OPEN mode.
    $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
    $hr = $pp->PSoC4_EraseAll($m_sLastError);
  }
  else
  {
    #Move to OPEN from PROTECTED. It automatically erases Flash and its Protection bits.
    my $flashProt =  Win32::OLE::Variant->new(VT_ARRAY | VT_UI1, 0); # do not care in PROTECTED mode
    my $chipProt =  Win32::OLE::Variant->new(VT_ARRAY | VT_UI1, 1); #move to OPEN

    $chipProt->Put(0, $CHIP_PROT_OPEN);

    $hr = $pp->PSoC4_WriteProtection($flashProt, $chipProt, $m_sLastError);
    if (not SUCCEEDED($hr)) { return $hr; }

    #Need to reacquire chip here to boot in OPEN mode.
    #ChipLevelProtection is applied only after Reset.
    $hr = $pp->DAP_Acquire($m_sLastError);
  }

  return $hr;
}

sub PSoC4_GetTotalFlashRowsCount
{
  my $hr;
  my $totalRows = 0;
  my $flashSize = $_[0];

  my $rowSize = Variant(VT_I4 | VT_BYREF, 0);
  my $rowsPerArray = Variant(VT_I4 | VT_BYREF, 0);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->PSoC4_GetFlashInfo($rowsPerArray, $rowSize, $m_sLastError);
  if (!SUCCEEDED($hr)) { return $hr };

  $totalRows = $flashSize / $rowSize;

  return ($hr, $totalRows, $rowSize);
}

sub ProgramFlash
{
  my $hr = 0;
  my $totalRows = 0;
  my $rowSize = 0;
  my $flashSize = $_[0];

  my @hResult = PSoC4_GetTotalFlashRowsCount($flashSize);
  $hr = $hResult[0];
  $totalRows = $hResult[1];
  $rowSize = $hResult[2];
  if (not SUCCEEDED($hr)) { return $hr; }
  print "Total rows: $totalRows \n";
  print $fh "Total rows: $totalRows \n";
  print "Total size to flash: $flashSize \n";
  print $fh "Total size to flash: $flashSize \n";

  #Program Flash array
  for(my $i = 0; $i < $totalRows; $i++)
  {
    $hr = $pp->PSoC4_ProgramRowFromHex($i, $m_sLastError);
    print "Written: $i from $totalRows\r";
    print $fh "Written: $i from $totalRows\n";
    if (not SUCCEEDED($hr)) { return $hr };
  }
  print "\n";
  return $hr;
}

sub PSoC4_VerifyFlash
{
  my $hr = 0;
  my $totalRows = 0;
  my $rowSize = 0;
  my $flashSize = $_[0];

  my @hResult = PSoC4_GetTotalFlashRowsCount($flashSize);
  $hr = $hResult[0];
  $totalRows = $hResult[1];
  $rowSize = $hResult[2];
  if (not SUCCEEDED($hr)) { return $hr; }

  my $verResult = Variant(VT_I4 | VT_BYREF, 0);
  #Verify Flash array
  for(my $i = 0; $i < $totalRows; $i++)
  {
     $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
     $hr = $pp->PSoC4_VerifyRowFromHex($i, $verResult, $m_sLastError);
     print "Verified: $i from $totalRows\r";
     print $fh "Verified: $i from $totalRows\n";
     if (not SUCCEEDED($hr)) { return $hr };
     if ($verResult == 0)
     {
       $m_sLastError = "Verification failed on ".$i." row.";
       return $E_FAIL;
     }
  }

  return $hr;
}

sub ProgramAll
{
  my $hr;

  #Port Initialization
  $hr = InitializePort();
  if (not SUCCEEDED($hr)) { return $hr; }
  print "Port initialization done.\n";
  # Set Hex File
  my $hexImageSize = Variant(VT_I4 | VT_BYREF, 0);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  print "Reading HEX file: $filenameWithDoubleBackSlash \n";
  print $fh "Reading HEX file: $filenameWithDoubleBackSlash \n";
  $hr = $pp->HEX_ReadFile($filenameWithDoubleBackSlash, $hexImageSize, $m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }
  print "Hex File loaded. Image Size = $hexImageSize B\n";
  print $fh "Hex File loaded. Image Size = $hexImageSize B\n";
  #Read chip level protection from hex and check Chip Level Protection mode
  #If it is VIRGIN then don't allow Programming, since it can destroy chip
  my $hex_chipProt;
  my $chipID = Variant(VT_VARIANT | VT_BYREF, []);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  print "Reading Chip-Level protection record from HEX file.\n";
  print $fh "Reading Chip-Level protection record from HEX file.\n";
  $hr = $pp->HEX_ReadChipProtection($chipID, $m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }
  $hex_chipProt = $chipID->Value();
  my @hex_chipProt = SplitArray($hex_chipProt);
  print "Chip Protection data from Hex File: \n";
  print $fh "Chip Protection data from Hex File: \n";
  foreach my $n (@hex_chipProt)
  { print $n,",";
    print $fh $n,",";
  }
  print "\n";
  print $fh "\n";
  if ($hex_chipProt[0] == $CHIP_PROT_VIRGIN)
  {
    $m_sLastError = "Transition to VIRGIN is not allowed. It will destroy the chip. Please contact Cypress if you need this specifically.";
    return $E_FAIL;
  }

  #Set Acquire Mode
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $pp->SetAcquireMode("Reset",$m_sLastError);

  #Acquire Device
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->DAP_Acquire($m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }

  #Check Hex File and Device compatibility
  my $fCompatibility;
  my @hResult;
  @hResult = CheckHexAndDeviceCompatibility();
  print "Checking Hex and Device compatibility\n";
  print $fh "Checking Hex and Device compatibility\n";
  $hr = $hResult[0];
  $fCompatibility = $hResult[1];
  if (not SUCCEEDED($hr)) { return $hr; }
  if ($fCompatibility == 0)
  {
    $m_sLastError = "The Hex file does not match the acquired device, please connect the appropriate device";
    return $E_FAIL;
  }

  #Erase All
  print "Erasing all flash content including security rows and chip-level protection.\n";
  print $fh "Erasing all flash content including security rows and chip-level protection.\n";
  $hr = PSoC4_EraseAll();
  if (not SUCCEEDED($hr)) { return $hr; }

  #Find checksum of Privileged Flash. Will be used in calculation of User CheckSum later
  my $checkSum_Privileged = 0x00;
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->PSoC4_CheckSum(0x8000, $checkSum_Privileged, $m_sLastError); #CheckSum All Flash ("Privileged + User" Rows)
  if (not SUCCEEDED($hr)) { return $hr; }

  #Program Flash
  print "Starting flashing row by row.\n";
  print $fh "Starting flashing row by row.\n";
  $hr = ProgramFlash($hexImageSize);
  if (not SUCCEEDED($hr)) { return $hr };

  #Verify Rows
  print "Verifying of flashing.\n";
  print $fh "Verifying of flashing.\n";
  $hr = PSoC4_VerifyFlash($hexImageSize);
  if (not SUCCEEDED($hr)) { return $hr };

  #Protect All arrays
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  print "Protecting all flash arrays and writing chip-level protection using data from the hex file.\n";
  print $fh "Protecting all flash arrays and writing chip-level protection using data from the hex file.\n";
  $hr = $pp->PSoC4_ProtectAll($m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }

  #Verify protection ChipLevelProtection and Protection data
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  print "Verifying flash and chip-level protection using data from the opened hex file.\n";
  print $fh "Verifying flash and chip-level protection using data from the opened hex file.\n";
  $hr = $pp->PSoC4_VerifyProtect($m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }

  #CheckSum verification
  my $checkSum_User = 0;
  my $checkSum_UserPrivileged = Variant(VT_I4 | VT_BYREF, 0);

  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  print "Calculating the checksum for whole flash (privileged + user rows). \n";
  print $fh "Calculating the checksum for whole flash (privileged + user rows). \n";
  $hr = $pp->PSoC4_CheckSum(0x8000, $checkSum_UserPrivileged, $m_sLastError); #CheckSum All Flash ("Privileged + User" Rows)
  if (not SUCCEEDED($hr)) { return $hr; }
  $checkSum_User = $checkSum_UserPrivileged - $checkSum_Privileged; #find checksum of User Flash rows

  my $hexChecksum = Variant(VT_I2 | VT_BYREF, 0);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  print "Reading the checksum from the hex file.\n";
  print $fh "Reading the checksum from the hex file.\n";
  $hr = $pp->HEX_ReadChecksum($hexChecksum, $m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }

  $checkSum_User = $checkSum_User & 0xFFFF;
  $hexChecksum = $hexChecksum & 0xFFFF;
  if ($checkSum_User != $hexChecksum)
  {
     printf ("Mismatch of Checksum: Expected 0x%X, Got 0x%X\n", $checkSum_User, $hexChecksum);
     print $fh "Mismatch of Checksum: Expected 0x%X, Got 0x%X\n", $checkSum_User, $hexChecksum;
     return -1;
  }
  else
  {
     printf ("Checksum is matched. Value is: 0x%X\n", $checkSum_User);
     printf $fh "Checksum is matched. Value is: 0x%X\n", $checkSum_User;
  }

  #Release PSoC3 device
  $hr = $pp->DAP_ReleaseChip($m_sLastError);

  return $hr;
}

sub UpgradeBlock
{
  my $hr;

  #Port Initialization
  $hr = InitializePort();
  if (not SUCCEEDED($hr)) { return $hr; }

  #Set Acquire Mode
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $pp->SetAcquireMode("Reset",$m_sLastError);

  #Acquire Device
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->DAP_Acquire($m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }

  #Get flash characteristics
  my $rowSize = Variant(VT_I4 | VT_BYREF, 0);
  my $rowsPerArray = Variant(VT_I4 | VT_BYREF, 0);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->PSoC4_GetFlashInfo($rowsPerArray, $rowSize, $m_sLastError);
  if (!SUCCEEDED($hr)) { return $hr };

  #Write row
  my $writeData = Win32::OLE::Variant->new(VT_ARRAY | VT_UI1, $rowSize);
  for (my $i = 0; $i < $rowSize; $i++) { $writeData->Put($i, $i & 0xFF); }

  my $rowID = $rowSize - 1;
  $hr = $pp->PSoC4_WriteRow($rowID, $writeData, $m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }

  #Verify Row
  my $readData = Variant(VT_VARIANT | VT_BYREF, []);
  $m_sLastError = Variant(VT_BSTR | VT_BYREF, "No Error");
  $hr = $pp->PSoC4_ReadRow($rowID, $readData, $m_sLastError);
  if (not SUCCEEDED($hr)) { return $hr; }
  $readRow = $readData->Value();
  @readRow = SplitArray($readRow);
  $size = @readRow;

  for (my $i = 0; $i < $size; $i++)
  {
    if ($readRow[$i] != $writeData->Get($i))
    {
      $hr = -1;
      last;
    }
  }
  if (not SUCCEEDED($hr))
  {
    $m_sLastError = "Verification of User area failed!";
    return $hr;
  }
}

sub Execute()
{
  my $hr;
  #Open Port - get last (connected) port in the ports list
  print "Trying the port open.\n";
  print $fh "Trying the port open.\n";
  $hr = OpenPort();
  if (not SUCCEEDED($hr)) { return $hr; }
  print "Start of programm all.\n";
  print $fh "Start of programm all.\n";
  $hr = ProgramAll();
  #$hr = UpgradeBlock();

  ClosePort();
  print "Port was closed.\n";
  print $fh "Port was closed.\n";
  return $hr;
}

# Begin main
my $hr = 0;
print "Program All using COM-object interface only\n";
print $fh "Program All using COM-object interface only\n";
#Use Version Independent Prog ID to instantiate COM-object
$pp = Win32::OLE->new("PSoCProgrammerCOM.PSoCProgrammerCOM_Object", "Quit");
#For version dependent Prog ID use below commented line, but update there COM-object version (e.g. 14)
#$pp = Win32::OLE->new("PSoCProgrammerCOM.PSoCProgrammerCOM_Object.14", "Quit");

$hr = Execute();
if (SUCCEEDED($hr))
  { @str = "Succeeded!\n";}
else
  { @str = ("Failed! ", $m_sLastError, "\n"); }
print @str;
print $fh @str;
close $fh;
#$wait = <STDIN>;
#chomp($wait);
undef $pp;
# End main
exit $hr;
