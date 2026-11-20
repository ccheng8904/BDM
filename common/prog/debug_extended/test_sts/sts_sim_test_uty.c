/************************************
* Simulation Based Testing o PC
************************************/

#ifdef STS_SIM_TEST_CI

#include <stdio.h>
#include <stdint.h>

#include "sts_sim_test_uty.h"

void STS_writeChar_K(char Character)
{
    printf("%c", Character);
}

void STS_writeUint8Hex_K(uint8_t Numeral)
{
    printf("0x%02x", Numeral);
}

void STS_writeUint16Hex_K(uint16_t Numeral)
{
    printf("0x%04x", Numeral);
}

void STS_writeUint32Hex_K(uint32_t Numeral)
{
    printf("0x%08x", Numeral);
}

void STS_writeUint8ArrayHex_K(const uint8_t *ArrayOfNumerals, uint32_t NumberOfArrayElements)
{
    uint32_t Index;

    Index = 0U;
    while (Index < NumberOfArrayElements)
    {
        printf("0x%02x ", ArrayOfNumerals[Index]);
        Index++;
    }
}

void STS_writeUint16ArrayHex_K(const uint16_t *ArrayOfNumerals, uint32_t NumberOfArrayElements)
{
    uint32_t Index;

    Index = 0U;
    while (Index < NumberOfArrayElements)
    {
        printf("0x%04x ", ArrayOfNumerals[Index]);
        Index++;
    }
}

void STS_writeUint32ArrayHex_K(const uint32_t *ArrayOfNumerals, uint32_t NumberOfArrayElements)
{
    uint32_t Index;

    Index = 0U;
    while (Index < NumberOfArrayElements)
    {
        printf("0x%08x ", ArrayOfNumerals[Index]);
        Index++;
    }
}

void STS_writeString_K(const char *String)
{
    printf(String);
}

void STS_writeStrChar_K(const char *String, char Character)
{
    printf(String);
    STS_writeChar_K(Character);
    printf("\n\n");
}

void STS_writeStrUint8Hex_K(const char *String, uint8_t Numeral)
{
    printf(String);
    STS_writeUint8Hex_K(Numeral);
    printf("\n\n");
}

void STS_writeStrUint16Hex_K(const char *String, uint16_t Numeral)
{
    printf(String);
    STS_writeUint16Hex_K(Numeral);
    printf("\n\n");
}

void STS_writeStrUint32Hex_K(const char *String, uint32_t Numeral)
{
    printf(String);
    STS_writeUint32Hex_K(Numeral);
    printf("\n\n");
}

void STS_writeStrUint8ArrayHex_K(const char *String, const uint8_t *ArrayOfNumerals, uint32_t NumberOfArrayElements)
{
    printf(String);
    STS_writeUint8ArrayHex_K(ArrayOfNumerals, NumberOfArrayElements);
    printf("\n\n");
}

void STS_writeStrUint16ArrayHex_K(const char *String, const uint16_t *ArrayOfNumerals, uint32_t NumberOfArrayElements)
{
    printf(String);
    STS_writeUint16ArrayHex_K(ArrayOfNumerals, NumberOfArrayElements);
    printf("\n\n");
}

void STS_writeStrUint32ArrayHex_K(const char *String, const uint32_t *ArrayOfNumerals, uint32_t NumberOfArrayElements)
{
    printf(String);
    STS_writeUint32ArrayHex_K(ArrayOfNumerals, NumberOfArrayElements);
    printf("\n\n");
}

void STS_writeStrStr_K(const char *String, const char *String2)
{
    printf(String);
    printf(String2);
    printf("\n\n");
}

void STS_writeStrStrn_K(const char *String, const char *String2, uint32_t StrLen2)
{
    uint32_t Idx;

    printf(String);
    Idx = 0U;
    while (Idx < StrLen2)
    {
        printf("%c", String2[Idx]);
        Idx++;
    }
    printf("\n\n");
}

void STS_writeStrNL_K(const char *String)
{
    printf(String);
    printf("\n\n");
}

#endif /* From: STS_SIM_TEST_CI */

