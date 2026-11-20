"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : transform_architecture.py
Date of creation : 02.04.2020
Author Name/Dept : UAT Team, RBEI
Organization     : 
Description      : This Python script transforms The architecture from EA model to GXL format.
"""
try:
    import bauhaus

    del bauhaus
except ImportError:
    import sys, subprocess

    sys.exit(subprocess.call(["rfgscript"] + sys.argv))

from bauhaus import rfg
from bauhaus.shared import bauhaustool
from bauhaus.rfg import rfgtool, hierarchies, misc

PARAMETER_DESCRIPTION = {
    "graph": rfgtool.rfg_graph_argument,
    "source_view": {"type": "existing_view", "graph": "graph"},
    "target_view": {"type": "new_view", "graph": "graph"},
}


@rfgtool.with_rfg_types
@bauhaustool.BauhausTool(PARAMETER_DESCRIPTION, "rfg")
def perform(**kwargs):
    """Transforms architecture."""

    graph = kwargs["graph"]
    source_view = kwargs["source_view"]
    target_view = kwargs["target_view"]

    # this is done to ensure the target_view is cleared
    target_view_name = target_view.name()
    target_view.destroy()
    target_view = source_view.copy(target_view_name)

    nodes_to_remove = []
    # Required Interface
    for req_interface_node in source_view.xnodes(
        lambda n: n.is_of_subtype("UML RequiredInterface")
    ):
        parent_node = hierarchies.parent(source_view, req_interface_node)
        if not parent_node or not parent_node.is_of_subtype("UML Component"):
            perform.warning(
                'Interface "%s" does not have a proper parent component'
                % misc.get_attr_value(req_interface_node, "Source.Name", "")
            )
            continue
        for req_edge in req_interface_node.outgoings(
            source_view, lambda e: e.is_of_subtype("UML Dependency")
        ):
            new_dependency = graph.edge("Source_Dependency")
            target_view.add(
                new_dependency, source=parent_node, target=req_edge.target()
            )
        nodes_to_remove.append(req_interface_node)

    for component in source_view.xnodes(lambda n: n.is_of_subtype("UML Component")):
        # add "cycle" - this is similar to the tool add_cycles from the exercises.
        new_dependency = graph.edge("Source_Dependency")
        target_view.add(new_dependency, source=component, target=component)
        # add also a "private" node
        new_private_part = graph.node("Cluster")
        new_private_part["Source.Name"] = "Private (%s)" % misc.get_attr_value(
            component, "Source.Name", ""
        )
        new_hierarchy_edge = graph.edge("Belongs_To")
        target_view.add(new_hierarchy_edge, source=new_private_part, target=component)

    for node in nodes_to_remove:
        target_view.remove(node)

    return graph


# Code for starting the tool from command line
if __name__ == "__main__":
    perform.execute_as_command_line_tool(
        usage="%prog [options] <targetrfg>", description="Transforms architecture."
    )
