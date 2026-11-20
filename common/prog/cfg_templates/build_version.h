/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 ******************************************************************************/

#ifndef BUILD_VERSION_H
#define BUILD_VERSION_H

<$foreach(def,$(defines_version),macro(macro.define,subst("=","\t",$(def))))>
#endif /* BUILD_VERSION_H */
