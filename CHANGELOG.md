<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Fixed
- Fixed mirco stutters when new messages are appended to the output dock. - #1165
- Fixed unicode characters not correctly saved to tasks, in particular python tasks and file properties - #1125, #45

### Changed
- Improved performance of the python syntax highlighter - #1139
- (In-)active filter buttons for the logging level in the output dock are now better distinuishable. - #606 
- Warning for unknown classes is now only shown in the developer mode of GTlab - #1160

## [2.0.6] - 2023-11-07 
### Fixed
- Fix examples widget for new structure of process files - #593
- Fixed problems opening projects after project conversion.
  Now projects are not automatilly opened anymore after conversion and saved as a new project - #585
- Fixed all wizards were names "New Calculator Wizard" - #601
- Fixed GtTask::finished signal is not emitted, if the task was started using a process executor - #582
- Fixed batch command "set_variable" crashing, when providing additional command like "--dev" - #504

### Added
- Undo and redo toolbar buttons show tooltip information based on the operation being perform on click - #572

## [2.0.5] - 2023-08-22
### Changed
- Checkbox for permanently deleting project can now be toggled by clicking on the checkbox text - #549
- Task and calculator states will now more accurately show which task succeeded, failed or was skipped - #542
- A Project can be deleted/removed from session using the default delete shortcut - #564

### Added
- Added "Empty Task" action to all process dock context menus - #543
- Property struct containers can now also contain monitoring properties. - #547

### Fixed
- Deactivated optional properties are now greyed out - #551
- Fixed wrong gtlab config settings path in batch application - #569
- Slight performance increase when deleting one or multiple objects - #528
- Updated delete icon of property container items in the properties dock - #535

## [2.0.4] - 2023-06-02
### Fixed
- Fixed multiple inclusion error of GTlabConfig.cmake

## [2.0.3] - 2023-05-15
### Changed
- Switched build system to cmake. This makes it also easier to consume GTlab from other cmake based projects - #537

### Fixed
- Fixed replicating unnamed child objects. This bug appeared in the Sketchpad SplineComet calculator - #539

### Added
- Added option to output dock to ignore all incoming messages. This may be useful if lots of incomming messages are slowing down GTlab - #531

## [2.0.2] - 2023-05-04
### Fixed
- Fixed permanent adding of line endings / line breaks when opening a project; Especially for larger strings like python scripts - #536

## [2.0.1] - 2023-04-19

This is the first patch release of the 2.0 series.

### Added
- Modules can now have `optional_dependencies`, which are defined in the `module.json`
  metadatafile similar to `depenencies`. - #517
   
### Fixed
- Fixed objects were not marked as changed, when adding / removing property container entries - #529

### Changed
- Changed the location of the GTlab config files path to not collide with older versions - #530

## [2.0.0] - 2023-04-04
### Fixed
- Fixed "rename" action not beeing visible in the contextmenu of objects - #493
- Fixed uninitialized Task Group when saving a new project for the first time - #507

### Changed
- New Project Wizard will remember the last path used when creating a project - #508
- Preference Dialog will remember the last page visited and reopen it - #509

### Added
- Option to collapse/expand all items in the object selection dialog for object link properties - #395

## [2.0.0-rc1] - 2023-03-21
### Fixed
- Fixed / improved GtFileChooserProperty file filters to be also compatible to Qt filters - #446
- Fixed checkboxes beeing barely visible in darkmode - #465
- Fixed wizard style issues - #474
- Fixed potential data loss when using an externalized object - #453, #251
- Fixed graphical glitch in ruler when resizing the window - #425

### Added
- The output dock widget will now remember the filter for the log levels - #123

## [2.0.0-pr-beta2] - 2023-02-22
### Fixed
- Integer property change only triggers if user finished editing and not on each key press - #437
- Object names in the explorer dock are no longer truncated by default - #439
- Fixed project `task` folder is not copied when upgrading a project or creating a project backup - #441
- Fixed bug after upgrading a project to a new one - #436

## [2.0.0-pr-beta1] - 2023-02-10
### Changed
- __Highlight__: Updated the icon set. Default icons will adapt to theme - #374
- The directory of GTlab's config files has been changed on Linux to `~/.config/DLR/GTlab` - #327
- Animated splash screen - #418

### Added
- Added icon browser available in dev-mode - #374

### Fixed
- Fixed missing undo/redo functionality for object link and file/folder selection properties - #28
- Fixed missing functionality for print button in new toolbar widget - #422
- Fix content margins of preferences pages - #420
- Fixed low DPI pixmaps on high DPI monitors - #374

## [2.0.0-pr-alpha6] - 2023-02-03
### Changed
- The Output Dock now displays all log message in a table format - #347
- The log error message box will spawn delayed if one was closed recently - #373
- Modules that caused a crash on the last application run will no longer block the module for all other instances - #397
- To reenable a crashed module one must check the corresponding entry in the disbaled plugins dialog instead of unchecking it - #399

### Fixed
- Fixed a bug where the "Add Process" button was not disabled after closing a project - #369
- Fixed bug that prevents the process importer from working - #381 
- Fixed bug that prevented the creation of a new task when the project was created by another user - #388

### Added
- Added batch application commands `list_variables` and `set_variable <id> <value>` - #391
- Added batch command `enable_modules [--all] "Module_A" "Module_B"` to reenable disabled modules - #141
- Added batch command option `list [-m]` to list all modules
- App Arguments for setting a custom logging level (`--trace` for Trace Level and `--debug` for Debug and higher) - #389
- App Arguments for setting the verbosity level (`--silent` for no verbosity, `--medium` for medium verbosity and `--verbose` for very verbose output) - #376
- Ability for process components to show their progress in the process widget - #384

### Removed
- Removed option to enable dev mode in the GUI application using `-dev`. Use `--dev` instead.

## [2.0.0-pr-alpha5] - 2022-12-14
### Fixed
- Unknown or duplicate shortcuts in the shortcut preference page will be removed once "Restore" was clicked - #290 
- Fixed some shortcuts being registered multiple times - #361
- Fixed bug which lead to crash sometimes caused by key press event of process dock widget - #364

### Changed
- Error message box will now accumulate all error messages in a single widget instead of spawning multiple windows - #348
- Error message box will also be triggered by early error messages - #371
- Changed module loading process. The module loading is rewritten to determine the loading order of modules.
  Please file a bug, if you experience problems - #260

### Added
- Shortcut for closing current MDI-Tab (`Crtl+W` by default) - #357
- Added batch application command `load_module <my_module_file>` which can be used for module testing - #260

## [2.0.0-pr-alpha4] - 2022-12-06
### Fixed 
- Initial structure to avoid loading of GUI elements in console mode - #2
- Exporter: Setting an invalid or empty file extension will no longer crash GTlab - #130
- BugFix: Small check in table class prevents crash on linux for tables with only one point
- Changed ObjectUI Standard Behaviour for renaming to allow spaces - #109
- GtVersionNumber: correct function for operator '>' and '<' -#159 
- GtProcessDock: Dummy process elements are no longer mutable - #162
- Examples search path was set to a specific 'examples' dir - #168
- Fixed a crash when zooming in on a graphics scene containing a dense grid - #179
- Fixed bug in dynamic type registration in meta object system - #248

### Changed
- Saving a project was moved to a separate thread - #127
- Tasks can be skipped - #82, #176
- Removed "File->Import Project" menu. Instead, project are imported now via "File->Open Project". - #202
- New Splashscreen
- Restructuring of batch application including the new interface for modules to add batch functions - #265
- Moved GtObject functions `findObject` and `isDerivedFromClass` into the namespace `gt`
- Tasks are not longer stored in the project file. A new system has been implemented so that each task is saved in an individual file - #261

### Added
- __Highlight__: Added property containers to GtObjects. These allow to add an arbitrary number of properties to a GtObject. - #298
- __Highlight__: Shared functions, an interface share arbitrary functions with other modules without a compile time dependency.
  These functions need to be defined in the method GtModuleInterface::sharedFunctions - #207, #47
- __Highlight__: Introduced an experimental process runner, that executes tasks in a separate system process to avoid crashes of GTlab, when a task or calculator crashes.
  It an also be used for remote execution of tasks. Must be enabled in the preferences when in dev-mode - #256
- __Highlight__: Added support to ex-/internalize datasets at runtime into the HDF5 format. This can be used to reduce the memory consumption
  of large data objects. This behaviour can be enabled on a project basis in the project settings - #127
- __Highlight__: Modules can now define data model changes and register data model upgrade functions.
  The upgrade of the data model will be executed by GTlab for each module - #219
- __Highlight__: New look, including a dark mode - #163
- Backup and restore project of projects via the GTlab GUI - #294
- Project Settings dialog for setting project specific settings (opened using the contextmenu of a project) - #173
- Option to set the logging verbosity. - #346
- Multiple labels can be selected and deleted at once - #111
- Monitoring property for strings - #140
- The logging output will now provide additional information from which module the log originated - #268
- Shortcut settings for GTlab including the introduction of several new shortcuts and the option to maintain them in the preferences - #112, #171, #172
- Versionnumber based on GtVersionNumber for tasks - #129
- ObjectLink properties got an button to jump to linked object in explorer dock widget #128
- ObjectLink properties can be set by a drag and drop of objects from the explorer dock widget - #128
- New function for the batch application to list current sessions, projects and tasks - #292

### Removed
- Nozzle map was removed to be added in the map collection - #114 
- Removed DoubleMap container to be added to the post module - #116
- Removed multiple classes such as `GtDataZone` and `GtTable` from the Core framework.
  They are now part of the BasicTools module. - #342
- Removed dependency to GtNumerics - #353


## [1.7.2] - 2021-05-25
### Changed
- Update check functionality uses maintenance tool command line interface instead of parsing log files - #121
- Bug report url changed and enabled for non dev mode - #122

### Added
- Functionality to export framework footprint information  - #124
- Functionality to display framework footprint information in command line mode - #125

## [1.7.1] - 2021-05-17
### Fixed
- RPATH fix for all core libraries

## [1.7.0] - 2021-04-30
### Changed
- Modules are only excluded if the dependencies have a lower version number than required by the module

## [1.7.0-rc2] - 2020-12-21
### Added
- Functionality to exclude modules on framework startup  - #102

### Changed
- Framework version macro included in gtApp header

## [1.7.0-rc1] - 2020-11-18
### Added
- Ability to extend the object UI for existing classes  - #95
- Ability to write project specific comments  - #70
- Removing multiple selected projects from project explorer - #32
- Warning while closing GTlab if process is still running - #27
- GtResult: Convenience function to add result to label - #83

### Changed
- Module version information based on the framework versioning format (major, minor, patch)  - #89
- GtModeTypeProperty.getVal() is returning objectName instead of empty string - #34

### Fixed
- Removed inconsistency in handling of non-dimensional unit - #33

### Removed
- Process monitoring and dependency to qwt library - #9

## [1.7.0-alpha1] - 2020-11-06
### Added
- Convinience function to add datazones to resul-objects with labels easier  - #83
- ModeTypeProperty getVal() returns mode type id  - #34
- Improved comparison og ModeProperty and ModeTypeProperty - #34
- Option to remove multiple projects from session - #32

### Changed
- New general indication of version numbers. Used in modules and calculators, among others. - #89

### Fixed
- BugFix: Small check in table class prevents crash on linux for tables with only one point
- BugFix: Unitconversion for a non-dimensional unit

## [1.6.5] - 2020-09-16
### Fixed
- BugFix: Information about differences in version of the framework and project within ProjectAnalyzerDialog - #81
- BugFix: Invalidation of Access Data for Collections - #90
- BugFix: Wrong index in access data model when access data is removed - #92


## [1.6.4] - 2020-07-28
### Fixed
- BugFix: Fix crashes within the Process DockWidget if no selected project is open - #50
- BugFix: Potential memory leak in ProcessExecutor - #78
- BugFix: Auto delete application temp folder - #73
- BugFix: Locale options for linux builds - #80

### Added
- Handling of subproperties in connection editor - #39

## [1.6.3] - 2020-07-14
### Fixed
- BugFix: Invalid conversion from given type to variant - #77

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
