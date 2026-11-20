/* Define the functions for accessing of static elements here. */
/* Use as names of functions: DBGX_STS_<verb><Noun>(...) */

#include <stdint.h>
#include "api_sts.h"


void DBGX_STS_get_IntegerLen_Signed_Base_From_Format(uint32_t Format, uint8_t *IntegerLen, uint8_t *Signed, uint8_t *Base)
{
    #ifdef DBGX_TEST_API_CALLOUT
    DBGX_getIntegerCharacteristicsFromFormat(Format, IntegerLen, Signed, Base);
    #endif
}

uint32_t DBGX_STS_get_IntegerValue_From_ValuePointerContents(void *ValuePointer, uint8_t IntegerLen)
{
    #ifdef DBGX_TEST_API_CALLOUT
    return DBGX_get_IntegerValue_From_ValuePointerContents(ValuePointer, IntegerLen);
    #endif
}

void DBGX_STS_calc_Formatted_IntegerString(uint32_t IntegerValue, uint8_t IntegerLen, uint8_t Signed, uint8_t Base, char *IntegerString)
{
    #ifdef DBGX_TEST_API_CALLOUT
    DBGX_calc_Formatted_IntegerString(IntegerValue, IntegerLen, Signed, Base, IntegerString);
    #endif
}

void DBGX_STS_calc_Formatted_PositiveIntegerStringPart(uint32_t PositiveIntegerValueForCalc, uint8_t Base, uint8_t IntegerStringIndex, char *IntegerString)
{
    #ifdef DBGX_TEST_API_CALLOUT
    DBGX_calc_Formatted_PositiveIntegerStringPart(PositiveIntegerValueForCalc, Base, IntegerStringIndex, IntegerString);
    #endif
}

char DBGX_STS_convertCipherToCharacter(uint8_t Cipher)
{
    #ifdef DBGX_TEST_API_CALLOUT
    return DBGX_convertCipherToCharacter(Cipher);
    #endif
}

