BASEDIR=$(dirname "$0")

# path to GTlab
GTLAB_DIR=$PWD/build
# path to Qt-gcc_64
QT_DIR=/opt/Qt/5.12.5/gcc_64
# path to the squish install directory
SQUISH_DIR=/home/gitlab-runner/squish-6.6.0-qt-5.12/bin

# testsuites to test (sperated by spaces)
TESTSUITES="gtlab_core_gui_tests gtlab_interface_tests"
# base folder of shared scripts
SHARED_SCRIPTS_DIR="/home/gitlab-runner/squish-gui-testing/common"
# folder to shared scripts (sperated by spaces)
SHARED_SCRIPTS="shared_object_names shared_scripts shared_steps"

echo "gtlab instance:     '$GTLAB_DIR'"
echo "dev-tools dir:      '$DEVTOOLS_DIR'"
echo "qt-gcc install dir: '$QT_DIR'"
echo "squish install dir: '$SQUISH_DIR'"
echo "shared scripts dir: '$SHARED_SCRIPTS_DIR'"
echo "testsuites to test: '$TESTSUITES'"

# add shared scripts to squish path
for SCRIPT in $SHARED_SCRIPTS; do
  echo "adding '$SCRIPT' to squish script dir"
  export SQUISH_SCRIPT_DIR=$SQUISH_SCRIPT_DIR:$SHARED_SCRIPTS_DIR/$SCRIPT
done

# cp common folder (so that step defintions can be found)
cp -r $SHARED_SCRIPTS_DIR $PWD/$BASEDIR/common

# setting paths to libs (dependencies)
echo "setting paths to libs..."
export LIBRARY_PATH=$GTLAB_DIR/../lib/core
export LIBRARY_PATH=$GTLAB_DIR:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS_DIR/lib/numerics:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS_DIR/lib/logging:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/Qwt/lib:$LIBRARY_PATH
export LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/minpack/lib:$LIBRARY_PATH
export LIBRARY_PATH=$QT_DIR/lib:$LIBRARY_PATH
export LD_LIBRARY_PATH=$LIBRARY_PATH


# register AUT
echo "setting up AUT... "
$SQUISH_DIR/squishserver --config addAUT GTlab $GTLAB_DIR

# start squish server in new terminal 
echo "starting squishserver... "
konsole -e $SQUISH_DIR/squishserver --verbose --daemon --logfile $PWD/gui_tests_server_stdout.txt &

# give it some time to start
sleep 5

# return code to keep track of failed tests
ReturnCode=0

# iterate through every testsuite an execute it
echo "starting gui tests... "
for SUITE in $TESTSUITES; do
  echo "- testing testsuite: '$SUITE'..." 
  # executing runner on testsuite
  $SQUISH_DIR/squishrunner --testsuite $BASEDIR/$SUITE --exitCodeOnFail 1 --reportgen html,./gui_tests_web --reportgen junit,./gui_tests_junits/junit_$SUITE.xml --reportgen stdout,./gui_tests_stdout.txt
  # store latest return code
  RC=$?
  # adding rc to old return codes
  ReturnCode=$(($ReturnCode + $RC))
  echo "- tests finished with $RC"
done

# stop squish server
$SQUISH_DIR/squishserver --stop

# generate badge
python3 $SHARED_SCRIPTS_DIR/../_pipeline/generate_badge.py ./gui_tests_stdout.txt

# exit with return code
echo "$ReturnCode failed teststuites"
exit $ReturnCode
