/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Generic SW
 *  COMP_ABBREV      BIGEN LITEN
 ******************************************************************************/

#ifndef MARSHALING_H
#define MARSHALING_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  \mem_utility
 *
 *  \brief    Public functions and data types for (un)marshaling, mainly for communication.
 *
 *  \details  Big Endian (BIGEN) and Little Endian (LITEN) functions and data types are provided.\n
 *            The functions, that are provided here, are for (un)marshaling unsigned integers.\n
 *            If you want to (un)marshal other types, you need a union,
 *            containing your type and the unsigned integer of the same size.\n\n
 *            Before marshaling, transform your parameter to an unsigned int of the same size with the same bit values.\n
 *            For that, use the mentioned union.\n
 *            After un-marshaling, transform the acquired unsigned int to your data type by using the same union.\n\n
 *            This module is providing unions to switch between signed and unsigned integers with the same size and bit values. \n
 *            Data types like "float" might have a different number of bits on each platform.
 *            Therefore, no generic union can be provided for those. \n
 */

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "bsh_stdinc.h"
#include <stdint.h>

/**
 * \brief Use this union to translate an 8 bit signed to an unsigned int with the same bit values - and vice versa.
 */
union UintInt8  /*lint !e9018 Union keyword should not be used, but ok, if used as intended by this module*/
{
    uint8_t U8; //!< unsigned
    int8_t  I8; //!< signed
};

/**
 * \brief Use this union to translate a 16 bit signed to an unsigned int with the same bit values - and vice versa.
 */
union UintInt16 /*lint !e9018 Union keyword should not be used, but ok, if used as intended by this module*/
{
    uint16_t U16; //!< unsigned
    int16_t  I16; //!< signed
};

/**
 * \brief Use this union to translate a 32 bit signed to an unsigned int with the same bit values - and vice versa.
 */
union UintInt32 /*lint !e9018 Union keyword should not be used, but ok, if used as intended by this module*/
{
    uint32_t U32; //!< unsigned
    int32_t  I32; //!< signed
};

/**
 * \brief Use this union to translate a 64 bit signed to an unsigned int with the same bit values - and vice versa.
 */
union UintInt64 /*lint !e9018 Union keyword should not be used, but ok, if used as intended by this module*/
{
    uint64_t U64; //!< unsigned
    int64_t  I64; //!< signed
};

/**
*   \brief   Big Endian byte structure of sixteen bit Value
*
*   \details Can be cast onto big Endian byte array and vice versa.
*   \details Cast is best performed by union of pointers (containing pointer to struct and to byte array).
*/
struct BIGEN_U16
{
    uint8_t Hi; //!< High byte
    uint8_t Lo; //!< Low byte
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */

/**
*   \brief   Big Endian byte structure of thirty two bit Value
*
*   \details Can be cast onto big Endian byte array and vice versa.
*   \details Cast is best performed by union of pointers (containing pointer to struct and to byte array).
*/
struct BIGEN_U32
{
    uint8_t Hh; //!< High high byte
    uint8_t Hl; //!< High low byte
    uint8_t Lh; //!< Low high byte
    uint8_t Ll; //!< Low low bye
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */

/**
*   \brief   Big Endian byte structure of sixty four bit Value
*
*   \details Can be cast onto big Endian byte array and vice versa.
*   \details Cast is best performed by union of pointers (containing pointer to struct and to byte array).
*/
struct BIGEN_U64
{
    uint8_t Hhh; //!< High high high byte
    uint8_t Hhl; //!< High high low byte
    uint8_t Hlh; //!< High low high byte
    uint8_t Hll; //!< High low low byte
    uint8_t Lhh; //!< Low high high byte
    uint8_t Lhl; //!< Low high low byte
    uint8_t Llh; //!< Low low high byte
    uint8_t Lll; //!< Low low low byte
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */

/**
*   \brief   Little Endian byte structure of sixteen bit Value
*
*   \details Can be cast onto little Endian byte array and vice versa.
*   \details Cast is best performed by union of pointers (containing pointer to struct and to byte array).
*/
struct LITEN_U16
{
    uint8_t Lo; //!< Low byte
    uint8_t Hi; //!< High byte
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */

/**
*   \brief   Little Endian byte structure of thirty two bit Value
*
*   \details Can be cast onto little Endian byte array and vice versa.
*   \details Cast is best performed by union of pointers (containing pointer to struct and to byte array).
*/
struct LITEN_U32
{
    uint8_t Ll; //!< Low low bye
    uint8_t Lh; //!< Low high byte
    uint8_t Hl; //!< High low byte
    uint8_t Hh; //!< High high byte
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */

/**
*   \brief   Little Endian byte structure of sixty four bit Value
*
*   \details Can be cast onto little Endian byte array and vice versa.
*   \details Cast is best performed by union of pointers (containing pointer to struct and to byte array).
*/
struct LITEN_U64
{
    uint8_t Lll; //!< Low low low byte
    uint8_t Llh; //!< Low low high byte
    uint8_t Lhl; //!< Low high low byte
    uint8_t Lhh; //!< Low high high byte
    uint8_t Hll; //!< High low low byte
    uint8_t Hlh; //!< High low high byte
    uint8_t Hhl; //!< High high low byte
    uint8_t Hhh; //!< High high high byte
};/*lint -e9045 "non-hidden definition of type..." not hidden by intention */
/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

/**
*   \brief            Marshals unsigned sixteen bit value into big Endian byte structure.
*   \param[out] Bytes Structure to be filled
*   \param[in]  Value Value to be transformed into byte structure
*/
static inline void BIGEN_u16ToBytes(struct BIGEN_U16 *const Bytes, uint16_t Value);
static inline void BIGEN_u16ToBytes(struct BIGEN_U16 *const Bytes, uint16_t Value)
{
    Bytes->Hi = (uint8_t)(Value >> 8);
    Bytes->Lo = (uint8_t)Value;
}

/**
*   \brief            Marshals unsigned thirty two bit value into big Endian byte structure.
*   \param[out] Bytes Structure to be filled
*   \param[in]  Value Value to be transformed into byte structure
*/
static inline void BIGEN_u32ToBytes(struct BIGEN_U32 *const Bytes, uint32_t Value);
static inline void BIGEN_u32ToBytes(struct BIGEN_U32 *const Bytes, uint32_t Value)
{
    Bytes->Hh = (uint8_t)(Value >> 24);
    Bytes->Hl = (uint8_t)(Value >> 16);
    Bytes->Lh = (uint8_t)(Value >> 8);
    Bytes->Ll = (uint8_t)Value;
}

/**
*   \brief            Marshals unsigned sixty four bit value into big Endian byte structure.
*   \param[out] Bytes Structure to be filled
*   \param[in]  Value Value to be transformed into byte structure
*/
static inline void BIGEN_u64ToBytes(struct BIGEN_U64 *const Bytes, uint64_t Value);
static inline void BIGEN_u64ToBytes(struct BIGEN_U64 *const Bytes, uint64_t Value)
{
    Bytes->Hhh = (uint8_t)(Value >> 56);
    Bytes->Hhl = (uint8_t)(Value >> 48);
    Bytes->Hlh = (uint8_t)(Value >> 40);
    Bytes->Hll = (uint8_t)(Value >> 32);
    Bytes->Lhh = (uint8_t)(Value >> 24);
    Bytes->Lhl = (uint8_t)(Value >> 16);
    Bytes->Llh = (uint8_t)(Value >> 8);
    Bytes->Lll = (uint8_t)Value;
}

/**
*   \brief           Un-marshals big Endian byte structure into unsigned sixteen bit Value.
*   \param[in] Bytes Byte structure
*   \return          Sixteen bit value (unsigned)
*/
static inline uint16_t BIGEN_bytesToU16(const struct BIGEN_U16 *const Bytes);
static inline uint16_t BIGEN_bytesToU16(const struct BIGEN_U16 *const Bytes)
{
    return ((uint16_t)Bytes->Hi << 8)|((uint16_t)Bytes->Lo);
}

/**
*   \brief           Un-marshals big Endian byte structure into unsigned thirty two bit Value.
*   \param[in] Bytes Byte structure
*   \return          Thirty two bit value (unsigned)
*/
static inline uint32_t BIGEN_bytesToU32(const struct BIGEN_U32 *const Bytes);
static inline uint32_t BIGEN_bytesToU32(const struct BIGEN_U32 *const Bytes)
{
    return ((uint32_t)Bytes->Hh << 24)|((uint32_t)Bytes->Hl << 16)|((uint32_t)Bytes->Lh << 8)|((uint32_t)Bytes->Ll);
}

/**
*   \brief           Un-marshals big Endian byte structure into unsigned sixty four bit Value.
*   \param[in] Bytes Byte structure
*   \return          Sixty four bit value (unsigned)
*/
static inline uint64_t BIGEN_bytesToU64(const struct BIGEN_U64 *const Bytes);
static inline uint64_t BIGEN_bytesToU64(const struct BIGEN_U64 *const Bytes)
{
    return ((uint64_t)Bytes->Hhh << 56)|((uint64_t)Bytes->Hhl << 48)|((uint64_t)Bytes->Hlh << 40)|((uint64_t)Bytes->Hll << 32) |
           ((uint64_t)Bytes->Lhh << 24)|((uint64_t)Bytes->Lhl << 16)|((uint64_t)Bytes->Llh << 8)|((uint64_t)Bytes->Lll);
}

/**
*   \brief            Marshals unsigned sixteen bit value into little Endian byte structure.
*   \param[out] Bytes Structure to be filled
*   \param[in]  Value Value to be transformed into byte structure
*/
static inline void LITEN_u16ToBytes(struct LITEN_U16 *const Bytes, uint16_t Value);
static inline void LITEN_u16ToBytes(struct LITEN_U16 *const Bytes, uint16_t Value)
{
    Bytes->Hi = (uint8_t)(Value >> 8);
    Bytes->Lo = (uint8_t)Value;
}

/**
*   \brief            Marshals unsigned thirty two bit value into little Endian byte structure.
*   \param[out] Bytes Structure to be filled
*   \param[in]  Value Value to be transformed into byte structure
*/
static inline void LITEN_u32ToBytes(struct LITEN_U32 *const Bytes, uint32_t Value);
static inline void LITEN_u32ToBytes(struct LITEN_U32 *const Bytes, uint32_t Value)
{
    Bytes->Hh = (uint8_t)(Value >> 24);
    Bytes->Hl = (uint8_t)(Value >> 16);
    Bytes->Lh = (uint8_t)(Value >> 8);
    Bytes->Ll = (uint8_t)Value;
}

/**
*   \brief            Marshals unsigned sixty four bit value into little Endian byte structure.
*   \param[out] Bytes Structure to be filled
*   \param[in]  Value Value to be transformed into byte structure
*/
static inline void LITEN_u64ToBytes(struct LITEN_U64 *const Bytes, uint64_t Value);
static inline void LITEN_u64ToBytes(struct LITEN_U64 *const Bytes, uint64_t Value)
{
    Bytes->Hhh = (uint8_t)(Value >> 56);
    Bytes->Hhl = (uint8_t)(Value >> 48);
    Bytes->Hlh = (uint8_t)(Value >> 40);
    Bytes->Hll = (uint8_t)(Value >> 32);
    Bytes->Lhh = (uint8_t)(Value >> 24);
    Bytes->Lhl = (uint8_t)(Value >> 16);
    Bytes->Llh = (uint8_t)(Value >> 8);
    Bytes->Lll = (uint8_t)Value;
}

/**
*   \brief           Un-marshals little Endian byte structure into unsigned sixteen bit Value.
*   \param[in] Bytes Byte structure
*   \return          Sixteen bit value (unsigned)
*/
static inline uint16_t LITEN_bytesToU16(const struct LITEN_U16 *const Bytes);
static inline uint16_t LITEN_bytesToU16(const struct LITEN_U16 *const Bytes)
{
    return ((uint16_t)Bytes->Hi << 8)|((uint16_t)Bytes->Lo);
}

/**
*   \brief           Un-marshals little Endian byte structure into unsigned thirty two bit Value.
*   \param[in] Bytes Byte structure
*   \return          Thirty two bit value (unsigned)
*/
static inline uint32_t LITEN_bytesToU32(const struct LITEN_U32 *const Bytes);
static inline uint32_t LITEN_bytesToU32(const struct LITEN_U32 *const Bytes)
{
    return ((uint32_t)Bytes->Hh << 24)|((uint32_t)Bytes->Hl << 16)|((uint32_t)Bytes->Lh << 8)|((uint32_t)Bytes->Ll);
}

/**
*   \brief           Un-marshals little Endian byte structure into unsigned sixty four bit Value.
*   \param[in] Bytes Byte structure
*   \return          Sixty four bit value (unsigned)
*/
static inline uint64_t LITEN_bytesToU64(const struct LITEN_U64 *const Bytes);
static inline uint64_t LITEN_bytesToU64(const struct LITEN_U64 *const Bytes)
{
    return ((uint64_t)Bytes->Hhh << 56)|((uint64_t)Bytes->Hhl << 48)|((uint64_t)Bytes->Hlh << 40)|((uint64_t)Bytes->Hll << 32) |
           ((uint64_t)Bytes->Lhh << 24)|((uint64_t)Bytes->Lhl << 16)|((uint64_t)Bytes->Llh << 8)|((uint64_t)Bytes->Lll);
}

#ifdef __cplusplus
}
#endif

#endif /*MARSHALING_H*/
