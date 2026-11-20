"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : noinlinefunctioninheader.py
Date of creation : 20.06.2018
Author Name/Dept : Pabboj Sandeep
Organization     : RBEI
Description      : Checks for inline function definitions in header files.

"""
import generic
from bauhaus import ir
from axivion.routines.functions import functions_check
from bauhaus.ir.common.output import unparse


def inline_in_header(ir_graph):
    """Returns inline functions in header files"""

    return (
        node
        for node in ir_graph.nodes_of_type(ir.Physical, "Routine_Definition")
        if node.Attributes.is_inline and node.SLoc.File.is_of_type("User_Include_File")
    )


class No_inline_definition_in_Header(
    generic.GenericRule, functions_check.FunctionsBaseRule
):

    rulename = "NoInlineFunctionInHeader"
    ruletext = """Inline function definition in header file."""

    languages = ["C", "C++"]

    _guid = "c6962e0c-1e42-44ca-a9cf-4aa3dc89d435"

    _message_descriptions = {
        "Routine_Definition": (
            "Prefer a function to an inline function in header file.",
            "7b9fa1bb-932e-461c-87e7-21937276194b",
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
        """Checks for inline function definitions in header files."""

        nodes = inline_in_header(ir_graph)

        klass.add_messages(nodes, "Routine_Definition", entity_callback=unparse.entity)
