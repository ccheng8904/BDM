::%1 => axivion path
::%2 => python2 path
::%3 => axivion_python_module_path
::%4 => utest_gen_stubs_from_axivion_py
::%5 => $(out_path)\$(variant)\MkStubGenCfgFile.xml
::%6 => python2
::%7 => $(CURDIR)/$(teng_python_modules)
@set Path=%7;%1;%2;%2\Lib;%2\DLLs;%Path%
@set BAUHAUS_PYTHON=%6
@set PYTHONPATH=%7;%2\Lib;%2;%2\DLLs
@set PYTHONHOME=%7;%2\Lib;%3;%2;%2\DLLs;$(CURDIR)/$(teng_python_modules)
@echo Generating stubs and testframes...
@%6 %4 -cfg_file %5
