LIB_DIR=./lib/$TARGETDIRNAME
QT_DIR=/opt/Qt/5.15.2/gcc_64

export LIBRARY_PATH=$LIB_DIR:$LIBRARY_PATH
export LIBRARY_PATH=$QT_DIR:$LIBRARY_PATH
export LIBRARY_PATH=$QT_DIR/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/lib/logging:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/lib/numerics:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/lib/physics:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/lib/h5:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries/Qwt/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries/SplineLib/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries/minpack/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries/NLopt/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS/ThirdPartyLibraries/hdf5/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=$LIBRARY_PATH

valgrind --leak-check=yes --xml=yes --xml-file=valgrind-report.xml --suppressions=./tests/memcheck/valgrind_memcheck.supp ./build/$UNITTESTSNAME
