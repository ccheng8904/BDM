from pteng.public_layer import *
from pteng.globals.constants import exec_const
from argparse import ArgumentParser
import logging
import sys

class FailedException(Exception):
    def __init__(self, errlvl):
        self.errorlevel = errlvl

gLogger = logging.getLogger("global")


argP = ArgumentParser(description='Arguments for automated testexecution.')
argP.add_argument("-testfile", dest="testfile", nargs="*", required = False, help="desired list of testfile to execute")
argP.add_argument("-readjson", dest="json", nargs="*", required = False, help="read variales in json file and verify")
argP.add_argument("-prj", dest="prj", required = False, help="projects name")
args = argP.parse_args()

default_comconfig = r"<$COM_CFG_filepath>"
pstd_filepath = r"<$PSTD_FILEPATH>" 
psyd_filepath = r"<$PSYD_FILEPATH>"
cvrli_filepath = r"<$CVRLI_FILEPATH>"

gLogger.log(70, "The used instance of TargetDscr() is named \'target1\', so you can use this instance for further commands.")
target1 = TargetDscr()
gLogger.log(70, "Started import of PSTD and PSYD.")
target1.import_pstd(pstd_filepath)
target1.append_psyd(psyd_filepath)
target1.set_comconfig_from_file(default_comconfig)
target1.set_cvrli(cvrli_filepath)
target1.connect()

result_list = list()

if args.testfile:
    for file in args.testfile:
        result = target1.execute(file, project=args.prj)
        result_list.append(result)

            
if args.json:
    for vjson in args.json:
        target1.read_out_json(vjson)
        
target1.disconnect()
    
if exec_const.exception_int in result_list:
    print("PTEng:TestCaseException")
    #raise FailedException(2)
    sys.exit(2)
elif exec_const.failed_int in result_list:
    print("PTEng:TestCaseFailed")
    #raise FailedException(1)
    sys.exit(1)
else:
    print("PTEng:TestCasePassed")
    print("Exiting with 0")
    sys.exit(0)