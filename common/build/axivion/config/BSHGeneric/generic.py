#  Axivion Bauhaus Suite
#  http://www.axivion.com/
#  Copyright (C) Axivion GmbH, 2010

from bauhaus import style


class GenericRule(style.rule.RulesetRule):
    severity = "required"
    rulegroup = "Generic"
    languages = ["C", "C++", "C#"]
