/*******************************************************************************
*   Copyright (c) 2017 BSH Hausgeraete GmbH,
*   Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
*
*   All rights reserved. This program and the accompanying materials
*   are protected by international copyright laws.
*   Please contact copyright holder for licensing information.
*
********************************************************************************
*   PROJECT          FWU
*   COMP_ABBREV      MAL
*******************************************************************************/

#ifndef MAL_STATUSES_H_
#define MAL_STATUSES_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file
 *
 *  \ingroup  mal
 *
 *  \brief    general status definitions for mal.
 *
 */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC DEFINITIONS                                                         */
/******************************************************************************/



/******************************************************************************/
/* PUBLIC TYPE DEFINITIONS                                                    */
/******************************************************************************/

/**
*   \brief List of statuses from specification that can be reached when attempting to perform operations on a memory.
*/
enum MAL_Status
{
    MAL_ST_OK=0U,
    MAL_ST_SETUP_MODULE_ERROR=6U,
    MAL_ST_ACTION_PROHIBITED_ERROR=7U,
    MAL_ST_OUT_OF_RANGE_ERROR=8U,
    MAL_ST_ERASE_ERROR=9U,
    MAL_ST_WRITE_ERROR=10U,
    MAL_ST_READ_ERROR=11U,
    MAL_ST_WRITE_DATA_LEN_MISMATCH=12U,
    MAL_ST_WRITE_BUFFER_DATA_LEFT=13U,
    MAL_ST_ERROR=255U
};

/******************************************************************************/
/* PUBLIC FUNCTION DECLARATIONS                                               */
/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
