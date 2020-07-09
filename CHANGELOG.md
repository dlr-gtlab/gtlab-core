# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [1.6.2] - 2020-07-09
### Fixed
- BugFix: Deserialization of property lists of old type "string" - #76

## [1.6.1] - 2020-07-03
### Fixed
- BugFix: Potential memory leak in ProcessExecutor - #75
- BugFix: ContourPlot crash (Maps/AeroPlots) is fixed by several safty functions #74

## [1.6.0] - 2020-06-05
### Added
- ProjectAnalyzer to identify corrupted save data and framework incompatibilities - #65, #64, #62
- Objects of unknown classes are displayed within project/process tree - #1
- XML attributes within project files are saved in alphabetical order - #59
- Exporter and Importer for Processelements - #58
- Backup functionality for project files when the project is saved - #10
- Changelog file in repository - #72
- Installation steps in readme - #4

### Changed
- Migration to new build configuration (Qt12, MSVC 2017 Build Tools)

### Fixed 
- BugFix: Project Analyzer opens after the creation of a new project - #71
- BugFix: Process Importer does not change UUID if element already exist - #67
- BugFix: Renaming a project fails under certain conditions - #60
- BugFix: Linux Adaptations - #22
- BugFix: Skipping or unskipping process elements is not recorded in the redo/undo system - #15
- BugFix: 'Add...' button under 'Processes/Calculators' stays enabled after closing project - #13

### Removed
- Removed: Schedule functionality is moved to BasicTools Module - #63 
- Removed: Spline functionality is moved to BasicTools Module - #7 
- Removed: Filter functionality in the explorer widget to display results separately - #57