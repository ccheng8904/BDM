# ======================================================================================================================
# File Name       : ReportSplitter.py
# Date of creation: 29th Aug 2018
# Author Name/Dept: Siddharth Trivedi
# Organization    : RBEI
# Description     : The script splits the project based Axivion static analysis report to source file based
#                   static analysis report.
# Python Version  : 3.5.2
# File Version    : 1.1
# Modified on     : 4rth Oct 2018
# Modified by     : Siddharth Trivedi
# Modification Description: This version generates the source violation report identical to the way of Axivion message
#                   reporting. This version is implemented in class based mechanism.
# ======================================================================================================================

import optparse
import os
import sys
import ntpath


class AxivionReadSource(object):
    def __init__(self, arg_d):
        self.arg_d = arg_d

    def get_includes_in_source(self):
        """This method uses Axivion library to get all the included file in source files
        To execute this method the script must be executed through rfgscript.exe
        :rtype: dict
        """
        from bauhaus import ir

        data_d = dict()
        includes_d = dict()
        node_count = 0
        try:
            my_graph = ir.Graph(self.arg_d["i"])
        except:
            sys.stderr.write(
                "ReportSplitter Error : Wrong file or file path given with -i option in command line argument\n"
            )
            sys.exit()
        try:
            for node in my_graph.nodes_of_type(
                ir.Physical, "Plain_User_Include_Declaration"
            ):
                if node:
                    node_count += 1
                    includes_d.update({node_count: {node.Parent.Name: [node.Name]}})
            for key, value in includes_d.items():
                if value.keys()[0] in data_d:
                    data_d[value.keys()[0]] += value.values()[0]
                else:
                    data_d[value.keys()[0]] = value.values()[0]
            include_data_d = {
                ntpath.basename(key): list(set(value)) for key, value in data_d.items()
            }
            return {"res": "P", "data": include_data_d}
        except:
            return {
                "res": "F",
                "ed": "ReportSplitter Error : Did not execute 'get_includes_in_source' successfully. "
                "Returning from AxivionReadSource class",
            }


class ArgumentParser(object):
    @staticmethod
    def get_arguments():
        """The method parses the command line argument and provide the report format and report path
        :rtype: dict
        """
        parser = optparse.OptionParser()
        parser.add_option(
            "-r",
            "--reporttype",
            action="store",
            type="string",
            dest="report_type",
            help="Provide report format with -r or --reporttype option",
        )
        parser.add_option(
            "-p",
            "--reportname",
            action="store",
            type="string",
            dest="report_path",
            help="Provide project report name along with path with -p or --reportname option",
        )
        parser.add_option(
            "-i",
            "--ir",
            action="store",
            type="string",
            dest="ir_path",
            help="Provide ir file name along with path with -i or --ir option",
        )
        (options, args) = parser.parse_args()
        return {
            "path": options.report_path,
            "format": options.report_type,
            "i": options.ir_path,
        }


class Splitter(object):
    def __init__(self, arg_d, includes_d):
        self.arg_d = arg_d
        self.includes_d = includes_d

    @staticmethod
    def valid_message_format(line):
        """checks if the report has valid message Format.
            Note: Update the function accordingly if the violation message format gets updated for Axivion
        :type line: str
        :rtype: bool
        """
        import re

        axivion_message_format = (
            "Axivion: +((.*\.c)|(.*\.h)|(.*\.cpp)|(.*\.hpp))\s+[0-9]+\s+[0-9]+\s+.*"
        )
        match_obj = re.match(axivion_message_format, line, re.M | re.I)
        if match_obj:
            return {"match": True, "match_group": match_obj.group(1)}
        else:
            return {"match": False}

    def parse_filename_from_line(self, line):
        """Parses filename from the line from report.
        :type line: str
        :rtype: str
        """
        status = self.valid_message_format(line)
        if status["match"]:
            return ntpath.basename(status["match_group"].split()[0])
        else:
            return None

    def project_report_reader(self):
        """The method reads the project report file and returns line on demand
        :rtype: str
        """
        try:
            project_report = open(self.arg_d["path"], "r")
        except:
            sys.stderr.write(
                "ReportSplitter Error : Wrong file or file path given with -p option in argument\n"
            )
            sys.exit()
        for line in project_report:
            if self.valid_message_format(line)["match"]:
                yield line
            else:
                sys.stderr.write(
                    "ReportSplitter Error : "
                    "Axivion violation message format has been changed in 'stylecheck_config.py."
                    "ReportSplitter.py needs to be update\n"
                )
                sys.exit

    @staticmethod
    def get_include_tree(file_name, source_files_d):
        """The methods is to get includes inside included files
        :type file_name: str
        :type source_files: dict
        :rtype: list
        """
        import copy

        base_includes_l = source_files_d[file_name]
        processed_files = list()
        permit = True

        def update_list(processed_files, includes_l):
            files_l = copy.deepcopy(includes_l)
            for file in includes_l:
                if (file in source_files_d) and (file not in processed_files):
                    files_l.extend(source_files_d[file])
                processed_files.extend([file])
            return list(set(processed_files)), list(set(files_l))

        while permit:
            list_1, list_2 = update_list(processed_files, base_includes_l)
            base_includes_l = list_2
            processed_files.extend(list_1)
            list_1.sort()
            list_2.sort()
            if list_1 == list_2:
                permit = False
        base_includes_l.extend([file_name])
        return base_includes_l

    def arrange_message(self):
        """The method arranges data in form of file name and contains and provides in tuple form
        :rtype: tuple
        """
        # Create a dictionary in form of {sourcefile_name1: [ ], .., sourcefile_nameN: []}
        source_and_message = {
            key: [] for key, value in self.includes_d.items() if key.endswith(".c")
        }
        # Updates data_dict and yields filename and message in tuple format as (source_name1, [message1, .., messageN])
        for file_name, message in source_and_message.items():
            for line in self.project_report_reader():
                if file_name == self.parse_filename_from_line(
                    line
                ) or self.parse_filename_from_line(line) in self.get_include_tree(
                    file_name, self.includes_d
                ):
                    source_and_message[file_name].append(line)
            yield file_name, message

    def source_report_generator(self):
        """Creates the sourcefile if file not exists and write the message if message is not exists in file."""
        # Put the relevant data in it's respective source file
        for data in self.arrange_message():
            file_name = (
                os.path.dirname(self.arg_d["path"])
                + "\\"
                + data[0].split(".")[0]
                + "."
                + self.arg_d["format"]
            )
            if os.path.isfile(file_name):
                with open(file_name, "r+") as source_file:
                    if list(set(source_file).symmetric_difference(set(data[1]))) != []:
                        source_file.seek(0)
                        source_file.truncate()
                        for message in data[1]:
                            source_file.write(message)
            else:
                with open(file_name, "w+") as source_file:
                    for message in data[1]:
                        source_file.write(message)
        os.remove(self.arg_d["path"])


if __name__ == "__main__":
    arg_o = ArgumentParser.get_arguments()
    include_info_d = AxivionReadSource(arg_o).get_includes_in_source()
    if include_info_d["res"] == "P":
        splitter_o = Splitter(arg_o, include_info_d["data"])
        splitter_o.source_report_generator()
    else:
        sys.stderr.write(include_info_d["ed"])
        sys.exit()
