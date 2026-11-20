from bauhaus.ir.common.languages import translation_limits

CONFIG.Propagate_Metrics = True
CONFIG.Report_Propagated_Metric_Values = "root"
CONFIG.Report_All_Metric_Values = True

MetricMessage.fmt = (
    "Axivion: %(filename)s(%(line)d):%(column)d: %(disabled)s%(metric_name)s, %(display_name)s: "
    + "%(metric_value)f; MIN: %(min_value)f;  MAX: %(max_value)f; %(violation)s: %(entity_type)s: %(entity)s"
)
# See the AXIVION style check guide

ACTIVE = [
    # metrics for assessing the size of c/c++ artefacts
    "Metric-Lines.Code",
    "Metric-NPath",
    "Metric-Maximum_Extended_Nesting",
    "Metric-Bosch.BMI",
    "Metric-McCabe_Complexity",
    "Metric-HIS.STMT",
    "Metric-Lines.File.Code",
]

RULES["Metric-Lines.Code"].display_name = "STST3"
RULES["Metric-NPath"].display_name = "STPTH"
RULES["Metric-Maximum_Extended_Nesting"].display_name = "STMIF"
RULES["Metric-McCabe_Complexity"].display_name = "COMPLEXITY"
RULES["Metric-HIS.STMT"].display_name = "STATEMENTS"
RULES["Metric-Lines.File.Code"].display_name = "LINES_IN_FILE"

RULES["Metric-Lines.Code"].max_value = 1
RULES["Metric-NPath"].max_value = 1
RULES["Metric-Maximum_Extended_Nesting"].max_value = 1
RULES["Metric-Bosch.BMI"].min_value = 40.0
RULES["Metric-McCabe_Complexity"].max_value = 16
RULES["Metric-Lines.File.Code"].max_value = 800
