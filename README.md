# GTlab Framework

GTlab is an interactive, cross-platform simulation and pre-design environment for gas turbines and aircraft engines.

## Libraries

### What you need

What you need:

- GTlab-DevTools
  - GTlab-Logging
  - GTlab-Numerics
  - SplineLib
  - MINPACK
  - QWT
  - NLopt
  - (optional) GoogleTest
- Qt 5.12
- (MS Windows) Microsoft Visual Studio Build Tools 2017
- (Linux) GCC

## Building GTlab

The easiest way to build GTlab is from the QtCreator IDE - but it is
also possible to use an IDE of your choice.

### QtCreator

-   Copy *local_settings.pri* form *features* directory to GTlab *root*
    directory
-   Edit *DEV_TOOLS* path inside the *local_settings.pri* to GTlab-DevTools location on 
    your system
-   Run *QtCreator*
-   Open *gtlab.pro*
-   Run *qmake*
-   Build!

### Additional Settings

To make use of multiple cores in the compilation process use command line
argument:

Unix:   -jn (n = number of cores)

--------