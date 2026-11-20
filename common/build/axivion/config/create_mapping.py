"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : create_mapping.py
Date of creation : 28.04.2020
Author Name/Dept : UAT Team, RBEI
Organization     : 
Description      : This Python script generates a mapping between the architecture and actual implementation.
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
    "architecture_view": {"type": "existing_view", "graph": "graph"},
    "hierarchy_view": {"type": "existing_view", "graph": "graph"},
    "mapping_view": {"type": "new_view", "graph": "graph"},
}


@rfgtool.with_rfg_types
@bauhaustool.BauhausTool(PARAMETER_DESCRIPTION, "rfg")
def perform(**kwargs):
    """Create a mapping."""

    graph = kwargs["graph"]
    architecture_view = kwargs["architecture_view"]
    hierarchy_view = kwargs["hierarchy_view"]
    mapping_view = kwargs["mapping_view"]

    def get_normalized_name(name):
        # return name without underscores and in lower casing
        return name.replace("_", "").lower()

    def create_mapping(source, target):
        # create a mapping edge
        edge = graph.edge("Maps_To")
        mapping_view.insert(edge, source=source, target=target)

    def locate_private_part(component):
        for child in hierarchies.children(architecture_view, component):
            if child.is_of_subtype("Cluster") and misc.get_attr_value(
                child, "Source.Name", ""
            ).startswith("Private"):
                return child
        return None

    def is_component(node):
        return node.is_of_subtype("UML Component")

    def is_package(node):
        return node.is_of_subtype("UML Package")

    def get_name(node):
        return misc.get_attr_value(node, "Source.Name", "")

    normalized_filename_to_node = {}
    normalized_routinename_to_node = {}

    for routine_node in hierarchy_view.xnodes(lambda n: n.is_of_subtype("Routine")):
        normalized_routinename_to_node[
            get_normalized_name(get_name(routine_node))
        ] = routine_node

    normalized_packagename_to_node = {}
    normalized_componentname_to_node = {}
    normalized_interfacename_to_node = {}

    for package_node in architecture_view.xnodes(is_package):
        package_normalized_name = get_normalized_name(get_name(package_node))
        normalized_packagename_to_node[package_normalized_name] = package_node
    for component_node in architecture_view.xnodes(is_component):
        component_normalized_name = get_normalized_name(get_name(component_node))
        normalized_componentname_to_node[component_normalized_name] = component_node
        for interface_node in [
            n
            for n in hierarchies.children(architecture_view, component_node)
            if n.is_of_subtype("UML ProvidedInterface")
        ]:
            interface_normalized_name = get_normalized_name(get_name(interface_node))
            normalized_interfacename = "%s%s" % (
                component_normalized_name,
                interface_normalized_name,
            )
            normalized_interfacename_to_node[normalized_interfacename] = interface_node

    # mapping for Mapping package since it does not have components in it
    for file_node in hierarchy_view.xnodes(lambda n: n.is_of_subtype("File")):
        filename = get_normalized_name(misc.get_attr_value(file_node, "Source.Name"))
        if "mapping" in filename:
            print(
                'Found component mapping: File "%s" -> "%s"'
                % (get_name(file_node), "Mapping")
            )
            create_mapping(file_node, normalized_packagename_to_node["mapping"])
        else:
            pass

    # mapping for Dbus package since it does not have components in it
    for directory_node in hierarchy_view.xnodes(lambda n: n.is_of_subtype("Directory")):
        dir_name = get_normalized_name(
            misc.get_attr_value(directory_node, "Source.Name")
        )
        if dir_name == "dbus":
            for file_node in [
                n
                for n in hierarchies.children(hierarchy_view, directory_node)
                if n.is_of_subtype("File")
            ]:
                filename = get_normalized_name(
                    misc.get_attr_value(file_node, "Source.Name")
                )
                if "mapping".lower() not in filename:
                    print(
                        'Found component mapping: File "%s" -> "%s"'
                        % (get_name(file_node), "DBus2")
                    )
                    create_mapping(file_node, normalized_packagename_to_node["dbus2"])
                else:
                    pass

    for file_node in hierarchy_view.xnodes(lambda n: n.is_of_subtype("File")):
        filename = get_normalized_name(misc.get_attr_value(file_node, "Source.Name"))
        for component_name, component_node in normalized_componentname_to_node.items():
            if filename.startswith(component_name):
                print(
                    'Found component mapping: File "%s" -> "%s"'
                    % (get_name(file_node), get_name(component_node))
                )
                private_part = locate_private_part(component_node)
                if private_part is None:
                    perform.warning(
                        'No private node for "%s" found! '
                        % misc.get_attr_value(component_node, "Source.Name", "")
                    )
                    continue
                create_mapping(file_node, private_part)
                for routine_node in [
                    n
                    for n in hierarchies.children(hierarchy_view, file_node)
                    if n.is_of_subtype("Routine")
                ]:
                    routine_name = get_normalized_name(get_name(routine_node))
                    for (
                        interface_designator,
                        interface_node,
                    ) in normalized_interfacename_to_node.items():
                        if routine_name.startswith(interface_designator):
                            print(
                                'Found routine mapping: "%s" -> "%s"'
                                % (get_name(routine_node), get_name(interface_node))
                            )
                            create_mapping(routine_node, interface_node)

    return graph


# Code for starting the tool from command line
if __name__ == "__main__":
    perform.execute_as_command_line_tool(
        usage="%prog [options] <targetrfg>", description="Create a mapping."
    )
