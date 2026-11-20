/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  PROJECT          Non Volatile Memory library
 *  COMP_ABBREV      NVMEM
 ******************************************************************************/
 
/******************************************************************************/
/* DOCUMENTATION                                                              */
/******************************************************************************/
/** \file  cacheXS.c
 *  \brief Application specific implementation / configuration for
 *         Caching data in Non Volatile Memory library                        */
/******************************************************************************/
/* INCLUDES                                                                   */
/******************************************************************************/
#include "bsh_stdinc.h"
#include "LibTypes.h"
#include "cache.h"
#include "cacheXS.h"
#include "cachepriv.h"

//lint -save -e923 cast from pointer to unsigned int [MISRA 2012 Rule 11.1, required], [MISRA 2012 Rule 11.4, required], [MISRA 2012 Rule 11.6, required]

/* Definitions and Declarations */
/* VARIABLES ****************************************************************************************/
#if defined(CACHE_INCLUDE_VEEPROM) && !defined(CACHE_INCLUDE_EEPROM)
#if defined(VEEPROM_LARGE)
#define FIELD_ITEM_COUNT 592 //last partition item index + its size

static uint8_t CACHE_aucCache[FIELD_ITEM_COUNT];
/* CACHE_tPartitionTableVeeprom*/
/**
 * \brief partition table defining partition of the VEEPROM
 *
 * The entries of this array define the location where data is cached,
 * length of each portion of data that is stored in one drawer. The index of 
 * the table define the drawer identifier.
 *
 * \attention Maximum number of entries in this table is #CACHE_MAX_DRAWER!
 *
 * \note example
 * \code
 *       / Partition Table for VEEPROM contains 3 drawers.
 *       / A Flag, and two arrays with given size
 *       tVEEPROMDrawerType CACHE_tPartitionTableVeeprom[ ] = 
 *       {
 *          {&XYZ_bFlag, sizeof(XYZ_bFlag)}, 
 *          {&XXX_pucData[0], 16},
 *          {&XXX_pucOtherData[0], sizeof(XXX_pucOtherData)},
 *        };
 * \endcode
 *
 * \note The index of the table defines the drawer identifier that is used for this data: 
 *        - CACHE_tPartitionTableVeeprom[0] contains drawer identifier 0 and its drawer size,
 *        - CACHE_tPartitionTableVeeprom[1] contains drawer identifier 1 and its drawer size,
 *        - ...
 *        - CACHE_tPartitionTableVeeprom[n] contains drawer identifier n and its drawer size.
 *
 * \note have access to the data of this table using the functions:
 *       - Cache::CACHE_ucGetDrawerSize() and 
 *       - Cache::CACHE_pucGetCacheAddress()
 */
const tVEEPROMDrawerType CACHE_tPartitionTableVeeprom[ ] =
{

   {
      &CACHE_aucCache[0],   1  //  _1_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[1],   1  //  _2_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[2],   1  //  _3_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[3],   1  //  _4_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[4],   1  //  _5_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[5],   1  //  _6_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[6],   1  //  _7_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[7],   1  //  _8_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[8],   1  //  _9_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[9],   1  // _10_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[10],  1  // _11_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[11],  1  // _12_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[12],  1  // _13_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[13],  1  // _14_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[14],  1  // _15_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[15],  1  // _16_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[16],  1  // _17_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[17],  1  // _18_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[18],  1  // _19_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[19],  1  // _20_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[20],  1  // _21_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[21],  1  // _22_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[22],  1  // _23_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[23],  1  // _24_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[24],  1  // _25_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[25],  1  // _26_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[26],  1  // _27_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[27],  1  // _28_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[28],  1  // _29_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[29],  1  // _30_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[30],  1  // _31_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[31],  1  // _32_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[32],  1  // _33_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[33],  1  // _34_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[34],  1  // _35_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[35],  1  // _36_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[36],  1  // _37_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[37],  1  // _38_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[38],  1  // _39_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[39],  1  // _40_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[40],  1  // _41_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[41],  1  // _42_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[42],  1  // _43_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[43],  1  // _44_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[44],  1  // _45_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[45],  1  // _46_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[46],  1  // _47_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[47],  1  // _48_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[48],  1  // _49_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[49],  1  // _50_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[50],  1  // _51_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[51],  1  // _52_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[52],  1  // _53_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[53],  1  // _54_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[54],  1  // _55_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[55],  1  // _56_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[56],  1  // _57_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[57],  1  // _58_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[58],  1  // _59_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[59],  1  // _60_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[60],  1  // _61_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[61],  1  // _62_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[62],  1  // _63_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[63],  1  // _64_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[64],  1  // _65_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[65],  1  // _66_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[66],  1  // _67_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[67],  1  // _68_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[68],  1  // _69_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[69],  1  // _70_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[70],  1  // _71_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[71],  1  // _72_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[72],  1  // _73_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[73],  1  // _74_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[74],  1  // _75_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[75],  1  // _76_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[76],  1  // _77_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[77],  1  // _78_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[78],  1  // _79_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[79],  1  // _80_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[80],  1  // _81_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[81],  1  // _82_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[82],  1  // _83_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[83],  1  // _84_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[84],  1  // _85_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[85],  1  // _86_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[86],  1  // _87_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[87],  1  // _88_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[88],  1  // _89_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[89],  1  // _90_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[90],  1  // _91_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[91],  1  // _92_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[92],  1  // _93_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[93],  1  // _94_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[94],  1  // _95_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[95],  1  // _96_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[96],  1  // _97_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[97],  1  // _98_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[98],  1  // _99_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[99],  1  //_100_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[100], 1  //_101_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[101], 1  //_102_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[102], 1  //_103_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[103], 1  //_104_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[104], 1  //_105_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[105], 1  //_106_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[106], 1  //_107_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[107], 1  //_108_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[108], 1  //_109_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[109], 1  //_110_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[110], 1  //_111_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[111], 1  //_112_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[112], 1  //_113_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[113], 1  //_114_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[114], 1  //_115_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[115], 1  //_116_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[116], 1  //_117_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[117], 1  //_118_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[118], 1  //_119_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[119], 1  //_120_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[120], 1  //_121_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[121], 1  //_122_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[122], 1  //_123_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[123], 1  //_124_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[124], 1  //_125_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[125], 1  //_126_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[126], 1  //_127_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[127], 1  //_128_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[128], 1  //_129_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[129], 1  //_130_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[130], 1  //_131_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[131], 1  //_132_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[132], 1  //_133_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[133], 1  //_134_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[134], 1  //_135_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[135], 1  //_136_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[136], 1  //_137_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[137], 1  //_138_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[138], 1  //_139_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[139], 1  //_140_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[140], 1  //_141_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[141], 1  //_142_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[142], 1  //_143_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[143], 1  //_144_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[144], 1  //_145_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[145], 1  //_146_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[146], 1  //_147_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[147], 1  //_148_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[148], 1  //_149_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[149], 1  //_150_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[150], 1  //_151_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[151], 1  //_152_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[152], 1  //_153_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[153], 1  //_154_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[154], 1  //_155_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[155], 1  //_156_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[156], 1  //_157_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[157], 1  //_158_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[158], 1  //_159_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[159], 1  //_160_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[160], 1  //_161_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[161], 1  //_162_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[162], 1  //_163_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[163], 1  //_164_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[164], 1  //_165_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[165], 1  //_166_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[166], 1  //_167_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[167], 1  //_168_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[168], 1  //_169_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[169], 1  //_170_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[170], 1  //_171_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[171], 1  //_172_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[172], 1  //_173_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[173], 1  //_174_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[174], 1  //_175_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[175], 1  //_176_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[176], 1  //_177_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[177], 1  //_178_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[178], 1  //_179_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[179], 1  //_180_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[180], 1  //_181_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[181], 1  //_182_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[182], 1  //_183_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[183], 1  //_184_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[184], 1  //_185_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[185], 1  //_186_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[186], 1  //_187_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[187], 1  //_188_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[188], 1  //_189_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[189], 1  //_190_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[190], 1  //_191_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[191], 1  //_192_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[192], 1  //_193_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[193], 1  //_194_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[194], 1  //_195_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[195], 1  //_196_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[196], 1  //_197_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[197], 1  //_198_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[198], 1  //_199_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[199], 1  //_200_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[200], 1  //_201_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[201], 1  //_202_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[202], 1  //_203_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[203], 3  //_204_CACHE_PLACE_3BYTES
   }
   ,
   {
      &CACHE_aucCache[206], 2  //_205_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[208], 4  //_206_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[212], 4  //_207_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[216], 2  //_208_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[218], 10 //_209_CACHE_PLACE_10BYTES
   }
   ,
   {
      &CACHE_aucCache[228], 4 //_210_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[232], 4  //_211_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[236], 4  //_212_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[240], 4  //_213_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[244], 4  //_214_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[248], 4  //_215_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[252], 4  //_216_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[256], 4  //_217_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[260], 4  //_218_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[264], 4  //_219_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[268], 4  //_220_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[272], 4  //_221_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[276], 4  //_222_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[280], 4  //_223_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[284], 4  //_224_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[288], 4  //_225_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[292], 4  //_226_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[296], 4  //_227_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[300], 4  //_228_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[304], 4  //_229_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[308], 4  //_230_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[312], 4  //_231_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[316], 4  //_232_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[320], 4  //_233_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[324], 4  //_234_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[328], 4  //_235_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[332], 4  //_236_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[336], 4  //_237_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[340], 4  //_238_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[344], 4  //_239_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[348], 4  //_240_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[352], 4  //_241_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[356], 4  //_242_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[360], 4  //_243_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[364], 4  //_244_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[368], 4  //_245_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[372], 4  //_246_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[376], 4  //_247_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[380], 4  //_248_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[384], 4  //_249_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[388], 4  //_250_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[392], 4  //_251_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[396], 4  //_252_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[400], 4  //_253_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[404], 4  //_254_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[408], 4  //_255_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[412], 4  //_256_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[416], 4  //_257_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[420], 4  //_258_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[424], 4  //_259_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[428], 4  //_260_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[432], 4  //_261_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[436], 4  //_262_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[440], 4  //_263_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[444], 4  //_264_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[448], 4  //_265_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[452], 4  //_266_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[456], 4  //_267_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[460], 4  //_268_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[464], 4  //_269_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[468], 4  //_270_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[472], 4  //_271_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[476], 4  //_272_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[480], 4  //_273_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[484], 4  //_274_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[488], 4  //_275_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[492], 4  //_276_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[496], 4  //_277_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[500], 4  //_278_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[504], 4  //_279_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[508], 4  //_280_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[512], 4  //_281_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[516], 4  //_282_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[520], 4  //_283_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[524], 4  //_284_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[528], 4  //_285_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[532], 4  //_286_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[536], 4  //_287_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[540], 4  //_288_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[544], 4  //_289_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[548], 4  //_290_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[552], 4  //_291_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[556], 4  //_292_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[560], 4  //_293_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[564], 4  //_294_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[568], 4  //_295_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[572], 4  //_296_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[576], 4  //_297_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[580], 4  //_298_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[584], 4  //_299_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[588], 4  //_300_CACHE_PLACE_4BYTES
   }
}
;

/**
 * \brief number of drawers of VEEPROM partition table
 * 
 * 
 */

const tDrawerID CACHE_tNumberOfDrawersVEEPROM = (tDrawerID)UTI_NELEMENTS ( CACHE_tPartitionTableVeeprom );

/**
 * \brief bitfield storing if cached data is valid
 *
 * bit 0 shows validity of cached value(s) for drawer 0, bit 1 for drawer 1, ... bit n for drawer n.
 * If bit is set, the cached value for this drawer is valid, otherwise not.
 */
uint8_t CACHE_aucDrawerCacheValidVeeprom[ ( UTI_NELEMENTS( CACHE_tPartitionTableVeeprom ) + 7u ) / 8u ];

#else // not defined(VEEPROM_LARGE) part
#define FIELD_ITEM_COUNT 198 //last partition item index + its size

static uint8_t CACHE_aucCache[FIELD_ITEM_COUNT];
/* CACHE_tPartitionTableVeeprom*/
/**
 * \brief partition table defining partition of the VEEPROM
 *
 * The entries of this array define the location where data is cached,
 * length of each portion of data that is stored in one drawer. The index of 
 * the table define the drawer identifier.
 *
 * \attention Maximum number of entries in this table is #CACHE_MAX_DRAWER!
 *
 * \note example
 * \code
 *       / Partition Table for VEEPROM contains 3 drawers.
 *       / A Flag, and two arrays with given size
 *       tVEEPROMDrawerType CACHE_tPartitionTableVeeprom[ ] = 
 *       {
 *          {&XYZ_bFlag, sizeof(XYZ_bFlag)}, 
 *          {&XXX_pucData[0], 16},
 *          {&XXX_pucOtherData[0], sizeof(XXX_pucOtherData)},
 *        };
 * \endcode
 *
 * \note The index of the table defines the drawer identifier that is used for this data: 
 *        - CACHE_tPartitionTableVeeprom[0] contains drawer identifier 0 and its drawer size,
 *        - CACHE_tPartitionTableVeeprom[1] contains drawer identifier 1 and its drawer size,
 *        - ...
 *        - CACHE_tPartitionTableVeeprom[n] contains drawer identifier n and its drawer size.
 *
 * \note have access to the data of this table using the functions:
 *       - Cache::CACHE_ucGetDrawerSize() and 
 *       - Cache::CACHE_pucGetCacheAddress()
 */
const tVEEPROMDrawerType CACHE_tPartitionTableVeeprom[ ] =
{

   {
      &CACHE_aucCache[0],   1  //  _1_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[1],   1  //  _2_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[2],   1  //  _3_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[3],   1  //  _4_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[4],   1  //  _5_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[5],   1  //  _6_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[6],   1  //  _7_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[7],   1  //  _8_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[8],   1  //  _9_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[9],   1  // _10_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[10],  1  // _11_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[11],  1  // _12_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[12],  1  // _13_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[13],  1  // _14_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[14],  1  // _15_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[15],  1  // _16_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[16],  1  // _17_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[17],  1  // _18_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[18],  1  // _19_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[19],  1  // _20_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[20],  1  // _21_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[21],  1  // _22_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[22],  1  // _23_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[23],  1  // _24_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[24],  1  // _25_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[25],  1  // _26_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[26],  1  // _27_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[27],  1  // _28_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[28],  1  // _29_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[29],  1  // _30_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[30],  1  // _31_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[31],  1  // _32_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[32],  1  // _33_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[33],  1  // _34_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[34],  1  // _35_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[35],  1  // _36_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[36],  1  // _37_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[37],  1  // _38_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[38],  1  // _39_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[39],  1  // _40_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[40],  1  // _41_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[41],  1  // _42_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[42],  1  // _43_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[43],  1  // _44_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[44],  1  // _45_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[45],  1  // _46_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[46],  1  // _47_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[47],  1  // _48_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[48],  1  // _49_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[49],  1  // _50_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[50],  1  // _51_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[51],  1  // _52_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[52],  1  // _53_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[53],  1  // _54_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[54],  1  // _55_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[55],  1  // _56_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[56],  1  // _57_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[57],  1  // _58_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[58],  1  // _59_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[59],  1  // _60_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[60],  1  // _61_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[61],  1  // _62_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[62],  1  // _63_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[63],  1  // _64_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[64],  1  // _65_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[65],  1  // _66_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[66],  1  // _67_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[67],  1  // _68_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[68],  1  // _69_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[69],  1  // _70_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[70],  1  // _71_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[71],  1  // _72_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[72],  1  // _73_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[73],  1  // _74_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[74],  1  // _75_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[75],  1  // _76_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[76],  1  // _77_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[77],  1  // _78_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[78],  1  // _79_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[79],  1  // _80_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[80],  1  // _81_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[81],  1  // _82_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[82],  1  // _83_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[83],  1  // _84_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[84],  1  // _85_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[85],  1  // _86_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[86],  1  // _87_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[87],  1  // _88_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[88],  1  // _89_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[89],  1  // _90_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[90],  1  // _91_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[91],  1  // _92_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[92],  1  // _93_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[93],  1  // _94_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[94],  1  // _95_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[95],  1  // _96_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[96],  1  // _97_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[97],  1  // _98_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[98],  1  // _99_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[99],  1  //_100_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[100], 1  //_101_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[101], 1  //_102_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[102], 1  //_103_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[103], 1  //_104_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[104], 1  //_105_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[105], 1  //_106_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[106], 1  //_107_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[107], 1  //_108_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[108], 1  //_109_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[109], 1  //_110_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[110], 1  //_111_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[111], 1  //_112_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[112], 1  //_113_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[113], 1  //_114_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[114], 1  //_115_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[115], 1  //_116_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[116], 1  //_117_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[117], 1  //_118_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[118], 1  //_119_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[119], 1  //_120_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[120], 1  //_121_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[121], 2  //_122_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[123], 3  //_123_CACHE_PLACE_3BYTES
   }
   ,
   {
      &CACHE_aucCache[126], 4  //_124_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[130], 5  //_125_CACHE_PLACE_5BYTES
   }
   ,
   {
      &CACHE_aucCache[135], 6  //_126_CACHE_PLACE_6BYTES
   }
   ,
   {
      &CACHE_aucCache[141], 7  //_127_CACHE_PLACE_7BYTES
   }
   ,
   {
      &CACHE_aucCache[148], 8  //_128_CACHE_PLACE_8BYTES
   }
   ,
   {
      &CACHE_aucCache[156], 9  //_129_CACHE_PLACE_9BYTES
   }
   ,
   {
      &CACHE_aucCache[165], 10 //_130_CACHE_PLACE_10BYTES
   }
   ,
   {
      &CACHE_aucCache[175], 11 //_131_CACHE_PLACE_11BYTES
   }
   ,
   {
      &CACHE_aucCache[186], 12 //_132_CACHE_PLACE_12BYTES
   }
}
;

/**
 * \brief number of drawers of VEEPROM partition table
 * 
 * 
 */

const uint8_t CACHE_ucNumberOfDrawersVEEPROM = (uint8_t)UTI_NELEMENTS ( CACHE_tPartitionTableVeeprom );

/**
 * \brief bitfield storing if cached data is valid
 *
 * bit 0 shows validity of cached value(s) for drawer 0, bit 1 for drawer 1, ... bit n for drawer n.
 * If bit is set, the cached value for this drawer is valid, otherwise not.
 */
uint8_t CACHE_aucDrawerCacheValidVeeprom[ ( UTI_NELEMENTS( CACHE_tPartitionTableVeeprom ) + 7u ) / 8u ];
#endif //#if defined(VEEPROM_LARGE)
#endif //#if defined(CACHE_INCLUDE_VEEPROM) && !defined(CACHE_INCLUDE_EEPROM)

#if defined(CACHE_INCLUDE_EEPROM) && !defined(CACHE_INCLUDE_VEEPROM)
#if defined(EXTERNAL_LARGE)
#define FIELD_ITEM_COUNT 308 //last partition item index + its size

static uint8_t CACHE_aucCache[FIELD_ITEM_COUNT];
/* CACHE_tPartitionTableEeeprom*/
/**
 * \brief partition table defining partition of the EEPROM
 *
 * The entries of this array define the location where data is cached and its
 * length. The index within the table define the drawer identifier.
 *
 * \attention Maximum number of entries in this table is #CACHE_MAX_DRAWER!
 *
 * \note example
 * \code
 *       / Partition Table of Eeprom contains 3 drawers: one Flag and 2 arrays of given size
 *       tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ] = 
 *       {
 *          {&XYZ_bFlag, sizeof(XYZ_bFlag), ulAddEEP_1}, 
 *          {&XXX_pucData[0], 16, ulAddEEP_2},
 *          {&XXX_pucOtherData[0], sizeof(XXX_pucOtherData), ulAddEEP_3},
 *        };
 * \endcode
 *
 * \note The index of the table defines the drawer identifier that is used for this data: 
 *        - CACHE_tPartitionTableEeprom[0] contains drawer identifier 0 and its drawer size,
 *        - CACHE_tPartitionTableEeprom[1] contains drawer identifier 1 and its drawer size,
 *        - ...
 *        - CACHE_tPartitionTableEeprom[n] contains drawer identifier n and its drawer size.
 *
 * \note have access to the data of this table using the functions:
 *       - Cache::CACHE_ucGetDrawerSize() and 
 *       - Cache::CACHE_pucGetCacheAddress()
 *
 */
const tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ] =
{
   /* cache address     , size, virtual EEPROM address = prev variable position + prev variable size */
   {
      &CACHE_aucCache[  0], 1, (uint32_t)&CACHE_aucCache[  0]  //_1_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[  1], 1, (uint32_t)&CACHE_aucCache[  1]  //_2_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[  2], 1, (uint32_t)&CACHE_aucCache[  2]  //_3_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[  3], 1, (uint32_t)&CACHE_aucCache[  3]  //_4_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[  4], 1, (uint32_t)&CACHE_aucCache[  4]  //_5_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[  5], 1, (uint32_t)&CACHE_aucCache[  5]  //_6_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[  6], 1, (uint32_t)&CACHE_aucCache[  6]  //_7_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[  7], 1, (uint32_t)&CACHE_aucCache[  7]  //_8_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[  8], 1, (uint32_t)&CACHE_aucCache[  8]  //_9_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[  9], 1, (uint32_t)&CACHE_aucCache[  9]  //_10_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 10], 1, (uint32_t)&CACHE_aucCache[ 10]  //_11_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 11], 1, (uint32_t)&CACHE_aucCache[ 11]  //_12_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 12], 1, (uint32_t)&CACHE_aucCache[ 12]  //_13_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 13], 1, (uint32_t)&CACHE_aucCache[ 13]  //_14_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 14], 1, (uint32_t)&CACHE_aucCache[ 14]  //_15_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 15], 1, (uint32_t)&CACHE_aucCache[ 15]  //_16_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 16], 1, (uint32_t)&CACHE_aucCache[ 16]  //_17_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 17], 1, (uint32_t)&CACHE_aucCache[ 17]  //_18_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 18], 1, (uint32_t)&CACHE_aucCache[ 18]  //_19_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 19], 1, (uint32_t)&CACHE_aucCache[ 19]  //_20_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 20], 1, (uint32_t)&CACHE_aucCache[ 20]  //_21_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 21], 1, (uint32_t)&CACHE_aucCache[ 21]  //_22_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 22], 1, (uint32_t)&CACHE_aucCache[ 22]  //_23_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 23], 1, (uint32_t)&CACHE_aucCache[ 23]  //_24_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 24], 1, (uint32_t)&CACHE_aucCache[ 24]  //_25_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 25], 1, (uint32_t)&CACHE_aucCache[ 25]  //_26_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 26], 1, (uint32_t)&CACHE_aucCache[ 26]  //_27_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 27], 1, (uint32_t)&CACHE_aucCache[ 27]  //_28_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 28], 1, (uint32_t)&CACHE_aucCache[ 28]  //_29_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 29], 1, (uint32_t)&CACHE_aucCache[ 29]  //_30_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 30], 1, (uint32_t)&CACHE_aucCache[ 30]  //_31_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 31], 1, (uint32_t)&CACHE_aucCache[ 31]  //_32_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 32], 1, (uint32_t)&CACHE_aucCache[ 32]  //_33_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 33], 1, (uint32_t)&CACHE_aucCache[ 33]  //_34_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 34], 1, (uint32_t)&CACHE_aucCache[ 34]  //_35_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 35], 1, (uint32_t)&CACHE_aucCache[ 35]  //_36_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 36], 1, (uint32_t)&CACHE_aucCache[ 36]  //_37_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 37], 1, (uint32_t)&CACHE_aucCache[ 37]  //_38_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 38], 1, (uint32_t)&CACHE_aucCache[ 38]  //_39_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 39], 1, (uint32_t)&CACHE_aucCache[ 39]  //_40_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 40], 1, (uint32_t)&CACHE_aucCache[ 40]  //_41_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 41], 1, (uint32_t)&CACHE_aucCache[ 41]  //_42_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 42], 1, (uint32_t)&CACHE_aucCache[ 42]  //_43_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 43], 1, (uint32_t)&CACHE_aucCache[ 43]  //_44_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 44], 1, (uint32_t)&CACHE_aucCache[ 44]  //_45_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 45], 1, (uint32_t)&CACHE_aucCache[ 45]  //_46_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 46], 1, (uint32_t)&CACHE_aucCache[ 46]  //_47_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 47], 1, (uint32_t)&CACHE_aucCache[ 47]  //_48_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 48], 1, (uint32_t)&CACHE_aucCache[ 48]  //_49_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 49], 1, (uint32_t)&CACHE_aucCache[ 49]  //_50_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 50], 1, (uint32_t)&CACHE_aucCache[ 50]  //_51_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 51], 1, (uint32_t)&CACHE_aucCache[ 51]  //_52_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 52], 1, (uint32_t)&CACHE_aucCache[ 52]  //_53_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 53], 1, (uint32_t)&CACHE_aucCache[ 53]  //_54_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 54], 1, (uint32_t)&CACHE_aucCache[ 54]  //_55_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 55], 1, (uint32_t)&CACHE_aucCache[ 55]  //_56_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 56], 1, (uint32_t)&CACHE_aucCache[ 56]  //_57_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 57], 1, (uint32_t)&CACHE_aucCache[ 57]  //_58_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 58], 1, (uint32_t)&CACHE_aucCache[ 58]  //_59_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 59], 1, (uint32_t)&CACHE_aucCache[ 59]  //_60_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 60], 1, (uint32_t)&CACHE_aucCache[ 60]  //_61_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 61], 1, (uint32_t)&CACHE_aucCache[ 61]  //_62_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 62], 1, (uint32_t)&CACHE_aucCache[ 62]  //_63_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 63], 1, (uint32_t)&CACHE_aucCache[ 63]  //_64_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 64], 1, (uint32_t)&CACHE_aucCache[ 64]  //_65_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 65], 1, (uint32_t)&CACHE_aucCache[ 65]  //_66_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 66], 1, (uint32_t)&CACHE_aucCache[ 66]  //_67_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 67], 1, (uint32_t)&CACHE_aucCache[ 67]  //_68_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 68], 1, (uint32_t)&CACHE_aucCache[ 68]  //_69_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 69], 1, (uint32_t)&CACHE_aucCache[ 69]  //_70_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 70], 1, (uint32_t)&CACHE_aucCache[ 70]  //_71_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 71], 1, (uint32_t)&CACHE_aucCache[ 71]  //_72_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 72], 1, (uint32_t)&CACHE_aucCache[ 72]  //_73_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 73], 1, (uint32_t)&CACHE_aucCache[ 73]  //_74_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 74], 1, (uint32_t)&CACHE_aucCache[ 74]  //_75_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 75], 1, (uint32_t)&CACHE_aucCache[ 75]  //_76_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 76], 1, (uint32_t)&CACHE_aucCache[ 76]  //_77_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 77], 1, (uint32_t)&CACHE_aucCache[ 77]  //_78_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 78], 1, (uint32_t)&CACHE_aucCache[ 78]  //_79_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 79], 1, (uint32_t)&CACHE_aucCache[ 79]  //_80_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 80], 1, (uint32_t)&CACHE_aucCache[ 80]  //_81_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 81], 1, (uint32_t)&CACHE_aucCache[ 81]  //_82_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 82], 1, (uint32_t)&CACHE_aucCache[ 82]  //_83_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 83], 1, (uint32_t)&CACHE_aucCache[ 83]  //_84_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 84], 1, (uint32_t)&CACHE_aucCache[ 84]  //_85_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 85], 1, (uint32_t)&CACHE_aucCache[ 85]  //_86_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 86], 1, (uint32_t)&CACHE_aucCache[ 86]  //_87_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 87], 1, (uint32_t)&CACHE_aucCache[ 87]  //_88_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 88], 1, (uint32_t)&CACHE_aucCache[ 88]  //_89_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 89], 1, (uint32_t)&CACHE_aucCache[ 89]  //_90_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 90], 1, (uint32_t)&CACHE_aucCache[ 90]  //_91_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 91], 1, (uint32_t)&CACHE_aucCache[ 91]  //_92_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 92], 1, (uint32_t)&CACHE_aucCache[ 92]  //_93_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 93], 1, (uint32_t)&CACHE_aucCache[ 93]  //_94_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 94], 1, (uint32_t)&CACHE_aucCache[ 94]  //_95_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 95], 1, (uint32_t)&CACHE_aucCache[ 95]  //_96_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 96], 1, (uint32_t)&CACHE_aucCache[ 96]  //_97_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 97], 1, (uint32_t)&CACHE_aucCache[ 97]  //_98_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 98], 1, (uint32_t)&CACHE_aucCache[ 98]  //_99_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 99], 1, (uint32_t)&CACHE_aucCache[ 99]  //_100_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[100], 1, (uint32_t)&CACHE_aucCache[100]  //_101_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[101], 1, (uint32_t)&CACHE_aucCache[101]  //_102_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[102], 1, (uint32_t)&CACHE_aucCache[102]  //_103_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[103], 1, (uint32_t)&CACHE_aucCache[103]  //_104_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[104], 1, (uint32_t)&CACHE_aucCache[104]  //_105_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[105], 1, (uint32_t)&CACHE_aucCache[105]  //_106_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[106], 1, (uint32_t)&CACHE_aucCache[106]  //_107_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[107], 1, (uint32_t)&CACHE_aucCache[107]  //_108_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[108], 1, (uint32_t)&CACHE_aucCache[108]  //_109_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[109], 1, (uint32_t)&CACHE_aucCache[109]  //_110_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[110], 1, (uint32_t)&CACHE_aucCache[110]  //_111_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[111], 1, (uint32_t)&CACHE_aucCache[111]  //_112_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[112], 1, (uint32_t)&CACHE_aucCache[112]  //_113_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[113], 1, (uint32_t)&CACHE_aucCache[113]  //_114_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[114], 1, (uint32_t)&CACHE_aucCache[114]  //_115_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[115], 1, (uint32_t)&CACHE_aucCache[115]  //_116_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[116], 1, (uint32_t)&CACHE_aucCache[116]  //_117_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[117], 1, (uint32_t)&CACHE_aucCache[117]  //_118_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[118], 1, (uint32_t)&CACHE_aucCache[118]  //_119_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[119], 1, (uint32_t)&CACHE_aucCache[119]  //_120_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[120], 1, (uint32_t)&CACHE_aucCache[120]  //_121_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[121], 1, (uint32_t)&CACHE_aucCache[121]  //_122_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[122], 1, (uint32_t)&CACHE_aucCache[122]  //_123_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[123], 1, (uint32_t)&CACHE_aucCache[123]  //_124_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[124], 1, (uint32_t)&CACHE_aucCache[124]  //_125_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[125], 1, (uint32_t)&CACHE_aucCache[125]  //_126_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[126], 1, (uint32_t)&CACHE_aucCache[126]  //_127_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[127], 1, (uint32_t)&CACHE_aucCache[127]  //_128_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[128], 1, (uint32_t)&CACHE_aucCache[128]  //_129_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[129], 1, (uint32_t)&CACHE_aucCache[129]  //_130_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[130], 1, (uint32_t)&CACHE_aucCache[130]  //_131_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[131], 1, (uint32_t)&CACHE_aucCache[131]  //_132_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[132], 1, (uint32_t)&CACHE_aucCache[132]  //_133_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[133], 1, (uint32_t)&CACHE_aucCache[133]  //_134_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[134], 1, (uint32_t)&CACHE_aucCache[134]  //_135_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[135], 1, (uint32_t)&CACHE_aucCache[135]  //_136_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[136], 1, (uint32_t)&CACHE_aucCache[136]  //_137_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[137], 1, (uint32_t)&CACHE_aucCache[137]  //_138_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[138], 1, (uint32_t)&CACHE_aucCache[138]  //_139_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[139], 1, (uint32_t)&CACHE_aucCache[139]  //_140_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[140], 1, (uint32_t)&CACHE_aucCache[140]  //_141_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[141], 1, (uint32_t)&CACHE_aucCache[141]  //_142_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[142], 1, (uint32_t)&CACHE_aucCache[142]  //_143_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[143], 1, (uint32_t)&CACHE_aucCache[143]  //_144_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[144], 1, (uint32_t)&CACHE_aucCache[144]  //_145_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[145], 1, (uint32_t)&CACHE_aucCache[145]  //_146_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[146], 1, (uint32_t)&CACHE_aucCache[146]  //_147_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[147], 1, (uint32_t)&CACHE_aucCache[147]  //_148_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[148], 1, (uint32_t)&CACHE_aucCache[148]  //_149_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[149], 1, (uint32_t)&CACHE_aucCache[149]  //_150_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[150], 1, (uint32_t)&CACHE_aucCache[150]  //_151_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[151], 1, (uint32_t)&CACHE_aucCache[151]  //_152_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[152], 1, (uint32_t)&CACHE_aucCache[152]  //_153_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[153], 1, (uint32_t)&CACHE_aucCache[153]  //_154_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[154], 1, (uint32_t)&CACHE_aucCache[154]  //_155_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[155], 1, (uint32_t)&CACHE_aucCache[155]  //_156_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[156], 1, (uint32_t)&CACHE_aucCache[156]  //_157_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[157], 1, (uint32_t)&CACHE_aucCache[157]  //_158_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[158], 1, (uint32_t)&CACHE_aucCache[158]  //_159_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[159], 1, (uint32_t)&CACHE_aucCache[159]  //_160_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[160], 1, (uint32_t)&CACHE_aucCache[160]  //_161_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[161], 1, (uint32_t)&CACHE_aucCache[161]  //_162_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[162], 1, (uint32_t)&CACHE_aucCache[162]  //_163_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[163], 1, (uint32_t)&CACHE_aucCache[163]  //_164_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[164], 1, (uint32_t)&CACHE_aucCache[164]  //_165_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[165], 1, (uint32_t)&CACHE_aucCache[165]  //_166_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[166], 1, (uint32_t)&CACHE_aucCache[166]  //_167_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[167], 1, (uint32_t)&CACHE_aucCache[167]  //_168_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[168], 1, (uint32_t)&CACHE_aucCache[168]  //_169_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[169], 1, (uint32_t)&CACHE_aucCache[169]  //_170_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[170], 1, (uint32_t)&CACHE_aucCache[170]  //_171_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[171], 1, (uint32_t)&CACHE_aucCache[171]  //_172_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[172], 1, (uint32_t)&CACHE_aucCache[172]  //_173_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[173], 1, (uint32_t)&CACHE_aucCache[173]  //_174_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[174], 1, (uint32_t)&CACHE_aucCache[174]  //_175_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[175], 1, (uint32_t)&CACHE_aucCache[175]  //_176_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[176], 1, (uint32_t)&CACHE_aucCache[176]  //_177_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[177], 1, (uint32_t)&CACHE_aucCache[177]  //_178_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[178], 1, (uint32_t)&CACHE_aucCache[178]  //_179_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[179], 1, (uint32_t)&CACHE_aucCache[179]  //_180_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[180], 1, (uint32_t)&CACHE_aucCache[180]  //_181_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[181], 1, (uint32_t)&CACHE_aucCache[181]  //_182_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[182], 1, (uint32_t)&CACHE_aucCache[182]  //_183_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[183], 1, (uint32_t)&CACHE_aucCache[183]  //_184_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[184], 1, (uint32_t)&CACHE_aucCache[184]  //_185_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[185], 1, (uint32_t)&CACHE_aucCache[185]  //_186_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[186], 1, (uint32_t)&CACHE_aucCache[186]  //_187_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[187], 1, (uint32_t)&CACHE_aucCache[187]  //_188_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[188], 1, (uint32_t)&CACHE_aucCache[188]  //_189_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[189], 1, (uint32_t)&CACHE_aucCache[189]  //_190_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[190], 1, (uint32_t)&CACHE_aucCache[190]  //_191_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[191], 1, (uint32_t)&CACHE_aucCache[191]  //_192_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[192], 1, (uint32_t)&CACHE_aucCache[192]  //_193_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[193], 1, (uint32_t)&CACHE_aucCache[193]  //_194_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[194], 1, (uint32_t)&CACHE_aucCache[194]  //_195_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[195], 1, (uint32_t)&CACHE_aucCache[195]  //_196_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[196], 1, (uint32_t)&CACHE_aucCache[196]  //_197_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[197], 1, (uint32_t)&CACHE_aucCache[197]  //_198_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[198], 1, (uint32_t)&CACHE_aucCache[198]  //_199_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[199], 1, (uint32_t)&CACHE_aucCache[199]  //_200_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[200], 1, (uint32_t)&CACHE_aucCache[200]  //_201_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[201], 1, (uint32_t)&CACHE_aucCache[201]  //_202_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[202], 1, (uint32_t)&CACHE_aucCache[202]  //_203_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[203], 1, (uint32_t)&CACHE_aucCache[203]  //_204_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[204], 1, (uint32_t)&CACHE_aucCache[204]  //_205_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[205], 1, (uint32_t)&CACHE_aucCache[205]  //_206_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[206], 1, (uint32_t)&CACHE_aucCache[206]  //_207_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[207], 1, (uint32_t)&CACHE_aucCache[207]  //_208_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[208], 1, (uint32_t)&CACHE_aucCache[208]  //_209_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[209], 1, (uint32_t)&CACHE_aucCache[209]  //_210_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[210], 1, (uint32_t)&CACHE_aucCache[210]  //_211_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[211], 1, (uint32_t)&CACHE_aucCache[211]  //_212_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[212], 1, (uint32_t)&CACHE_aucCache[212]  //_213_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[213], 1, (uint32_t)&CACHE_aucCache[213]  //_214_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[214], 1, (uint32_t)&CACHE_aucCache[214]  //_215_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[215], 1, (uint32_t)&CACHE_aucCache[215]  //_216_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[216], 1, (uint32_t)&CACHE_aucCache[216]  //_217_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[217], 1, (uint32_t)&CACHE_aucCache[217]  //_218_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[218], 1, (uint32_t)&CACHE_aucCache[218]  //_219_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[219], 1, (uint32_t)&CACHE_aucCache[219]  //_220_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[220], 1, (uint32_t)&CACHE_aucCache[220]  //_221_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[221], 1, (uint32_t)&CACHE_aucCache[221]  //_222_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[222], 1, (uint32_t)&CACHE_aucCache[222]  //_223_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[223], 1, (uint32_t)&CACHE_aucCache[223]  //_224_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[224], 1, (uint32_t)&CACHE_aucCache[224]  //_225_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[225], 1, (uint32_t)&CACHE_aucCache[225]  //_226_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[226], 1, (uint32_t)&CACHE_aucCache[226]  //_227_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[227], 1, (uint32_t)&CACHE_aucCache[227]  //_228_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[228], 1, (uint32_t)&CACHE_aucCache[228]  //_229_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[229], 1, (uint32_t)&CACHE_aucCache[229]  //_230_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[230], 1, (uint32_t)&CACHE_aucCache[230]  //_231_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[231], 1, (uint32_t)&CACHE_aucCache[231]  //_232_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[232], 1, (uint32_t)&CACHE_aucCache[232]  //_233_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[233], 1, (uint32_t)&CACHE_aucCache[233]  //_234_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[234], 1, (uint32_t)&CACHE_aucCache[234]  //_235_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[235], 1, (uint32_t)&CACHE_aucCache[235]  //_236_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[236], 1, (uint32_t)&CACHE_aucCache[236]  //_237_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[237], 1, (uint32_t)&CACHE_aucCache[237]  //_238_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[238], 1, (uint32_t)&CACHE_aucCache[238]  //_239_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[239], 1, (uint32_t)&CACHE_aucCache[239]  //_240_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[240], 1, (uint32_t)&CACHE_aucCache[240]  //_241_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[241], 1, (uint32_t)&CACHE_aucCache[241]  //_242_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[242], 1, (uint32_t)&CACHE_aucCache[242]  //_243_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[243], 1, (uint32_t)&CACHE_aucCache[243]  //_244_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[244], 1, (uint32_t)&CACHE_aucCache[244]  //_245_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[245], 1, (uint32_t)&CACHE_aucCache[245]  //_246_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[246], 1, (uint32_t)&CACHE_aucCache[246]  //_247_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[247], 1, (uint32_t)&CACHE_aucCache[247]  //_248_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[248], 1, (uint32_t)&CACHE_aucCache[248]  //_249_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[249], 1, (uint32_t)&CACHE_aucCache[249]  //_250_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[250], 1, (uint32_t)&CACHE_aucCache[250]  //_251_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[251], 1, (uint32_t)&CACHE_aucCache[251]  //_252_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[252], 1, (uint32_t)&CACHE_aucCache[252]  //_253_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[253], 1, (uint32_t)&CACHE_aucCache[253]  //_254_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[254], 1, (uint32_t)&CACHE_aucCache[254]  //_255_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[255], 1, (uint32_t)&CACHE_aucCache[255]  //_256_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[256], 1, (uint32_t)&CACHE_aucCache[256]  //_257_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[257], 1, (uint32_t)&CACHE_aucCache[257]  //_258_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[258], 1, (uint32_t)&CACHE_aucCache[258]  //_259_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[259], 1, (uint32_t)&CACHE_aucCache[259]  //_260_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[260], 1, (uint32_t)&CACHE_aucCache[260]  //_261_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[261], 1, (uint32_t)&CACHE_aucCache[261]  //_262_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[262], 1, (uint32_t)&CACHE_aucCache[262]  //_263_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[263], 1, (uint32_t)&CACHE_aucCache[263]  //_264_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[264], 1, (uint32_t)&CACHE_aucCache[264]  //_265_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[265], 1, (uint32_t)&CACHE_aucCache[265]  //_266_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[266], 1, (uint32_t)&CACHE_aucCache[266]  //_267_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[267], 1, (uint32_t)&CACHE_aucCache[267]  //_268_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[268], 1, (uint32_t)&CACHE_aucCache[268]  //_269_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[269], 1, (uint32_t)&CACHE_aucCache[269]  //_270_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[270], 1, (uint32_t)&CACHE_aucCache[270]  //_271_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[271], 1, (uint32_t)&CACHE_aucCache[271]  //_272_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[272], 1, (uint32_t)&CACHE_aucCache[272]  //_273_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[273], 1, (uint32_t)&CACHE_aucCache[273]  //_274_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[274], 1, (uint32_t)&CACHE_aucCache[274]  //_275_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[275], 1, (uint32_t)&CACHE_aucCache[275]  //_276_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[276], 1, (uint32_t)&CACHE_aucCache[276]  //_277_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[277], 1, (uint32_t)&CACHE_aucCache[277]  //_278_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[278], 1, (uint32_t)&CACHE_aucCache[278]  //_279_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[279], 1, (uint32_t)&CACHE_aucCache[279]  //_280_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[280], 1, (uint32_t)&CACHE_aucCache[280]  //_281_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[281], 1, (uint32_t)&CACHE_aucCache[281]  //_282_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[282], 1, (uint32_t)&CACHE_aucCache[282]  //_283_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[283], 1, (uint32_t)&CACHE_aucCache[283]  //_284_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[284], 1, (uint32_t)&CACHE_aucCache[284]  //_285_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[285], 1, (uint32_t)&CACHE_aucCache[285]  //_286_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[286], 1, (uint32_t)&CACHE_aucCache[286]  //_287_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[287], 1, (uint32_t)&CACHE_aucCache[287]  //_288_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[288], 1, (uint32_t)&CACHE_aucCache[288]  //_289_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[289], 1, (uint32_t)&CACHE_aucCache[289]  //_290_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[290], 1, (uint32_t)&CACHE_aucCache[290]  //_291_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[291], 1, (uint32_t)&CACHE_aucCache[291]  //_292_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[292], 1, (uint32_t)&CACHE_aucCache[292]  //_293_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[293], 1, (uint32_t)&CACHE_aucCache[293]  //_294_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[294], 1, (uint32_t)&CACHE_aucCache[294]  //_295_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[295], 2, (uint32_t)&CACHE_aucCache[295]  //_296_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[297], 4, (uint32_t)&CACHE_aucCache[297]  //_297_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[301], 1, (uint32_t)&CACHE_aucCache[301]  //_298_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[302], 2, (uint32_t)&CACHE_aucCache[302]  //_299_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[304], 4, (uint32_t)&CACHE_aucCache[304]  //_300_CACHE_PLACE_4BYTES
   }
}
;

/**
 * \brief number of drawers of EEPROM partition table
 */

const tDrawerID CACHE_tNumberOfDrawersEEPROM = (tDrawerID)UTI_NELEMENTS( CACHE_tPartitionTableEeprom );

/**
 * \brief bitfield storing if cached data is valid
 *
 * bit 0 shows validity of cached value(s) for drawer 0, bit 1 for drawer 1, ... bit n for drawer n.
 * If bit is set, the cached value for this drawer is valid, otherwise not.
 */
uint8_t CACHE_aucDrawerCacheValidEeprom[ ( (tDrawerID)UTI_NELEMENTS( CACHE_tPartitionTableEeprom ) + 7u ) / 8u ];

const uint32_t CACHE_ulNegativeOffsetForEepAddress = (uint32_t)CACHE_aucCache; //lint !e923 cast from pointer to unsigned int
const uint32_t CACHE_ulOffsetForEepAddress         = 0UL;
#else // not defined(EXTERNAL_LARGE) part
#define FIELD_ITEM_COUNT 14 //last partition item index + its size

static uint8_t CACHE_aucCache[FIELD_ITEM_COUNT];
/*~A:CACHE_tPartitionTableEeeprom*/
/**
 * \brief partition table defining partition of the EEPROM
 *
 * The entries of this array define the location where data is cached and its
 * length. The index within the table define the drawer identifier.
 *
 * \attention Maximum number of entries in this table is #CACHE_MAX_DRAWER!
 *
 * \note example
 * \code
 *       / Partition Table of Eeprom contains 3 drawers: one Flag and 2 arrays of given size
 *       tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ] = 
 *       {
 *          {&XYZ_bFlag, sizeof(XYZ_bFlag), ulAddEEP_1}, 
 *          {&XXX_pucData[0], 16, ulAddEEP_2},
 *          {&XXX_pucOtherData[0], sizeof(XXX_pucOtherData), ulAddEEP_3},
 *        };
 * \endcode
 *
 * \note The index of the table defines the drawer identifier that is used for this data: 
 *        - CACHE_tPartitionTableEeprom[0] contains drawer identifier 0 and its drawer size,
 *        - CACHE_tPartitionTableEeprom[1] contains drawer identifier 1 and its drawer size,
 *        - ...
 *        - CACHE_tPartitionTableEeprom[n] contains drawer identifier n and its drawer size.
 *
 * \note have access to the data of this table using the functions:
 *       - Cache::CACHE_ucGetDrawerSize() and 
 *       - Cache::CACHE_pucGetCacheAddress()
 *
 */
//lint -save -e923 cast from pointer to unsigned int
const tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ] =
{
   /* cache address     , size, virtual EEPROM address = prev variable position + prev variable size */
   {
      &CACHE_aucCache[ 0], 1, (uint32_t)&CACHE_aucCache[ 0]  //_1_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 1], 2, (uint32_t)&CACHE_aucCache[ 1] //_2_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[ 3], 4, (uint32_t)&CACHE_aucCache[ 3] //_3_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[ 7], 1, (uint32_t)&CACHE_aucCache[ 7] //_4_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[ 8], 2, (uint32_t)&CACHE_aucCache[ 8] //_5_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[10], 4, (uint32_t)&CACHE_aucCache[10] //_6_CACHE_PLACE_4BYTES
   }
};

//lint -restore -e923 

/**
 * \brief number of drawers of EEPROM partition table
 */

const uint8_t CACHE_ucNumberOfDrawersEEPROM = (uint8_t)UTI_NELEMENTS( CACHE_tPartitionTableEeprom );

/**
 * \brief bitfield storing if cached data is valid
 *
 * bit 0 shows validity of cached value(s) for drawer 0, bit 1 for drawer 1, ... bit n for drawer n.
 * If bit is set, the cached value for this drawer is valid, otherwise not.
 */
uint8_t CACHE_aucDrawerCacheValidEeprom[ ( (uint8_t)UTI_NELEMENTS( CACHE_tPartitionTableEeprom ) + 7u ) / 8u ];

const uint32_t CACHE_ulNegativeOffsetForEepAddress = (uint32_t)CACHE_aucCache; //lint !e923 cast from pointer to unsigned int
const uint32_t CACHE_ulOffsetForEepAddress         = 0UL;
#endif //defined(EXTERNAL_LARGE)
#endif // defined(CACHE_INCLUDE_EEPROM) && !defined(CACHE_INCLUDE_VEEPROM)

#if defined(CACHE_INCLUDE_EEPROM) && defined(CACHE_INCLUDE_VEEPROM)
#define FIELD_ITEM_COUNT 243 //last partition item index + its size

static uint8_t CACHE_aucCache[FIELD_ITEM_COUNT];
/* CACHE_tPartitionTableVeeprom*/
/**
 * \brief partition table defining partition of the VEEPROM
 *
 * The entries of this array define the location where data is cached,
 * length of each portion of data that is stored in one drawer. The index of
 * the table define the drawer identifier.
 *
 * \attention Maximum number of entries in this table is #CACHE_MAX_DRAWER!
 *
 * \note example
 * \code
 *       / Partition Table for VEEPROM contains 3 drawers.
 *       / A Flag, and two arrays with given size
 *       tVEEPROMDrawerType CACHE_tPartitionTableVeeprom[ ] =
 *       {
 *          {&XYZ_bFlag, sizeof(XYZ_bFlag)},
 *          {&XXX_pucData[0], 16},
 *          {&XXX_pucOtherData[0], sizeof(XXX_pucOtherData)},
 *        };
 * \endcode
 *
 * \note The index of the table defines the drawer identifier that is used for this data:
 *        - CACHE_tPartitionTableVeeprom[0] contains drawer identifier 0 and its drawer size,
 *        - CACHE_tPartitionTableVeeprom[1] contains drawer identifier 1 and its drawer size,
 *        - ...
 *        - CACHE_tPartitionTableVeeprom[n] contains drawer identifier n and its drawer size.
 *
 * \note have access to the data of this table using the functions:
 *       - Cache::CACHE_ucGetDrawerSize() and
 *       - Cache::CACHE_pucGetCacheAddress()
 */
const tVEEPROMDrawerType CACHE_tPartitionTableVeeprom[ ] =
{

   {
      &CACHE_aucCache[0],   1  //  _1_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[1],   1  //  _2_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[2],   1  //  _3_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[3],   1  //  _4_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[4],   1  //  _5_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[5],   1  //  _6_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[6],   1  //  _7_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[7],   1  //  _8_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[8],   1  //  _9_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[9],   1  // _10_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[10],  1  // _11_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[11],  1  // _12_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[12],  1  // _13_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[13],  1  // _14_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[14],  1  // _15_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[15],  1  // _16_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[16],  1  // _17_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[17],  1  // _18_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[18],  1  // _19_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[19],  1  // _20_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[20],  1  // _21_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[21],  1  // _22_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[22],  1  // _23_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[23],  1  // _24_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[24],  1  // _25_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[25],  1  // _26_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[26],  1  // _27_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[27],  1  // _28_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[28],  1  // _29_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[29],  1  // _30_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[30],  1  // _31_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[31],  1  // _32_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[32],  1  // _33_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[33],  1  // _34_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[34],  1  // _35_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[35],  1  // _36_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[36],  1  // _37_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[37],  1  // _38_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[38],  1  // _39_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[39],  1  // _40_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[40],  1  // _41_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[41],  1  // _42_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[42],  1  // _43_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[43],  1  // _44_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[44],  1  // _45_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[45],  1  // _46_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[46],  1  // _47_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[47],  1  // _48_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[48],  1  // _49_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[49],  1  // _50_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[50],  1  // _51_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[51],  1  // _52_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[52],  1  // _53_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[53],  1  // _54_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[54],  1  // _55_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[55],  1  // _56_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[56],  1  // _57_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[57],  1  // _58_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[58],  1  // _59_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[59],  1  // _60_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[60],  1  // _61_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[61],  1  // _62_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[62],  1  // _63_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[63],  1  // _64_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[64],  1  // _65_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[65],  1  // _66_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[66],  1  // _67_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[67],  1  // _68_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[68],  1  // _69_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[69],  1  // _70_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[70],  1  // _71_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[71],  1  // _72_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[72],  1  // _73_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[73],  1  // _74_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[74],  1  // _75_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[75],  1  // _76_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[76],  1  // _77_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[77],  1  // _78_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[78],  1  // _79_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[79],  1  // _80_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[80],  1  // _81_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[81],  1  // _82_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[82],  1  // _83_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[83],  1  // _84_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[84],  1  // _85_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[85],  1  // _86_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[86],  1  // _87_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[87],  1  // _88_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[88],  1  // _89_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[89],  1  // _90_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[90],  1  // _91_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[91],  1  // _92_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[92],  1  // _93_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[93],  1  // _94_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[94],  1  // _95_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[95],  1  // _96_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[96],  1  // _97_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[97],  1  // _98_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[98],  1  // _99_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[99],  1  //_100_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[100], 1  //_101_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[101], 1  //_102_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[102], 1  //_103_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[103], 1  //_104_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[104], 1  //_105_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[105], 1  //_106_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[106], 1  //_107_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[107], 1  //_108_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[108], 1  //_109_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[109], 1  //_110_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[110], 1  //_111_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[111], 1  //_112_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[112], 1  //_113_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[113], 1  //_114_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[114], 1  //_115_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[115], 1  //_116_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[116], 1  //_117_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[117], 1  //_118_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[118], 1  //_119_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[119], 1  //_120_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[120], 1  //_121_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[121], 1  //_122_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[122], 1  //_123_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[123], 1  //_124_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[124], 1  //_125_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[125], 1  //_126_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[126], 1  //_127_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[127], 1  //_128_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[128], 1  //_129_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[129], 1  //_130_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[130], 1  //_131_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[131], 1  //_132_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[132], 1  //_133_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[133], 1  //_134_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[134], 1  //_135_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[135], 1  //_136_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[136], 1  //_137_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[137], 1  //_138_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[138], 1  //_139_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[139], 1  //_140_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[140], 1  //_141_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[141], 1  //_142_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[142], 1  //_143_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[143], 1  //_144_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[144], 1  //_145_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[145], 1  //_146_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[146], 1  //_147_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[147], 1  //_148_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[148], 1  //_149_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[149], 1  //_150_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[150], 1  //_151_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[151], 1  //_152_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[152], 1  //_153_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[153], 1  //_154_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[154], 1  //_155_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[155], 1  //_156_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[156], 1  //_157_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[157], 1  //_158_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[158], 1  //_159_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[159], 1  //_160_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[160], 1  //_161_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[161], 1  //_162_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[162], 1  //_163_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[163], 1  //_164_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[164], 1  //_165_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[165], 1  //_166_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[166], 1  //_167_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[167], 1  //_168_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[168], 1  //_169_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[169], 1  //_170_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[170], 1  //_171_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[171], 1  //_172_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[172], 1  //_173_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[173], 1  //_174_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[174], 1  //_175_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[175], 1  //_176_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[176], 1  //_177_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[177], 1  //_178_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[178], 1  //_179_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[179], 1  //_180_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[180], 1  //_181_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[181], 1  //_182_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[182], 1  //_183_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[183], 1  //_184_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[184], 1  //_185_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[185], 1  //_186_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[186], 1  //_187_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[187], 1  //_188_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[188], 1  //_189_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[189], 1  //_190_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[190], 1  //_191_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[191], 1  //_192_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[192], 1  //_193_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[193], 1  //_194_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[194], 1  //_195_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[195], 1  //_196_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[196], 1  //_197_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[197], 1  //_198_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[198], 1  //_199_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[199], 1  //_200_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[200], 1  //_201_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[201], 1  //_202_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[202], 1  //_203_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[203], 3  //_204_CACHE_PLACE_3BYTES
   }
   ,
   {
      &CACHE_aucCache[206], 2  //_205_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[208], 4  //_206_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[212], 4  //_207_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[216], 2  //_208_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[218], 10 //_209_CACHE_PLACE_10BYTES
   }
   ,
   {
      &CACHE_aucCache[228], 4 //_210_CACHE_PLACE_4BYTES
   }
}
;
/*~E*/
/**
 * \brief number of drawers of VEEPROM partition table
 *
 *
 */
/*~T*/
const uint8_t CACHE_ucNumberOfDrawersVEEPROM = (uint8_t)UTI_NELEMENTS ( CACHE_tPartitionTableVeeprom );
/*~T*/
/**
 * \brief bitfield storing if cached data is valid
 *
 * bit 0 shows validity of cached value(s) for drawer 0, bit 1 for drawer 1, ... bit n for drawer n.
 * If bit is set, the cached value for this drawer is valid, otherwise not.
 */
uint8_t CACHE_aucDrawerCacheValidVeeprom[ ( UTI_NELEMENTS( CACHE_tPartitionTableVeeprom ) + 7u ) / 8u ];

/*~A:CACHE_tPartitionTableEeeprom*/
/**
 * \brief partition table defining partition of the EEPROM
 *
 * The entries of this array define the location where data is cached and its
 * length. The index within the table define the drawer identifier.
 *
 * \attention Maximum number of entries in this table is #CACHE_MAX_DRAWER!
 *
 * \note example
 * \code
 *       / Partition Table of Eeprom contains 3 drawers: one Flag and 2 arrays of given size
 *       tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ] =
 *       {
 *          {&XYZ_bFlag, sizeof(XYZ_bFlag), ulAddEEP_1},
 *          {&XXX_pucData[0], 16, ulAddEEP_2},
 *          {&XXX_pucOtherData[0], sizeof(XXX_pucOtherData), ulAddEEP_3},
 *        };
 * \endcode
 *
 * \note The index of the table defines the drawer identifier that is used for this data:
 *        - CACHE_tPartitionTableEeprom[0] contains drawer identifier 0 and its drawer size,
 *        - CACHE_tPartitionTableEeprom[1] contains drawer identifier 1 and its drawer size,
 *        - ...
 *        - CACHE_tPartitionTableEeprom[n] contains drawer identifier n and its drawer size.
 *
 * \note have access to the data of this table using the functions:
 *       - Cache::CACHE_ucGetDrawerSize() and
 *       - Cache::CACHE_pucGetCacheAddress()
 *
 */
const tEEPROMDrawerType CACHE_tPartitionTableEeprom[ ] =
{
   /* cache address     , size, virtual EEPROM address = prev variable position + prev variable size */
   {
      &CACHE_aucCache[232], 1, (uint32_t)&CACHE_aucCache[232]  //_211_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[233], 2, (uint32_t)&CACHE_aucCache[233] //_212_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[235], 4, (uint32_t)&CACHE_aucCache[235] //_213_CACHE_PLACE_4BYTES
   }
   ,
   {
      &CACHE_aucCache[239], 1, (uint32_t)&CACHE_aucCache[239] //_214_CACHE_PLACE_1BYTE
   }
   ,
   {
      &CACHE_aucCache[240], 2, (uint32_t)&CACHE_aucCache[240] //_215_CACHE_PLACE_2BYTES
   }
   ,
   {
      &CACHE_aucCache[242], 4, (uint32_t)&CACHE_aucCache[242] //_216_CACHE_PLACE_4BYTES
   }
}
;

/**
 * \brief number of drawers of EEPROM partition table
 */
const uint8_t CACHE_ucNumberOfDrawersEEPROM = (uint8_t)UTI_NELEMENTS( CACHE_tPartitionTableEeprom );

/**
 * \brief bitfield storing if cached data is valid
 *
 * bit 0 shows validity of cached value(s) for drawer 0, bit 1 for drawer 1, ... bit n for drawer n.
 * If bit is set, the cached value for this drawer is valid, otherwise not.
 */
uint8_t CACHE_aucDrawerCacheValidEeprom[ ( (uint8_t)UTI_NELEMENTS( CACHE_tPartitionTableEeprom ) + 7u ) / 8u ];

const uint32_t CACHE_ulNegativeOffsetForEepAddress = (uint32_t)CACHE_aucCache;
const uint32_t CACHE_ulOffsetForEepAddress         = 0UL;
#endif //defined(CACHE_INCLUDE_EEPROM) && defined(CACHE_INCLUDE_VEEPROM)

/* Functions*/
#ifdef CACHE_INCLUDE_EEPROM
#if defined(EXTERNAL_LARGE)
/*~A:CACHE_vModifyEEPROMaddress()*/
void CACHE_vModifyEEPROMaddress(uint32_t *pulAddress, tDrawerID tIndex)
{
   (void)tIndex;
   *pulAddress = (*pulAddress - CACHE_ulNegativeOffsetForEepAddress) + CACHE_ulOffsetForEepAddress;
}
/*~E*/
#else //part of not defined(EXTERNAL_LARGE)
/*~A:CACHE_vModifyEEPROMaddress()*/
void CACHE_vModifyEEPROMaddress(uint32_t *pulAddress, uint8_t ucIndex)
{
   (void)ucIndex;
   *pulAddress = (*pulAddress - CACHE_ulNegativeOffsetForEepAddress) + CACHE_ulOffsetForEepAddress;
}
/*~E*/
#endif //defined(EXTERNAL_LARGE)
#endif //ifdef CACHE_INCLUDE_EEPROM
//lint -restore -e923
