# distributed help concept
# Requirements:
# - Chrome browser installed
# - Markdown Preview Plus plugin installed and properly enabled in Chrome browser
#
# links: https://peter.sh/experiments/chromium-command-line-switches/#load-extension
# https://superuser.com/questions/33548/starting-google-chrome-in-application-mode
# https://www.ghacks.net/2013/10/06/list-useful-google-chrome-command-line-switches/

help_show ?= false

help_url_path_prefix = file:///$(CURDIR)
# the path to hal is depended on platform e.g. common/prog/MSP/stm32g0
help_hal_path  = common/prog/MSP/doc
help_mcal_path = common/prog/MSP/mcal/doc
help_main_file = README.md
help_main_file_path = common
help_subject =
help_list_hal  = hal hdio huart htim hadc hspi hint hwdt hsup
help_list_mcal = mcal madc mcal_callback mdio mexti mi2c mpcm mspi mtim muart mwdt mdma
help_list_components = msp dbus debug firmware_update funcsafe math mem_utility \
mipy_interface retarget security schedulers_bm std_lib testapp
# NOTE: BootManager keyword cannot be used due to existing target name for built-in variant
help_list_fwu_components = bootmanager Boot-Manager
help_list_devices  = dbuscan
help_list_keywords = make build tools compilers doxygen chocolatey vectorcast \
lint lintplus code_generator 
help_list_misra_rules  = misra misra2.1 misra2.2 misra2.3 misra2.4 misra2.5 misra2.6 misra2.7
help_list_misra_rules += misra3.1

ifeq (help,$(firstword $(MAKECMDGOALS)))
    # use the rest as arguments for "help"
    help_args := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
    ifeq (,$(help_args))
        help_subject_file :=$(help_url_path_prefix)/$(help_main_file_path)/$(help_main_file)
        help_show = true
    else
        help_first_argument := $(wordlist 2,2,$(MAKECMDGOALS))
        # check if first argument is name of component
        ifeq ($(help_first_argument), $(filter $(help_first_argument), $(help_list_components)))
            help_subject = $(help_first_argument)
            # TODO: decide if will be used fixed name README or variable help_subject will be used.
            help_subject_file = $(help_url_path_prefix)/common/prog/$(help_subject)/README.md
            help_show = true
        # check if first argument is name of firmware_update component
        else ifeq ($(help_first_argument), $(filter $(help_first_argument), $(help_list_fwu_components)))
            help_subject = $(subst -,,$(help_first_argument))
            help_subject_file = $(help_url_path_prefix)/common/prog/firmware_update/BootManager/doc/$(help_subject).md
            help_show = true
        # check if first argument is name of component from devices/ directory
        else ifeq ($(help_first_argument), $(filter $(help_first_argument), $(help_list_devices)))
            help_subject = $(help_first_argument)
            help_subject_file = $(help_url_path_prefix)/common/prog/devices/$(help_subject)/doc/README.md
            help_show = true
        # check if first argument is name of HAL component
        else ifeq ($(help_first_argument), $(filter $(help_first_argument), $(help_list_hal)))
            help_subject = $(help_first_argument)
            help_subject_file = $(help_url_path_prefix)/$(help_hal_path)/$(help_subject).md
            help_show = true
        # check if first argument is name of MCAL component
        else ifeq ($(help_first_argument), $(filter $(help_first_argument), $(help_list_mcal)))
            help_subject = $(help_first_argument)
            ifeq ($(help_subject),mcal)
                help_subject_file = $(help_url_path_prefix)/common/prog/MSP/mcal/README.md
            else
                help_subject_file = $(help_url_path_prefix)/$(help_mcal_path)/$(help_subject).md
            endif
            help_show = true
        # check if first argument is generic keyword
        else ifeq ($(help_first_argument), $(filter $(help_first_argument), $(help_list_keywords)))
            help_subject = $(help_first_argument)
            help_subject_file = $(help_url_path_prefix)/common/build/help/$(help_subject).md
            help_show = true
        # check if first argument is misra rule
        else ifeq ($(help_first_argument), $(filter $(help_first_argument), $(help_list_misra_rules)))
            help_subject = $(help_first_argument)
            help_subject_file = $(help_url_path_prefix)/common/build/help/misra/$(help_subject).md
            help_show = true
        else
            help_show = false
        endif
    endif
    # ...and turn them into do-nothing targets
    $(eval $(help_args):;@:)
endif

browser = chrome
browser_mode_app = --app=
#browser_mode_app = --kiosk=
browser_opt = --new-window
browser_opt += --disable-translate
browser_opt += --window-position=30,10
browser_opt += --window-size=650,900
browser_opt += --disable-features=Translate
#browser_opt += --file-url-path-alias "/alias=/$(CURDIR)"
browser_path ="C:\Program Files (x86)\Google\Chrome\Application\"
# set background color with RGBA integer value in hex, e.g. 'ff0000ff' for red
#browser_bacround_color = --default-background-color=ff000001

# help is written in markdown file previewed in chrome running in app mode and Markdown Preview Plus plugin enabled
# suggest is run in kiosk mode, no borders etc. will be shown.
# Furthermore, pressing F11 will not exit this mode
# --kiosk http://example.com/
#TODO: add parameter after make help <parameter> and pass it to the help target

.PHONY: help
help:
ifeq ($(help_show),false)
	@echo The help keyword '$(help_first_argument)' was not found
	@echo Available keywords are:
	@echo $(help_list_keywords)
	@echo $(help_list_components)
	@echo $(help_list_mcal)
	@echo $(help_list_hal)
	@echo $(help_list_devices)
else
#	$(browser_path)$(browser) $(browser_mode_app)$(help_main_file) $(browser_opt) $(browser_bacround_color)
	@$(browser_path)$(browser) $(browser_mode_app)$(help_subject_file) $(browser_opt) $(browser_bacround_color)
endif
