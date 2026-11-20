"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.
File Name        : analysis_config.py
Date of creation : 8.05.2018
Author Name/Dept : Siddharth Trivedi
Organization     : RBEI
Description      : Config file is updated to support common mascking mechanism for PC-Lint and Axivion.
"""

import actions, formats, scopes, rule_selections, justifications, entities, shlex, re, sys

try:
    import os
    import imp

    framework_config_path = os.environ["BAUHAUS_CONFIG"]
    framework_tool_path = "..//..//..//tools//python//BSH_Misra_Config.py"
    BSH_Misra_Config_path = os.path.abspath(
        os.path.realpath(os.path.join(framework_config_path, framework_tool_path))
    )
    BSH_Misra_Config = imp.load_source("bsh_misra_config", BSH_Misra_Config_path)

except ImportError:
    sys.stderr.write(
        'CommonMasking ImportError: "analysis_config.py" can not find "BSH_Misra_Config.py" file\n'
    )

# select the scope based on the routine specified in the comment
def get_scope_for_routine(comment, routine_name):
    return scopes.SelectedRoutineScope(routine_name)


def comparison(id, parsed_id):
    """Function returns the status in dictionary form.
    Returns True if parsed Lint message ID matches with Lint ID configured
    in BSH_Misra_Config dictionary else Return False. User may give ? or *
    in message ID."""

    # Update the value of 'LINT_ID_MAX_SIZE' if Lint increases the message ID
    LINT_ID_MAX_SIZE = 4

    # Update the dictionary 'REG_EX' if Lint increases the message ID
    #  If asterisk('*') has been put at end of the number, below RegEx will
    # be replaced in place of '*'
    #  here, we maintain the size as maximum size of Lint message Id
    #    i.e 77* will be replaced by '77[0-9]{0,2}$'
    #        *77 will be replaced by '^[0-9]{0,2}77'
    # In case of '?', it will be replced by '[0-9]' or '^[0-9]' or '[0-9]$'
    # according to its position
    REG_EX = {
        1: "(?:[1-9][0-9]{3}|[1-9][0-9]{2}|[1-9][0-9]|[1-9])",
        2: "[0-9]{0,3}",
        3: "[0-9]{0,2}",
        4: "[0-9]",
    }

    result = {"Match": False}

    length = len(parsed_id)
    count = parsed_id.count("?") if "?" in parsed_id else 0
    asterisk = True if "*" in parsed_id else False
    if count == 0 and id == parsed_id and asterisk == False:
        result["Match"] = True
        return result
    # Matching the ID having '?' character
    if count > 0:
        if parsed_id.endswith("?") and length == 1:
            parsed_id = parsed_id.replace(
                "?", "^(?:[1-9][0-9]{3}|[1-9][0-9]{2}|[1-9][0-9]|[1-9])$"
            )
        if parsed_id.endswith("?"):
            parsed_id = parsed_id + "$"
        if parsed_id.startswith("?"):
            parsed_id = "^" + parsed_id
        parsed_id = parsed_id.replace("?", "[0-9]")
        match = re.match(parsed_id, id)
        if match:
            result["Match"] = True
            return result
    # Matching the id having '*' character
    if count == 0 and asterisk:
        if parsed_id.endswith("*"):
            parsed_id = parsed_id + "$"
        if parsed_id.startswith("*"):
            parsed_id = "^" + parsed_id
        if length == 1:
            parsed_id = parsed_id.replace("*", REG_EX[1])
        else:
            for key in range(1, LINT_ID_MAX_SIZE):
                if length == key and (
                    parsed_id.endswith("$") or parsed_id.startswith("^")
                ):
                    parsed_id = parsed_id.replace("*", REG_EX[key])
        if not (parsed_id.endswith("*") or parsed_id.startswith("*")):
            parsed_id = parsed_id.replace("*", "[0-9]")
        match = re.match(parsed_id, id)
        if match:
            result["Match"] = True
            return result
    return result


def bsh_rule_parser(parsed_lint_id):
    """Parse The Lint Message and Maping it to Misra rule name,
    e.g. Message 621 maps MisraC2012-5.1,MisraC2012-5.2,
                          MisraC2012-5.4,MisraC2012-5.5
    mapping between Lint message ID and Misra Rules has been
    configured in 'BSH_Misra_Config' file. Update it in need"""

    misra_list = []
    id_match = {parsed_lint_id[i]: 0 for i in range(len(parsed_lint_id))}
    # Match the parsed ID from code anannotation with the ID listed in ,
    # 'BSH_Misra_Config.bsh_misra_config'
    for misra_rule, lint_id in BSH_Misra_Config.bsh_misra_config.iteritems():
        for i in range(len(lint_id)):
            for j in range(len(parsed_lint_id)):
                result = comparison(str(lint_id[i]), str(parsed_lint_id[j]))
                if result["Match"]:
                    misra_list.append(misra_rule)
                    id_match[parsed_lint_id[j]] = 1
    misra_list = [
        misra_list[index].replace("R", "MisraC2012-")
        if misra_list[index].startswith("R")
        else misra_list[index].replace("D", "MisraC2012Directive-")
        if misra_list[index].startswith("D")
        else misra_list[index]
        for index in range(len(misra_list))
    ]
    # for id, count in id_match.iteritems():
    #     if count == 0 :
    #          sys.stderr.write('Error: No Misra rule mapped in "BSH_Misra_Config.py" for Lint Id "%s" in: "%s" \n'
    #                            % (id,comment))
    misra_list = ["NONE"] if len(misra_list) == 0 else misra_list
    misra_list = list(set(misra_list))
    return misra_list


def comment_parser(comment):
    annotation = str(comment)
    # Remove '*/' from end of the code annotation
    #  This will help to deal with '*' if it is provided in message Id
    annotation = (
        annotation.replace("*/", "") if annotation.endswith("*/") else annotation
    )
    return annotation


def suppress_rest_of_file(comment, text):
    listed_id = []
    annotation = comment_parser(comment)
    matchObj = re.match(".*lint\s+((\-e\d+\s*)+)\s*", annotation)
    if matchObj:
        listed_id = matchObj.group(1).split("-e")
        listed_id = [listed_id[i].strip() for i in range(len(listed_id))]
        for i in range(len(listed_id)):
            listed_id[i] = listed_id[i].split(" ", 1)[0]
    else:
        pass
    misra_list = bsh_rule_parser(listed_id)
    return misra_list


def activate_rest_of_file(comment, text):
    listed_id = []
    annotation = comment_parser(comment)
    matchObj = re.match(".*lint\s+((\+e\d+\s*)+)\s*", annotation)
    if matchObj:
        listed_id = matchObj.group(1).split("+e")
        listed_id = [listed_id[i].strip() for i in range(len(listed_id))]
        for i in range(len(listed_id)):
            listed_id[i] = listed_id[i].split(" ", 1)[0]
    else:
        pass
    misra_list = bsh_rule_parser(listed_id)
    return misra_list


def suppress_same_line(comment, text):
    listed_id = []
    annotation = comment_parser(comment)
    matchObj = re.match(".*lint\s+((\!e\d+\s*)+)\s*", annotation)
    if matchObj:
        listed_id = matchObj.group(1).split("!e")
        listed_id = [listed_id[i].strip() for i in range(len(listed_id))]
        for i in range(len(listed_id)):
            listed_id[i] = listed_id[i].split(" ", 1)[0]
    else:
        pass
    misra_list = bsh_rule_parser(listed_id)
    return misra_list


def save_and_restore(comment, text):
    listed_id = []
    annotation = comment_parser(comment)
    matchObj = re.match(".*lint\s*(-save|-restore)\s*((\-e\d+\s*)+)\s*", annotation)
    if matchObj:
        listed_id = matchObj.group(2).split("-e")
        listed_id = [listed_id[i].strip() for i in range(len(listed_id))]
        for i in range(len(listed_id)):
            listed_id[i] = listed_id[i].split(" ", 1)[0]
    else:
        pass
    misra_list = bsh_rule_parser(listed_id)
    return misra_list


""" AXIVION FORMATS """

# This format allows a list of subkind globbing patterns which ends at the
# first : (everything after that colon is seen as justification)
format_axivion = formats.Format("AXIVION Default")
format_axivion.format = "\s*AXIVION\s+(Enable|Disable)\s*(Style\w*|Metric\w*|Dead\w*|Architecture\w*|Clone\w*|Cycle\w*)?\s*([^:]*)(:.*)?"
format_axivion.fields = {
    formats.field_action: (1, actions.parse_enable_disable),
    formats.field_kind: (2, rule_selections.parse_issue_kind),
    formats.field_rule: (3, rule_selections.parse_rule_list),
    formats.field_justification: (4, justifications.parse_justification_after_colon),
    formats.field_scope: scopes.scope_rest_of_file,
}


"""Disable the Rules for rest of the file
   Syntax = /*lint -e# [-e#..]*/  
           (here # is Lint message ID. )"""
format_axivion_suppress = formats.Format("Axivion Lint Rest Of File Suppress")
format_axivion_suppress.format = r"lint\s+((\-e\d+\s*)+)\s*"
format_axivion_suppress.fields = {
    formats.field_action: actions.action_suppress,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, suppress_rest_of_file),
    formats.field_scope: scopes.scope_rest_of_file,
}

"""Enable the Rules for rest of the file
   Syntax = /*lint -e# [-e#..]*/  
           (here # is Lint message ID. )"""
format_axivion_suppress = formats.Format("Axivion Lint Rest Of File Activate")
format_axivion_suppress.format = r"lint\s+((\+e\d+\s*)+)\s*"
format_axivion_suppress.fields = {
    formats.field_action: actions.action_activate,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, activate_rest_of_file),
    formats.field_scope: scopes.scope_rest_of_file,
}

"""Disable the Rules for same line
   Syntax = /*lint !e#[,#..]*/  
            (here # is Lint message ID. 
             adhering to lint syntax, multiple Id is expected here)"""
format_axivion_Same_line_suppress = formats.Format("Axivion Lint Suppress Same Line")
format_axivion_Same_line_suppress.format = r"lint\s+((\!e\d+\s*)+)\s*"
format_axivion_Same_line_suppress.fields = {
    formats.field_action: actions.action_suppress,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, suppress_same_line),
    formats.field_scope: scopes.scope_same_line,
}

"""Disable the Rules for next construct
   Syntax = /*lint -e{#[,#..]}*/  
            (here # is Lint message ID. 
             adhering to lint syntax, multiple Id is expected here)"""
format_axivion_next_construct_suppress = formats.Format(
    "Axivion Lint Suppress Next Construct"
)
format_axivion_next_construct_suppress.format = (
    r"lint\s+\-e\{([1-9]?[0-9\?\*,\s]*)\}\s*"
)
format_axivion_next_construct_suppress.fields = {
    formats.field_action: actions.action_suppress,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, rule_selections.parse_lint_rulenumbers),
    formats.field_scope: scopes.scope_next_construct,
}

"""Disable the Rules for same construct
   Syntax = /*lint --e{#[,#..]}*/  
            (here # is Lint message ID. 
            adhering to lint syntax, multiple Id is expected here)"""
format_axivion_same_construct_suppress = formats.Format(
    "Axivion Lint Suppress Same Construct"
)
format_axivion_same_construct_suppress.format = (
    r"lint\s+\-\-e\{([1-9]?[0-9\?\*,\s]*)\}\s*"
)
format_axivion_same_construct_suppress.fields = {
    formats.field_action: actions.action_suppress,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, rule_selections.parse_lint_rulenumbers),
    formats.field_scope: scopes.scope_same_construct,
}

# lint save and restore
"""Disable the Rules for section of code
   Syntax = /*lint -save -e# [-e#] */  
            (here # is Lint message ID. 
             adhering to lint syntax, single Id is expected here)"""
format_axivion_section_code_suppress = formats.Format(
    "Axivion Lint Suppress Section of code"
)
format_axivion_section_code_suppress.format = r"lint\s*\-save\s*((\-e\d+\s*)+)\s*"
format_axivion_section_code_suppress.fields = {
    formats.field_action: actions.action_suppress,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, save_and_restore),
    formats.field_scope: scopes.scope_rest_of_file,
}

"""Enables the Rules for section of code
   Syntax = /*lint -restore -e# */  
            (here # is Lint message ID. 
             adhering to lint syntax, single Id is expected here)"""
format_axivion_section_code_suppress = formats.Format(
    "Axivion Lint Activate Section of code"
)
format_axivion_section_code_suppress.format = r"lint\s*\-restore\s*((\-e\d+\s*)+)\s*"
format_axivion_section_code_suppress.fields = {
    formats.field_action: actions.action_activate,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, save_and_restore),
    formats.field_scope: scopes.scope_rest_of_file,
}


# and then use this (with the regex group number) as the value for the scope field, e.g.
# example: lint -save -efunc(<nr>,<name>)
format_lint_func = formats.Format("Axivion Lint Suppress for a function using -save")
format_lint_func.format = (
    r"lint\s+\-save\s+(-|\+)efunc\s*\(\s*(\d+)\s*,\s*([^\)]+)\)\s*"
)
format_lint_func.fields = {
    formats.field_action: (1, actions.parse_plus_minus),
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (2, rule_selections.parse_lint_rulenumber),
    formats.field_scope: (3, get_scope_for_routine),
}

# and then use this (with the regex group number) as the value for the scope field, e.g.
# example: lint -efunc(<nr>,<name>)
format_lint_func = formats.Format("Axivion Lint Suppress for a function")
format_lint_func.format = r"lint\s+(-|\+)efunc\s*\(\s*(\d+)\s*,\s*([^\)]+)\)\s*"
format_lint_func.fields = {
    formats.field_action: (1, actions.parse_plus_minus),
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (2, rule_selections.parse_lint_rulenumber),
    formats.field_scope: (3, get_scope_for_routine),
}

format_axivion_next_line = formats.Format("AXIVION Next Line")
format_axivion_next_line.format = "\s*AXIVION Next Line\s+([^:]*)(:.*)?"
format_axivion_next_line.fields = {
    formats.field_action: actions.action_suppress,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, rule_selections.parse_rules),
    formats.field_justification: (2, justifications.parse_justification_after_colon),
    formats.field_scope: scopes.scope_next_line,
}

format_lint_in_macro = formats.Format("Axivion Lint Suppress in macro")
format_lint_in_macro.format = r"\s*lint\s+\-emacro\s*\(\s*(\d+)\s*,\s*([^\)]+)\)\s*"
format_lint_in_macro.fields = {
    formats.field_action: actions.action_suppress,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, rule_selections.parse_lint_rulenumber),
    formats.field_scope: scopes.scope_rest_of_file,
    formats.field_macro: (2, entities.parse_macro_name),
}

format_lint_symbol = formats.Format("Axivion Lint Suppress symbol")
format_lint_symbol.format = r"\s*lint\s+\-esym\s*\(\s*(\d+)\s*,\s*([^\)]+)\)\s*"
format_lint_symbol.fields = {
    formats.field_action: actions.action_suppress,
    formats.field_kind: rule_selections.issues_style,
    formats.field_rule: (1, rule_selections.parse_lint_rulenumber),
    formats.field_scope: scopes.scope_rest_of_file,
    formats.field_entity: (2, entities.parse_entity_name),
}


# MISRA C Rule 21.2 Exception - Identifier starting with '_' are allowed when the subsequent character follows a module abbreviation
format_axivion_project_wide = formats.Format("Module Prefix Project Wide")
format_axivion_project_wide.format = (
    "\s*AXIVION PROJECT ALLOW MODULE PREFIX\s+([^ ]+)\s+([^ ]+)\s+([^: ]+)\s*(:.*)?"
)
format_axivion_project_wide.fields = {
    formats.field_action: actions.action_suppress,
    formats.field_kind: (1, rule_selections.parse_issue_kind),
    formats.field_rule: (2, rule_selections.parse_rules),
    formats.field_entity: (3, entities.parse_macro_regex),
    formats.field_justification: (4, justifications.parse_justification_after_colon),
    formats.field_scope: scopes.scope_project,
}

# Initial state per source file
initial_formats = {
    "*": [
        "AXIVION Default",
        "Axivion Lint Rest Of File Suppress",
        "Axivion Lint Rest Of File Activate",
        "Axivion Lint Suppress Next Construct",
        "Axivion Lint Suppress Same Construct",
        "Axivion Lint Suppress Same Line",
        "Axivion Lint Suppress Section of code",
        "Axivion Lint Activate Section of code",
        "Axivion Lint Suppress for a function using -save",
        "Axivion Lint Suppress for a function",
        "AXIVION Next Line",
        "Axivion Lint Suppress in macro",
        "Axivion Lint Suppress symbol",
        "Module Prefix Project Wide",
    ]
}

# Global comments active throughout the project, can be used to suppress messages
project_comments = [
    "AXIVION PROJECT ALLOW MODULE PREFIX style MisraC2012-21.2 _[A-Z]: Prefix globally allowed",
    "AXIVION PROJECT ALLOW MODULE PREFIX style MisraC2012-21.1 __[A-Z]: Prefix globally allowed",
    "AXIVION PROJECT ALLOW MODULE PREFIX style MisraC2012-21.1 _[A-Z]: Prefix globally allowed",
    "AXIVION PROJECT ALLOW MODULE PREFIX style MisraC2012-21.2 __[A-Z]: Prefix globally allowed",
]
