# The Build process of MCU Framework
---


The build process in MCU Framework is automated by using GNU make.
 It can be started right after a user finish setup of project by using Setup utility.
 After setup all necessary setting are set for build.

## Building

You can build your project with following command:

`make project=MyProject variant=MyVariant -j`

or you can build all together by running this command:

`make project=MyProject all -j`

The `-j` here means that all processor cores will be used for make process. You can find more make targets for build, debug, test, flash and documentation here: [List of targets in Framework make process](./make_targets.md)

Note: if a project name is not specified during setup of project then you can ommit `project=MyProject` in the build command:

`make all -j`

There are built-in variants and application variants. All are merged to one binary at the end of the build process.

After the build process finish its job the binary files are created in the `out` folder. There are folders named by names of built-in variants and project specific variant. E.g. for FWU3 build type:
- the built-in variants folders
    - `out/BootManager`
    - `out/lromLoader`
    - `out/programmer`
    - `out/ramLoader`
- and the project specific variant folder
    - `out/MyVariant`

    There are folders inside each folder

    `bin`
    `prog`

Binary files `hex` and `mot` can be found in `bin` folder.
A project specific binary files `MyVariant.hex` or `MyVariant.hex` contains the binaries from built-in variants.

[Home page of help](../../README.md)
