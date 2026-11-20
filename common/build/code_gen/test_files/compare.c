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
 *  COMP_ABBREV      SCH
 ******************************************************************************/

// Test if variable is defined (should not be defined)
NOT DEFINED
NOT DEFINED

// Test if variable is defined (must be defined)
DEFINED
DEFINED

// Test if variable is defined (must not be defined)
NOT DEFINED
NOT DEFINED

// Test of 'define' function by defining array of dictionaries (no output)
// Test of "foreach" function - natural order
Year has 365 days.
Week has 7 days.
Month has 30 days.

// Test of 'foreach' with 'sort' function - sort by "days"
Week has 7 days.
Month has 30 days.
Year has 365 days.

// Test of 'foreach' with 'rsort' function - reverse sort by "days"
Year has 365 days.
Month has 30 days.
Week has 7 days.

// Test of 'len' function - should be 3
Length of array is: 3

// Test of 'macro' with different symbols
`~!@#$%^&*()-_=+[];:'"\|,./?
// Should be:
`~!@#$%^&*()-_=+[];:'"\|,./?

// Test of case change functions
my_led_string_123
MY_LED_STRING_123
myLedString123
MyLedString123
