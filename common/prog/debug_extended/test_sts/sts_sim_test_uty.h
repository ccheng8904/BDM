#ifndef STS_SIM_TEST_UTY_H
#define STS_SIM_TEST_UTY_H

/*lint -save -e9026 Function-like macro for the test simulation. The test simulation runs only with a .exe file under Windows. Therefore these notes can be turned off. */

#ifdef STS_SIM_TEST_CI

#define MUART_CFG_8N1(baud)     {1, 0, 1, (baud)}

typedef struct MUART_Channel
{
    const uint8_t        *muart;
    const uint8_t        *rxPin;
    const uint8_t        *txPin;
    const uint8_t   scfg;
} MUART_Channel;

struct MUART_Handle
{
    const uint8_t        *testNumber;
};
 
struct MUART_Config
{
    uint32_t              dataBits;   ///< Number of data bits
    uint32_t              parity;     ///< Parity configuration
    uint32_t              stopBits;   ///< Stop bits configuration
    uint32_t              baudrate;   ///< Baud rate configuration
};
#endif

#ifdef STS_SIM_TEST_CI

#include <stdio.h>

#define STS_writeChar(Character)    \
    STS_writeChar_K(Character)

#define STS_writeUint8Hex(Numeral)  \
    STS_writeUint8Hex_K(8Numeral)

#define STS_writeUint16Hex(Numeral) \
    STS_writeUint16Hex_K(Numeral)

#define STS_writeUint32Hex(Numeral) \
    STS_writeUint32Hex_K(Numeral)

#define STS_writeUint8ArrayHex(ArrayOfNumerals, NumberOfArrayElements) \
    STS_writeUint8ArrayHex_K(ArrayOfNumerals, NumberOfArrayElements)

#define STS_writeUint16ArrayHex(ArrayOfNumerals, NumberOfArrayElements) \
    STS_writeUint16ArrayHex_K(ArrayOfNumerals, NumberOfArrayElements)

#define STS_writeUint32ArrayHex(ArrayOfNumerals, NumberOfArrayElements) \
    STS_writeUint32ArrayHex_K(ArrayOfNumerals, NumberOfArrayElements)

#define STS_writeString(String)   \
    STS_writeString_K(String)

#define STS_writeSourceLocation() \
    printf("Func: %s; File: %s; Line: %d\n", __func__, __FILE__, __LINE__)

#define STS_logStringAndChar(String, Character)                         \
    STS_PSL();                                                     \
    STS_writeStrChar_K(String, Character)

#define STS_logStringAndU8Hex(String, Numeral)                            \
    STS_PSL();                                                     \
    STS_writeStrUint8Hex_K(String, Numeral)

#define STS_logStringAndU16Hex(String, Numeral)                           \
    STS_PSL();                                                     \
    STS_writeStrUint16Hex_K(String, Numeral)

#define STS_logStringAndU32Hex(String, Numeral)                           \
    STS_PSL();                                                     \
    STS_writeStrUint32Hex_K(String, Numeral)

#define STS_logStringAndU8ArrayHex(String, ArrayOfNumerals, NumberOfArrayElements)          \
    STS_PSL();                                                                   \
    STS_writeStrUint8ArrayHex_K(String, ArrayOfNumerals, NumberOfArrayElements)

#define STS_logStringAndU16ArrayHex(String, ArrayOfNumerals, NumberOfArrayElements)         \
    STS_PSL();                                                                   \
    STS_writeStrUint16ArrayHex_K(String, ArrayOfNumerals, NumberOfArrayElements)

#define STS_logStringAndU32ArrayHex(String, ArrayOfNumerals, NumberOfArrayElements)         \
    STS_PSL();                                                                   \
    STS_writeStrUint32ArrayHex_K(String, ArrayOfNumerals, NumberOfArrayElements)

#define STS_logStringAndString(String, String2)                              \
    STS_PSL();                                                     \
    STS_writeStrStr_K(String, String2)

#define STS_logStringAndStringWithLen(String, String2, StrLen2)                    \
    STS_PSL();                                                     \
    STS_writeStrStrn_K(String, String2, StrLen2)

#define STS_logString(String)                                         \
    STS_PSL();                                                     \
    STS_writeStrNL_K(String)

#define STS_PCHAR(Character)              STS_writeChar(Character)
#define STS_PU8H(Numeral)                 STS_writeUint8Hex(Numeral)
#define STS_PU16H(Numeral)                STS_writeUint16Hex(Numeral)
#define STS_PU32H(Numeral)                STS_writeUint32Hex(Numeral)
#define STS_PU8AH(ArrayOfNumerals, NumberOfArrayElements)  STS_writeUint8ArrayHex(ArrayOfNumerals, NumberOfArrayElements)
#define STS_PU16AH(ArrayOfNumerals, NumberOfArrayElements) STS_writeUint16ArrayHex(ArrayOfNumerals, NumberOfArrayElements)
#define STS_PU32AH(ArrayOfNumerals, NumberOfArrayElements) STS_writeUint32ArrayHex(ArrayOfNumerals, NumberOfArrayElements)
#define STS_PS(String)                    STS_writeString(String)
#define STS_PSL()                         STS_writeSourceLocation()

#else /* From: #ifdef STS_SIM_TEST_CI */

#define STS_writeChar(Character)
#define STS_writeUint8Hex(Numeral)
#define STS_writeUint16Hex(Numeral)
#define STS_writeUint32Hex(Numeral)
#define STS_writeUint8ArrayHex(ArrayOfNumerals, NumberOfArrayElements)
#define STS_writeUint16ArrayHex(ArrayOfNumerals, NumberOfArrayElements)
#define STS_writeUint32ArrayHex(ArrayOfNumerals, NumberOfArrayElements)
#define STS_writeString(String)
#define STS_writeSourceLocation()

#define STS_logStringAndChar(String, Character)
#define STS_logStringAndU8Hex(String, Numeral)
#define STS_logStringAndU16Hex(String, Numeral)
#define STS_logStringAndU32Hex(String, Numeral)
#define STS_logStringAndU8ArrayHex(String, ArrayOfNumerals, NumberOfArrayElements)
#define STS_logStringAndU16ArrayHex(String, ArrayOfNumerals, NumberOfArrayElements)
#define STS_logStringAndU32ArrayHex(String, ArrayOfNumerals, NumberOfArrayElements)
#define STS_logStringAndString(String, String2)
#define STS_logStringAndStringWithLen(String, String2, StrLen2)
#define STS_logString(String)

#define STS_PCHAR(Character)
#define STS_PU8H(Numeral)
#define STS_PU16H(Numeral)
#define STS_PU32H(Numeral)
#define STS_PU8AH(ArrayOfNumerals, NumberOfArrayElements)
#define STS_PU16AH(ArrayOfNumerals, NumberOfArrayElements)
#define STS_PU32AH(ArrayOfNumerals, NumberOfArrayElements)
#define STS_PS(String)
#define STS_PSL()

#endif /* From: #ifdef STS_SIM_TEST_CI */

#ifdef STS_SIM_TEST_CI
extern void STS_writeChar_K(char Character);
extern void STS_writeUint8Hex_K(uint8_t Numeral);
extern void STS_writeUint16Hex_K(uint16_t Numeral);
extern void STS_writeUint32Hex_K(uint32_t Numeral);
extern void STS_writeUint8ArrayHex_K(const uint8_t *ArrayOfNumerals, uint32_t NumberOfArrayElements);
extern void STS_writeUint16ArrayHex_K(const uint16_t *ArrayOfNumerals, uint32_t NumberOfArrayElements);
extern void STS_writeUint32ArrayHex_K(const uint32_t *ArrayOfNumerals, uint32_t NumberOfArrayElements);
extern void STS_writeString_K(const char *String);

extern void STS_writeStrChar_K(const char *String, char Character);
extern void STS_writeStrUint8Hex_K(const char *String, uint8_t Numeral);
extern void STS_writeStrUint16Hex_K(const char *String, uint16_t Numeral);
extern void STS_writeStrUint32Hex_K(const char *String, uint32_t Numeral);
extern void STS_writeStrUint8ArrayHex_K(const char *String, const uint8_t *ArrayOfNumerals, uint32_t NumberOfArrayElements);
extern void STS_writeStrUint16ArrayHex_K(const char *String, const uint16_t *ArrayOfNumerals, uint32_t NumberOfArrayElements);
extern void STS_writeStrUint32ArrayHex_K(const char *String, const uint32_t *ArrayOfNumerals, uint32_t NumberOfArrayElements);
extern void STS_writeStrStr_K(const char *String, const char *String2);
extern void STS_writeStrStrn_K(const char *String, const char *String2, uint32_t StrLen2);
extern void STS_writeStrNL_K(const char *String);
#endif /* From: #ifdef STS_SIM_TEST_CI */


/*lint -restore -e9026 */
#endif   /* From: ifndef STS_SIM_TEST_UTY_H */


