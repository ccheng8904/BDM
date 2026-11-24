/*******************************************************************************
 *  Copyright (c) 2017 BSH Hausgeraete GmbH,
 *  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
 *
 *  All rights reserved. This program and the accompanying materials
 *  are protected by international copyright laws.
 *  Please contact copyright holder for licensing information.
 *
 *******************************************************************************
 *  VARIANT          <$toupper($(variant_base))>
 ******************************************************************************/

#ifndef <$toupper($(variant_base))>_H
#define <$toupper($(variant_base))>_H

<$foreach(def,$(defines),macro(macro.define,subst("=","\t",$(def))))>
#endif /* <$toupper($(variant_base))>_H */
