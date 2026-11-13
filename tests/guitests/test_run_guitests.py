# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
# SPDX-License-Identifier: MPL-2.0+

"""Test GUI runner lifecycle with local TCP server/runner fixtures, without Squish."""

import json
import os
from pathlib import Path
import shutil
import socket
import subprocess
import tempfile
import unittest


SERVER = '''#!/usr/bin/env python3
import os, socket, sys, time
from pathlib import Path
mode = os.environ['FIXTURE_MODE']
if '--config' in sys.argv:
    sys.exit(2 if mode == 'config-failure' else 0)
Path('server.pid').write_text(str(os.getpid()))
if mode == 'startup-failure':
    print('fixture server startup failed', flush=True)
    sys.exit(2)
if mode == 'timeout':
    time.sleep(60)
port = int(sys.argv[sys.argv.index('--port') + 1])
with socket.socket() as listener:
    listener.bind(('127.0.0.1', port))
    listener.listen()
    while True:
        connection, _ = listener.accept()
        connection.close()
'''

RUNNER = '''#!/usr/bin/env python3
import json, os, signal, socket, sys, time
from pathlib import Path
arguments = sys.argv[1:]
port = int(arguments[arguments.index('--port') + 1])
assert arguments[arguments.index('--host') + 1] == '127.0.0.1'
assert port == int(os.environ['SQUISH_PORT'])
with socket.create_connection(('127.0.0.1', port), timeout=1):
    pass
with open('runner.calls', 'a') as calls:
    calls.write(json.dumps(arguments) + '\\n')
if os.environ['FIXTURE_MODE'] == 'crash':
    os.kill(int(Path('server.pid').read_text()), signal.SIGTERM)
    for _ in range(100):
        try:
            with socket.create_connection(('127.0.0.1', port), timeout=0.1):
                pass
        except OSError:
            break
        time.sleep(0.01)
    sys.exit(1)
sys.exit(1 if os.environ['FIXTURE_MODE'] == 'suite-failure' else 0)
'''


class GuiRunnerLifecycleTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        scripts = self.root / 'tests/guitests'
        scripts.mkdir(parents=True)
        shutil.copyfile(Path(__file__).with_name('run_guitests.sh'),
                        scripts / 'run_guitests.sh')
        for suite in ('suite_a', 'suite_b'):
            (scripts / 'gtlab_core_tests' / suite).mkdir(parents=True)
        badge = scripts / 'testing_resources/_badge/generate_badge.py'
        badge.parent.mkdir(parents=True)
        badge.write_text('')
        binaries = self.root / 'bin'
        binaries.mkdir()
        for name, contents in (('squishserver', SERVER), ('squishrunner', RUNNER)):
            binary = binaries / name
            binary.write_text(contents)
            binary.chmod(0o755)
        # Choose an available local port instead of using the real Squish port.
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', 0))
            self.port = listener.getsockname()[1]
        self.environment = dict(os.environ, SQUISH_DIR=str(binaries),
                                SQUISH_PORT=str(self.port),
                                SQUISH_STARTUP_TIMEOUT='1',
                                TMPDIR=str(self.root), DEVTOOLS_DIR='/fixture',
                                QT_DIR_LINUX2='/fixture')

    def run_fixture(self, mode):
        result = subprocess.run(
            ['bash', 'tests/guitests/run_guitests.sh'], cwd=self.root,
            env=dict(self.environment, FIXTURE_MODE=mode),
            capture_output=True, text=True, timeout=10)
        pid_file = self.root / 'server.pid'
        if pid_file.exists():
            with self.assertRaises(ProcessLookupError):
                os.kill(int(pid_file.read_text()), 0)
        calls = self.root / 'runner.calls'
        self.calls = ([json.loads(line) for line in calls.read_text().splitlines()]
                      if calls.exists() else [])
        return result

    def test_ready_server_runs_both_suites_and_is_cleaned_up(self):
        result = self.run_fixture('success')
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        self.assertEqual(len(self.calls), 2)
        for call, suite in zip(self.calls, ('suite_a', 'suite_b')):
            self.assertIn('junit,./gui_tests_junits/junit_' + suite + '.xml', call)

    def test_test_failures_still_run_remaining_suites(self):
        result = self.run_fixture('suite-failure')
        self.assertNotEqual(result.returncode, 0)
        self.assertEqual(len(self.calls), 2)

    def test_failed_startup_preserves_log_and_does_not_run_tests(self):
        result = self.run_fixture('startup-failure')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('fixture server startup failed', result.stderr)
        self.assertEqual(self.calls, [])

    def test_startup_timeout_stops_server_without_running_tests(self):
        result = self.run_fixture('timeout')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('failed to start', result.stderr)
        self.assertEqual(self.calls, [])

    def test_server_crash_stops_remaining_suites(self):
        result = self.run_fixture('crash')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('squishserver stopped before', result.stderr)
        self.assertEqual(len(self.calls), 1)

    def test_occupied_port_is_not_used_or_stopped(self):
        with socket.socket() as listener:
            listener.bind(('127.0.0.1', self.port))
            listener.listen()
            result = self.run_fixture('success')
            self.assertNotEqual(result.returncode, 0)
            self.assertIn('already occupied', result.stderr)
            self.assertEqual(self.calls, [])
            self.assertEqual(listener.getsockname()[1], self.port)

    def test_configuration_failure_does_not_start_server_or_tests(self):
        result = self.run_fixture('config-failure')
        self.assertNotEqual(result.returncode, 0)
        self.assertIn('could not register', result.stderr)
        self.assertEqual(self.calls, [])
        self.assertFalse((self.root / 'server.pid').exists())


if __name__ == '__main__':
    unittest.main()
