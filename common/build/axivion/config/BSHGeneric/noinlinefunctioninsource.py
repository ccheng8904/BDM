"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : noinlinefunctioninsource.py
Date of creation : 08.06.2018
Author Name/Dept : Siddharth Trivedi
Organization     : RBEI
Description      : Checks for the existence of inline functions in C files.

"""

import generic
from bauhaus import ir
from axivion.routines.functions import functions_check
from bauhaus.ir.common.output import unparse
from itertools import chain


def inline_in_source(ir_graph):
    """Returns inline functions in source files"""

    return (
        node
        for node in ir_graph.nodes_of_type(ir.Physical, "Routine_Definition")
        if node.Attributes.is_inline and node.SLoc.File.is_of_type("Primary_File")
    )


class No_inline_definition_in_Source(
    generic.GenericRule, functions_check.FunctionsBaseRule
):

    rulename = "NoInlineFunctionInSource"
    ruletext = """Inline function definition in source file."""

    languages = ["C", "C++"]

    _guid = "3345a3b2-44a6-4561-8185-e27f5f3bfadd"

    _message_descriptions = {
        "Routine_Definition": (
            "Inline function definition in source file.",
            "cc54a030-f49a-45b8-94eb-004d58cb9edf",
        )
    }

    @classmethod
    def get_rulehtml(klass):
        return """{header}
The declaration of a non-inline function in a header file violates the One Definition Rule
if the header file is included in multiple compilation units.

<h5>Configuration</h5>
    {config}
""".format(
            header=klass.get_rulehtml_header(),
            config=klass.get_rulehtml_configuration(),
        )

    @classmethod
    def execute(klass, ir_graph):
        """Checks for (non-inline) function definitions in header files."""

        nodes = inline_in_source(ir_graph)

        klass.add_messages(nodes, "Routine_Definition", entity_callback=unparse.entity)
