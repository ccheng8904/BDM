# Changelog
All notable changes to Code Generator will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).
For code generator documentation see [Code Generator homepage](../../../build/help/code_generator.md)

## [1.2.2]
### Added
- Added command line option to pass additional parameters

## [1.2.1]
### Added
- New functions for case change of strings
  - camelCase, CamelCase
  - snake_case, SNAKE_CASE
- Added possibility to reference dictionary entry using variable expansion used as key name
### Changed
- Extended functionality of search paths to configuration files and code generator functions ("-s" command line argument)
### Fixed
- Expansion of system variables was not working correctly

## [1.2.0]
### Changed
- Small rework of config loader to merge dictionaries from configuration files recursively
### Fixed
- Function for replacing of single variable was not working with lists with length 1. (bug introduced in 1.1.0)

## [1.1.0]
### Added
- Added possibility to load custom python scripts and register code generator functions
- New built-in functions
  - include - function for including of user python scripts
  - load_csv - function for loading data from CSV file to dictionary
### Changed
- Partial redesign of internal structure of code generator, all changes should be backward compatible.
- Complete rework of registration of code generator functions

## [1.0.0] - Initial version

