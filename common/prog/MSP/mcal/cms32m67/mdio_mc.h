/*******************************************************************************
 *  Copyright (c) 2023 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          MCAL
 *  COMP_ABBREV      MDIO
 *  PROCESSOR        CMS32M67xx
 ******************************************************************************/


/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file     mdio_mc.h
 *
 *  \ingroup  MCAL
 *
 *  \brief    Public definitions and declarations for unit MDIO.
 *
 */

#ifndef MDIO_MC_H
#define MDIO_MC_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/

#include "BSH_stdinc.h"
#include "processor.h"
#include "mcal/mcal_types.h"
#include "mcal/mdio_types.h"


/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/
/*lint -save -e9026 Function-like macro... [MISRA 2012 Directive 4.9, advisory] */

// Port width definitions (max. number of pins per port)
#define MDIO_PORT_WIDTH    8U

// Feature definitions
#define MDIO_HAS_FEATURE_ATOMIC_TOGGLE

// CMS32M67xx: P0CFG, P1CFG, ... - the peripheral output function selection is
// a "alternate function" number, where 0 means plain GPIO
#define MDIO_ALT_FUNC_GPIO          MDIO_AF0
#define MDIO_ALT_FUNC_TXD0          MDIO_AF1
#define MDIO_ALT_FUNC_CCP0A         MDIO_AF2
#define MDIO_ALT_FUNC_ADCTRIG       MDIO_AF3

// Static configuration definitions
#ifndef MDIO_ALT_FUNC_DEFAULT
    #define MDIO_ALT_FUNC_DEFAULT  MDIO_AF0
#endif

#ifndef MDIO_SPEED_DEFAULT
    #define MDIO_SPEED_DEFAULT    MDIO_SPEED_LOW
#endif

#define MDIO_SCFG_DEFAULT          {MDIO_ALT_FUNC_DEFAULT, MDIO_SPEED_DEFAULT}

#define MDIO_SCFG_AF(af)           {af, MDIO_SPEED_DEFAULT}

#define MDIO_SCFG(af, speed)       {af, speed}


/*lint -restore -e9026 */

/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/
/* "explicit cast from uint32_t to volatile uint32_t *" tested to work correctly
 * 'MDIO_Pins' is typographically ambiguous with respect to 'MDIO_PIN5'*/
//lint -save -e923 -e9046
typedef uint16_t MDIO_Pins;
//lint -restore -e923 -e9046

static inline MDIO_Pins MDIO_getPinMask(enum MDIO_Pin pin)
{
    uint32_t pinMask = 1UL << (uint32_t)pin;
    return (MDIO_Pins)pinMask;
}


/* MDIO_Index and MDIO_InstancePtrs must correspond */
enum MDIO_PortIndex
{
    MDIO_PORTA_INDEX = 0,       ///< CMS32M67xx PORT0
    MDIO_PORTB_INDEX,           ///< CMS32M67xx PORT1
    MDIO_PORTC_INDEX,           ///< CMS32M67xx PORT2
    MDIO_PORTD_INDEX,           ///< CMS32M67xx PORT3
    MDIO_PORTE_INDEX,           ///< CMS32M67xx PORT4
    MDIO_PORTF_INDEX,           ///< CMS32M67xx PORT5
    MDIO_PORT_CNT
};


struct MDIO_Port
{
    PORT_Type               *PORTx;
    enum MDIO_PortIndex     portIndex;
};


enum MDIO_AltFunc
{
    MDIO_AF0 = 0,
    MDIO_AF1,
    MDIO_AF2,
    MDIO_AF3,
    MDIO_AF4,
    MDIO_AF5,
    MDIO_AF6,
    MDIO_AF7,
    MDIO_AF8,
    MDIO_AF9
};


enum MDIO_Speed
{
    MDIO_SPEED_LOW = 0,
    MDIO_SPEED_HIGH = 1
};


struct MDIO_StaticConfig
{
    enum MDIO_AltFunc       altFunc;
    enum MDIO_Speed         speed;
};


struct MDIO_Port;

struct MDIO_Channel
{
    const struct MDIO_Port  *port;
    const enum MDIO_Pin     pin;
    const struct MDIO_StaticConfig  scfg;
};


/******************************************************************************/
/* PUBLIC DATA DECLARATIONS                                                   */
/******************************************************************************/

extern const struct MDIO_Port MDIOA;
extern const struct MDIO_Port MDIOB;
extern const struct MDIO_Port MDIOC;
extern const struct MDIO_Port MDIOD;
extern const struct MDIO_Port MDIOE;
extern const struct MDIO_Port MDIOF;


/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

extern void MDIO_configureOutputType(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_OutputType outputType);
extern void MDIO_configurePullType(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_PullType pullType);
extern void MDIO_configureMode(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_Mode mode);

extern void MDIO_configureSpeed(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_Speed speed);
extern void MDIO_configureAlternateFunction(const struct MDIO_Port *port, enum MDIO_Pin pin, enum MDIO_AltFunc altFunc);


#ifdef __cplusplus
}
#endif

#endif // MDIO_MC_H
