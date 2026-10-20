# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
# SPDX-License-Identifier: MPL-2.0+

import contextlib
import io
import json
import os
import subprocess
import sys
import tempfile
import unittest
import urllib.error
import xml.etree.ElementTree as ET
import zipfile
from pathlib import Path
from unittest.mock import MagicMock, patch

import compiler_warnings as cw


SCRIPT = Path(cw.__file__).resolve()
RUNNER_ROOT = Path('/home/runner/work/gtlab-core/gtlab-core')


class CompilerWarningTests(unittest.TestCase):
    def setUp(self):
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary_directory.cleanup)
        self.root = Path(self.temporary_directory.name)

    @staticmethod
    def warning(line=10, message='unused variable', **changes):
        return {'file': 'src/file.cpp', 'line': line, 'column': 4,
                'message': message, 'option': '-Wunused-variable', **changes}

    @staticmethod
    def report(warnings=(), **changes):
        return {'schema_version': cw.SCHEMA_VERSION, 'revision': 'base',
                'compiler': 'c++ (Ubuntu 13.3.0-6ubuntu2) 13.3.0',
                'platform': cw.PLATFORM, 'build_succeeded': True,
                'warnings': list(warnings), **changes}

    def write_report(self, name, report):
        path = self.root / name
        path.write_text(json.dumps(report), encoding='utf-8')
        return path

    def compare(self, current, baseline=None, extra=(), use_subprocess=True):
        current_path = self.write_report('current.json', current)
        arguments = ['compare', '--current', str(current_path),
                     '--summary', str(self.root / 'summary.md'),
                     '--checkstyle', str(self.root / 'warnings.xml'),
                     '--base-revision', 'base', *extra]
        if baseline is not None:
            arguments += ['--baseline', str(self.write_report('baseline.json', baseline))]
        environment = {**os.environ, 'GITHUB_TOKEN': '', 'GITHUB_REPOSITORY': '',
                       'GITHUB_OUTPUT': str(self.root / 'outputs.txt')}
        if use_subprocess:
            completed = subprocess.run([sys.executable, str(SCRIPT), *arguments],
                                       env=environment, capture_output=True, text=True)
            result = completed.returncode
        else:
            with patch.dict(os.environ, environment), patch('sys.argv', [str(SCRIPT), *arguments]), contextlib.redirect_stdout(io.StringIO()):
                result = cw.main()
        summary = self.root / 'summary.md'
        return result, summary.read_text() if summary.exists() else ''

    def parse(self, text):
        log = self.root / 'build.log'
        log.write_text(text, encoding='utf-8')
        return cw.parse_log(log, self.root)

    def test_real_ubuntu_ninja_log_including_notes_and_repeated_diagnostics(self):
        fixture = SCRIPT.parent / 'fixtures' / 'compiler-warnings-ubuntu.log'
        warnings = cw.parse_log(fixture, RUNNER_ROOT)
        self.assertEqual(len(warnings), 4)
        self.assertEqual(warnings[0]['file'], 'src/dataprocessor/gt_qtutilities.cpp')
        self.assertEqual(warnings[0]['line'], 21)
        self.assertEqual(warnings[0]['option'], '-Wdeprecated-declarations')
        self.assertEqual(warnings[-1], warnings[-2])

    def test_columnless_header_and_colored_clang_formats(self):
        warnings = self.parse(
            'In file included from src/file.cpp:1:\n'
            f'{self.root}/include/../include/api.h:17: warning:   obsolete   API [-Wdeprecated-declarations]\n'
            '\x1b[1m\x1b[Ksrc/file.cpp:21:3: \x1b[0mwarning: unused parameter [-Wunused-parameter]\n'
            'include/config.h:8:2: warning: #warning configuration fallback [-Wcpp]\n'
            '   8 | #warning configuration fallback\n'
            'include/api.h:1:2: note: declared here\n'
        )
        self.assertEqual(len(warnings), 3)
        self.assertEqual(warnings[0]['file'], 'include/api.h')
        self.assertEqual(warnings[0]['column'], 0)
        self.assertEqual(warnings[0]['message'], 'obsolete API')
        self.assertEqual(warnings[1]['column'], 3)
        self.assertEqual(warnings[1]['file'], 'src/file.cpp')

    def test_unsupported_diagnostics_fail_analysis_instead_of_reporting_zero(self):
        for diagnostic in ['cc1plus: warning: unsupported flag',
                           'warning: unsupported flag', '/usr/bin/ld: warning: missing stack note']:
            with self.subTest(diagnostic=diagnostic):
                with self.assertRaisesRegex(ValueError, 'Unrecognized warning'):
                    self.parse(diagnostic + '\n')
        log = self.root / 'build.log'
        output = self.root / 'bad-report.json'
        completed = subprocess.run(
            [sys.executable, str(SCRIPT), 'analyze', '--log', str(log),
             '--root', str(self.root), '--output', str(output), '--revision', 'head',
             '--compiler', 'gcc', '--build-succeeded', 'true'], capture_output=True, text=True)
        self.assertEqual(completed.returncode, 2)
        self.assertFalse(output.exists())

    def test_line_changes_preserve_identity(self):
        changes = cw.compare_reports(self.report([self.warning(10)]), self.report([self.warning(99)]))
        self.assertEqual([len(changes[key]) for key in ('new', 'existing', 'resolved')], [0, 1, 0])
        self.assertEqual(changes['existing'][0]['line'], 99)

    def test_changed_identities_are_detected_with_unchanged_total(self):
        changes = cw.compare_reports(self.report([self.warning()]), self.report([self.warning(message='unused parameter')]))
        self.assertEqual([len(changes[key]) for key in ('new', 'existing', 'resolved')], [1, 0, 1])

    def test_repeated_diagnostics_count_occurrences(self):
        baseline = self.report([self.warning(), self.warning(), self.warning()])
        current = self.report([self.warning(99), self.warning(100)])
        changes = cw.compare_reports(baseline, current)
        self.assertEqual([len(changes[key]) for key in ('new', 'existing', 'resolved')], [0, 2, 1])

    def test_new_occurrence_end_to_end_emits_current_location_xml_outputs_and_failure(self):
        result, summary = self.compare(self.report([self.warning(100), self.warning(101)], revision='head'),
                                       self.report([self.warning(10)]))
        self.assertEqual(result, 1)
        self.assertIn('New: **1**', summary)
        self.assertIn('Existing: **1**', summary)
        diagnostic = ET.parse(self.root / 'warnings.xml').find('file/error')
        self.assertEqual(diagnostic.attrib['line'], '101')
        self.assertEqual(diagnostic.attrib['column'], '4')
        self.assertEqual((self.root / 'outputs.txt').read_text(), 'new_warnings=1\n')

    def test_resolved_warnings_pass_with_empty_checkstyle(self):
        result, summary = self.compare(self.report(), self.report([self.warning()]))
        self.assertEqual(result, 0)
        self.assertIn('Resolved: **1**', summary)
        self.assertEqual(list(ET.parse(self.root / 'warnings.xml').getroot()), [])

    def test_missing_baseline_is_unverified(self):
        result, summary = self.compare(self.report([self.warning()]))
        self.assertEqual(result, 0)
        self.assertIn('Comparison **not verified**', summary)
        self.assertEqual((self.root / 'outputs.txt').read_text(), 'new_warnings=0\n')

    def test_stale_master_baseline_is_unverified(self):
        result, summary = self.compare(self.report([self.warning()]), self.report(revision='older-master'))
        self.assertEqual(result, 0)
        self.assertIn('not verified', summary)
        self.assertIn('does not represent the PR base revision', summary)

    def test_release_target_is_unsupported_and_never_queries_master_or_advises_rebase(self):
        with patch.object(cw, 'api_json', side_effect=AssertionError('unexpected master request')):
            result, summary = self.compare(self.report([self.warning()]), self.report(),
                                           extra=['--target-branch', '2.0.X', '--head-revision', 'head',
                                                  '--repository', 'owner/repo', '--token', 'read-only'],
                                           use_subprocess=False)
        self.assertEqual(result, 0)
        self.assertIn('unsupported target: 2.0.X', summary)
        self.assertNotIn('Please rebase', summary)

    def test_mismatched_compiler_is_unverified(self):
        result, summary = self.compare(self.report([self.warning()]), self.report(compiler='clang version 18.1.3'))
        self.assertEqual(result, 0)
        self.assertIn('compiler identity/version differs', summary)

    def test_failed_or_incomplete_build_is_never_compared(self):
        result, summary = self.compare(self.report([self.warning()]), self.report(build_succeeded=False))
        self.assertEqual(result, 0)
        self.assertIn('incomplete or failed build', summary)
        result, summary = self.compare(self.report(build_succeeded=False), self.report())
        self.assertEqual(result, 0)
        self.assertIn('PR compilation was incomplete or failed', summary)

    def test_malformed_baseline_warning_fields_are_unverified(self):
        invalid = [('message', None), ('file', []), ('option', 4), ('line', '10'),
                   ('line', True), ('line', 0), ('column', -1), ('column', None)]
        for field, value in invalid:
            with self.subTest(field=field, value=value):
                baseline = self.report([self.warning(**{field: value})])
                result, summary = self.compare(self.report(), baseline)
                self.assertEqual(result, 0)
                self.assertIn('malformed warning report', summary)

    def test_broken_current_report_is_an_analysis_failure(self):
        result, _ = self.compare(self.report([self.warning(message=None)]), self.report())
        self.assertEqual(result, 2)

    def test_incompatible_baseline_schema_or_platform_is_unverified(self):
        for metadata in ({'schema_version': 1}, {'platform': 'ubuntu-22.04'}, {'build_succeeded': 'true'}):
            with self.subTest(metadata=metadata):
                result, summary = self.compare(self.report(), self.report(**metadata))
                self.assertEqual(result, 0)
                self.assertIn('malformed warning report', summary)

    def test_rebase_hint_uses_pr_head_ancestry_instead_of_base_snapshot(self):
        for behind in (0, 2):
            with self.subTest(behind=behind), patch.object(cw, 'api_json', return_value={'behind_by': behind}) as api:
                result, summary = self.compare(self.report([self.warning()]), self.report(),
                                               extra=['--repository', 'owner/repo', '--token', 'read-only',
                                                      '--head-revision', 'actual-pr-head'], use_subprocess=False)
                self.assertEqual(result, 1)
                self.assertEqual('Please rebase onto the latest master and rerun the checks.' in summary, behind > 0)
                self.assertIn('/compare/master...actual-pr-head', api.call_args.args[0])

    @staticmethod
    def artifact(identifier=1, **changes):
        return {'id': identifier, 'created_at': f'2026-10-{identifier:02d}T12:00:00Z',
                'name': cw.REPORT_NAME, 'expired': False, 'size_in_bytes': 100,
                'workflow_run': {'id': identifier, 'head_branch': 'master', 'head_sha': 'base'}, **changes}

    def discover(self, artifacts, reports, job_conclusion='success', run_changes=None):
        def api(url, token):
            if '/actions/artifacts?' in url:
                page = int(url.rsplit('page=', 1)[1])
                return {'artifacts': artifacts[(page - 1) * 100:page * 100]}
            if '/jobs?' in url:
                return {'jobs': [{'name': 'Build / Build and test (ubuntu-24.04)',
                                  'status': 'completed', 'conclusion': job_conclusion}]}
            identifier = int(url.rsplit('/', 1)[1])
            return {'id': identifier, 'head_branch': 'master', 'head_sha': 'base',
                    'event': 'push', 'status': 'completed', 'path': '.github/workflows/ci.yml',
                    **(run_changes or {})}

        def download(url, token, destination):
            identifier = int(url.split('/')[-2])
            report = reports[identifier]
            if isinstance(report, bytes):
                destination.write_bytes(report)
                return
            with zipfile.ZipFile(destination, 'w') as archive:
                archive.writestr('compiler-warnings.json', json.dumps(report))

        with patch.object(cw, 'api_json', side_effect=api) as api_mock, patch.object(cw, 'download_artifact', side_effect=download) as download_mock:
            result = cw.find_baseline('owner/repo', 'read-only', self.root / 'download.zip', self.report(), 'base')
            return result, api_mock, download_mock

    def test_artifact_pagination_continues_beyond_one_thousand_entries(self):
        artifacts = [self.artifact(expired=True) for _ in range(1000)] + [self.artifact(2)]
        result, api, _ = self.discover(artifacts, {2: self.report()})
        self.assertIsNotNone(result)
        self.assertTrue(any('page=11' in call.args[0] for call in api.call_args_list))

    def test_discovery_skips_invalid_newer_reports_and_accepts_prefixed_ubuntu_job_name(self):
        artifacts = [self.artifact(i) for i in (1, 3, 2, 4, 5)]
        reports = {1: self.report(), 2: self.report([self.warning(message=None)]),
                   3: self.report(compiler='different compiler'), 4: b'not a zip',
                   5: self.report(build_succeeded=False)}
        result, _, download = self.discover(artifacts, reports)
        self.assertEqual(result, self.report())
        self.assertEqual(len(download.call_args_list), 5)
        self.assertIn('/artifacts/5/zip', download.call_args_list[0].args[0])

    def test_discovery_rejects_expired_stale_and_failed_build_artifacts(self):
        for artifacts, conclusion, changes in (
            ([self.artifact(expired=True)], 'success', None),
            ([self.artifact(workflow_run={'id': 1, 'head_branch': 'master', 'head_sha': 'old'})], 'success', None),
            ([self.artifact()], 'failure', None),
            ([self.artifact()], None, None),
            ([self.artifact()], 'success', {'event': 'pull_request'}),
            ([self.artifact()], 'success', {'status': 'in_progress'}),
        ):
            with self.subTest(artifacts=artifacts, conclusion=conclusion, changes=changes):
                result, _, download = self.discover(artifacts, {1: self.report()}, conclusion, changes)
                self.assertIsNone(result)
                download.assert_not_called()

    def test_artifact_storage_redirect_receives_no_repository_token(self):
        redirect = urllib.error.HTTPError('https://api.github.com/download', 302, 'Found',
                                          {'Location': 'https://storage.example/report.zip'}, None)
        opener = MagicMock()
        opener.open.side_effect = redirect
        response = MagicMock()
        response.__enter__.return_value.read.return_value = b'zip bytes'
        with patch.object(cw.urllib.request, 'build_opener', return_value=opener), patch.object(cw.urllib.request, 'urlopen', return_value=response) as storage:
            cw.download_artifact('https://api.github.com/download', 'private-read-token', self.root / 'artifact.zip')
        self.assertEqual(storage.call_args.args, ('https://storage.example/report.zip',))
        self.assertEqual((self.root / 'artifact.zip').read_bytes(), b'zip bytes')


if __name__ == '__main__':
    unittest.main()
