import os,sys
from bauhaus.ir.common.languages import translation_limits

sys.path.append(os.environ["BAUHAUS_CONFIG"])

# Configure Message

# Default (not correct for 6.1.3 Plus)
# Message.fmt1 = '%(filename)s:%(line)d:%(column)d: %(severity)s:' ' %(message)s (Rule %(rulegroup)s-%(rulename)s)'
# Message.fmt2 = '%(line)d:%(column)d'
# Message.lsep = '\n        ' # default for long messages
# Message.bsep = ', '         # default for brief messages

# Default with disable
# Message.disabled = 'DISABLED'
# Message.fmt1 = '%(filename)s:%(line)d:%(column)d: %(severity)s:' ' %(message)s (Rule %(rulegroup)s-%(rulename)s):%(disabled)s'
# Message.fmt1 = '%(filename)s:%(line)d:%(column)d: %(severity)s:' ' %(message)s (Rule %(rulegroup)s-%(rulename)s):enabled%(disabled)s'


# The Following is activated by: set AXIVION_LOCAL_BUILD=True
# attention: don't use 0 in compare - does not work - presumably because this is interpreted by Python as  None

Message.fmt1 = "Axivion: %(filename)s\t%(line)d\t%(column)d\t%(severity)s\t%(message)s\t%(rulegroup)s\t%(rulename)s\t%(disabled)s"
Message.fmt2 = "%(line)d\t%(column)d"
Message.disabled = "DISABLED"
Message.lsep = "\n\t"
Message.bsep = "\t"

# For those rules which needs ir analysis enabled
CONFIG.Run_IRAnalysis_Checks = True

# print 'fmt1: ', Message.fmt1

# suggestion of Mr. Bellon:  overwrite 'enabled' with Backspace - but does not work
# Message.fmt1 = '%(filename)s\t%(line)d\t%(column)d\t%(severity)s\t%(message)s\t%(rulegroup)s-%(rulename)s\tenabled%(disabled)s'
# Message.disabled = '\b\b\b\b\b\b\bdisabled'


# RULES['Generic-DoxygenCommentAtDefinition'].severity = 'my_severity'

# Configure Rules
# RULES['Generic-DoxygenCommentAtDefinition'].definition_types = ('Routine_Definition')
# RULES['Generic-NoMagicNumbers'].exclude_pp_literal = False
# RULES['Generic-LocalScope'].maxcomplexity =  20

# RULES['MisraC2012-2.3'].severity = 'my_severity' #klappt
# RULES['MisraC2012-2.7'].message = 'my_message' #klappt nicht
# RULES['MisraC2012-2.7'].rulegroup = "MisraC2012-local" #klappt, auch mit tagbs (\t) drin
# RULES['MisraC2012-2.7'].rulenname = "my 2.7.\ttest" #klappt nicht

# 11.12.14 UlBr: wenn man rulegroup ändert, funktioniert das disabeln über Scanner.config nicht mehr
# RULES['MisraC2012-1.1'].rulegroup = 'MisraC2012-unit' # UlBr: not provided with 6.2 RC1
# RULES['MisraC2012-1.2'].rulegroup = 'MisraC2012-unit'

# BSH PED Config

# Configuring the severity of rules
RULES["MisraC2012-2.2"].severity = "advisory"
RULES["MisraC2012-2.6"].severity = "mandatory"
RULES["MisraC2012-2.7"].severity = "required"
RULES["MisraC2012-3.1"].severity = "mandatory"
RULES["MisraC2012-3.2"].severity = "mandatory"
RULES["MisraC2012-4.2"].severity = "mandatory"
RULES["MisraC2012-5.3"].severity = "mandatory"
RULES["MisraC2012-5.6"].severity = "mandatory"
RULES["MisraC2012-5.7"].severity = "mandatory"
RULES["MisraC2012-5.8"].severity = "mandatory"
RULES["MisraC2012-5.9"].severity = "mandatory"
RULES["MisraC2012-6.2"].severity = "mandatory"
RULES["MisraC2012-7.1"].severity = "mandatory"
RULES["MisraC2012-7.2"].severity = "advisory"
RULES["MisraC2012-7.3"].severity = "advisory"
RULES["MisraC2012-8.1"].severity = "mandatory"
RULES["MisraC2012-8.2"].severity = "mandatory"
RULES["MisraC2012-8.3"].severity = "mandatory"
RULES["MisraC2012-8.5"].severity = "mandatory"
RULES["MisraC2012-8.6"].severities["undefined_function"] = "advisory"
RULES["MisraC2012-8.6"].severities["undefined_variable"] = "advisory"
RULES["MisraC2012-8.6"].severities["function_defined_multiple_times"] = "required"
RULES["MisraC2012-8.6"].severities["variable_defined_multiple_times"] = "required"
RULES["MisraC2012-8.10"].severity = "mandatory"
RULES["MisraC2012-8.11"].severity = "required"
RULES["MisraC2012-8.12"].severity = "mandatory"
RULES["MisraC2012-8.14"].severity = "mandatory"
RULES["MisraC2012-9.2"].severity = "mandatory"
RULES["MisraC2012-11.4"].severity = "required"
RULES["MisraC2012-11.5"].severity = "required"
RULES["MisraC2012-11.9"].severity = "mandatory"
RULES["MisraC2012-12.1"].severity = "required"
RULES["MisraC2012-12.4"].severity = "required"
RULES["MisraC2012-13.2"].severity = "mandatory"
RULES["MisraC2012-13.4"].severity = "required"
RULES["MisraC2012-14.3"].severity = "advisory"
RULES["MisraC2012-15.1"].severity = "required"
RULES["MisraC2012-16.3"].severity = "advisory"
RULES["MisraC2012-17.5"].severity = "required"
RULES["MisraC2012-21.6"].severity = "advisory"
RULES["MisraC2012-21.12"].severity = "required"
RULES["CertC-STR37"].severity = "mandatory"
RULES["Generic-NoMagicNumbers"].severity = "required"
RULES["Generic-NoInlineFunctionInHeader"].severity = "advisory"
RULES["Generic-NoInlineFunctionInSource"].severity = "required"
RULES["Generic-TerminateElifWithElse"].severity = "advisory"
RULES["Generic-DoNotIncludeCfiles"].severity = "required"
RULES["Generic-DoNotRenameFuncOrVar"].severity = "required"
RULES["Generic-BSH_HungarianNotation_NamingRule"].severity = "advisory"
RULES["Generic-BSH_Variable_NamingRule"].severity = "advisory"
RULES["Generic-BSH_Enumerator_NamingRule"].severity = "mandatory"
RULES["Generic-BSH_FolderAndFile_NamingRule"].severity = "advisory"
RULES["Generic-BSH_Function_NamingRule"].severity = "mandatory"
RULES["Generic-BSH_ISR_Function_NamingRule"].severity = "advisory"
RULES["Generic-BSH_MACRO_NamingRule"].severity = "mandatory"
RULES["Generic-BSH_MacroWithFunction_NamingRule"].severity = "advisory"
RULES["Generic-BSH_Structure_NamingRule"].severity = "advisory"
RULES["Generic-BSH_Typedef_NamingRule"].severity = "advisory"
RULES["Generic-BSH_Union_NamingRule"].severity = "advisory"
RULES["Generic-stdlibfunc"].severity = "mandatory"

# Configuring the rulegroup of rules
RULES["CertC-STR37"].rulegroup = "MisraC2012"
RULES["Generic-NoMagicNumbers"].rulegroup = "BSHRule"
RULES["Generic-NoInlineFunctionInHeader"].rulegroup = "BSHRule"
RULES["Generic-NoInlineFunctionInSource"].rulegroup = "BSHRule"
RULES["MisraC-8.5"].rulegroup = "BSHRule"
RULES["Generic-TerminateElifWithElse"].rulegroup = "BSHRule"
RULES["Generic-DoNotIncludeCfiles"].rulegroup = "BSHRule"
RULES["Generic-DoNotRenameFuncOrVar"].rulegroup = "BSHRule"
RULES["Generic-BSH_HungarianNotation_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_Variable_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_Enumerator_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_FolderAndFile_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_Function_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_ISR_Function_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_MACRO_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_MacroWithFunction_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_Structure_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_Typedef_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-BSH_Union_NamingRule"].rulegroup = "BSHNamingRule"
RULES["Generic-stdlibfunc"].rulegroup = "MisraC2012"

# Configuring the rulename of rules
RULES["CertC-STR37"].rulename = "21.13"
RULES["Generic-NoMagicNumbers"].rulename = "2-NoMagicNumbers"
RULES["Generic-NoInlineFunctionInHeader"].rulename = "5-NoInlineFunctionInHeader"
RULES["Generic-NoInlineFunctionInSource"].rulename = "6-NoInlineFunctionInSource"
RULES["MisraC-8.5"].rulename = "13-NoFuncVarDefinitionInHeader"
RULES["Generic-TerminateElifWithElse"].rulename = "8-TerminateElifWithElse"
RULES["Generic-DoNotIncludeCfiles"].rulename = "7-DoNotIncludeCfiles"
RULES["Generic-DoNotRenameFuncOrVar"].rulename = "9-DoNotRenameFuncOrVar"
RULES["Generic-BSH_HungarianNotation_NamingRule"].rulename = "HungarianNotation"
RULES["Generic-BSH_Variable_NamingRule"].rulename = "VariableName"
RULES["Generic-BSH_Enumerator_NamingRule"].rulename = "EnumeratorName"
RULES["Generic-BSH_FolderAndFile_NamingRule"].rulename = "FolderAndFileName"
RULES["Generic-BSH_Function_NamingRule"].rulename = "FunctionName"
RULES["Generic-BSH_ISR_Function_NamingRule"].rulename = "ISR_FunctionName"
RULES["Generic-BSH_MACRO_NamingRule"].rulename = "MACROName"
RULES["Generic-BSH_MacroWithFunction_NamingRule"].rulename = "MacroWithFunctionName"
RULES["Generic-BSH_Structure_NamingRule"].rulename = "StructureName"
RULES["Generic-BSH_Typedef_NamingRule"].rulename = "TypedefName"
RULES["Generic-BSH_Union_NamingRule"].rulename = "UnionName"
RULES["Generic-stdlibfunc"].rulename = "21.20"
RULES["MisraC++-0.1.1"].severity = "required"
RULES["MisraC++-0.1.3"].severity = "advisory"
RULES["MisraC++-0.1.4"].severity = "advisory"
RULES["MisraC++-0.1.5"].severity = "advisory"
RULES["MisraC++-0.1.6"].severity = "advisory"
RULES["MisraC++-0.1.7"].severity = "advisory"
RULES["MisraC++-0.1.8"].severity = "required"
RULES["MisraC++-0.1.11"].severity = "advisory"
RULES["MisraC++-0.1.12"].severity = "advisory"
RULES["MisraC++-0.2.1"].severity = "mandatory"
RULES["MisraC++-0.3.1"].severity = "advisory"
RULES["MisraC++-0.3.2"].severity = "advisory"
RULES["MisraC++-0.4.2"].severity = "required"
# RULES['MisraC++-1.0.1'].severity='required'
# RULES['MisraC++-1.0.2'].severity='advisory'
# RULES['MisraC++-1.0.3'].severity='required'
# RULES['MisraC++-2.2.1'].severity='required'
RULES["MisraC++-2.3.1"].severity = "mandatory"
RULES["MisraC++-2.5.1"].severity = "mandatory"
RULES["MisraC++-2.7.1"].severity = "required"
RULES["MisraC++-2.7.2"].severity = "required"
RULES["MisraC++-2.7.3"].severity = "required"
RULES["MisraC++-2.10.1"].severity = "advisory"
RULES["MisraC++-2.10.2"].severity = "mandatory"
RULES["MisraC++-2.10.3"].severity = "mandatory"
RULES["MisraC++-2.10.4"].severity = "required"
RULES["MisraC++-2.10.5"].severity = "advisory"
RULES["MisraC++-2.10.6"].severity = "required"
RULES["MisraC++-2.13.1"].severity = "mandatory"
RULES["MisraC++-2.13.2"].severity = "mandatory"
RULES["MisraC++-2.13.3"].severity = "mandatory"
RULES["MisraC++-2.13.4"].severity = "advisory"
RULES["MisraC++-2.13.5"].severity = "required"
RULES["MisraC++-3.1.1"].severity = "required"
RULES["MisraC++-3.1.2"].severity = "required"
RULES["MisraC++-3.1.3"].severity = "required"
RULES["MisraC++-3.2.1"].severity = "required"
RULES["MisraC++-3.2.2"].severity = "required"
RULES["MisraC++-3.2.3"].severity = "required"
RULES["MisraC++-3.2.4"].severity = "mandatory"
RULES["MisraC++-3.3.1"].severity = "mandatory"
RULES["MisraC++-3.3.2"].severity = "required"
RULES["MisraC++-3.4.1"].severity = "advisory"
RULES["MisraC++-3.9.1"].severity = "mandatory"
RULES["MisraC++-3.9.2"].severity = "required"
RULES["MisraC++-3.9.3"].severity = "required"
RULES["MisraC++-4.5.1"].severity = "required"
RULES["MisraC++-4.5.2"].severity = "required"
RULES["MisraC++-4.5.3"].severity = "required"
RULES["MisraC++-4.10.1"].severity = "required"
RULES["MisraC++-4.10.2"].severity = "required"
RULES["MisraC++-5.0.1"].severity = "mandatory"
RULES["MisraC++-5.0.2"].severity = "advisory"
RULES["MisraC++-5.0.3"].severity = "required"
RULES["MisraC++-5.0.4"].severity = "required"
RULES["MisraC++-5.0.5"].severity = "required"
RULES["MisraC++-5.0.6"].severity = "required"
RULES["MisraC++-5.0.7"].severity = "required"
RULES["MisraC++-5.0.8"].severity = "required"
RULES["MisraC++-5.0.9"].severity = "required"
RULES["MisraC++-5.0.10"].severity = "required"
RULES["MisraC++-5.0.11"].severity = "required"
RULES["MisraC++-5.0.12"].severity = "required"
RULES["MisraC++-5.0.14"].severity = "advisory"
RULES["MisraC++-5.0.15"].severity = "advisory"
RULES["MisraC++-5.0.16"].severity = "required"
RULES["MisraC++-5.0.17"].severity = "required"
RULES["MisraC++-5.0.18"].severity = "required"
RULES["MisraC++-5.0.19"].severity = "required"
RULES["MisraC++-5.0.20"].severity = "required"
RULES["MisraC++-5.0.21"].severity = "required"
RULES["MisraC++-5.2.1"].severity = "required"
RULES["MisraC++-5.2.2"].severity = "required"
RULES["MisraC++-5.2.3"].severity = "advisory"
RULES["MisraC++-5.2.4"].severity = "required"
RULES["MisraC++-5.2.5"].severity = "required"
RULES["MisraC++-5.2.6"].severity = "required"
RULES["MisraC++-5.2.7"].severity = "required"
RULES["MisraC++-5.2.8"].severity = "required"
RULES["MisraC++-5.2.9"].severity = "required"
RULES["MisraC++-5.2.10"].severity = "advisory"
RULES["MisraC++-5.2.11"].severity = "required"
RULES["MisraC++-5.2.12"].severity = "required"
RULES["MisraC++-5.3.1"].severity = "required"
RULES["MisraC++-5.3.2"].severity = "required"
RULES["MisraC++-5.3.3"].severity = "required"
RULES["MisraC++-5.3.4"].severity = "mandatory"
RULES["MisraC++-5.8.1"].severity = "required"
RULES["MisraC++-5.14.1"].severity = "required"
RULES["MisraC++-5.17.1"].severity = "advisory"
RULES["MisraC++-5.18.1"].severity = "advisory"
RULES["MisraC++-5.19.1"].severity = "required"
RULES["MisraC++-6.2.1"].severity = "required"
RULES["MisraC++-6.2.2"].severity = "mandatory"
RULES["MisraC++-6.2.3"].severity = "advisory"
RULES["MisraC++-6.3.1"].severity = "required"
RULES["MisraC++-6.4.1"].severity = "required"
RULES["MisraC++-6.4.4"].severity = "required"
RULES["MisraC++-6.4.5"].severity = "advisory"
RULES["MisraC++-6.4.6"].severity = "required"
RULES["MisraC++-6.4.7"].severity = "required"
RULES["MisraC++-6.4.8"].severity = "required"
RULES["MisraC++-6.5.1"].severity = "advisory"
RULES["MisraC++-6.5.2"].severity = "required"
RULES["MisraC++-6.5.3"].severity = "required"
RULES["MisraC++-6.5.4"].severity = "advisory"
RULES["MisraC++-6.6.1"].severity = "required"
RULES["MisraC++-6.6.2"].severity = "required"
RULES["MisraC++-7.1.1"].severity = "required"
RULES["MisraC++-7.1.2"].severity = "required"
RULES["MisraC++-7.2.1"].severity = "required"
RULES["MisraC++-7.3.1"].severity = "advisory"
RULES["MisraC++-7.3.2"].severity = "advisory"
RULES["MisraC++-7.3.3"].severity = "required"
RULES["MisraC++-7.3.5"].severity = "required"
RULES["MisraC++-7.3.6"].severity = "mandatory"
RULES["MisraC++-7.4.1"].severity = "required"
RULES["MisraC++-7.4.2"].severity = "required"
RULES["MisraC++-7.4.3"].severity = "required"
RULES["MisraC++-7.5.1"].severity = "required"
RULES["MisraC++-7.5.2"].severity = "required"
RULES["MisraC++-7.5.3"].severity = "required"
RULES["MisraC++-7.5.4"].severity = "advisory"
RULES["MisraC++-8.3.1"].severity = "advisory"
RULES["MisraC++-8.4.1"].severity = "advisory"
RULES["MisraC++-8.4.2"].severity = "required"
RULES["MisraC++-8.4.3"].severity = "mandatory"
RULES["MisraC++-8.4.4"].severity = "required"
RULES["MisraC++-8.5.1"].severity = "required"
RULES["MisraC++-8.5.2"].severity = "required"
RULES["MisraC++-8.5.3"].severity = "advisory"
RULES["MisraC++-9.3.1"].severity = "required"
RULES["MisraC++-9.3.2"].severity = "required"
RULES["MisraC++-9.3.3"].severity = "required"
RULES["MisraC++-9.5.1"].severity = "advisory"
RULES["MisraC++-9.6.1"].severity = "advisory"
RULES["MisraC++-9.6.2"].severity = "required"
RULES["MisraC++-9.6.3"].severity = "required"
RULES["MisraC++-9.6.4"].severity = "required"
RULES["MisraC++-10.1.2"].severity = "advisory"
RULES["MisraC++-10.1.3"].severity = "required"
RULES["MisraC++-10.2.1"].severity = "advisory"
RULES["MisraC++-10.3.1"].severity = "advisory"
RULES["MisraC++-10.3.2"].severity = "required"
RULES["MisraC++-10.3.3"].severity = "advisory"
RULES["MisraC++-11.0.1"].severity = "advisory"
RULES["MisraC++-12.1.1"].severity = "required"
RULES["MisraC++-12.1.3"].severity = "advisory"
RULES["MisraC++-12.8.1"].severity = "required"
RULES["MisraC++-12.8.2"].severity = "advisory"
RULES["MisraC++-14.5.1"].severity = "required"
RULES["MisraC++-14.5.2"].severity = "required"
RULES["MisraC++-14.5.3"].severity = "required"
RULES["MisraC++-14.6.1"].severity = "required"
RULES["MisraC++-14.6.2"].severity = "advisory"
RULES["MisraC++-14.7.2"].severity = "required"
RULES["MisraC++-14.7.3"].severity = "required"
RULES["MisraC++-14.8.1"].severity = "advisory"
RULES["MisraC++-14.8.2"].severity = "advisory"
RULES["MisraC++-15.0.1"].severity = "required"
RULES["MisraC++-15.0.2"].severity = "advisory"
RULES["MisraC++-15.0.3"].severity = "required"
RULES["MisraC++-15.1.1"].severity = "required"
RULES["MisraC++-15.1.2"].severity = "required"
RULES["MisraC++-15.1.3"].severity = "required"
RULES["MisraC++-15.3.1"].severity = "advisory"
RULES["MisraC++-15.3.2"].severity = "advisory"
RULES["MisraC++-15.3.3"].severity = "required"
RULES["MisraC++-15.3.4"].severity = "required"
RULES["MisraC++-15.3.5"].severity = "required"
RULES["MisraC++-15.3.6"].severity = "required"
RULES["MisraC++-15.3.7"].severity = "required"
RULES["MisraC++-15.4.1"].severity = "required"
RULES["MisraC++-15.5.1"].severity = "advisory"
RULES["MisraC++-15.5.2"].severity = "required"
RULES["MisraC++-15.5.3"].severity = "required"
RULES["MisraC++-16.0.3"].severity = "advisory"
RULES["MisraC++-16.0.5"].severity = "required"
RULES["MisraC++-16.0.6"].severity = "required"
RULES["MisraC++-16.0.7"].severity = "required"
RULES["MisraC++-16.0.8"].severity = "required"
RULES["MisraC++-16.1.1"].severity = "required"
RULES["MisraC++-16.1.2"].severity = "required"
RULES["MisraC++-16.2.3"].severity = "required"
RULES["MisraC++-16.2.4"].severity = "required"
RULES["MisraC++-16.2.5"].severity = "required"
RULES["MisraC++-16.2.6"].severity = "required"
RULES["MisraC++-16.3.1"].severity = "advisory"
RULES["MisraC++-16.3.2"].severity = "advisory"
RULES["MisraC++-16.6.1"].severity = "advisory"
RULES["MisraC++-17.0.1"].severity = "required"
RULES["MisraC++-17.0.2"].severity = "required"
RULES["MisraC++-17.0.3"].severity = "required"
RULES["MisraC++-17.0.5"].severity = "required"
RULES["MisraC++-18.0.1"].severity = "required"
RULES["MisraC++-18.0.2"].severity = "required"
RULES["MisraC++-18.0.3"].severity = "required"
RULES["MisraC++-18.0.4"].severity = "required"
RULES["MisraC++-18.0.5"].severity = "required"
RULES["MisraC++-18.2.1"].severity = "required"
RULES["MisraC++-18.7.1"].severity = "required"
RULES["MisraC++-19.3.1"].severity = "required"
RULES["MisraC++-27.0.1"].severity = "required"

RULES["MisraC++-0.1.8"].exclude_constructors = True
RULES["MisraC++-0.1.8"].exclude_virtual_destructors = True

RULES["MisraC2012-5.1"].maxlen = 63
# Set the maximum number of macros allowed in a translation unit in C99 to the value of CPP standard
RULES["MisraC2012-1.1"].limits[translation_limits.macros] = 65536
RULES["MisraC2012-1.1"].limits[translation_limits.chars_in_line] = 65536
RULES["MisraC2012-1.1"].limits[translation_limits.significant_chars_external] = 63
RULES["MisraC2012-1.1"].limits[translation_limits.nested_includes] = 256
RULES["MisraC2012-1.1"].limits[translation_limits.parenthesized_expressions] = 1024

# Rules not to be checked for common folder
RULES["MisraC2012-8.11"].excludes = set(["common/*"])
RULES["MisraC2012-2.3"].excludes = set(["common/*"])
RULES["MisraC2012-2.5"].excludes = set(["common/*"])
RULES["MisraC2012-8.12"].excludes = set(["common/*"])
RULES["MisraC2012-11.4"].excludes = set(["common/prog/MSP/*"])
RULES["MisraC2012-11.4"].report_at_macro_invocation = False

# Activate Accordant Rules
ACTIVE = []
My_Misra_System = [
    "MisraC2012-1.3",
    "MisraC2012-2.1",
    "MisraC2012-2.2",
    "MisraC2012-2.3",
    #    'MisraC2012-2.4', # this rule will not be followed (working group decision)
    "MisraC2012-2.5",
    "MisraC2012-5.1",
    "MisraC2012-5.6",
    "MisraC2012-5.7",
    "MisraC2012-5.8",
    "MisraC2012-5.9",
    "MisraC2012-8.3",
    "MisraC2012-8.5",
    "MisraC2012-8.6",
    "MisraC2012-8.7",
    "MisraC2012-8.9",
    "MisraC2012-8.13",
    "MisraC2012-9.1",  # no phyton file available
    "MisraC2012-12.2",
    "MisraC2012-13.1",
    "MisraC2012-13.2",
    #    'MisraC2012-13.5', # this rule will not be followed (working group decision)
    "MisraC2012-14.1",
    "MisraC2012-14.2",
    "MisraC2012-14.3",  # no phyton file available
    "MisraC2012-17.2",
    "MisraC2012-17.5",
    #    'MisraC2012-17.8', # this rule will not be followed (working group decision)
    "MisraC2012-18.1",  # no phyton file available
    "MisraC2012-18.2",  # no phyton file available
    "MisraC2012-18.3",  # no phyton file available
    "MisraC2012-18.6",  # no phyton file available
    "MisraC2012-19.1",
    "MisraC2012-21.14",
    "MisraC2012-21.16",
    "MisraC2012-21.19",
    "MisraC2012-22.1",  # no phyton file available
    "MisraC2012-22.2",  # no phyton file available
    "MisraC2012-22.3",  # no phyton file available
    "MisraC2012-22.4",  # no phyton file available
    "MisraC2012-22.5",
    "MisraC2012-22.6",  # no phyton file available
    "MisraC2012-22.7",
    "MisraC2012-22.8",
    "MisraC2012-22.9",
    "MisraC2012-22.10",
    "CertC-STR37",  # Configuring this rule for Rule 21.13 Amendment1
    "Generic-stdlibfunc",
]

My_Misra_Local = [
    "MisraC2012-1.1",
    "MisraC2012-1.2",
    "MisraC2012-2.6",
    "MisraC2012-2.7",
    "MisraC2012-3.1",
    "MisraC2012-3.2",
    "MisraC2012-4.1",
    "MisraC2012-4.2",
    "MisraC2012-5.2",
    "MisraC2012-5.3",
    "MisraC2012-5.4",
    "MisraC2012-5.5",
    "MisraC2012-6.1",
    "MisraC2012-6.2",
    "MisraC2012-7.1",
    "MisraC2012-7.2",
    "MisraC2012-7.3",
    "MisraC2012-7.4",
    "MisraC2012-8.1",
    "MisraC2012-8.2",
    "MisraC2012-8.4",
    "MisraC2012-8.8",
    "MisraC2012-8.10",
    "MisraC2012-8.11",
    "MisraC2012-8.12",
    "MisraC2012-8.14",
    "MisraC2012-9.2",
    "MisraC2012-9.3",
    "MisraC2012-9.4",
    "MisraC2012-9.5",
    "MisraC2012-10.1",
    "MisraC2012-10.2",
    "MisraC2012-10.3",
    "MisraC2012-10.4",
    "MisraC2012-10.5",
    "MisraC2012-10.6",
    "MisraC2012-10.7",
    "MisraC2012-10.8",
    "MisraC2012-11.1",
    "MisraC2012-11.2",
    "MisraC2012-11.3",
    "MisraC2012-11.4",
    "MisraC2012-11.5",
    "MisraC2012-11.6",
    "MisraC2012-11.7",
    "MisraC2012-11.8",
    "MisraC2012-11.9",
    "MisraC2012-12.1",
    "MisraC2012-12.3",
    "MisraC2012-12.4",
    "MisraC2012-12.5",
    "MisraC2012-13.3",
    "MisraC2012-13.4",
    "MisraC2012-13.6",
    #    'MisraC2012-14.4', # this rule will not be followed (working group decision)
    "MisraC2012-15.1",
    "MisraC2012-15.2",
    "MisraC2012-15.3",
    #    'MisraC2012-15.4', # this rule will not be followed (working group decision)
    #    'MisraC2012-15.5', # this rule will not be followed (working group decision)
    "MisraC2012-15.6",
    "MisraC2012-15.7",
    #    'MisraC2012-16.1', # this rule will not be followed (working group decision)
    "MisraC2012-16.2",
    "MisraC2012-16.3",
    "MisraC2012-16.4",
    "MisraC2012-16.5",
    #    'MisraC2012-16.6', # this rule will not be followed (working group decision)
    "MisraC2012-16.7",
    "MisraC2012-17.1",
    "MisraC2012-17.3",
    "MisraC2012-17.4",  # no phyton file available
    "MisraC2012-17.6",
    "MisraC2012-17.7",
    "MisraC2012-18.4",  # this rule will not be followed (working group decision)
    "MisraC2012-18.5",
    "MisraC2012-18.7",
    "MisraC2012-18.8",
    "MisraC2012-19.2",
    #    'MisraC2012-20.1', # this rule will not be followed (working group decision)
    "MisraC2012-20.2",
    "MisraC2012-20.3",
    "MisraC2012-20.4",
    "MisraC2012-20.5",
    "MisraC2012-20.6",
    "MisraC2012-20.7",
    "MisraC2012-20.8",
    "MisraC2012-20.9",  # no phyton file available
    "MisraC2012-20.10",
    "MisraC2012-20.11",
    "MisraC2012-20.12",
    "MisraC2012-20.13",  # no phyton file available
    "MisraC2012-20.14",  # no phyton file available
    "MisraC2012-21.1",
    "MisraC2012-21.2",
    "MisraC2012-21.3",
    "MisraC2012-21.4",
    "MisraC2012-21.5",
    "MisraC2012-21.6",
    "MisraC2012-21.7",
    "MisraC2012-21.8",
    "MisraC2012-21.9",
    "MisraC2012-21.10",
    "MisraC2012-21.11",
    "MisraC2012-21.12",
    "MisraC2012-21.15",
    # C++ Rules
    "MisraC++-0.1.1",
    "MisraC++-0.1.3",
    "MisraC++-0.1.4",
    "MisraC++-0.1.5",
    "MisraC++-0.1.6",
    "MisraC++-0.1.7",
    "MisraC++-0.1.8",
    "MisraC++-0.1.11",
    "MisraC++-0.1.12",
    "MisraC++-0.2.1",
    "MisraC++-0.3.1",
    "MisraC++-0.3.2",
    "MisraC++-0.4.2",
    "MisraC++-2.3.1",
    "MisraC++-2.5.1",
    "MisraC++-2.7.1",
    "MisraC++-2.7.2",
    "MisraC++-2.7.3",
    "MisraC++-2.10.1",
    "MisraC++-2.10.2",
    "MisraC++-2.10.3",
    "MisraC++-2.10.4",
    "MisraC++-2.10.5",
    "MisraC++-2.10.6",
    "MisraC++-2.13.1",
    "MisraC++-2.13.2",
    "MisraC++-2.13.3",
    "MisraC++-2.13.4",
    "MisraC++-2.13.5",
    "MisraC++-3.1.1",
    "MisraC++-3.1.2",
    "MisraC++-3.1.3",
    "MisraC++-3.2.1",
    "MisraC++-3.2.2",
    "MisraC++-3.2.3",
    "MisraC++-3.2.4",
    "MisraC++-3.3.1",
    "MisraC++-3.3.2",
    "MisraC++-3.4.1",
    "MisraC++-3.9.1",
    "MisraC++-3.9.2",
    "MisraC++-3.9.3",
    "MisraC++-4.5.1",
    "MisraC++-4.5.2",
    "MisraC++-4.5.3",
    "MisraC++-4.10.1",
    "MisraC++-4.10.2",
    "MisraC++-5.0.1",
    "MisraC++-5.0.2",
    "MisraC++-5.0.3",
    "MisraC++-5.0.4",
    "MisraC++-5.0.5",
    "MisraC++-5.0.6",
    "MisraC++-5.0.7",
    "MisraC++-5.0.8",
    "MisraC++-5.0.9",
    "MisraC++-5.0.10",
    "MisraC++-5.0.11",
    "MisraC++-5.0.12",
    "MisraC++-5.0.14",
    "MisraC++-5.0.15",
    "MisraC++-5.0.16",
    "MisraC++-5.0.17",
    "MisraC++-5.0.18",
    "MisraC++-5.0.19",
    "MisraC++-5.0.20",
    "MisraC++-5.0.21",
    "MisraC++-5.2.1",
    "MisraC++-5.2.2",
    "MisraC++-5.2.3",
    "MisraC++-5.2.4",
    "MisraC++-5.2.5",
    "MisraC++-5.2.6",
    "MisraC++-5.2.7",
    "MisraC++-5.2.8",
    "MisraC++-5.2.9",
    "MisraC++-5.2.10",
    "MisraC++-5.2.11",
    "MisraC++-5.2.12",
    "MisraC++-5.3.1",
    "MisraC++-5.3.2",
    "MisraC++-5.3.3",
    "MisraC++-5.3.4",
    "MisraC++-5.8.1",
    "MisraC++-5.14.1",
    "MisraC++-5.17.1",
    "MisraC++-5.18.1",
    "MisraC++-5.19.1",
    "MisraC++-6.2.1",
    "MisraC++-6.2.2",
    "MisraC++-6.2.3",
    "MisraC++-6.3.1",
    "MisraC++-6.4.1",
    "MisraC++-6.4.4",
    "MisraC++-6.4.5",
    "MisraC++-6.4.6",
    "MisraC++-6.4.7",
    "MisraC++-6.4.8",
    "MisraC++-6.5.1",
    "MisraC++-6.5.2",
    "MisraC++-6.5.3",
    "MisraC++-6.5.4",
    "MisraC++-6.6.1",
    "MisraC++-6.6.2",
    "MisraC++-7.1.1",
    "MisraC++-7.1.2",
    "MisraC++-7.2.1",
    "MisraC++-7.3.1",
    "MisraC++-7.3.2",
    "MisraC++-7.3.3",
    "MisraC++-7.3.5",
    "MisraC++-7.3.6",
    "MisraC++-7.4.1",
    "MisraC++-7.4.2",
    "MisraC++-7.4.3",
    "MisraC++-7.5.1",
    "MisraC++-7.5.2",
    "MisraC++-7.5.3",
    "MisraC++-7.5.4",
    "MisraC++-8.3.1",
    "MisraC++-8.4.1",
    "MisraC++-8.4.2",
    "MisraC++-8.4.3",
    "MisraC++-8.4.4",
    "MisraC++-8.5.1",
    "MisraC++-8.5.2",
    "MisraC++-8.5.3",
    #'MisraC++-9.3.1',
    "MisraC++-9.3.2",
    "MisraC++-9.3.3",
    "MisraC++-9.5.1",
    "MisraC++-9.6.1",
    "MisraC++-9.6.2",
    "MisraC++-9.6.3",
    "MisraC++-9.6.4",
    "MisraC++-10.1.2",
    "MisraC++-10.1.3",
    "MisraC++-10.2.1",
    "MisraC++-10.3.1",
    "MisraC++-10.3.2",
    "MisraC++-10.3.3",
    "MisraC++-11.0.1",
    "MisraC++-12.1.1",
    "MisraC++-12.1.3",
    "MisraC++-12.8.1",
    "MisraC++-12.8.2",
    "MisraC++-14.5.1",
    "MisraC++-14.5.2",
    "MisraC++-14.5.3",
    "MisraC++-14.6.1",
    "MisraC++-14.6.2",
    "MisraC++-14.7.2",
    "MisraC++-14.7.3",
    "MisraC++-14.8.1",
    "MisraC++-14.8.2",
    "MisraC++-15.0.1",
    "MisraC++-15.0.2",
    "MisraC++-15.0.3",
    "MisraC++-15.1.1",
    "MisraC++-15.1.2",
    "MisraC++-15.1.3",
    "MisraC++-15.3.1",
    "MisraC++-15.3.2",
    "MisraC++-15.3.3",
    "MisraC++-15.3.4",
    "MisraC++-15.3.5",
    "MisraC++-15.3.6",
    "MisraC++-15.3.7",
    "MisraC++-15.4.1",
    "MisraC++-15.5.1",
    "MisraC++-15.5.2",
    "MisraC++-15.5.3",
    "MisraC++-16.0.3",
    "MisraC++-16.0.5",
    "MisraC++-16.0.6",
    "MisraC++-16.0.7",
    "MisraC++-16.0.8",
    "MisraC++-16.1.1",
    "MisraC++-16.1.2",
    "MisraC++-16.2.3",
    "MisraC++-16.2.4",
    "MisraC++-16.2.5",
    "MisraC++-16.2.6",
    "MisraC++-16.3.1",
    "MisraC++-16.3.2",
    "MisraC++-16.6.1",
    "MisraC++-17.0.1",
    "MisraC++-17.0.2",
    "MisraC++-17.0.3",
    "MisraC++-17.0.5",
    "MisraC++-18.0.1",
    "MisraC++-18.0.2",
    "MisraC++-18.0.3",
    "MisraC++-18.0.4",
    "MisraC++-18.0.5",
    "MisraC++-18.2.1",
    "MisraC++-18.7.1",
    "MisraC++-19.3.1",
    "MisraC++-27.0.1",
]

BSHRule_Local = [
    "MisraC-8.5",  # configured for rule - No function or variable definition in header
    "Generic-NoMagicNumbers",
    "Generic-NoInlineFunctionInHeader",
    "Generic-NoInlineFunctionInSource",
    "Generic-TerminateElifWithElse",
    "Generic-DoNotIncludeCfiles",
    "Generic-DoNotRenameFuncOrVar",
]


BSHNaming_Convention = [
    #'Generic-BSH_HungarianNotation_NamingRule',
    "Generic-BSH_Variable_NamingRule",
    "Generic-BSH_Enumerator_NamingRule",
    "Generic-BSH_FolderAndFile_NamingRule",
    "Generic-BSH_Function_NamingRule",
    "Generic-BSH_ISR_Function_NamingRule",
    "Generic-BSH_MACRO_NamingRule",
    "Generic-BSH_MacroWithFunction_NamingRule",
    "Generic-BSH_Structure_NamingRule",
    "Generic-BSH_Typedef_NamingRule",
    "Generic-BSH_Union_NamingRule",
]


if "AXIVION_SYSTEM_STYLECHECK" in os.environ:
    if os.environ["AXIVION_SYSTEM_STYLECHECK"] == "True":
        ACTIVE.extend(My_Misra_System)

if "AXIVION_TRANSLUNIT_STYLECHECK" in os.environ:
    if os.environ["AXIVION_TRANSLUNIT_STYLECHECK"] == "True":
        ACTIVE.extend(My_Misra_Local)
#       ACTIVE.extend(BSHRule_Local) --> disabling these rules temporarily

if "AXIVION_NAMINGCONV_STYLECHECK" in os.environ:
    if os.environ["AXIVION_NAMINGCONV_STYLECHECK"] == "True":
        ACTIVE.extend(BSHNaming_Convention)

# Exclusively for axanalysis target
if not ACTIVE:
    ACTIVE.extend(My_Misra_System)
    ACTIVE.extend(My_Misra_Local)
#   ACTIVE.extend(BSHRule_Local) --> disabling these rules temporarily

for rulename, rule in RULES.items():
    if rulename.startswith("MisraC++"):
        rule.languages = [
            "C++",
        ]
    if rulename.startswith("MisraC2012"):
        rule.langauges = [
            "C",
        ]