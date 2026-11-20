#ifdef STS_SIM_TEST_CI

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "sts_sim_test_uty.h"
#include "debug_extended/api_cfg.h"
#include "app_dbgx_test_api_callout.h"
#include "api_sts.h"


void APP_DBGX_test_API_CALLOUT(void)
{
#ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED
    char Char;
#endif

#if defined(DBGX_BIN_DAT_FUNCTIONS_ENABLED) || defined(DBGX_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED)
uint32_t ErrorCode;
#endif

#ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED
    STS_logString("\n=== DBGX_logChar_ERR('A');  //def, on");
    DBGX_logChar_ERR('A');

    STS_logString("\n=== DBGX_logChar_INFO('B');  //undef, off");
    DBGX_logChar_INFO('B');

    STS_logString("\n=== DBGX_logStr_WARN(''BC'');  //def, on");
    DBGX_logStr_WARN("BC");

    STS_logString("\n=== DBGX_logStr_SCN_TOUCH(''DE'');  //def, on");
    DBGX_logStr_SCN_TOUCH("DE");

    STS_logString("\n=== DBGX_logStr_WARN_SCN_TOUCH(''FG'');  //def, on; def, on");
    DBGX_logStr_WARN_SCN_TOUCH("FG");

    STS_logString("\n=== DBGX_logStr_INFO_SCN_TOUCH(''fg'');  //undef, off; def, on");
    DBGX_logStr_INFO_SCN_TOUCH("fg");

    STS_logString("\n=== DBGX_logStr_SCN_POWER(''HI'');  //undef, off");
    DBGX_logStr_SCN_POWER("HI");

    STS_logString("\n=== DBGX_logStr_ERR_SCN_POWER(''JK'');  //def, on; undef, off");
    DBGX_logStr_ERR_SCN_POWER("JK");

    STS_logString("\n=== DBGX_logStr_INFO_SCN_POWER(''LM'');  //undef, off; undef, off");
    DBGX_logStr_INFO_SCN_POWER("LM");

    STS_logString("\n=== DBGX_logStr_SCN_CLOCK(''NO'');  //def, off");
    DBGX_logStr_SCN_CLOCK("NO");

    STS_logString("\n=== DBGX_logStr_ERR_SCN_CLOCK(''NO'');  //def, on; def, off");
    DBGX_logStr_ERR_SCN_CLOCK("NO");

    /* --------------------------------------------------------------------------------- */

    STS_logString("\n=== Char = DBGX_STS_convertCipherToCharacter(0);");
    Char = DBGX_STS_convertCipherToCharacter(0);
    STS_logStringAndChar("Char = ", Char);

    STS_logString("\n=== Char = DBGX_STS_convertCipherToCharacter(9);");
    Char = DBGX_STS_convertCipherToCharacter(9);
    STS_logStringAndChar("Char = ", Char);

    STS_logString("\n=== Char = DBGX_STS_convertCipherToCharacter(0x0A);");
    Char = DBGX_STS_convertCipherToCharacter(0x0A);
    STS_logStringAndChar("Char = ", Char);

    STS_logString("\n=== Char = DBGX_STS_convertCipherToCharacter(0x0F);");
    Char = DBGX_STS_convertCipherToCharacter(0x0F);
    STS_logStringAndChar("Char = ", Char);

    uint32_t PositiveIntegerValueForCalc;
    uint8_t Base;
    uint8_t IntegerStringIndex;
    char IntegerString[34];

    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=123U, Base=10U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 123U;
    Base                        = 10U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=123U, Base=16U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 123U;
    Base                        = 16U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=123U, Base=8U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 123U;
    Base                        = 8U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=123U, Base=2U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 123U;
    Base                        = 2U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);

    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=127U, Base=10U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 127U;
    Base                        = 10U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=127U, Base=16U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 127U;
    Base                        = 16U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=127U, Base=8U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 127U;
    Base                        = 8U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=127U, Base=2U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 127U;
    Base                        = 2U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);

    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=0U, Base=10U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 0U;
    Base                        = 10U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=0U, Base=16U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 0U;
    Base                        = 16U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);

    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=0U, Base=8U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 0U;
    Base                        = 8U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=0U, Base=2U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 0U;
    Base                        = 2U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);

    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=0xFFFFFFFF, Base=10U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 0xFFFFFFFF;
    Base                        = 10U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=0xFFFFFFFF, Base=16U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 0xFFFFFFFF;
    Base                        = 16U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=0xFFFFFFFF, Base=8U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 0xFFFFFFFF;
    Base                        = 8U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc=0xFFFFFFFF, Base=2U, IntegerStringIndex=0U, IntegerString);");
    PositiveIntegerValueForCalc = 0xFFFFFFFF;
    Base                        = 2U;
    IntegerStringIndex          = 0U;
    DBGX_STS_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    STS_logStringAndU8Hex("IntegerString[0] = ", IntegerString[0]);
    STS_logStringAndString("IntegerString = ", IntegerString);

    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x0FFFFFFE, IntegerLen=4U, Signed=DBGX_FORMAT_UNSIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=2U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x0FFFFFFE, 4U, DBGX_FORMAT_UNSIGNED >> (1 * DBGX_BITS_PER_BYTE), 2U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x0FFFFFFE, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=2U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x0FFFFFFE, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 2U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x80000000, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=16U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x80000000, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 16U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x80000001, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=16U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x80000001, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 16U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0xFFFFFFFF, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=16U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0xFFFFFFFF, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 16U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0xFFFFFFFE, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=16U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0xFFFFFFFE, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 16U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);

    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=2147483647, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(2147483647, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=1, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(1, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=-1, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(-1, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=-2147483648, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(-2147483648, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x80000000, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x80000000, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x80000001, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x80000001, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0xFFFFFFFF, IntegerLen=4U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0xFFFFFFFF, 4U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);

    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=32767, IntegerLen=2U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(32767, 2U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=1, IntegerLen=2U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(1, 2U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0, IntegerLen=2U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0, 2U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=-1, IntegerLen=2U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(-1, 2U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x8000, IntegerLen=2U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x8000, 2U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x8001, IntegerLen=2U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x8001, 2U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0xFFFF, IntegerLen=2U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0xFFFF, 2U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x7FFF, IntegerLen=2U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x7FFF, 2U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);

    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x01, IntegerLen=1U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x01, 1U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x00, IntegerLen=1U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(000, 1U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x80, IntegerLen=1U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x80, 1U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x81, IntegerLen=1U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x81, 1U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0xFF, IntegerLen=1U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0xFF, 1U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);
    STS_logString("\n=== DBGX_STS_calc_Formatted_IntegerString(IntegerValue=0x7F, IntegerLen=1U, Signed=DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE); Base=10U, IntegerString);");
    DBGX_STS_calc_Formatted_IntegerString(0x7F, 1U, DBGX_FORMAT_SIGNED >> (1 * DBGX_BITS_PER_BYTE), 10U, IntegerString);
    STS_logStringAndString("IntegerString = ", IntegerString);

    uint8_t Numeral_8Bit;
    uint16_t Numeral_16Bit;
    uint32_t Numeral_32Bit;
    uint32_t IntegerValue;
    Numeral_8Bit = 0x12;
    Numeral_16Bit = 0x3456;
    Numeral_32Bit = 0x789ABCDE;
    STS_logString("\n=== IntegerValue = DBGX_STS_get_IntegerValue_From_ValuePointerContents(&(Numeral_32Bit=0x789ABCDE), 4U)");
    IntegerValue = DBGX_STS_get_IntegerValue_From_ValuePointerContents(&Numeral_32Bit, 4U);
    STS_logStringAndU32Hex("IntegerValue = ", IntegerValue);
    STS_logString("\n=== IntegerValue = DBGX_STS_get_IntegerValue_From_ValuePointerContents(&(Numeral_16Bit=0x3456), 2U)");
    IntegerValue = DBGX_STS_get_IntegerValue_From_ValuePointerContents(&Numeral_16Bit, 2U);
    STS_logStringAndU32Hex("IntegerValue = ", IntegerValue);
    STS_logString("\n=== IntegerValue = DBGX_STS_get_IntegerValue_From_ValuePointerContents(&(Numeral_8Bit=0x12), 1U)");
    IntegerValue = DBGX_STS_get_IntegerValue_From_ValuePointerContents(&Numeral_8Bit, 1U);
    STS_logStringAndU32Hex("IntegerValue = ", IntegerValue);

    uint8_t IntegerLen, Signed;
    STS_logString("\n=== DBGX_STS_get_IntegerLen_Signed_Base_From_Format(0xAABBCCDD, &IntegerLen, &Signed, &Base);");
    DBGX_STS_get_IntegerLen_Signed_Base_From_Format(0xAABBCCDD, &IntegerLen, &Signed, &Base);
    STS_logStringAndU8Hex("IntegerLen = ", IntegerLen);
    STS_logStringAndU8Hex("Signed = ", Signed);
    STS_logStringAndU8Hex("Base = ", Base);

    Numeral_32Bit = 0x789ABCDE;
    STS_logString("\n=== DBGX_logInt_WARN_SCN_PYROLYSE(&Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_WARN_SCN_PYROLYSE(Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED);
    Numeral_32Bit = 0xFFFFFFFF;
    STS_logString("\n=== DBGX_logInt_SCN_TOUCH(&Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_SCN_TOUCH(Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED);
    Numeral_32Bit = 0x00000000;
    STS_logString("\n=== DBGX_logInt_SCN_TOUCH(&Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_SCN_TOUCH(Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED);

    Numeral_16Bit = 0x3456;
    STS_logString("\n=== DBGX_logInt_ERR(&Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED);
    Numeral_16Bit = 0xFFFF;
    STS_logString("\n=== DBGX_logInt_ERR(&Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED);

    Numeral_16Bit = 0x0000;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_16Bit=0x0000), DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED);

    Numeral_8Bit = 0x12;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=12), DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED);
    Numeral_8Bit = 0xFF;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=0xFF), DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED);
    Numeral_8Bit = 0x00;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=0x00), DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_16 | DBGX_FORMAT_UNSIGNED);

    Numeral_32Bit = 0x789ABCDE;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_32Bit=0x789ABCDE), DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED);
    Numeral_32Bit = 0xFFFFFFFF;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_32Bit=0xFFFFFFFF), DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED);
    Numeral_32Bit = 0x00000000;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_32Bit=0x00000000), DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED);

    Numeral_16Bit = 0x3456;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_16Bit=0x3456), DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED);
    Numeral_16Bit = 0xFFFF;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_16Bit=0xFFFF), DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED);
    Numeral_16Bit = 0x0000;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_16Bit=0x0000), DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED);

    Numeral_8Bit = 0x12;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=12), DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED);
    Numeral_8Bit = 0xFF;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=0xFF), DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED);
    Numeral_8Bit = 0x00;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=0x00), DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_10 | DBGX_FORMAT_UNSIGNED);

    Numeral_32Bit = 0x789ABCDE;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_32Bit=0x789ABCDE), DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED);
    Numeral_32Bit = 0xFFFFFFFF;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_32Bit=0xFFFFFFFF), DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED);
    Numeral_32Bit = 0x00000000;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_32Bit=0x00000000), DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_32Bit, DBGX_FORMAT_INTLEN_32 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED);

    Numeral_16Bit = 0x3456;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_16Bit=0x3456), DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED);
    Numeral_16Bit = 0xFFFF;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_16Bit=0xFFFF), DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED);
    Numeral_16Bit = 0x0000;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_16Bit=0x0000), DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_FORMAT_INTLEN_16 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED);

    Numeral_8Bit = 0x12;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=12), DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED);
    Numeral_8Bit = 0xFF;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=0xFF), DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED);
    Numeral_8Bit = 0x00;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=0x00), DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_FORMAT_INTLEN_8 | DBGX_FORMAT_BASE_2 | DBGX_FORMAT_UNSIGNED);

    int8_t  INumeral_8Bit;
    int16_t INumeral_16Bit;
    int32_t INumeral_32Bit;

    INumeral_8Bit = -1;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_8Bit=-1), DBGX_INT8_DECIMAL)");
    DBGX_logInt_ERR(INumeral_8Bit, DBGX_INT8_DECIMAL);
    INumeral_8Bit = -128;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_8Bit=-128), DBGX_INT8_DECIMAL)");
    DBGX_logInt_ERR(INumeral_8Bit, DBGX_INT8_DECIMAL);
    INumeral_8Bit = 127;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_8Bit=127), DBGX_INT8_DECIMAL)");
    DBGX_logInt_ERR(INumeral_8Bit, DBGX_INT8_DECIMAL);
    INumeral_8Bit = 1;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_8Bit=1), DBGX_INT8_DECIMAL)");
    DBGX_logInt_ERR(INumeral_8Bit, DBGX_INT8_DECIMAL);
    INumeral_8Bit = 0;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_8Bit=0), DBGX_INT8_DECIMAL)");
    DBGX_logInt_ERR(INumeral_8Bit, DBGX_INT8_DECIMAL);
    Numeral_8Bit = 255;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_8Bit=255), DBGX_UINT8_DECIMAL)");
    DBGX_logInt_ERR(Numeral_8Bit, DBGX_UINT8_DECIMAL);

    INumeral_16Bit = -1;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_16Bit=-1), DBGX_INT16_DECIMAL)");
    DBGX_logInt_ERR(INumeral_16Bit, DBGX_INT16_DECIMAL);
    INumeral_16Bit = -32768;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_16Bit=-32768), DBGX_INT16_DECIMAL)");
    DBGX_logInt_ERR(INumeral_16Bit, DBGX_INT16_DECIMAL);
    INumeral_16Bit = 32767;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_16Bit=32767), DBGX_INT16_DECIMAL)");
    DBGX_logInt_ERR(INumeral_16Bit, DBGX_INT16_DECIMAL);
    INumeral_16Bit = 1;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_16Bit=1), DBGX_INT16_DECIMAL)");
    DBGX_logInt_ERR(INumeral_16Bit, DBGX_INT16_DECIMAL);
    INumeral_16Bit = 0;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_16Bit=0), DBGX_INT16_DECIMAL)");
    DBGX_logInt_ERR(INumeral_16Bit, DBGX_INT16_DECIMAL);
    Numeral_16Bit = 0xFFFF;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_16Bit=0xFFFF), DBGX_UINT16_DECIMAL)");
    DBGX_logInt_ERR(Numeral_16Bit, DBGX_UINT16_DECIMAL);

    INumeral_32Bit = -1;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_32Bit=-1), DBGX_INT32_DECIMAL)");
    DBGX_logInt_ERR(INumeral_32Bit, DBGX_INT32_DECIMAL);
    INumeral_32Bit = -2147483648;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_32Bit=-2147483648), DBGX_INT32_DECIMAL)");
    DBGX_logInt_ERR(INumeral_32Bit, DBGX_INT32_DECIMAL);
    INumeral_32Bit = 2147483647;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_32Bit=2147483647), DBGX_INT32_DECIMAL)");
    DBGX_logInt_ERR(INumeral_32Bit, DBGX_INT32_DECIMAL);
    INumeral_32Bit = 1;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_32Bit=1), DBGX_INT32_DECIMAL)");
    DBGX_logInt_ERR(INumeral_32Bit, DBGX_INT32_DECIMAL);
    INumeral_32Bit = 0;
    STS_logString("\n=== DBGX_logInt_ERR(&(INumeral_32Bit=0), DBGX_INT32_DECIMAL)");
    DBGX_logInt_ERR(INumeral_32Bit, DBGX_INT32_DECIMAL);
    Numeral_32Bit = 0xFFFFFFFF;
    STS_logString("\n=== DBGX_logInt_ERR(&(Numeral_32Bit=0xFFFFFFFF), DBGX_UINT32_DECIMAL)");
    DBGX_logInt_ERR(Numeral_32Bit, DBGX_UINT32_DECIMAL);

    INumeral_32Bit = 0x12345678;
    STS_logString("\n=== DBGX_logInt_NonFiltered(&(INumeral_32Bit=0x12345678), DBGX_INT32_DECIMAL)");
    DBGX_logInt_NonFiltered(INumeral_32Bit, DBGX_INT32_DECIMAL);
    int32_t Array_32Bit[] = {123, -456, 789};
    STS_logString("\n=== DBGX_logIntArr_ERR(Array_32Bit={123, -456, 789}, 3U, DBGX_INT32_DECIMAL)");
    DBGX_logIntArr_ERR(Array_32Bit, 3U, DBGX_INT32_DECIMAL);
    int32_t Array_16Bit[] = {-30000, 32000, 0};
    STS_logString("\n=== DBGX_logIntArr_NonFiltered(Array_16Bit={-30000, 32000, 0}, 3U, DBGX_INT32_DECIMAL)");
    DBGX_logIntArr_NonFiltered(Array_16Bit, 3U, DBGX_INT32_DECIMAL);
    STS_logString("\n=== DBGX_logIntArr_NonFiltered(Array_16Bit={-30000, 32000, 0}, 0U, DBGX_INT32_DECIMAL)");
    DBGX_logIntArr_NonFiltered(Array_16Bit, 0U, DBGX_INT32_DECIMAL);

    STS_logString("\n=== DBGX_logTime_NonFiltered()");
    DBGX_logTime_NonFiltered();
    STS_logString("\n=== DBGX_logTime_ERR()");
    DBGX_logTime_ERR();

    STS_logString("\n=== DBGX_logLocat_NonFiltered()");
    DBGX_logLocat_NonFiltered();
    STS_logString("\n=== DBGX_logLocat_ERR()");
    DBGX_logLocat_ERR();
#endif /* ifdef DBGX_TEXT_DAT_FUNCTIONS_ENABLED */

#ifdef DBGX_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED
    STS_logString("\n=== DBGX_handleErr(0x82345678)");
    DBGX_handleErr(0x82345678);
    STS_logString("\n=== DBGX_handleErrWithParam(0x86543210, 0xAABBCCDD)");
    DBGX_handleErrWithParam(0x86543210, 0xAABBCCDD);

    STS_logString("\n=== DBGX_handleErrIf(1 == 1, 0x8FFFFFFF)");
    DBGX_handleErrIf(1 == 1, 0x8FFFFFFF);

    STS_logString("\n=== DBGX_handleErrIf(1 == 1, 0x8FFFFFFF)");
    DBGX_handleErrIf(1 == 2, 0x8FFFFFFF);

    STS_logString("\n=== DBGX_handleErrWithParamIf(1, 0x8EDCBA98, 0x12345678)");
    DBGX_handleErrWithParamIf(1, 0x8EDCBA98, 0x12345678);

    STS_logString("\n=== DBGX_handleErrWithParamIf(0, 0x8EDCBA98, 0x12345678)");
    DBGX_handleErrWithParamIf(0, 0x8EDCBA98, 0x12345678);

    STS_logString("\n=== DBGX_handleErrWithParamIf(1, 0x8EDCBA98 | DBGX_ERR_SOLVING_CALLBACK, 0x12345678)");
    DBGX_handleErrWithParamIf(1, 0x8EDCBA98 | DBGX_ERR_SOLVING_CALLBACK, 0x12345678);

    STS_logString("\n=== DBGX_handleErrWithParamIf(1, 0x8EDCBA98 | DBGX_ERR_SOLVING_RESET, 0x12345678)");
    DBGX_handleErrWithParamIf(1, 0x8EDCBA98 | DBGX_ERR_SOLVING_RESET, 0x12345678);

    STS_logString("\n=== DBGX_handleErrWithParamIf(1, 0x8EDCBA98 | DBGX_ERR_SOLVING_NONE, 0x12345678)");
    DBGX_handleErrWithParamIf(1, 0x8EDCBA98 | DBGX_ERR_SOLVING_NONE, 0x12345678);

uint32_t ErrorParameter;

    STS_logString("\n=== DBGX_readFirstError(&(ErrorCode=0x82345678), &(ErrorParameter0x00000000))");
    DBGX_readFirstError(&ErrorCode, &ErrorParameter);
    STS_logStringAndU32Hex("ErrorCode      = ", ErrorCode);
    STS_logStringAndU32Hex("ErrorParameter = ", ErrorParameter);

    STS_logString("\n=== DBGX_readLastError(&(ErrorCode=0x8EDCBA98), &(ErrorParameter=0x12345678))");
    DBGX_readLastError(&ErrorCode, &ErrorParameter);
    STS_logStringAndU32Hex("ErrorCode      = ", ErrorCode);
    STS_logStringAndU32Hex("ErrorParameter = ", ErrorParameter);
#endif /* ifdef DBGX_EXCEPTIONAL_ERROR_FUNCTIONS_ENABLED */

#ifdef DBGX_BIN_DAT_FUNCTIONS_ENABLED
uint8_t Data[] = {0xAA, 0xBB, 0xCC};

    STS_logString("\n=== ErrorCode = DBGX_sendData(0x03, DBGX_SCN_TOUCH, 0x02, Data={0xAA,0xBB,0xCC}, sizeof(Data))");
    /* int32_t DBGX_sendData(uint16_t Module_ID, uint32_t CallingFilter, uint16_t Function, uint8_t *Data, uint32_t DataLen); */
    ErrorCode = DBGX_sendData(0x03, DBGX_SCN_TOUCH, 0x02, Data, sizeof(Data));
    STS_logStringAndU32Hex("ErrorCode = ", ErrorCode);

    STS_logString("\n=== ErrorCode = DBGX_sendData(0x03, DBGX_INFO, 0x02, Data={0xAA,0xBB,0xCC}, sizeof(Data))");
    /* int32_t DBGX_sendData(uint16_t Module_ID, uint32_t CallingFilter, uint16_t Function, uint8_t *Data, uint32_t DataLen); */
    ErrorCode = DBGX_sendData(0x03,  DBGX_INFO, 0x02, Data, sizeof(Data));
    STS_logStringAndU32Hex("ErrorCode = ", ErrorCode);
#endif
}

#endif /* From: #ifdef STS_SIM_TEST_CI */

