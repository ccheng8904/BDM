# Getting started - setup project for development

If you want to setup a new project, please create your project repository and follow the steps below. You can find detailed instructions in [How to: Project development in Framework](https://wiki.bsh-sdd.com/display/PFS/How+to+Project+development+in+Framework)

```shell
git clone <web-URL-link-to-your-project>
cd <project-folder>
git submodule add --name Framework https://github-bshg.boschdevcloud.com/Generic/Framework.git common
cd common/
git checkout vX.X.X
cd ..
git add -A
git commit -m "Added Framework submodule vX.X.X"
```

With these steps you will add Framework as submodule to common folder in your project root path. Please remember to checkout the latest version vX.X.X of Framework.

## Initial Configuration

You can initialize your project by calling `common\setup.bat`
Detailed instruction see [How to: Setup project](https://wiki.bsh-sdd.com/display/PFS/How+to+Setup+project)


## Developing

To add project files please follow these steps [How to: Add project files to Framework](https://wiki.bsh-sdd.com/display/PFS/How+to+add+project+files+to+Framework)

Here's a [list of make variables and definitions for application](https://wiki.bsh-sdd.com/display/PFS/List+of+make+variables+and+definitions+for+application) that comes in handy when you need to make project specific changes.



[Home page of help](../../README.md)
