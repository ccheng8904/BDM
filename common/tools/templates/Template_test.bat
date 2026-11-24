::This bat-file will run a built-in Python Interpreter and preloaded py-files, for PC-Test the compiled C-Code of your project is within the binary.
::On startup of this bat, you have to decide for one of following:
::1.) normal-start, without any preconfiguration, then use parameter: -BASIC
::2.) connect and import project-structure-files (PSYD/PSTD) on startup: use no parameter
::3.) execute any test-xml-file on startup, then use desired xml-file as parameter, exg.: C:\TestXML\blub.xml
::4.) You want the PythonTestEngine to stay open, for example on execution from ptest with any config pyfile and any test-xml, then use two parameter, 
::remember in this case your config-file needs to process the parameter containing the xml: -IDE config.py C:\TestXML\blub.xml
::5.) You want the PythonTestEngine not to stay open, for example on execution from CI with any config pyfile and any test-xml, then use two parameter,
::remember in this case your config-file needs to process the parameter containing the xml:  -NOIDE config.py C:\TestXML\blub.xml
::according to 4 and 5.) You can pass parameters to the binary beside the –IDE/-NOIDE parameter.
@set Old_Path=%Path%
@set Old_PYTHONHOME=%PYTHONHOME%
@set Old_PYTHONPATH=%PYTHONPATH%
@set PYTHONHOME=<$PYTHONHOME>
@set PYTHONPATH=<$PYTHONPATH>
@set Path=<$PATH> 

@SET arg1=%1
@FOR /F "usebackq tokens=1*" %%I IN ('%*') DO @SET argn=%%J

@IF "%arg1%" == "-IDE" (
@call <$PTENG_BINARY> -i %argn%
@set error = %ERRORLEVEL%
@goto :NEXTSTEP
)

@IF "%arg1%" == "-NOIDE" (
@call <$PTENG_BINARY> %argn%
@set error = %ERRORLEVEL%
@goto :NEXTSTEP
)

@IF "%arg1%" == "-BASIC" (
@call <$PTENG_BINARY>
@set error = %ERRORLEVEL%
@goto :NEXTSTEP
)

@IF "%arg1%" == "-GUI" (
@call <$PTENG_BINARY> -i <$PATH_DEFAULTSETUP_PY> -gui
@set error = %ERRORLEVEL%
@goto :NEXTSTEP
)

:DEFAULT
@call <$PTENG_BINARY> -i <$PATH_DEFAULTSETUP_PY> -gui
@set error = %ERRORLEVEL%
:NEXTSTEP

@set PYTHONHOME=%Old_PYTHONHOME%
@set PYTHONPATH=%Old_PYTHONPATH%
@set Path=%Old_Path% 
EXIT /B %error%
