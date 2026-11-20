#ifndef STS_API_STS_H
#define STS_API_STS_H

#include "api.h"

    /* Declare the functions for accessing of static elements here. */
    /* (The same functions as in api_sts.c) */
    /* Use as names of functions: DBGX_STS_<verb><Noun>(...) */

extern void DBGX_STS_get_IntegerLen_Signed_Base_From_Format(uint32_t Format, uint8_t *IntegerLen, uint8_t *Signed, uint8_t *Base);
extern uint32_t DBGX_STS_get_IntegerValue_From_ValuePointerContents(void *ValuePointer, uint8_t IntegerLen);
extern void DBGX_STS_calc_Formatted_IntegerString(uint32_t IntegerValue, uint8_t IntegerLen, uint8_t Signed, uint8_t Base, char *IntegerString);
extern void DBGX_STS_calc_Formatted_PositiveIntegerStringPart(uint32_t PositiveIntegerValueForCalc, uint8_t Base, uint8_t IntegerStringIndex, char *IntegerString);
extern char DBGX_STS_convertCipherToCharacter(uint8_t Cipher);

#endif

