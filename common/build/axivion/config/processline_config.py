from processline_tools.architecture import eaxmi11_to_rfg
from processline_tools.architecture import architecture_check

import os
import transform_architecture
import create_mapping

try:
    XMI_FILE = XMI_FILE
except:
    print("XMI_FILE inp2ut not given!")

RAW_ARCHITECTURE_VIEW = "EA Architecture"
ARCHITECTURE_VIEW = "Architecture"


PROCESSLINE = [
    # import EA model
    (
        eaxmi11_to_rfg.perform,
        {"files": [XMI_FILE], "architecture_view": RAW_ARCHITECTURE_VIEW},
    ),
    # transform view structure:
    # add cycles for components
    # transform interfaces
    (
        transform_architecture.perform,
        {"source_view": RAW_ARCHITECTURE_VIEW, "target_view": ARCHITECTURE_VIEW},
    ),
    (
        create_mapping.perform,
        {
            "architecture_view": ARCHITECTURE_VIEW,
            "hierarchy_view": "File",
            "mapping_view": "Mapping",
        },
    ),
    (
        architecture_check.perform,
        {
            "architecture_view": ARCHITECTURE_VIEW,
            "hierarchy_view": "File",
            "mapping_view": "Mapping",
            "base_view": "Declaration Facts",
        },
    ),
]
