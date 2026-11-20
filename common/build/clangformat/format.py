"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : Format.py
Date of creation : 02.02.2021
Author Name/Dept : UAT Team, RBEI
Organization     :
Description      : This Python script supports the format make-target.
"""

import os
import sys
import argparse
import subprocess
import shutil
import re


class ClangFormat:
    """Main class to perform all our operations"""

    def __init__(self):
        self.platform_name = ""
        self.filename = ""
        self.no_code_analysis = ""
        self.format_tool = ""
        self.search_path = ""

    def valid_file(self):
        """
        Checks if parsed file is valid or not
        Raises:
            argparse.ArgumentTypeError: This error must be rasied if filename does not
            end with .c or .h extension
        """
        _, extension = os.path.splitext(self.filename)
        if extension.lower() not in (".c", ".h", ".cpp", ".hpp", ".cc"):
            raise argparse.ArgumentTypeError(
                "{} : File type must be .c or .h or .cpp or .hpp or .cc".format(
                    self.filename
                )
            )
        else:
            pass

    @classmethod
    def manage_arg_parse_format(cls):
        """
        This method creates a parser object and also the necessary arguments are defined.
        Return:
        A simple Namespace object will be built up from attributes parsed out of the command line
        """
        parser = argparse.ArgumentParser()
        parser.add_argument(
            "--filename",
            help="Locate the file in the project",
            action="store",
            dest="filename",
            default=None,
        )
        parser.add_argument(
            "--no_code_analysis",
            help="Get the no_code_analysis variable",
            nargs="+",
            action="store",
            dest="no_code_analysis",
            default=[""],
        )
        parser.add_argument(
            "--platform_name",
            help="Get the platform name",
            action="store",
            dest="platform_name",
            default=None,
        )
        parser.add_argument(
            "--format_tool",
            help="Locate the clang format tool ",
            action="store",
            dest="format_tool",
            default=None,
        )
        parser.add_argument(
            "--formatsearchpath",
            help="Get the location of all source files ",
            action="store",
            dest="search_path",
            default=None,
        )
        args = parser.parse_args()
        return args

    def set_manage_args(self):
        """
        This method calls the manage_arg_parse_format() function.
        """
        args = self.manage_arg_parse_format()
        self.filename = args.filename
        self.no_code_analysis = args.no_code_analysis
        self.platform_name = args.platform_name
        self.format_tool = args.format_tool
        self.search_path = args.search_path

    def single_file_formatting(self):
        """
        This method gets the path of the file entered by the user and if there are multiple
        occurences of the file , it returns a list containing all the occurences.
        """
        self.valid_file()
        final_list = []
        paths_list = [
            os.path.join(root, self.filename)
            for root, dirs, files in os.walk(self.search_path)
            if self.filename in files
        ]

        for element in paths_list:
            if self.platform_name in element and "stub" not in element:
                return [element]
            else:
                final_list.append(element)
        if not final_list:
            sys.stdout.write(
                "\nNote : Check for the existence of source file {}\n".format(
                    self.filename
                )
            )
            sys.exit(0)
        return final_list

    def full_format(self):
        """
        This method provides a list containing all the source files including
        the platform files present in the project.
        """
        all_paths_list = []
        finallist = []
        platform_list = [
            "efr32xg21",
            "mke14z",
            "mkv5x",
            "psoc4",
            "rx100",
            "rx200",
            "stm32f0",
            "stm32f1",
            "stm32f3",
            "stm32f4",
            "stm32g0",
            "stm32g4",
            "stm32l4",
            "Rx",
            "psoc_creator",
        ]
        for root, _, files in os.walk(self.search_path):
            for file_name in files:
                if file_name.endswith(tuple([".c", ".h"])) and "stub" not in root.lower():
                    full_path = os.path.join(root, file_name)
                    all_paths_list.append(full_path)
        for paths in all_paths_list:
            if any(platform in platform_list for platform in re.split("/|\\\\", paths)):
                if self.platform_name in paths:
                    finallist.append(paths)
            else:
                finallist.append(paths)

        list_no_code_analysis = list(map((self.search_path+"\\").__add__, self.no_code_analysis))
        for i in range(len(list_no_code_analysis)):
            finallist = [x for x in finallist if not x.startswith(list_no_code_analysis[i])]
        return finallist

    def run_format(self):
        """
        This method executes the command for the clang and also creates the directory
        required to store the output of the execution.
        """
        # To copy the file from build/clang_format to common
        config_file = os.path.dirname(os.path.realpath(__file__)) + r"\.clang-format"
        shutil.copy(config_file, self.search_path)

        if self.filename == "NONE":
            format_file_path = self.full_format()
            print("\nFormatting all the files in project")
            print("\nIt will take 2-5 minutes to complete the formatting. Please Wait")
        else:
            format_file_path = self.single_file_formatting()
            print("\nFormatting for: {}".format(self.filename))
        for files in list(format_file_path):
            format_cmd = self.format_tool + " " + files
            out_path = files + ".format"
            with open(out_path, "w") as log:
                subprocess.run(
                    format_cmd, stdout=log, stderr=subprocess.PIPE, shell=False
                )

        # To remove config file from common\prog after formatting
        config_filename = r".clang-format"
        fullname = os.path.join(self.search_path, config_filename)
        try:
            os.remove(fullname)
        except:
            print("\n Config File doesn't exist to remove")


if __name__ == "__main__":
    FORMATCLASS = ClangFormat()
    FORMATCLASS.set_manage_args()
    FORMATCLASS.run_format()
