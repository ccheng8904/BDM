"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : utility.py
Date of creation : 8.07.2018
Author Name/Dept : Siddharth Trivedi
Organization     : RBEI
Description      : Contains all the utility function which are used in namingconvention check.

"""


import os.path
from bauhaus.ir.common.types.qualified_types import is_const_qualified as is_const_type
import re

"""
Args:
    predicate : Function name
	type :  Variable type
	regexp : Regular expression
Returns :
    If predicate holds, match the regexp and check type;
    if predicate does not hold, return True (so do not check the naming)	
"""


def create_node_checker(predicate, type, regexp):
    def checker(node):
        if predicate(node, type):
            return regexp.match(node.Name)
        return True

    return checker


"""
Checks if the variable has small scope 
Args:
    node : node of a specific Axivion class
Returns:
    returns the true is variable has small scope
"""


def small_scope_var(node):
    return node.Initialization.is_of_type("Integer_Literal_Instruction") and (
        node.Initialization.Parent.is_of_type("CFG_Loop_Header_Jump_Block")
        or node.Initialization.Parent.is_of_type("CFG_If_Block")
    )


"""
Checks if the variable is constant and of given type 
Args:
    node : node of a specific Axivion class
	type : data type
Returns:
    returns the const variable based on type
"""


def is_var_const(node, type):
    if (
        node.Physical.Its_Type.is_of_type("Flags_Qualified_Type_Definition")
        and node.Physical.Its_Type.Unqualified_Type.Name == type
        and is_const_type(node.Its_Type)
        and (not small_scope_var(node))
    ):
        return node.Name
    elif (
        node.Its_Type.is_of_type("Flags_Qualified_Type")
        and node.Its_Type.Unqualified_Type.Name == type
        and is_const_type(node.Its_Type)
        and (not small_scope_var(node))
    ):
        return node.Name


"""
Finds Union member 
Args:
    node : node of a specific Axivion class
Returns:
    returns union member names
"""


def is_union_member(node):
    return node.Parent.is_of_type("Union_Type")


"""
Finds variable which are declared by as standard data type 
Args:
    node : node of a specific Axivion class
	type : data type
Returns:
    returns the type variable
"""


def is_standard_type_var(node, type):
    if node.Its_Type.is_of_type(type) and (not small_scope_var(node)):
        return node.Name


"""
Finds variable which are declared by as typedefed data type 
Args:
    node : node of a specific Axivion class
	typedef_type : typedefed data type
Returns:
    returns the typedefed type variable
"""


def is_typedef_type_var(node, typedef_type):
    if node.Its_Type.Name == typedef_type and (not small_scope_var(node)):
        return node.Name


"""
Finds the function names which has ISR keyword
Args:
    node : node of a specific Axivion class
Returns:
    returns only the function names which has ISR keyword
"""


def is_isr(node):
    match_str = re.match("^.*_ISR_.*$", node.Name)
    if match_str:
        return node.Name


"""
Finds the function names which has not ISR keyword
Args:
    node : node of a specific Axivion class
Returns:
    returns only the function names which has not ISR keyword
"""


def is_not_isr(node):
    match_str = re.match("^.*_ISR_.*$", node.Name)
    if not match_str:
        return node.Name


"""
Finds the function names which has ISR keyword and viod return type
Args:
    node : node of a specific Axivion class
Returns:
    returns only the function names which has ISR keyword and viod return type
"""


def is_void_isr(node):
    match_str = re.match("^.*_ISR_.*$", node.Name)
    if match_str and node.Return_Type.Name == "void":
        return node.Name


"""
Finds the typedefed function pointer
Args:
    node : node of a specific Axivion class
Returns:
    returns typedefed function pointer
"""


def is_func_ptr_typedef(node):
    if node.Original_Type.is_of_type(
        "Normal_Pointer_Type"
    ) and node.Original_Type.Pointed_To_Type.is_of_type("Function_Type"):
        return node.Name


"""
Finds the function pointer
Args:
    node : node of a specific Axivion class
Returns:
    returns function pointer
"""


def is_func_ptr(node):
    if node.Its_Type.is_of_type("Normal_Pointer_Type"):
        if node.Its_Type.Pointed_To_Type.is_of_type("Function_Type"):
            return node.Name
    elif node.Its_Type.is_of_type(
        "Flags_Qualified_Type"
    ) and node.Its_Type.Unqualified_Type.is_of_type("Normal_Pointer_Type"):
        if node.Its_Type.Unqualified_Type.Pointed_To_Type.is_of_type(
            "Function_Type"
        ) and is_const_type(node.Its_Type):
            return node.Name


"""
Finds only the function names which has void retuen type
Args:
    node : node of a specific Axivion class
Returns:
    returns only the function names which has void retuen type
"""


def is_func_void(node):
    if node.Return_Type.Name == "void":
        return node.Name


"""
Finds only the file names which are starting in lower case
Args:
    node : node of a specific Axivion class
Returns:
    returns only the file names which are starting in lower case
"""


def is_starting_lower(node):
    if (os.path.basename(node.Full_Name)[0]).isupper():
        return node.Name


"""
Finds pointer variable which are declared by given data type 
Args:
    node : node of a specific Axivion class
	type : data type
Returns:
    returns the pointer variable of given data type 
"""


def is_type_ptr_var(node, type):
    if not small_scope_var(node) and (
        (
            node.Its_Type.is_of_type(
                "Normal_Pointer_Type"
            )  # checking for normal pointer variable
            and (
                node.Its_Type.Pointed_To_Type.Name == type
                or (
                    node.Its_Type.Pointed_To_Type.is_of_type("Normal_Pointer_Type")
                    and node.Its_Type.Pointed_To_Type.Pointed_To_Type.Name == type
                )
                or (
                    node.Its_Type.Pointed_To_Type.is_of_type("Normal_Pointer_Type")
                    and node.Its_Type.Pointed_To_Type.Pointed_To_Type.is_of_type(
                        "Normal_Pointer_Type"
                    )
                    and node.Its_Type.Pointed_To_Type.Pointed_To_Type.Pointed_To_Type.Name
                    == type
                )
                or (
                    node.Its_Type.Pointed_To_Type.is_of_type("Normal_Pointer_Type")
                    and node.Its_Type.Pointed_To_Type.Pointed_To_Type.is_of_type(
                        "Normal_Pointer_Type"
                    )
                    and node.Its_Type.Pointed_To_Type.Pointed_To_Type.Pointed_To_Type.is_of_type(
                        "Normal_Pointer_Type"
                    )
                    and node.Its_Type.Pointed_To_Type.Pointed_To_Type.Pointed_To_Type.Pointed_To_Type.Name
                    == type
                )
            )
        )
        or (
            node.Physical.Its_Type.is_of_type("Normal_Pointer_Type_Definition")
            # checking for constant pointer variable i.e. int8_t const *p; or, const int *p;
            and (
                (
                    node.Physical.Its_Type.Pointed_To_Type.is_of_type(
                        "Flags_Qualified_Type_Definition"
                    )
                    and node.Physical.Its_Type.Pointed_To_Type.Unqualified_Type.Logical.Name
                    == type
                )
                or (
                    node.Physical.Its_Type.Pointed_To_Type.is_of_type(
                        "Normal_Pointer_Type_Definition"
                    )
                    and node.Physical.Its_Type.Pointed_To_Type.Pointed_To_Type.is_of_type(
                        "Flags_Qualified_Type_Definition"
                    )
                    and node.Physical.Its_Type.Pointed_To_Type.Pointed_To_Type.Unqualified_Type.Logical.Name
                    == type
                )
                or (
                    node.Physical.Its_Type.Pointed_To_Type.is_of_type(
                        "Normal_Pointer_Type_Definition"
                    )
                    and node.Physical.Its_Type.Pointed_To_Type.Pointed_To_Type.is_of_type(
                        "Normal_Pointer_Type_Definition"
                    )
                    and node.Physical.Its_Type.Pointed_To_Type.Pointed_To_Type.Pointed_To_Type.is_of_type(
                        "Flags_Qualified_Type_Definition"
                    )
                    and node.Physical.Its_Type.Pointed_To_Type.Pointed_To_Type.Pointed_To_Type.Unqualified_Type.Logical.Name
                    == type
                )
            )
        )
        or (
            node.Its_Type.is_of_type("Flags_Qualified_Type")
            # checking for constant pointer variable i.e. int8_t * const p; or int * const p;
            and (
                (
                    node.Its_Type.Unqualified_Type.is_of_type("Normal_Pointer_Type")
                    and node.Its_Type.Unqualified_Type.Pointed_To_Type.Name == type
                )
                or (
                    node.Its_Type.Unqualified_Type.is_of_type("Normal_Pointer_Type")
                    and node.Its_Type.Unqualified_Type.Pointed_To_Type.is_of_type(
                        "Normal_Pointer_Type"
                    )
                    and node.Its_Type.Unqualified_Type.Pointed_To_Type.Pointed_To_Type.Name
                    == type
                )
                or (
                    node.Its_Type.Unqualified_Type.is_of_type("Normal_Pointer_Type")
                    and node.Its_Type.Unqualified_Type.Pointed_To_Type.is_of_type(
                        "Normal_Pointer_Type"
                    )
                    and node.Its_Type.Unqualified_Type.Pointed_To_Type.Pointed_To_Type.is_of_type(
                        "Normal_Pointer_Type"
                    )
                    and node.Its_Type.Unqualified_Type.Pointed_To_Type.Pointed_To_Type.Pointed_To_Type.Name
                    == type
                )
            )
        )
    ):
        return node.Name
