"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : bshnamingconvention.py
Date of creation : 8.07.2018
Author Name/Dept : Siddharth Trivedi
Organization     : RBEI
Description      : Contains the rule implementation specific to BSH naming convention document (version4.4).

"""


from bauhaus import ir
import os.path
import generic

from axivion.style.naming import naming_convention
from axivion.style.naming.naming_convention import *
import utility


class BSH_Naming_For_HungarianNotation(
    generic.GenericRule, naming_convention.NamingRule
):
    rulename = "BSH_HungarianNotation_NamingRule"
    ruletext = """Check Hungarian Notation naming conventions."""

    # The class hierarchy is indicated by indentation.
    # As this class has hungarian notation implementation
    # if any new convention is introduced, code will have to be updated as suggested below.

    """Each partial naming rule for the identifier of a certain node type has the following format:

    node_type_name : [ (checker, message), (checker, message), ... ]

    "checker" can be either a regular expression (in which case it must match the identifier)
    or a complex checker consisting of a guarding predicate (guard) and an regular expression
    (in which case the guard governs if the regexp must match).

    If a checker fails, its message is issued.

    All checkers in the list must match in order to get no message, i.e., if you want to state different regexps for
    different combinations of aspects (visibility etc), make sure the guards create the needed disjointness of
    your regexp checks. If multiple checkers fail for the same identifier, you will end up with multiple messages for that identifier.

    Some aspects are expressed by attributes (constness, visibility),
    some by the hierarchy (staticness)."""

    naming = {
        ir.Physical: {
            "Unmutable_Named_Bit_Field_Definition": [
                (
                    re.compile("^([A-Z][A-Z0-9]{1,5}_)?bf[A-Z][A-Za-z0-9_]+$"),
                    "BSH naming rule suggests Bitfield variable name must contain 'bf' prefix before DataDescription and must follow camel case",
                ),
            ],
        },
        ir.Logical: {
            "Variable": [
                (
                    create_checker(
                        utility.is_func_ptr,
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pf[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Function pointer variable name must contain 'pf' prefix before DataDescription and must follow camel case and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_standard_type_var,
                        "Bounded_Array_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?a[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Array variable name must contain 'a' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_standard_type_var,
                        "C_Char_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?c[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Char variable name must contain 'c' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_standard_type_var,
                        "C_Signed_Char_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?sc[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Signed char variable name must contain 'sc' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_standard_type_var,
                        "C_Unsigned_Char_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?uc[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned char variable name must contain 'uc' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_standard_type_var,
                        "C_Int_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?si[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Int variable name must contain 'si' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_standard_type_var,
                        "C_Unsigned_Int_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ui[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned int variable name must contain 'ui' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_standard_type_var,
                        "C_Unsigned_Long_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ul[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned long variable name must contain 'ul' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_standard_type_var,
                        "C_Long_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?sl[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Signed long variable name must contain 'sl' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_standard_type_var,
                        "Boolean_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?b[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Bool variable name must contain 'b' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_typedef_type_var,
                        "int8_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s8[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int8_t variable name must contain 's8' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_typedef_type_var,
                        "uint8_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u8[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint8_t variable name must contain 'u8' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_typedef_type_var,
                        "int16_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s16[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int16_t variable name must contain 's16' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_typedef_type_var,
                        "uint16_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u16[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint16_t variable name must contain 'u16' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_typedef_type_var,
                        "int32_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s32[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int32_t variable name must contain 's32' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_typedef_type_var,
                        "uint32_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u32[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint32_t variable name must contain 'u32' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_typedef_type_var,
                        "int64_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s64[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int64_t variable name must contain 's64' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_typedef_type_var,
                        "uint64_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u64[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint64_t variable name must contain 'u64' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "char",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pc[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Char pointer variable name must contain 'pc' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "signed char",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?psc[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Signed char pointer variable name must contain 'psc' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "unsigned char",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?puc[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned char pointer variable name must contain 'puc' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "signed int",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?psi[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Int pointer variable name must contain 'psi' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "unsigned int",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pui[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned int pointer variable name must contain 'pui' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "unsigned long",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pul[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned long pointer variable name must contain 'pul' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "signed long",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?psl[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Signed long pointer variable name must contain 'psl' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "bool",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pb[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Bool pointer variable name must contain 'pb' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "void",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pv[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Void pointer variable name must contain 'pv' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "int8_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ps8[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int8_t pointer variable name must contain 'ps8' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "uint8_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pu8[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint8_t pointer variable name must contain 'pu8' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "int16_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ps16[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int16_t pointer variable name must contain 'ps16' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "uint16_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pu16[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint16_t pointer variable name must contain 'pu16' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "int32_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ps32[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int32_t pointer variable name must contain 'ps32' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "uint32_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pu32[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint32_t pointer variable name must contain 'pu32' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "int64_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ps64[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int64_t pointer variable name must contain 'ps64' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_type_ptr_var,
                        "uint64_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?pu64[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint64_t pointer variable name must contain 'pu64' prefix before DataDescription and must follow camel case",
                ),
            ],
            "Local_Variable": [
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "char",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?c[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Char variable name must contain 'c' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "signed char",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?sc[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Signed char variable name must contain 'sc' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "unsigned char",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?uc[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned char variable name must contain 'uc' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "signed int",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?si[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Int variable name must contain 'si' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "unsigned int",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ui[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned int variable name must contain 'ui' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "unsigned long",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ul[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned long variable name must contain 'ul' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "signed long",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?sl[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Signed long variable name must contain 'sl' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "int8_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s8[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int8_t variable name must contain 's8' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "uint8_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u8[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint8_t variable name must contain 'u8' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "int16_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s16[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int16_t variable name must contain 's16' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "uint16_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u16[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint16_t variable name must contain 'u16' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "int32_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s32[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int32_t variable name must contain 's32' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "uint32_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u32[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint32_t variable name must contain 'u32' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "int64_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s64[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int64_t variable name must contain 's64' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "uint64_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u64[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint64_t variable name must contain 'u64' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "Boolean_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?b[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Bool variable name must contain 'b' prefix before DataDescription and must follow camel case",
                ),
            ],
            "Global_Variable": [
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "char",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?c[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Char variable name must contain 'c' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "signed char",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?sc[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Signed char variable name must contain 'sc' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "unsigned char",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?uc[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned char variable name must contain 'uc' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "signed int",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?si[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Int variable name must contain 'si' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "unsigned int",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ui[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned int variable name must contain 'ui' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "unsigned long",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?ul[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Unsigned long variable name must contain 'ul' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "signed long",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?sl[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Signed long variable name must contain 'sl' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "int8_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s8[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int8_t variable name must contain 's8' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "uint8_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u8[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint8_t variable name must contain 'u8' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "int16_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s16[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int16_t variable name must contain 's16' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "uint16_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u16[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint16_t variable name must contain 'u16' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "int32_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s32[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int32_t variable name must contain 's32' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "uint32_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u32[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint32_t variable name must contain 'u32' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "int64_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?s64[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests int64_t variable name must contain 's64' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "uint64_t",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?u64[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests uint64_t variable name must contain 'u64' prefix before DataDescription and must follow camel case",
                ),
                (
                    utility.create_node_checker(
                        utility.is_var_const,
                        "Boolean_Type",
                        re.compile("^([A-Z][A-Z0-9]{1,5}_)?b[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests Bool variable name must contain 'b' prefix before DataDescription and must follow camel case",
                ),
            ],
            "Function": [
                (
                    create_checker(
                        utility.is_func_void,
                        re.compile("^[A-Z][A-Z0-9]{1,5}_v[A-Z][A-Za-z0-9_]+$"),
                    ),
                    "BSH naming rule suggests void function name must contain 'v' prefix before DataDescription and must follow camel case",
                )
            ],
            "Array_Type": [
                (
                    re.compile("^([A-Z][A-Z0-9]{1,5}_)?a[A-Z][A-Za-z0-9_]+$"),
                    "BSH naming rule suggests array name must contain 'a' prefix before DataDescription and must follow camel case",
                )
            ],
        },
    }


class BSH_Naming_For_MacroWithFunction(
    generic.GenericRule, naming_convention.NamingRule
):
    rulename = "BSH_MacroWithFunction_NamingRule"
    ruletext = (
        """Check naming conventions as BSH standard for Macro used as functions."""
    )

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Physical: {
            "Function_Macro_Definition": [
                (
                    re.compile("^[A-Z][A-Z0-9]{1,5}_[a-zA-Z0-9_]+$"),
                    "BSH naming convention rule violated for functions defined as MACRO name",
                )
            ]
        }
    }


class BSH_Naming_For_MACRO(generic.GenericRule, naming_convention.NamingRule):
    rulename = "BSH_MACRO_NamingRule"
    ruletext = """Check Macro naming conventions as BSH standard."""

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Physical: {
            "Source_Object_Macro_Definition": [
                (
                    re.compile("^[A-Z][A-Z0-9]{1,5}_[A-Z_0-9]+$"),
                    "BSH naming convention rule violated for MACRO name",
                )
            ],
        }
    }


class BSH_Naming_For_Variable(generic.GenericRule, naming_convention.NamingRule):
    rulename = "BSH_Variable_NamingRule"
    ruletext = """Check Variable naming conventions as BSH standard."""

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Logical: {
            "Global_Variable": [
                (
                    re.compile(
                        "(^[A-Za-z0-9_]+$)|(^[A-Z][A-Z0-9]{1,5}_([a-z0-9]{1,3})?[A-Z][a-zA-Z0-9_]+$)"
                    ),
                    "BSH naming convention rule violated for variable name",
                ),
            ],
            "Local_Variable": [
                (
                    re.compile(
                        "(^[A-Za-z0-9_]+$)|(^[A-Z][A-Z0-9]{1,5}_([a-z0-9]{1,3})?[A-Z][a-zA-Z0-9_]+$)"
                    ),
                    "BSH naming convention rule violated for variable name",
                )
            ],
        }
    }


class BSH_Naming_For_Function(generic.GenericRule, naming_convention.NamingRule):
    rulename = "BSH_Function_NamingRule"
    ruletext = """Check Function naming conventions as BSH standard."""

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Logical: {
            "Function": [
                (
                    create_checker(
                        utility.is_not_isr,
                        re.compile("^[A-Z][A-Z0-9]{1,5}_[A-Za-z0-9_]+$"),
                    ),
                    "BSH naming convention rule violated for function name",
                )
            ],
        }
    }


class BSH_Naming_For_ISR_Function(generic.GenericRule, naming_check.NamingBaseRule):
    rulename = "BSH_ISR_Function_NamingRule"
    ruletext = """Check ISR Function naming conventions as BSH standard."""

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Logical: {
            "Function": [
                (
                    create_checker(
                        utility.is_isr,
                        re.compile("^[A-Z][A-Z0-9]{1,5}_ISR_[A-Za-z0-9_]+$"),
                    ),
                    "BSH naming convention rule violated for ISR function name",
                )
            ],
        }
    }


class BSH_Naming_For_Union(generic.GenericRule, naming_convention.NamingRule):
    rulename = "BSH_Union_NamingRule"
    ruletext = """Check Union naming conventions as BSH standard."""

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Logical: {
            "Union_Type": [
                (
                    re.compile("^([A-Z][A-Z0-9]{1,5}_)?[A-Za-z0-9_]+$"),
                    "BSH naming convention rule violated for union name",
                )
            ],
            "Field": [
                (
                    create_checker(
                        utility.is_union_member,
                        re.compile(
                            "(^[a-z0-9]{1,3}[A-Z][A-Za-z0-9_]+$)|(^[A-Za-z0-9_]+$)"
                        ),
                    ),
                    "BSH naming convention rule violated for union member variable name",
                )
            ],
        }
    }


class BSH_Naming_For_Structure(generic.GenericRule, naming_convention.NamingRule):
    rulename = "BSH_Structure_NamingRule"
    ruletext = """Check Structure naming conventions as BSH standard."""

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Logical: {
            "Class_Type": [
                (
                    re.compile("^([A-Z][A-Z0-9]{1,5}_)?[A-Za-z0-9_]+$"),
                    "BSH naming convention rule violated for structure name",
                )
            ],
            "Field": [
                (
                    create_checker(
                        is_struct_member,
                        re.compile(
                            "(^[a-z0-9]{1,3}[A-Z][A-Za-z0-9_]+$)|(^[A-Za-z0-9_]+$)"
                        ),
                    ),
                    "BSH naming convention rule violated for structure member variable name",
                ),
            ],
        }
    }


class BSH_Naming_For_Enumerator(generic.GenericRule, naming_convention.NamingRule):
    rulename = "BSH_Enumerator_NamingRule"
    ruletext = """Check Enumerator naming conventions as BSH standard."""

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Logical: {
            "Enumerator": [
                (
                    re.compile("^([A-Z][A-Z0-9]{1,5}_)?[A-Z0-9_]+$"),
                    "BSH naming convention rule violated for enum name",
                )
            ],
        }
    }


class BSH_Naming_For_Typedef(generic.GenericRule, naming_convention.NamingRule):
    rulename = "BSH_Typedef_NamingRule"
    ruletext = """Check Typedef naming conventions as BSH standard."""

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Logical: {
            "Typedef_Type": [
                (
                    re.compile(
                        "(^[A-Z][A-Z0-9]{1,5}_[A-Za-z0-9_]_t+$)|(^[A-Z][A-Za-z0-9_]_t+$)"
                    ),
                    "BSH naming convention rule violated for Typedef name",
                ),
                (
                    create_checker(
                        utility.is_func_ptr_typedef,
                        re.compile("^[A-Z][A-Za-z0-9_]_t+$"),
                    ),
                    "BSH naming convention rule violated for function pointer name",
                ),
            ],
        }
    }


class BSH_Naming_For_FolderAndFile(generic.GenericRule, naming_convention.NamingRule):
    rulename = "BSH_FolderAndFile_NamingRule"
    ruletext = """Check Folder and file naming conventions as PED standard."""

    naming = {
        # The class hierarchy is indicated by indentation.
        ir.Physical: {
            "Primary_File": [
                (
                    re.compile("^[a-zA-Z][a-zA-Z0-9\.]+\.[a-zA-Z0-9]+$"),
                    "BSH naming convention rule violated for file and folder name",
                )
            ],
            "User_Include_File": [
                (
                    re.compile("^[a-zA-Z][a-zA-Z0-9\.]+\.[a-zA-Z0-9]+$"),
                    "BSH naming convention rule violated for file and folder name",
                )
            ],
            "Primary_File": [
                (
                    utility.is_starting_lower,
                    "BSH naming convention recommends that file names start with upper case",
                )
            ],
            "User_Include_File": [
                (
                    utility.is_starting_lower,
                    "BSH naming convention recommends that file names start with upper case",
                )
            ],
        },
    }
