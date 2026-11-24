# -*- coding: utf-8 -*-
import math
import os
import webbrowser
from operator import itemgetter

list_getinitial = []
list_final = []


# this function will create two lists out of the mstc reports
def create_list(input_file):
    for line in input_file:
        line_initial = []
        line_get = []
        line = line.replace(":", ",")
        line = line.replace(";", ",")
        line = line.split(",")
        if (
            line[9] != " ?\n"
            and (line[4] == " STST3" or line[4] == " STPTH" or line[4] == " STMIF")
            and (".c" in line[0])
        ):
            list_initial = [line[0], line[9], line[4], line[5]]
            list_get = [line[0], line[9]]
            list_getinitial.append(list_initial)
            list_final.append(list_get)


# this function would remove duplicate file path and file names
def remove_duplicates(list_final):
    for i in range(len(list_final)):
        c = 0
        for j in range(len(list_final)):
            if (
                list_final[i][0] == list_final[j][0]
                and list_final[i][1] == list_final[j][1]
            ):
                c = c + 1
        if c > 1:
            list_final[i][0] = ""
            list_final[i][1] = ""


# this function would calculate the BMI for each of the function name
def calculate_BMI(list_final):
    met = [" STST3", " STMIF", " STPTH"]

    for i in range(len(list_final)):
        MF1 = MF2 = MF3 = 0
        for j in range(len(list_getinitial)):
            if (
                list_final[i][0] == list_getinitial[j][0]
                and list_final[i][1] == list_getinitial[j][1]
                and list_getinitial[j][2] == met[0]
            ):
                MF1 = 0.11 * float((list_getinitial[j][3]))
                list_final[i].append(list_getinitial[j][3])
            if (
                list_final[i][0] == list_getinitial[j][0]
                and list_final[i][1] == list_getinitial[j][1]
                and list_getinitial[j][2] == met[1]
            ):
                MF2 = 0.13 * float((list_getinitial[j][3]))
                list_final[i].append(list_getinitial[j][3])
            if (
                list_final[i][0] == list_getinitial[j][0]
                and list_final[i][1] == list_getinitial[j][1]
                and list_getinitial[j][2] == met[2]
            ):
                MF3 = float(list_getinitial[j][3])
                MF3 = 4.31 * math.log(MF3, 2)
                list_final[i].append(list_getinitial[j][3])

        BMI_value = 93.06 - MF1 - MF2 - MF3
        if MF1 == 0:
            list_final[i].append(MF1)
        if MF2 == 0:
            list_final[i].append(MF2)
        if MF3 == 0:
            list_final[i].append(MF3)

        list_final[i].append(BMI_value)
    list_final.sort(key=itemgetter(5))


# this function would generate the html file
def generate_html():
    framework_out_html = os.path.join(framework_out_path, "BMI.html")
    html_file = open(framework_out_html, "w")
    css_path = os.path.abspath(
        os.path.realpath(os.path.join(framework_project_path, framework_tool_path))
    )
    css_path = css_path + "\\" + "mixed" + "\\" + "BMI_styles.css"
    html_string = (
        '<!DOCTYPE html> <html> <head> <link rel="stylesheet" type="text/css"  '
        "href=%s> <title>BMI</title></head> "
        '<div class="split left"><div class="centered"><table  bgcolor="#DCDCDC"> <tr><th>FILEPATH</th><TH>'
        "FUNCTION NAME</TH><TH>STST3</TH><TH>STMIF</TH><TH>STPTH</TH><TH>BMI</TH></tr>"
        % (css_path)
    )
    for i in range(len(list_final)):
        if list_final[i][2] != 0 or list_final[i][3] != 0 or list_final[i][4] != 0:
            html_string = html_string + "<tr>"
            html_string = html_string + '<td id="first">' + list_final[i][0] + "</td> "
            html_string = html_string + '<td id="second">' + list_final[i][1] + "</td>"
            html_string = (
                html_string + '<td id="third">' + str(list_final[i][2]) + "</td>"
            )
            html_string = (
                html_string + '<td id="fourth">' + str(list_final[i][3]) + "</td>"
            )
            html_string = (
                html_string + '<td id="fifth">' + str(list_final[i][4]) + "</td>"
            )
            html_string = (
                html_string + '<td id="sixth">' + str(list_final[i][5]) + "</td>"
            )
            html_string = html_string + "</tr>"

    html_string = (
        html_string + "</table>" + '</div></div> <br><br><div class="split right"> '
        ' <div class="centered"><div id="box">'
        "BMI FORMULA== 93.06 –0.11*STST3 –0.13*STMIF –4.31*log2(STPTH)"
        "</div></div></div>"
    )
    html_string = html_string + "</table>"
    html_string = html_string + "</html>"
    html_file.write(html_string)
    html_file.close()

    webbrowser.open_new_tab(framework_out_html)


if __name__ == "__main__":
    framework_out_path = os.environ["AXIVION_PATH_OUT"]
    framework_variant_name = os.environ["AXIVION_VARIANT_NAME"]
    framework_tool_path = os.environ["TOOL_PATH"]
    framework_project_path = os.environ["AXIVION_PROJECT_PATH"]
    framework_mstc = os.path.join(framework_out_path, framework_variant_name)
    framework_mstc = framework_mstc + ".metric"
    open_file = open(framework_mstc, "r")
    create_list(open_file)
    remove_duplicates(list_final)
    calculate_BMI(list_final)
    open_file.close()
    generate_html()
