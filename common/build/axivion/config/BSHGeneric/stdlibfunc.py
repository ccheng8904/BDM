"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : stdlibfunc.py
Date of creation : 19.07.2018
Author Name/Dept : Jennifer Deepa R
Organization     : RBEI
Description      : The pointer returned by the Standard Library functions shall not be used following a subsequent 
                   call to the same function

"""

import generic
from bauhaus import ir
from bauhaus.ir.common.output import unparse
from collections import defaultdict
from collections import OrderedDict
import json, ast
from itertools import chain

""" converts default_dict to dictionary

Args:
    default_dict : The default dictionary which has function call information
	               and return variable list

Returns:
    dict(default_dict) : Returns a dictionary
"""


def defaultdict2dict(default_dict):
    for key, value in default_dict.items():
        if isinstance(value, dict):
            default_dict[key] = defaultdict2dict(value)
    return dict(default_dict)


""" Finds the usage of return value in the dictionary
Args:
    key_to_find : Return value to be searched
    dictionary :  key_to_find to be searched in function and 
                  return value dict

Returns:
    keys : Returns a key of the value found
"""


def find(key_to_find, dictionary):
    for key, value in dictionary.iteritems():
        if value == key_to_find:
            yield key
        elif isinstance(value, OrderedDict):
            for keys, values in value.iteritems():
                if isinstance(values, list):
                    for i in range(len(values)):
                        if values[i] == key_to_find:
                            yield keys


""" Reports the violation in the .sstc file
Args:
    funcs_return_var_sorted : OrderedDict of the function and return values
    libfunc :  standard library functions to be searched
    var_list : Return variables of used in the functions by the standard library functions
    line_list : Line number and the node obtained from ir_graph
	
Returns:
    keys : Returns a key of the value found
"""


def reporting(klass, ir_graph, funcs_return_var_sorted, libfunc, var_list, line_list):
    res_var_list = defaultdict(dict)
    for keys, values in funcs_return_var_sorted.iteritems():
        if type(values) is OrderedDict:
            for key, value in values.iteritems():
                if libfunc in value:
                    try:
                        res_var_list[keys][int(key)] = value[1]
                    except:
                        res_var_list[keys][int(key)] = "no_return"
    res_var_list_sort = json.JSONDecoder(object_pairs_hook=OrderedDict).decode(
        json.dumps(res_var_list, sort_keys=True)
    )

    listoffirstoccurence = defaultdict(list)
    tobesearched = defaultdict(dict)
    for keys, values in res_var_list_sort.iteritems():
        if type(values) is OrderedDict:
            if len(values.values()) > 1:
                listoffirstoccurence[values.keys()[1]].append(
                    list(find(values.values()[0], var_list))
                )
    listofline = list(chain.from_iterable(listoffirstoccurence))
    listoffirstoccurence = ast.literal_eval(json.dumps(listoffirstoccurence))
    listofviolationsline = []
    for keys, values in listoffirstoccurence.items():
        for iterator in values[0]:
            if iterator > keys:
                listofviolationsline.append(int(iterator))
        else:
            pass
    for iterator in listofviolationsline:
        klass.add_message(
            line_list[iterator], "Rule21.20", entity=unparse.entity(libfunc)
        )


""" Removes the duplicate elements from list"""


def remove_duplicates(values):
    output = []
    seen = set()
    for value in values:
        if value not in seen:
            output.append(value)
            seen.add(value)
    return output


""" Finds the usage of return value in the dictionary
Args:
    statement_sequence : All the expression statement within the statement_sequence gets evaluated
    node :  Parse static function(node) in which standard library functions are used
                  
Returns:
    var_list, call_list, line_list, return_var_list : Returns lists of data to analyse and print
	                                                  violations on .sstc report
"""


def expression_statement(klass, ir_graph, statement_sequence, node):
    try:
        """ Checks for expression which has Standard_Assignment and store return values of\
				setlocale ,asctime and gmtime """
        if statement_sequence.Expr.is_of_type("Standard_Assignment"):
            return_var_list[node.Name][
                statement_sequence.Expr.SLoc.Line
            ] = statement_sequence.Expr.Target.Name
            """ checks for standard library function calls \
					when return variable is of type const """
            if statement_sequence.Expr.Source.is_of_type(
                "Standard_Implicit_Conversion"
            ):
                if statement_sequence.Expr.Source.Operand.is_of_type(
                    "Function_Notation_Call"
                ):
                    call_list[node.Name][
                        statement_sequence.Expr.SLoc.Line
                    ] = statement_sequence.Expr.Source.Operand.Call_Target.Name
                    line_list[
                        statement_sequence.Expr.SLoc.Line
                    ] = statement_sequence.Expr.Source.SLoc
            else:
                pass
    except:
        pass
    try:
        """checks for standard library function calls and return variables when
        return variable is of type non const"""
        if statement_sequence.Expr.Source.is_of_type("Function_Notation_Call"):
            call_list[node.Name][
                statement_sequence.Expr.SLoc.Line
            ] = statement_sequence.Expr.Source.Call_Target.Name
            return_var_list[node.Name][
                statement_sequence.Expr.SLoc.Line
            ] = statement_sequence.Expr.Target.Name
            line_list[
                statement_sequence.Expr.SLoc.Line
            ] = statement_sequence.Expr.Source.SLoc
    except:
        pass
    try:
        """ Checks for expression of type Function_Notation_Call """
        if statement_sequence.Expr.is_of_type("Function_Notation_Call"):
            """ Checking for functions whose return value is not assigned to a pointer """
            if (
                statement_sequence.Expr.Call_Target.Name == "setlocale"
                or "asctime"
                or "gmtime"
                or "localeconv"
                or "localtime"
                or "strerror"
                or "ctime"
                or "getenv"
            ):
                call_list[node.Name][
                    statement_sequence.Expr.SLoc.Line
                ] = statement_sequence.Expr.Call_Target.Name
            """ Checking for usage of variables """
            for i in range(len(statement_sequence.Expr.Arguments)):
                if statement_sequence.Expr.Arguments[i].is_of_type(
                    "Unqualified_Direct_Object_Selection"
                ):
                    var_list[node.Name][statement_sequence.Expr.SLoc.Line].append(
                        statement_sequence.Expr.Arguments[i].Name
                    )
                    line_list[
                        statement_sequence.Expr.SLoc.Line
                    ] = statement_sequence.Expr.Arguments[i].SLoc
                """ Checking for usage of return value after subsequent call - for gmtime - \
						accessing the structure elements """
                if statement_sequence.Expr.Arguments[i].is_of_type(
                    "Arrow_Direct_Member_Selection"
                ):
                    var_list[node.Name][statement_sequence.Expr.SLoc.Line].append(
                        statement_sequence.Expr.Arguments[i].Composite_Object.Name
                    )
                    line_list[
                        statement_sequence.Expr.SLoc.Line
                    ] = statement_sequence.Expr.Arguments[i].SLoc
                if statement_sequence.Expr.Arguments[i].is_of_type(
                    "Function_Notation_Call"
                ):
                    for j in range(len(statement_sequence.Expr.Arguments[i].Arguments)):
                        if (
                            statement_sequence.Expr.Arguments[i]
                            .Arguments[j]
                            .is_of_type("Standard_Implicit_Conversion")
                        ):
                            var_list[node.Name][
                                statement_sequence.Expr.SLoc.Line
                            ].append(
                                statement_sequence.Expr.Arguments[i]
                                .Arguments[j]
                                .Operand.Name
                            )
                            line_list[statement_sequence.Expr.SLoc.Line] = (
                                statement_sequence.Expr.Arguments[i].Arguments[j].SLoc
                            )
    except:
        pass
    else:
        pass
    return var_list, call_list, line_list, return_var_list


class stdlibfunc(generic.GenericRule):

    rulename = "stdlibfunc"
    ruletext = """The pointer returned by the Standard Library functions asctime, ctime, gmtime, localtime,
                 localeconv, getenv, setlocale or strerror shall not be used following a subsequent call to 
                 the same function"""

    languages = ["C", "C++"]

    _guid = "1445F688-3173-4CFF-AE6F-51049FC24E11"
    _message_descriptions = {
        "Rule21.20": (
            "The pointer returned by the Standard Library functions shall not be used "
            " following a subsequent call to the same "
            " function",
            "4306F978-AB8C-4A5F-BE89-DBAEA0D2ACD8",
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
        global call_list, return_var_list, line_list, var_list, func_gmtime, func_setlocale, func_asctime, func_ctime, func_getenv, func_localeconv, func_strerror, func_localtime
        """ stores all standard library function calls """
        call_list = defaultdict(dict)
        """ stores all return variables of standard library function calls """
        return_var_list = defaultdict(dict)
        """ stores all variables which is used after the standard library function calls """
        var_list = defaultdict(lambda: defaultdict(list))
        """ stores all line number of function calls, return variables and use of variables """
        line_list = defaultdict(dict)
        """ stores the static function names in which gmtime is used """
        func_gmtime = defaultdict(list)
        """ stores the static function names in which setlocale is used """
        func_setlocale = defaultdict(list)
        """ stores the static function names in which asctime is used """
        func_asctime = defaultdict(list)
        """ stores the static function names in which localtime is used """
        func_localtime = defaultdict(list)
        """ stores the static function names in which getenv is used """
        func_getenv = defaultdict(list)
        """ stores the static function names in which ctime is used """
        func_ctime = defaultdict(list)
        """ stores the static function names in which localeconv is used """
        func_localeconv = defaultdict(list)
        """ stores the static function names in which strerror is used """
        func_strerror = defaultdict(list)
        global static_functions_to_be_checked
        static_functions_to_be_checked = []

        for node in ir_graph.nodes_of_type(
            ir.Physical, ("Unqualified_Direct_Object_Selection")
        ):
            if node.Name == "gmtime":
                func_gmtime[node.Parent.Logical.Parent.Physical].append(node)
            if node.Name == "setlocale":
                func_setlocale[node.Parent.Logical.Parent.Physical].append(node)
            if node.Name == "asctime":
                func_asctime[node.Parent.Logical.Parent.Physical].append(node)
            if node.Name == "getenv":
                func_getenv[node.Parent.Logical.Parent.Physical].append(node)
            if node.Name == "localtime":
                func_localtime[node.Parent.Logical.Parent.Physical].append(node)
            if node.Name == "ctime":
                func_ctime[node.Parent.Logical.Parent.Physical].append(node)
            if node.Name == "localeconv":
                func_localeconv[node.Parent.Logical.Parent.Physical].append(node)
            if node.Name == "strerror":
                func_strerror[node.Parent.Logical.Parent.Physical].append(node)

        """ checks for the static functions only if standard library functions are used more than once """
        for static_funcs in (
            func_asctime,
            func_setlocale,
            func_gmtime,
            func_getenv,
            func_localtime,
            func_ctime,
            func_localeconv,
            func_strerror,
        ):
            for keys, values in static_funcs.items():
                if len(values) > 1:
                    static_functions_to_be_checked.append(keys)

        """ list of static functions which needs to be scanned to check violations for setlocale, asctime and gmtime usage """
        static_functions_to_be_checked = remove_duplicates(
            static_functions_to_be_checked
        )

        for node in static_functions_to_be_checked:
            for func_definition in node.Children:
                for statement_sequence in func_definition.Children:
                    if statement_sequence.is_of_type("Expression_Statement"):
                        (
                            var_list,
                            call_list,
                            line_list,
                            return_var_list,
                        ) = expression_statement(
                            klass, ir_graph, statement_sequence, node
                        )

                    if statement_sequence.is_of_type("If_Statement"):
                        if statement_sequence.Then_Branch.is_of_type(
                            "Statement_Sequence"
                        ):
                            for nodeexpr in statement_sequence.Then_Branch.Children:
                                (
                                    var_list,
                                    call_list,
                                    line_list,
                                    return_var_list,
                                ) = expression_statement(
                                    klass, ir_graph, nodeexpr, node
                                )
                        if statement_sequence.Else_Branch.is_of_type(
                            "Statement_Sequence"
                        ):
                            for nodeexpr in statement_sequence.Else_Branch.Children:
                                (
                                    var_list,
                                    call_list,
                                    line_list,
                                    return_var_list,
                                ) = expression_statement(
                                    klass, ir_graph, nodeexpr, node
                                )

        combine_dict = defaultdict(lambda: defaultdict(list))
        for iterator in (call_list, return_var_list):
            for key, value in iterator.items():
                for keys, values in value.items():
                    combine_dict[key][int(keys)].append(values)

        funcs_return_var = defaultdict2dict(combine_dict)

        funcs_return_var_sort = json.JSONDecoder(object_pairs_hook=OrderedDict).decode(
            json.dumps(funcs_return_var, sort_keys=True)
        )
        var_list_sort = json.JSONDecoder(object_pairs_hook=OrderedDict).decode(
            json.dumps(var_list, sort_keys=True)
        )
        line_list = dict(line_list)
        """ check for violations for usage of below functions"""
        stdlibfunctions = [
            "setlocale",
            "asctime",
            "gmtime",
            "ctime",
            "getenv",
            "localtime",
            "localeconv",
            "strerror",
        ]
        for stdlibfunc in stdlibfunctions:
            reporting(
                klass,
                ir_graph,
                funcs_return_var_sort,
                stdlibfunc,
                var_list_sort,
                line_list,
            )
