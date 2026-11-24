#*******************************************************************************
#* © 2009-2017, Cypress Semiconductor Corporation
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
package PPCOM;
use Class::Struct;
BEGIN { }
use constant $enumeCanPowerDevice =
{
  CAN_MEASURE_POWER   => 4,
  CAN_POWER_DEVICE    => 1,
  CAN_READ_POWER      => 2,
  CAN_MEASURE_POWER_2 => 8,
};
use constant $enumCanProgram =
{
  CAN_PROGRAM_CARBON => 1,
  CAN_PROGRAM_ENCORE => 2,
};
use constant $enumInterfaces =
{
  JTAG => 1,
  ISSP => 2,
  I2C  => 4,
  SWD  => 8,
  SPI  => 16,
};
use constant $enumFrequencies =
{
  FREQ_48_0  => 0,
  FREQ_24_0  => 4,
  FREQ_16_0  => 16,
  FREQ_03_2  => 24,
  FREQ_06_0  => 96,
  FREQ_12_0  => 132,
  FREQ_08_0  => 144,
  FREQ_01_6  => 152,
  FREQ_01_5  => 192,
  FREQ_03_0  => 224,
  FREQ_RESET => 252,
};
use constant $enumI2Cspeed =
{
  CLK_100K  => 1,
  CLK_400K  => 2,
  CLK_50K   => 4,
};
use constant $enumSonosArrays =
{
  ARRAY_FLASH          => 1,
  ARRAY_EEPROM         => 2,
  ARRAY_NVL_USER       => 4,
  ARRAY_NVL_FACTORY    => 8,
  ARRAY_NVL_WO_LATCHES => 16,
  ARRAY_ALL            => 31,
};
use constant $enumUpgradeFirmware =
{
  FINALIZE      => 3,
  INITIALIZE    => 0,
  UPGRADE_BLOCK => 1,
  VERIFY_BLOCK  => 2,
};
use constant $enumValidAcquireModes =
{
  CAN_POWER_CYCLE_ACQUIRE  => 2,
  CAN_POWER_DETECT_ACQUIRE => 4,
  CAN_RESET_ACQUIRE        => 1,
};
use constant $enumVoltages =
{
  VOLT_18V => 8,
  VOLT_25V => 4,
  VOLT_33V => 2,
  VOLT_50V => 1,
};

return 1;
END { }
