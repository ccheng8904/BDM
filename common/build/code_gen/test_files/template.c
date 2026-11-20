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
<$ifdef(var, "DEFINED", "NOT DEFINED")>
<$ifndef(var, "NOT DEFINED", "DEFINED")>

// Test if variable is defined (must be defined)
<$define(var, 0)>
<$ifdef(var, "DEFINED", "NOT DEFINED")>
<$ifndef(var, "NOT DEFINED", "DEFINED")>

// Test if variable is defined (must not be defined)
<$undef(var)>
<$ifdef(var, "DEFINED", "NOT DEFINED")>
<$ifndef(var, "NOT DEFINED", "DEFINED")>

// Test of 'define' function by defining array of dictionaries (no output)
<$define(list, [{"name":"Year", "days":365}, {"name":"Week", "days":7}, {"name":"Month", "days":30}])>
// Test of "foreach" function - natural order
<$foreach(var, list, macro("$1 has $2 days.\n", var.name, var.days))>
// Test of 'foreach' with 'sort' function - sort by "days"
<$foreach(var, sort(list, "days"), macro("$1 has $2 days.\n", var.name, var.days))>
// Test of 'foreach' with 'rsort' function - reverse sort by "days"
<$foreach(var, rsort(list, "days"), macro("$(var.name) has $(var.days) days.\n"))>
// Test of 'len' function - should be 3
<$macro("Length of array is: $1", len(list))>

// Test of 'macro' with different symbols
<$macro("`~!@#$%^&*()-_=+[];:'\"\\|,./?")>
// Should be:
`~!@#$%^&*()-_=+[];:'"\|,./?

// Test of case change functions
<$snake_case("My_LEDString123")>
<$SNAKE_CASE("My_LEDString123")>
<$camelCase("My_LEDString123")>
<$CamelCase("My_LEDString123")>
