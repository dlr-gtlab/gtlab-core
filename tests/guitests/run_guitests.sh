#!/bin/bash

# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

# description: executes a set of gui testing suites
#
# author: Marius Bröcker AT-TWK
# email:  marius.broecker@dlr.de
# 
# required system variables:
#  - QT_DIR_LINUX2:		Path to Qt gcc_64 directory
#  - SQUISH_DIR: 		Squish installation directory
#  - DEVTOOLS_DIR:		GTlab Dev-Tools path
# optional: SQUISH_PORT (default 4322), SQUISH_STARTUP_TIMEOUT (seconds, default 30)
#
# local variables:

# directory containg GTlab binaries
GTLAB_DIR=$PWD/build

# Use the same endpoint for the server and every runner invocation.
SQUISH_PORT=${SQUISH_PORT:-4322}
SQUISH_STARTUP_TIMEOUT=${SQUISH_STARTUP_TIMEOUT:-30}
SERVER_LOG=$PWD/gui_tests_server_stdout.txt
SERVER_PID=

cleanup()
{
  if [[ -n "$SERVER_PID" ]]; then
    kill "$SERVER_PID" 2>/dev/null || true
    wait "$SERVER_PID" 2>/dev/null || true
  fi
}
trap cleanup EXIT
trap 'exit 130' INT
trap 'exit 143' TERM

port_is_open()
{
  (: > /dev/tcp/127.0.0.1/"$SQUISH_PORT") >/dev/null 2>&1
}

server_is_ready()
{
  kill -0 "$SERVER_PID" 2>/dev/null && port_is_open
}


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
echo "qt-gcc install dir: '$QT_DIR_LINUX2'"
echo "squish install dir: '$SQUISH_DIR'"
echo -e "testsuites to test: '\n$TESTSUITES\n'"

# add shared script dirs to squish path
for DIR in $SCRIPT_DIRS; do
  echo "adding '$DIR' to squish script dir"
  export SQUISH_SCRIPT_DIR=$SQUISH_SCRIPT_DIR:$DIR
done

# setting paths to libs (gtlab dependencies)
echo "setting library path variable..."
LIBRARY_PATH=$GTLAB_DIR
LIBRARY_PATH=$DEVTOOLS_DIR/lib/logging:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/lib/numerics:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/lib/physics:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/lib/h5:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/Qwt/lib:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/SplineLib/lib:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/minpack/lib:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/NLopt/lib:$LIBRARY_PATH
LIBRARY_PATH=$DEVTOOLS_DIR/ThirdPartyLibraries/hdf5/lib:$LIBRARY_PATH
LIBRARY_PATH=$QT_DIR_LINUX2/lib:$LIBRARY_PATH
export LIBRARY_PATH=$LIBRARY_PATH
export LD_LIBRARY_PATH=$LIBRARY_PATH
export QT_SCALE_FACTOR_ROUNDING_POLICY=Round
export QT_SCALE_FACTOR=1.0


# ---- execution ----

# Serialize jobs using the shared Squish AUT configuration on this host.
exec 9> "${TMPDIR:-/tmp}/gtlab-squish-${UID}.lock"
if ! flock -w 3000 9; then
  echo "ERROR: timed out waiting for the Squish GUI test lock" >&2
  exit 1
fi

# Never stop a server that belongs to another job.
if port_is_open; then
  echo "ERROR: Squish port $SQUISH_PORT is already occupied" >&2
  exit 1
fi

# Register the AUT before starting the server; fail if configuration fails.
echo "setting up AUT..."
if ! "$SQUISH_DIR/squishserver" --config addAUT GTlab "$GTLAB_DIR"; then
  echo "ERROR: could not register the GTlab AUT" >&2
  exit 1
fi

# Keep the foreground server as our child, without a terminal or daemon wrapper.
echo "starting squishserver on port $SQUISH_PORT..."
"$SQUISH_DIR/squishserver" --verbose --port "$SQUISH_PORT" > "$SERVER_LOG" 2>&1 9>&- &
SERVER_PID=$!
startup_deadline=$((SECONDS + SQUISH_STARTUP_TIMEOUT))
until server_is_ready; do
  if ! kill -0 "$SERVER_PID" 2>/dev/null || (( SECONDS >= startup_deadline )); then
    echo "ERROR: squishserver failed to start on port $SQUISH_PORT" >&2
    cat "$SERVER_LOG" >&2
    exit 1
  fi
  sleep 1
done

# return code to keep track of failed tests
RC=0

# iterate through every testsuite and execute it
echo "starting gui tests... "
for TESTSUITE in $TESTSUITES; do
  echo "- testing testsuite: '$TESTSUITE'..." 
  # executing runner on testsuite (generate html, junit and txt log files)
  if ! server_is_ready; then
    echo "ERROR: squishserver stopped before $TESTSUITE" >&2
    cat "$SERVER_LOG" >&2
    RC=$((RC + 1))
    break
  fi
  suite_name=$(basename "$TESTSUITE")
  "$SQUISH_DIR/squishrunner" --host 127.0.0.1 --port "$SQUISH_PORT" \
    --testsuite "$TESTSUITE" --exitCodeOnFail 1 \
    --reportgen html,./gui_tests_web \
    --reportgen "junit,./gui_tests_junits/junit_$suite_name.xml" \
    --reportgen stdout,./gui_tests_stdout.txt 9>&-
  # store latest return code
  rc=$?
  # adding rc to old return codes
  ((RC=RC+rc))
  echo "  tests finished with $rc"
done

# The EXIT trap stops only the server started by this job.

# generate badge
echo "generating badge..."
python3 $BASEDIR/testing_resources/_badge/generate_badge.py ./gui_tests_stdout.txt

# exit with return code
echo "$RC failed teststuites"
exit $RC
