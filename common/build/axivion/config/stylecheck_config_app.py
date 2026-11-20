import os
from bauhaus.ir.common.languages import translation_limits


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

# RULES["MisraC2012Directive-4.1"].severity = "required"
# RULES["MisraC2012Directive-4.1"].execute_checks_covered_in_other_rules = True

RULES["MisraC2012-22.1"].severity = "required"

# Activate Accordant Rules
ACTIVE = [
    # "MisraC2012Directive-4.1",
    "MisraC2012-22.1",
]

for rulename, rule in RULES.items():
    if rulename.startswith("MisraC++"):
        rule.languages = [
            "C++",
        ]
    if rulename.startswith("MisraC"):
        rule.langauges = [
            "C",
        ]
        rule.individual_excludes.add("app/*/prog/MSP/*")
        rule.individual_excludes.add("app/*/prog/dbus/*")
        rule.individual_excludes.add("app/*/prog/ped_fw/*")
        rule.individual_excludes.add("app/*/prog/firmware_update/*")
        rule.individual_excludes.add("common/*")
