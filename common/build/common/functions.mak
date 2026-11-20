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
#  Description      Top level generic build
#*******************************************************************************


# Function for refreshing path variables
# rel_path:         relative path from root of repository to current component folder
# rel_prog_path:    relative path from prog folder to component folder
# rel_cfg_path:     relative path from root of repository to configuration folder
#                   for current component
#
# note: variable assignment must be using ":=", otherwise you will get wrong result
define refresh_paths
$(eval rel_path=$(patsubst %/,%,$(dir $(lastword $(MAKEFILE_LIST))))) \
$(eval $(foreach _path,$(make_search_path), $(if $(findstring $(_path), $(rel_path)), $(eval rel_prog_path=$(subst $(_path)/,,$(rel_path)))))) \
$(eval rel_cfg_path=$(addprefix $(cfg_prog_path)/,$(rel_prog_path)))
endef


# Function for displaying tool error message
# parameter 1: tool for which the error message will be displayed
define tool_error
@echo External tool "$1" not found. Please check if your tool is present on this path
endef


# function to filter a list of sources by basefilenames
# parameter 1: list with source-filepathes
# parameter 2: lattice to filter
# result: list with source-filepathes, that is present in the lattice
define source_lattice
$(strip\
$(eval _unique_out=)\
$(foreach src, $1, $(if $(findstring $(notdir $(basename $(src))),$2), $(eval _unique_out += $(src))))\
$(_unique_out))
endef


# remove multiple occurrences of same string in firs parameter
# this function is case sensitive, so "abc" and "ABC" are not same strings
define unique
$(strip\
$(eval _unique_out:=)\
$(foreach var,$1,$(if $(filter $(var),$(_unique_out)),,$(eval _unique_out += $(var))))\
$(_unique_out))
endef


# function for searching of files within given paths
# parameter 1: search paths
# parameter 2: files to find
# result: list of found files
define find_files
$(foreach var,$1,$(wildcard $(addprefix $(var)/,$2)))
endef


# function for concatenating of content of files with pattern
# all subfolders are also searched
# parameter 1: folder to search
# parameter 2: output file
# parameter 3: file name pattern
define concat_files
$(python) $(py_modules)/concat_files.py $1 $2 $3
endef


# function for printing string to file
# if file is in subfolder(s), folders will be created
# parameter 1: path to the file
# parameter 2: string to write
define fprintf
$(shell $(python) $(py_fprintf) $1 $2)
endef

# print two strings in one line, the first white, the second strong cyan colored
ifeq ($(mcufw_colored_output),true)
prntlineWhiteCyan = @echo [0m$1[96m$2[0m
else
prntlineWhiteCyan = @echo $1$2
endif

# print two strings in one line, the first in blue, second yellow colored
ifeq ($(mcufw_colored_output),true)
prntlineBlueYellow = @echo [34m$1[93m$2[0m
else
prntlineBlueYellow = @echo $1$2
endif

# print three strings in one line, the first white, the second normal cyan, the third strong cyan colored
ifeq ($(mcufw_colored_output),true)
prntlineWhiteNCyanSCyan = @echo [0m$1[36m$2[96m$3[0m
else
prntlineWhiteNCyanSCyan = @echo $1$2$3
endif

# print three strings in one line, the first white, the second normal cyan, the third strong cyan colored
ifeq ($(mcufw_colored_output),true)
prntlineYellowNCyanSCyan = @echo [33m$1[36m$2[96m$3[0m
else
prntlineYellowNCyanSCyan = @echo $1$2$3
endif

# print in light red color
ifeq ($(mcufw_colored_output),true)
prntRd = @echo [91m$1[0m
else
prntRd = @echo $1
endif

# print in red color
ifeq ($(mcufw_colored_output),true)
prntR = @echo [31m$1[0m
else
prntR = @echo $1
endif


#print in green color
ifeq ($(mcufw_colored_output),true)
prntGr = @echo [32m$1[0m
else
prntGr = @echo $1
endif

#print in light blue color
ifeq ($(mcufw_colored_output),true)
prntBl = @echo [94m$1[0m
else
prntBl = @echo $1
endif

#print in blue color
ifeq ($(mcufw_colored_output),true)
prntB = @echo [34m$1[0m
else
prntB = @echo $1
endif

#print in magenta/violet color
ifeq ($(mcufw_colored_output),true)
prntV = @echo [35m$1[0m
else
prntV = @echo $1
endif

#print in dark yellow color
ifeq ($(mcufw_colored_output),true)
prntYd = @echo [33m$1[0m
else
prntYd = @echo $1
endif

#print in dark gray color
ifeq ($(mcufw_colored_output),true)
prntGd = @echo [90m$1[0m
else
prntGd = @echo $1
endif

# print in default white (no specified) color, a New Line after output
prnt = @echo $1

# print a New Line
prntnl = @echo.

# print in default white (no specified) color and No New Line after output
define prntnnl
@echo | set /p dummyName=$1
endef

ifdef mkdbg
define dbg
$(info *** Debug:$1)
endef
endif
INCLUDE_DIRS = Dir


define calc
$(shell $(perl) -e "print $1;")
endef

define calcf
$(shell $(perl) -e "print (sprintf(\"$2\", $1));")
endef
  
define calcx
$(shell $(perl) -e "print (sprintf(\"%%X\", $1));" 2>> $(out_path)/$(variant)/perlcalc.log)
endef

# calculates aligned size of bytes in $1, depending of minimal write block size in $2
define calcAlign
$(shell $(perl) -e "print (sprintf(\"%%X\",((int(($1 - 1) / $2) + 1) * $2)));" )
endef

define match
$(shell $(perl) -e "print $1=~ m/$2/;")
endef

define minX
$(shell $(perl) -e "if ($1 > $2) {print sprintf(\"%X\", $2)} else {print sprintf(\"%X\", $1)};")
endef

define maxX
$(shell $(perl) -e "if ($1 > $2) {print sprintf(\"%X\", $1)} else {print sprintf(\"%X\", $2)};")
endef

# compare numeric values in parameters $1 and $2
# if $1 is greater_than $2 then function returns true
define greater_then
$(shell $(perl) -e "if ($1 > $2) {print \"true\"} else {print \"false\"};")
endef

# returns a string length for parameter $1
# usefull for small makefile variables length calculation
define length
$(shell $(perl) -e "print (length "$1");")
endef

# returns a big string length for parameter $1
# parameter $1 is a variable name
# usefull for big makefile variables length calculation (more than 8192 characters)
define biglength
$(file >$(out_path)/$(variant)/$1.len,$($1))
$(shell $(perl) -e "print sprintf(((-s \"$(out_path)/$(variant)/$1.len\") -2)); unlink \"$(out_path)/$(variant)/$1.len\";")
endef

define prepost
$(addsuffix $3,$(addprefix $1,$2))
endef

# replaces the linux '/' with the DOS '\'
# parameter 1: directory with slashes to be replaced wit back slashes
define dospath
$(subst /,\,$1)

endef

define linuxnpath
$(subst \,/,$1)

endef

# deletes one file
define del
del $(subst /,\,$1) >NUL 2>&1
endef

# deletes whole directory
define deltree
call $(xrmdir) $(subst /,\,$1)
endef

# deletes a file
# parameter 1: file to be deleted
define delfile
@del $(subst /,\,$1) >NUL 2>&1
endef

# deletes files recursively
# parameter 1: file pattern to be deleted recursive
define delfiles_recursive
@del /F /S /Q $(subst /,\,$1) >NUL 2>&1
endef


# applies current date to a file
# parameter 1: file to be touched
define touch
$(xtouch) $(subst /,\,$1)

endef

# creates a directory recursively
# parameter 1: directory to be created
define mkdir
@$(python) $(mkfolders) -folders $1
endef

# copy a file, if required the target directory will be created
# parameter 1: source file
# parameter 2: target file
define copy
@$(call mkdir, $(dir $(subst \,/,$2)))
@$(xcopy) $(subst /,\,$1) $(subst /,\,$2)

endef



# replace text in a file and print result to stdout
# parameter 1: search pattern (perl regular expression)
# parameter 2: replace pattern (perl regular expression)
# parameter 3: file name
define replace_file
@$(perl) -pe s/$1/$2/g $3

endef

# replace text in a file and save result to original file
# parameter 1: search pattern (perl regular expression)
# parameter 2: replace pattern (perl regular expression)
# parameter 3: file name
define replace_in_file
@$(perl) -pi.tmp -e s/$1/$2/g $3

endef

# perform file comparison
# parameter 1: 1. file to compare
# parameter 2: 2. file to compare
# result: "True" if files are equal, otherwise "False"
define compare_files
$(shell $(xfilecmp) $(subst /,\,$1) $(subst /,\,$2))
endef


#converts all lowercase letters to uppercase
define upper
$(shell $(perl) -e "print uc $1;")
endef

#converts all uppercase letters to lowercase
define lower
$(shell $(perl) -e "print lc $1;")
endef

define dateString
$(shell $(perl) -e "printf \"%04d%02d%02d\n\",((localtime)[5] +1900),((localtime)[4] +1),(localtime)[3]";) 
endef

# new line definition
define newline


endef

# print time stamp - only for debug makefile
# e.g. put $(warning $(debug_time_stamp)) in the makefile
define debug_time_stamp
$(shell cmd /C @echo %TIME%)
endef

# print time stamp in target's rule
# e.g. <tab>$(call time_stamp_in_rule)
define time_stamp_in_rule
@echo %TIME%
endef

#converts all lowercase letters to uppercase and the other way round, no perl 
lowercase = $(subst A,a,$(subst B,b,$(subst C,c,$(subst D,d,$(subst E,e,$(subst F,f,$(subst G,g,$(subst H,h,$(subst I,i,$(subst J,j,$(subst K,k,$(subst L,l,$(subst M,m,$(subst N,n,$(subst O,o,$(subst P,p,$(subst Q,q,$(subst R,r,$(subst S,s,$(subst T,t,$(subst U,u,$(subst V,v,$(subst W,w,$(subst X,x,$(subst Y,y,$(subst Z,z,$(1)))))))))))))))))))))))))))
uppercase = $(subst a,A,$(subst b,B,$(subst c,C,$(subst d,D,$(subst e,E,$(subst f,F,$(subst g,G,$(subst h,H,$(subst i,I,$(subst j,J,$(subst k,K,$(subst l,L,$(subst m,M,$(subst n,N,$(subst o,O,$(subst p,P,$(subst q,Q,$(subst r,R,$(subst s,S,$(subst t,T,$(subst u,U,$(subst v,V,$(subst w,W,$(subst x,X,$(subst y,Y,$(subst z,Z,$(1)))))))))))))))))))))))))))

# embrace a list of words with an xml-tag
# parameter 1: macro with list, that shall be embraced with xml-tag
# parameter 2: xml-tag
# result: \n<xml-tag>list_entry1</xml-tag> \n<xml-tag>list_entry2</xml-tag> ...
xml_embrace = $(foreach i, $(1),<$(2)>$(call uppercase,$(basename $(i)))</$(2)>)

xml_embrace_no_uc = $(foreach i, $(1),<$(2)>$(basename $(i))</$(2)>)

# list of all test targets
tst_all_targets := tst_unique tst_touch tst_replace tst_copy tst_deltree tst_uc tst_calcx tst_min_max tst_dateString tst_length tst_greater_then tst_calcAlign

# do not make dependencies for test targets
no_dep_targets += $(tst_all_targets) tst_all tst_print

tst_all: $(tst_all_targets)


tst_unique:
	@$(call prntRd,Testing unique function)
	@echo input variable: 123 456 789 ABC 123 abc DEF GHI ABC
	@echo output variable: $(call unique, 123 456 789 ABC 123 abc DEF GHI ABC)


tst_touch:
	@$(call prntRd,Testing touch function)
	@copy make.bat make.tst >nul
	@$(call touch,make.tst)
	@dir make.tst
	@del make.tst > nul


tst_replace:
	@$(call prntRd,Testing replace function)
	@$(call replace_file,call,calll,make.bat)


tst_copy:
	@$(call prntRd,Testing copy function)
	@echo copying make.bat to test/make.bat
	@$(call copy,make.bat,test/make.bat)
	@dir test


tst_deltree:
	@$(call prntRd,Testing deltree function)
	@echo deleting 'test' folder
	@$(call deltree,test)


tst_uc:
	@$(call prntRd,Testing upper case function)
	@echo input variable: 'tester'
	@echo output variable: $(call upper, 'tester')


tst_calcx:
	@$(call prntRd,Testing hex calculation function)
	@echo 0xF3000-0xF0000 = 0x$(call calcx,0xF3000-0xF0000)
	@echo 0xF3000+0x03000 = 0x$(call calcx,0xF3000+0x03000)


tst_min_max:
	@$(call prntRd,Testing minx/maxX functions)
	@echo maximum of 0x3000 and 0x4000 is 0x$(call maxX,0x3000,0x4000)
	@echo minimum of 0x3000 and 0x4000 is 0x$(call minX,0x3000,0x4000)
	@echo maximum of 0x3000 and 0x2FFF is 0x$(call maxX,0x3000,0x2FFF)
	@echo minimum of 0x3000 and 0x2FFF is 0x$(call minX,0x3000,0x2FFF)


tst_dateString:
	@$(call prntRd,Testing dateString function)
	@echo $(call dateString)

tst_length:
	@$(call prntRd,Testing length String function)
	@echo Calculating length of string: $(derivative), the length is: $(call length,$(derivative))

tst_greater_then:
	@$(call prntRd,Testing function greater_then)
	@echo Is lenght of string '$(derivative)' greater than '$(platform)'? Answer: $(call greater_then,$(call length,$(derivative)),$(call length,$(platform)))
	@echo Is lenght of string '$(platform)' greater than '$(derivative)'? Answer: $(call greater_then,$(call length,$(platform)),$(call length,$(derivative)))

tst_calcAlign:
	@$(call prntRd,Testing calculation of data alignment function)
	@echo Minimal write block size is 0x$(write_block_size)
	@echo ProdTime size 10 will be aligned to.....0x$(call calcAlign,0xA,0x$(write_block_size))
	@echo Tracing ID size 29 will be aligned to...0x$(call calcAlign,0x1D,0x$(write_block_size))
	@echo HW ID size 22 will be aligned to........0x$(call calcAlign,0x16,0x$(write_block_size))
	@echo FW ID table size 104 will be aligned to.0x$(call calcAlign,0x68,0x$(write_block_size))

tst_print:
	@$(call prntnnl,"No new line:")
	@$(call prntnnl,"Text")
	@$(call prnt,Default text with a new line afte text)
	@$(call prntRd,Red text.)
	@$(call prntlineWhiteCyan,Print two strings in one line - the first white, the second cyan colored.)
