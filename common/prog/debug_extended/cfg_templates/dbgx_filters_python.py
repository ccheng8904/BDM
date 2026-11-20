from cg_tools import *

reference = ["__logIntVar__", "__logStrIntVar__", "__logStrIntLocatVar__", "__logStrIntLocatFullVar__", 
                "__logIntArrVar__", "__logStrIntArrVar__", "__logStrIntArrLocatVar__", "__logStrIntArrLocatFullVar__"]
myDict = {"__logIntVar__": "DBGX_LOG_INT_VARIABLES", 
            "__logStrIntVar__": "DBGX_LOG_STR_INT_VARIABLES", 
            "__logStrIntLocatVar__": "DBGX_LOG_STR_INT_LOCAT_VARIABLES", 
            "__logStrIntLocatFullVar__": "DBGX_LOG_STR_INT_LOCAT_FULL_VARIABLES",
            "__logIntArrVar__": "DBGX_LOG_INT_ARR_VARIABLES", 
            "__logStrIntArrVar__": "DBGX_LOG_STR_INT_ARR_VARIABLES", 
            "__logStrIntArrLocatVar__": "DBGX_LOG_STR_INT_ARR_LOCAT_VARIABLES", 
            "__logStrIntArrLocatFullVar__": "DBGX_LOG_STR_INT_ARR_LOCAT_FULL_VARIABLES"}


@register_function("dbgx_check_filter_config")
def dbgx_check_filter_config(self, args, unused):
    filter_infos = self.get_arg(args, 0)
    
    output = ""
    redundant_element = []
    list = []
    for domain in filter_infos:
        if domain["domain"] not in list:
           list.append(domain["domain"])
        else:
           redundant_element.append(domain["domain"])
    
    list = []
    for domain in filter_infos:
        for element in domain["scenario"]:
            if element[0] not in list:
                list.append(element[0])
            else:
                redundant_element.append(element[0])

    if redundant_element:
        output = "#warning The following domain or scenario in the debug_extended filter configuration is multiple defined: " + str(redundant_element)
    
    return output

# The first element in "dbgx_activated_filters" holds the valid level filters
def extract_level_filter_infos(filter_infos, activated_filter):
    level_filter_infos = []
    for domain in filter_infos:
        if domain["domain"] == activated_filter[0]:
            level_filter_infos = domain["level"]
    return level_filter_infos

# Only the filter domains, listed in "dbgx_activated_filters" are allowed to be active
def extract_scenario_filter_infos(filter_infos, activated_filter):
    scenario_filter_infos = []
    for domain in filter_infos:
        for scenario in domain["scenario"]:
            if str(domain["domain"]) in activated_filter:
                scenario_filter_infos.append(scenario)
            else:
                scenario_filter_infos.append([scenario[0], "undef", "off"])
    return scenario_filter_infos

@register_function("dbgx_list_level_dyn_filters")
def dbgx_list_level_dyn_filters(self, args, unused):
    filter_infos = self.get_arg(args, 0)
    activated_filter = self.get_arg(args, 1)
    level_filters = extract_level_filter_infos(filter_infos, activated_filter)
    
    output = ""
    for idx, lvl in enumerate(level_filters):
        string = str("#define DBGX_" + lvl[0]).ljust(40)
        string += str(" (((uint32_t)" + str((idx % 32) + 1) + " << DBGX_LEV_BIT_IDX_SHIFT)").ljust(45)
        string += str("| (uint32_t)" + str((idx//32)) + " )").ljust(25)
        string += str(" /* " + lvl[0][0] + " */").ljust(15)
        output += string + "\n"

    return output

@register_function("dbgx_list_scenario_dyn_filters")
def dbgx_list_scenario_dyn_filters(self, args, unused):
    filter_infos = self.get_arg(args, 0)
    activated_filter = self.get_arg(args, 1)
    scenario_filters = extract_scenario_filter_infos(filter_infos, activated_filter)

    output = ""
    index = 0
    for scn in scenario_filters:
        string = str("#define DBGX_SCN_" + scn[0]).ljust(40)
        if(scn[1] == "def"):
            string += str(" (((uint32_t)" + str((index % 32) + 1) + " << DBGX_SCN_BIT_IDX_SHIFT)").ljust(45)
            string += str("| (uint32_t)" + str((index//32) + 1) + " )").ljust(25)
            string += str(" /* 0x" + format(((index//32) + 1), '02x') + format(((index % 32) + 1), '02x') + " */").ljust(15)
            if ((index+1) % 32) == 0:
                string += "\n"
            index += 1
        output += string + "\n"

    return output

@register_function("dbgx_number_filter_flag_words")
def dbgx_number_filter_flag_words(self, args, unused):
    filter_infos = self.get_arg(args, 0)
    activated_filter = self.get_arg(args, 1)
    level_filters = extract_level_filter_infos(filter_infos, activated_filter)
    scenario_filters = extract_scenario_filter_infos(filter_infos, activated_filter)

    output = ((len(level_filters) // 32) + 1) + ((len(scenario_filters) // 32) + 1)
    return str(output)

@register_function("dbgx_level_dyn_flag")
def dbgx_level_dyn_flag(self, args, unused):
    filter_infos = self.get_arg(args, 0)
    activated_filter = self.get_arg(args, 1)
    level_filters = extract_level_filter_infos(filter_infos, activated_filter)
    
    output = ""
    for idx, lvl in enumerate(level_filters, start=0):
        if lvl[2] == "on":
            dynlvl = "1"
        else:
            dynlvl = "0"

        string = ""
        if lvl == 0 or idx == 0:
            string = str("          ").ljust(10)
        else:
            string = str("        | ").ljust(10)

        string += str("((uint32_t)" + dynlvl + " <<      (" + str(idx+1) + " -1))").ljust(30)
        string += str("  /* 0").ljust(10)
        string += str(lvl[0][0]).ljust(17)
        string += str(str(lvl[0]).ljust(24)).ljust(30) 
        string += str(str(2**idx).ljust(10)).ljust(11) 
        string += str(str(format(2**idx, '#010X')).ljust(10)).ljust(10) 
        string += " */ \\"
        output += string + "\n"
    
    return output

@register_function("dbgx_scenario_dyn_flag")
def dbgx_scenario_dyn_flag(self, args, unused):
    filter_infos = self.get_arg(args, 0)
    activated_filter = self.get_arg(args, 1)
    scenario_filters = extract_scenario_filter_infos(filter_infos, activated_filter)
    
    output = ""
    index = 0
    for scn in scenario_filters:
        string = ""
        if(scn[1] == "def"):
            if scn[2] == "on":
                dynscn = "1"
            else:
                dynscn = "0"

            if scn == 0 or ((index) % 32) == 0:
                string = "          "
            else:
                string = "        | "

            string += str("((uint32_t)" + dynscn + " <<      (" + str(((index) % 32) + 1) + "-1))").ljust(30)
            string += str("  /* " + str((index//32) + 1)).ljust(10)
            string += str("0x" + format(((index//32) + 1), '02x') + format(((index % 32) + 1), '02x')).ljust(17)
            string += str(scn[0]).ljust(30)
            string += str(str(2**(index%32)).ljust(10)).ljust(11)
            string += str(str(format(2**(index%32), '#010X')).ljust(10)).ljust(10) 
            string += " */ \\"
            
            if ((index+1) % 32) == 0:
                string += "\n    )," + 116*" " + "\\" + "\n    (" + 117*" " + "\\"
            
            output += string + "\n"
            index += 1

    return output

@register_function("dbgx_level_functions")
def dbgx_level_functions(self, args, unused):
    api_functions = self.get_arg(args, 0)    
    filter_infos = self.get_arg(args, 1)
    activated_filter = self.get_arg(args, 2)
    level_filter_infos = extract_level_filter_infos(filter_infos, activated_filter)
    
    output = ""
    for lvl in level_filter_infos:
        for api in api_functions["api_functions"]:
            string = ""
            param_list = ""
            if len(api) != 1:
                for par in api[1:]:
                    param_list += par + ", "
                param_list = param_list [:-2]
            string += str("#define " + str(api[0]) + "_" + str(lvl[0]) + "(" + str(param_list) + ")").ljust(110)

            param_list = param_list.replace("...", "")  # Replace "..." of the variadic macros with  "__VA_ARGS" for the right side of the function like macro

            if lvl[1] == "def":
                if any(ext in param_list for ext in reference):
                    index = param_list[param_list.find("__log") : param_list.find("Var__")] + "Var__"
                    string += str(myDict[str(index)]) + "(" + api[0] + ", " +"DBGX_" + str(lvl[0]) + ", " + str(param_list) + ")"
                else:
                    string += api[0] + "_Wrapper(" + "DBGX_" + str(lvl[0])
                    if len(param_list) > 2:
                        string +=  ", " + param_list
                    string += ")"
            output += string + "\n"
        output += "\n"

    return output

@register_function("dbgx_scenario_functions")
def dbgx_scenario_functions(self, args, unused):
    api_functions = self.get_arg(args, 0)
    filter_infos = self.get_arg(args, 1)
    activated_filter = self.get_arg(args, 2)

    level_filter_infos = [["", "undef", "off"]]
    level_filter_infos += extract_level_filter_infos(filter_infos, activated_filter)
    scenario_filter_infos = extract_scenario_filter_infos(filter_infos, activated_filter)

    output = ""
    for scn in scenario_filter_infos:
        for api in api_functions["api_functions"]:
            for lvl in level_filter_infos:
                string = ""
                param_list = ""
                if len(api) != 1:
                    for par in api[1:]:
                        param_list += par + ", "
                    param_list = param_list [:-2]

                string += "#define " + api[0]   
                if str(lvl[0]) != "":
                    string += "_" + str(lvl[0])
                string += "_SCN_" + str(scn[0]) + "(" + param_list + ") "
                string = string.ljust(110)

                param_list = param_list.replace("...", "")  # Replace "..." of the variadic macros with  "__VA_ARGS" for the right side of the function like macro
                
                if any(ext in param_list for ext in reference):
                    index = param_list[param_list.find("__log") : param_list.find("Var__")] + "Var__"

                    if lvl[1] == "def" and scn[1] == "def":
                        string += str(myDict[str(index)]) + "(" + api[0] + ", "
                        if str(lvl[0]) != "":
                            string += "DBGX_" + str(lvl[0]) + " | "
                        string += "DBGX_SCN_" + str(scn[0])
                        if len(param_list) > 2:
                            string +=  ", " + str(param_list)
                        string += ")"

                    if lvl[1] == "undef" and scn[1] == "def":
                        string += str(myDict[str(index)]) + "(" + api[0] + ", DBGX_SCN_" + str(scn[0])
                        if len(param_list) > 2:
                            string +=  ", " + str(param_list)
                        string += ")"
                        
                    if lvl[1] == "def" and scn[1] == "undef":
                        string += str(myDict[str(index)]) + "(" + api[0] + ", DBGX_" + str(lvl[0])
                        if len(param_list) > 2:
                            string +=  ", " + str(param_list)
                        string += ")"

                else:
                    if lvl[1] == "def" and scn[1] == "def":
                        string += api[0] + "_Wrapper("
                        if str(lvl[0]) != "":
                            string += "DBGX_" + str(lvl[0]) + " | "
                        string += "DBGX_SCN_" + str(scn[0])
                        if len(param_list) > 2:
                            string +=  ", " + param_list
                        string += ")"

                    if lvl[1] == "undef" and scn[1] == "def":
                        string += api[0] + "_Wrapper(" + "DBGX_SCN_" + str(scn[0])
                        if len(param_list) > 2:
                            string +=  ", " + param_list
                        string += ")"
                        
                    if lvl[1] == "def" and scn[1] == "undef":
                        string += api[0] + "_Wrapper(" + "DBGX_" + str(lvl[0])
                        if len(param_list) > 2:
                            string +=  ", " + param_list
                        string += ")"

                output += string + "\n"
        output += "\n"

    return output

@register_function("dbgx_non_filtered_functions")
def dbgx_non_filtered_functions(self, args, unused):
    api_functions = self.get_arg(args, 0)

    output = ""
    for api in api_functions["api_functions"]:
        param_list = ""
        string = ""
        if len(api) != 1:
            for par in api[1:]:
                param_list += par + ", "
            param_list = param_list [:-2]
        string += str("#define " + str(api[0]) + "_NonFiltered(" + str(param_list) + ")").ljust(110)

        param_list = param_list.replace("...", "")  # Replace "..." of the variadic macros with  "__VA_ARGS" for the right side of the function like macro

        if any(ext in param_list for ext in reference):
            index = param_list[param_list.find("__log") : param_list.find("Var__")] + "Var__"
            string += str(myDict[str(index)]) + "_NON_FILTERED(" + api[0] + "_NonFiltered"     
            if len(param_list) > 0:
                string +=  ", " + str(param_list)
                string += ")"
   
        else:
            string += api[0] + "_NonFiltered_Wrapper("
            if len(param_list) > 0:
                string +=  param_list
            string += ")"
        output += string + "\n"
    return output

@register_function("dbgx_level_functions_doctest")
def dbgx_level_functions_doctest(self, args, unused):
    api_functions = self.get_arg(args, 0)    
    filter_infos = self.get_arg(args, 1)
    activated_filter = self.get_arg(args, 2)
    level_filter_infos = extract_level_filter_infos(filter_infos, activated_filter)
    
    output = ""
    for lvl in level_filter_infos:
        for api in api_functions["api_functions"]:
            string = ""
            param_list = ""
            if len(api) != 1:
                for par in api[1:]:
                    param_list += par + ", "
                param_list = param_list [:-2]
            string += "#define " + str(api[0]) + "_" + str(lvl[0]) + "(" + str(param_list) + ")"
            output += string + "\n"
        output += "\n"
    return output


@register_function("dbgx_scenario_functions_doctest")
def dbgx_scenario_functions_doctest(self, args, unused):
    api_functions = self.get_arg(args, 0)
    filter_infos = self.get_arg(args, 1)
    activated_filter = self.get_arg(args, 2)

    level_filter_infos = [["", "undef", "off"]]
    level_filter_infos += extract_level_filter_infos(filter_infos, activated_filter)
    scenario_filter_infos = extract_scenario_filter_infos(filter_infos, activated_filter)

    output = ""
    for scn in scenario_filter_infos:
        for api in api_functions["api_functions"]:
            for lvl in level_filter_infos:
                string = ""
                param_list = ""
                if len(api) != 1:
                    for par in api[1:]:
                        param_list += par + ", "
                    param_list = param_list [:-2]

                string += "#define " + api[0]   
                if str(lvl[0]) != "":
                    string += "_" + str(lvl[0])
                string += "_SCN_" + str(scn[0]) + "(" + param_list + ") "
                output += string + "\n"
        output += "\n"

    return output


@register_function("dbgx_non_filtered_functions_doctest")
def dbgx_non_filtered_functions_doctest(self, args, unused):
    api_functions = self.get_arg(args, 0)

    output = ""
    for api in api_functions["api_functions"]:
        param_list = ""
        string = ""
        if len(api) != 1:
            for par in api[1:]:
                param_list += par + ", "
            param_list = param_list [:-2]
        string += "#define " + str(api[0]) + "_NonFiltered(" + str(param_list) + ")"
        output += string + "\n"
    return output


def __main__():
    return ""
