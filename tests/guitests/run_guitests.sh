#!/bin/bash
# description: executes a set of gui testing suites
#
# author: Marius Bröcker AT-TWK
# email:  marius.broecker@dlr.de
# 
# required system variables:
#  - QT_DIR_LINUX: 		Path to Qt gcc_64 directory
#  - SQUISH_DIR: 		Squish installation directory
#  - DEVTOOLS_DIR:		GTlab Dev-Tools path
#
# local variables:

# directory containg GTlab binaries
GTLAB_DIR=$PWD/build

# timeout [min] before other squishrunner instances will be killed 
SQUISH_TIMEOUT=50


# ---- setup ----

# filepath
BASEDIR=$(dirname "$0")

# lists all subfolders containg auxiliary scripts
SCRIPT_DIRS=$(ls -d $PWD/$BASEDIR/testing_resources/*/)

# gtlabs core testsuites
CORE_TESTS=$(ls -d $PWD/$BASEDIR/gtlab_core_tests/*/)

# testsuites to test (separated by spaces)
TESTSUITES=$CORE_TESTS

echo "gtlab instance:     '$GTLAB_DIR'"
echo "dev-tools dir:      '$DEVTOOLS_DIR'"
echo "qt-gcc install dir: '$QT_DIR_LINUX'"
echo "squish install dir: '$SQUISH_DIR'"
echo -e "testsuites to test: '\n$TESTSUITES'"

# add shared script dirs to squish path
for DIR in $SCRIPT_DIRS; do
  echo "adding '$DIR' to squish script dir"
  export SQUISH_SCRIPT_DIR=$SQUISH_SCRIPT_DIR:$DIR
done

# setting paths to libs (gtlab dependencies)
echo "setting paths to libs..."
LIBRARY_PATH=$GTLAB_DIR/../lib/core
LIBRARY_PATH=$GTLAB_DIR:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/lib/logging:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/lib/numerics:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/lib/physics:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/lib/h5:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/Qwt/lib:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/SplineLib/lib:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/minpack/lib:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/NLopt/lib:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/hdf5/lib:$LIBRARY_PATH
LIBRARY_PATH=$QT_DIR_LINUX/lib:$LIBRARY_PATH
export LIBRARY_PATH=$LIBRARY_PATH
export LD_LIBRARY_PATH=$LIBRARY_PATH


# ---- execution ----

# check if squish is already running -> list processes -> grep squishserver
iter=0
while ps aux | grep -v grep | grep squishserver; do
  echo "waiting for other squish instances to finish... ($iter)"
  # increment counter
  ((iter=iter+1))
  # sleep 1 min 
  if [ "$iter" -le "$SQUISH_TIMEOUT" ]; then
    sleep 60
  # stop squishserver if counter reached max timout
  else
    echo "WARN: TIMEOUT - forcing squisserver to stop!"
	$SQUISH_DIR/squishserver --stop
	break
  fi
done

# register AUT
echo "setting up AUT... "
$SQUISH_DIR/squishserver --config addAUT GTlab $GTLAB_DIR

# start squish server in new terminal 
echo "starting squishserver... "
konsole -e $SQUISH_DIR/squishserver --verbose --daemon --logfile $PWD/gui_tests_server_stdout.txt &

# give it some time to start
sleep 5

# return code to keep track of failed tests
RC=0

# iterate through every testsuite and execute it
echo "starting gui tests... "
for TESTSUITE in $TESTSUITES; do
  echo "- testing testsuite: '$TESTSUITE'..." 
  # executing runner on testsuite (generate html, junit and txt log files)
  $SQUISH_DIR/squishrunner --testsuite $TESTSUITE --exitCodeOnFail 1 --reportgen html,./gui_tests_web --reportgen junit,./gui_tests_junits/junit_$SUITE.xml --reportgen stdout,./gui_tests_stdout.txt
  # store latest return code
  rc=$?
  # adding rc to old return codes
  ((RC=RC+rc))
  echo "- tests finished with $rc"
done

# stop squish server
$SQUISH_DIR/squishserver --stop

# generate badge
# python3 $GUI_TESTING_DIR/_pipeline/generate_badge.py ./gui_tests_stdout.txt

# exit with return code
echo "$RC failed teststuites"
exit $RC
