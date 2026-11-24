#*****************************************************************************************************************
# Copyright by BSH Bosch und Siemens Hausgeraete GmbH - EDS
#
#-----------------------------------------------------------------------------------------------------------------
#
# Use, reproduction and dissemination of this software or parts of it, is not permitted without express
# written authority of BSH EDS. The user is allowed to use this software exclusively for the development
# of electronic boards for BSH. Usage for other purposes is strictly prohibited (e.g. development of
# electronic boards for third parties). All rights, including copyright, rights created by patent grant
# or registration, and rights by protection of utility patents, are reserved. Violations will be
# prosecuted by civil and criminal law.
#
# The software was created and approved by acknowledged rules of technology. Because of the complexity
# of embedded controller software the user of this software has to perform his own tests to ensure
# proper functionality in his environment. The software was developed for usage as a software library.
# The user is sole responsible for every other usage.  The user may modify the software for adaptations
# needed for other microcontrollers, as well as to another compilers Metrowerks at own risk. 
# BSH will not support any adaptations or changes. BSH is authorised to use all adaptations for 
# own purposes free of charge.
#
# BSH assumes no liability for the functionality or reliability of the software even in concrete
# applications. BSH assumes no liability for consequential damages, except in case of intention or gross
# negligence. In any case the liability is limited to the typical and predictable damage.
#
# Technical changes are reserved.
#
#*****************************************************************************************************************
#   PROJECT          SW_LIB
#   FILE             %PM%
#   ARCHIVE          %PP%:%PI%
#   Interpreter      Tested with Tcl8.4
#*****************************************************************************************************************
#   AUTHOR           lehmannje
#   CREATED          2006-11-15
#   LAST CHANGE      %PRT%
#   LAST REVISION    %PR%
#   STATUS           %PS%
#*****************************************************************************************************************
#   DESCRIPTION                                                                                                   
#   This tool creates the document refman.pdf from refman.tex using pdflatex and makeindex
#   If required pdflatex is called up to 5 times to solve all open references
#   If there are any problems with createing the pdf please have look in the log file pdflatex.log
#   usage: pdflatex
#*****************************************************************************************************************
#   CHANGES
#   %PL%
#*****************************************************************************************************************

catch {exec pdflatex -interaction=nonstopmode >&pdflatex.log refman.tex} errmsg
catch {exec makeindex >>&pdflatex.log refman.idx} errmsg
catch {exec pdflatex -interaction=nonstopmode >>&pdflatex.log refman.tex } errmsg
for {set latex_count 1} {$latex_count <= 5} {incr latex_count} {;# do it not more than 5 times!
	if {$latex_count > 1} {puts "patflatex run: $latex_count"};  # shows index of run (if > 1)
    catch {exec pdflatex -interaction=nonstopmode >>&pdflatex.log refman.tex} errmsg;                    # execute tool pdfLatex, stdout & errout ->weg.txt
	set file [open refman.log];	                                 # open file "refman.log"
	set fileStr [read $file];	                                 # and read it into string fileStr
    if {[string first "Rerun" $fileStr]==-1} {break};            # break loop if "Rerun.." coonot be founfd in logfile
	close $file;                                                 # close file
}

