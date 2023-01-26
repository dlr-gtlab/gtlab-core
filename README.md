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
