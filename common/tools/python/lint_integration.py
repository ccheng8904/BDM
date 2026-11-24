import glob
import sys
import os

axivion_path_out = sys.argv[1]
lint_report_search_path = os.path.join(axivion_path_out, "..", "prog")
variant = sys.argv[2]
lint_report = os.path.join(axivion_path_out, variant + ".lin")


with open(lint_report, "w") as f:

    lfiles = glob.glob(lint_report_search_path + "/**/*.lin", recursive=True)
    for tempfile in lfiles:
        with open(tempfile, "r") as g:
            f.write(g.read())
