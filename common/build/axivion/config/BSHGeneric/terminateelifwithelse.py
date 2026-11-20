"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : terminateelifwithelse.py
Date of creation : 08.06.2018
Author Name/Dept : Pabboj Sandeep
Organization     : RBEI
Description      : Checks whether #elif is terminated with #else.

"""

import generic
from bauhaus import ir, shared
import itertools


class TerminateElifWithElse(generic.GenericRule):
    rulename = "TerminateElifWithElse"
    ruletext = """Terminate #elif with #else."""

    _guid = "3c0fbd08-1123-49aa-943f-cbd85c0a56c6"
    _message_descriptions = {
        "missing_else_in_elif": (
            "Terminate #elif with #else",
            "9686072c-0fff-4242-85d3-2255a53babfb",
        )
    }

    @classmethod
    def execute(klass, ir_graph):
        """ Checks existence of #else for the #elif directives in headers and c files """
        for file_node in ir_graph.nodes_of_type(ir.Physical, "File"):
            assert file_node.is_of_type("File")
            pp_elifs = sorted(
                file_node.descendants_of_type("Elif_Interface"),
                key=ir.Node.__key_sloc__,
            )
            for key, nodes in itertools.groupby(pp_elifs, key=ir.Node.__key_sloc__):
                nodes = list(nodes)
                all_true = all(node.Condition_Value for node in nodes)
                all_false = all(not node.Condition_Value for node in nodes)
                if all_true or all_false:
                    pp = nodes.pop()
                    start = pp.Position
                    end = pp.End_Position
                    if start.File != file_node or end.File != file_node:
                        continue
                    else_pos = pp.Else_Position
                    if else_pos and else_pos.File != file_node:
                        continue
                    if else_pos:
                        continue
                    else:
                        """ Reporting of violation if #else is missing for #elif directive """
                        klass.add_message(
                            shared.message.MessageSLoc(
                                file_node.Full_Name,
                                start.Physical_Line,
                                start.Physical_Column,
                            ),
                            "missing_else_in_elif",
                        )
