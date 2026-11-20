"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : donotincludecfiles.py
Date of creation : 20.06.2018
Author Name/Dept : Jennifer Deepa R
Organization     : RBEI
Description      : Checks whether C files are included in source/header files.

"""

import generic
from bauhaus import ir
from bauhaus.ir.common.output import unparse


def included_source(ir_graph):
    """Lists all included .C files in source"""

    return (
        node
        for node in ir_graph.nodes_of_type(
            ir.Physical, ("Plain_User_Include_Declaration")
        )
        if node.Name.endswith(".c")
    )


class donotincludecfiles(generic.GenericRule):

    rulename = "DoNotIncludeCfiles"
    ruletext = """Do not include C files."""

    languages = ["C", "C++"]

    _guid = "2403594f-29ef-473a-9e4b-385c59705a3b"
    _message_descriptions = {
        "Plain_User_Include_Declaration": (
            "Do not include C files",
            "7b1d52bd-1992-41fc-b85f-4f0f093c6bcd",
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
        """Checks for .c files inclusion in header/source files."""

        nodes = included_source(ir_graph)
        klass.add_messages(
            nodes, "Plain_User_Include_Declaration", entity_callback=unparse.entity
        )
