# CI for debug_extended component - local execution

## Create project  
   - Clone framework:  
   ``git clone https://github-bshg.boschdevcloud.com/Generic/Framework.git common``  
   - Clone ci (next to common folder):  
     ``git clone https://github-bshg.boschdevcloud.com/ecuci/ci_generic.git ci``
   
## Use the correct python installation
   For the CI test, you will need Python 3.5.2 and 3.9.1 from the chocolatey.
   Install both versions using the side-by-side and the 32-bit option (both in "Advanced Installation").  
   (For me) A Custom installation (without chocolatey) had to be removed before.  
   For further information contact Roman Dolinsky or Santhosh Pulasani.

## Setup virtual python environment
   Open cmd in project root and execute the following commands:  
   - ``py -m venv _ci_env_``  
   - ``call _ci_env_/Scripts/activate``
   - ``pip install ecuci-tools``

## Run the CI test
   Use the (activated) virtual python environment and execute:  
   - ``set PL_TT=Custom``
   - ``ecuci-gen -build Debug_Extended``
   - ``Debug_Extended.bat``
   - ``ecuci-validation``

   The command "ecuci-gen" will generate the file "Debug_Extended.bat" in your project root, which holds all execution commands according to the configuration files (BuildInfo.json, fw_builds.json, ...).  
   The command "ecuci-validation" will create the html overview, as you are probably used to. Call this after executing the test with "Debug_Extended.bat".

## Evaluate results
   The CI test automatically creates the app\\ and out\\ folder.  
   The result table will be stored here: out\\_ci_logs\\CI_dbgi_Vis_Table.html  

## Get help from the Wiki:
   Following wiki page helped me with the CI test:
   https://wiki.bsh-sdd.com/display/ECUCI/Python+TestEngine

## Change configuration for the ci test:
   The configuration for the ci test is stored in multiple .json files:
      BuildInfo.json, fw_builds.json, fw_stages.json

## Adapt Stub files:
   The Stub files for the ci test are located in: prog/debug_extended/Intel/Stubs  
   The testframes should be generated with each ci execution.
   To be save, it is best to delete these files after changes. (Works best for me).  

   The Common/ and UserStubs/ are the only 2 folders, which should be modified, if necessary.  
   The Common files are best to stub common used structures and functions.  
   The UserStubs can be used to stub module specific functions and structures for the modules under test.


## Further configuration:

All function calls, which refer to other modules (not under test) and can not be referenced automatically by the ci test, were handled by one of these two options:
   - Corresponding files were added to search_path
   - Stubbed within the files Intel/Stubs/Common/

In case of further problems with function calls, just follow the same structure and add the unknown functions.

All needed structures and defines were stubbed within the help-file sts_sim_test_uty.h. 
   
   
