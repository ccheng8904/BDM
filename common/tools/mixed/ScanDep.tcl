#*******************************************************************************
#  Copyright (c) 2017 BSH Hausgeraete GmbH,
#  Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de
#
#  All rights reserved. This program and the accompanying materials
#  are protected by international copyright laws.
#  Please contact copyright holder for licensing information.
#
#*******************************************************************************
#  Project          PP PED FW
#*******************************************************************************
#   DESCRIPTION
#   This tool filters the created dependency list.
#   It is able to get all header files in specified folders.
#   This is useful to get a header list for doxygen.
#   Normally doxygen documents all files in specified folders
#   If no search folder is defined than all headers are extracted
#   usage: Scandep -fdepfile -dfolder1 -dfolder2
#*******************************************************************************


#---------- procedure ToolInfo ----------------------------------
#  Just prints the usage of this tool
proc ToolInfo {} {
  puts "usage: Scandep -fdepfile -dfolder1 -dfolder2\n"

  puts "Example:\n"
  puts "    Scandep -ffvPanel.dep -dR:\Fv\Sw\Work\Howu -d.\\"
}
#----------------------------------------------------------------


#---------- Main procedure --------------------------------------

#--------- Scanning the commandline
foreach argument $argv {

    if {[scan $argument "-f%s" InFileName]==1} {
     # read in dpendency file if -f option
        set file [open $InFileName];
        set DepList [read $file]
        close $file
    } elseif {[scan $argument "-d%s" Folder]==1} {
     # add to dependency folder list if -d option
        lappend FolderList [file normalize $Folder]

    } else {ToolInfo; exit}


}

#------- call toolinfo if no -d or -f option was specified
if {(![info exists DepList])} {ToolInfo; exit}


regsub -all {[\\]} $DepList "/" DepList;  #replace all '\' with '/'

# filter all headers from dependency file and put them with normalized name in a list
set HeaderList ""
foreach depItem $DepList {
     if {[string match "*.h" $depItem]==1} {
         set Header [file normalize $depItem]
         if {(![info exists FolderList])} {
            lappend HeaderList $Header
         } else {
             if {([lsearch $FolderList [file dirname $Header]]!=-1) && ([lsearch $HeaderList $Header]==-1)} {
                lappend HeaderList $Header
             }
         }
     }
}


puts [lsort -unique -dictionary $HeaderList]
