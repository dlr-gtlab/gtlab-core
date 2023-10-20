<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

# GTlab Framework

GTlab is an interactive, cross-platform simulation and pre-design environment for gas turbines and aircraft engines.

## Libraries

### How to get GTlab-DevTools:  
In order to build GTlab or any GTlab module, you'll need the GTlab-DevTools.
You can find them on the download area of the [teamsite](https://teamsites-extranet.dlr.de/at/GTlab/Downloads/SitePages/Home.aspx).  
If you have no access to the teamsite please contact stanislaus.reitenbach@dlr.de.  

### What else do you need:
- Qt 5.15
- (MS Windows) Microsoft Visual Studio Build Tools 2019
- (Linux) GCC

## Building GTlab
The easiest way to build GTlab is from the QtCreator IDE - but it is
also possible to use an IDE of your choice.

### QtCreator
Note: CMake has support for multiple build systems. This is set via the "Generator" setting of cmake.
"Ninja" is the fastest make program, which is available on Linux and Windows, but must be downloaded first.
To enable ninja, go into QtCreator's preferences, Kits. Select the Kit  and choose "Ninja" or "Ninja Multi-Config" as the CMake generator.

-   Run *QtCreator*
-   Open CMakeLists.txt from gtlab's source directory
-   Choose build directories for debug and release mode
-   Set DevTools to the cmake configuration, by adding the variable `GTLAB_DEVTOOLS_DIR` with the devtools path (e.g. `/path/to/devtools/stable/2_0`) and press configure
-   Build!

### Command prompt
From within the source directory, run the following commands

- Configure the build: `cmake -B build -S . -G Ninja -DGTLAB_DEVTOOLS_DIR=<path/to/devtools>`
- Do the build: `cmake --build .`


--------
