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
                'platform': 'ubuntu-24.04', 'build_succeeded': True,
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
                     *extra]
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
             '--compiler', 'gcc', '--platform', 'ubuntu-24.04', '--build-succeeded', 'true'], capture_output=True, text=True)
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

    def test_older_successful_target_baseline_is_compared(self):
        result, summary = self.compare(self.report([self.warning()], revision='pr-merge'),
                                       self.report(revision='older-target-build'))
        self.assertEqual(result, 1)
        self.assertNotIn('not verified', summary)
        self.assertIn('older-target-build', summary)

    def test_release_target_is_compared_with_its_own_baseline(self):
        result, summary = self.compare(self.report([self.warning()]), self.report(),
                                       extra=['--target-branch', '2.0.X'])
        self.assertEqual(result, 1)
        self.assertIn('Baseline for <code>2.0.X</code>', summary)
        self.assertNotIn('unsupported target', summary)
        self.assertNotIn('Please rebase', summary)

    def test_unavailable_target_baseline_is_unverified(self):
        with patch.object(cw, 'find_baseline', side_effect=urllib.error.URLError('service unavailable')):
            result, summary = self.compare(self.report([self.warning()]),
                                           extra=['--target-branch', '2.0.X', '--repository', 'owner/repo', '--token', 'read-only'],
                                           use_subprocess=False)
        self.assertEqual(result, 0)
        self.assertIn('Comparison **not verified**', summary)
        self.assertIn('**1 warning(s)**', summary)
        self.assertEqual(list(ET.parse(self.root / 'warnings.xml').getroot()), [])

    def test_mismatched_compiler_is_unverified(self):
        result, summary = self.compare(self.report([self.warning()]), self.report(compiler='clang version 18.1.3'))
        self.assertEqual(result, 0)
        self.assertIn('compiler identity/version differs', summary)

    def test_runner_ubuntu_upgrade_makes_baseline_unverified(self):
        result, summary = self.compare(self.report([self.warning()], platform='ubuntu-26.04'), self.report())
        self.assertEqual(result, 0)
        self.assertIn('baseline Ubuntu version differs', summary)

    def test_report_records_actual_runner_version(self):
        log = self.root / 'empty-build.log'
        log.write_text('build complete\n')
        report = cw.make_report(log, self.root, 'head', 'gcc', 'ubuntu-26.04', True)
        loaded = cw.load_report(self.write_report('actual-runner.json', report))
        self.assertEqual(loaded['platform'], 'ubuntu-26.04')

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
        for metadata in ({'schema_version': 1}, {'platform': 'unsupported'}, {'build_succeeded': 'true'}):
            with self.subTest(metadata=metadata):
                result, summary = self.compare(self.report(), self.report(**metadata))
                self.assertEqual(result, 0)
                self.assertIn('malformed warning report', summary)

    def test_lagging_pr_fails_and_only_then_advises_rebasing_onto_its_target(self):
        for target in ('master', '2.0.X'):
            for behind in (0, 2):
                with self.subTest(target=target, behind=behind), patch.object(cw, 'api_json', return_value={'behind_by': behind}) as api:
                    result, summary = self.compare(self.report([self.warning()], revision='pr-merge'),
                                                   self.report(revision='older-successful-build'),
                                                   extra=['--target-branch', target, '--repository', 'owner/repo',
                                                          '--token', 'read-only', '--head-revision', 'actual-pr-head'],
                                                   use_subprocess=False)
                    self.assertEqual(result, 1)
                    hint = f'Please rebase onto the latest {target} and rerun CI.'
                    self.assertEqual(hint in summary, behind > 0)
                    self.assertIn(f'/compare/{target}...actual-pr-head', api.call_args.args[0])

    def test_no_rebase_hint_for_unchanged_warnings(self):
        with patch.object(cw, 'api_json', side_effect=AssertionError('unnecessary ancestry lookup')):
            result, summary = self.compare(self.report([self.warning()]), self.report([self.warning()]),
                                           extra=['--target-branch', '2.0.X', '--repository', 'owner/repo',
                                                  '--token', 'read-only', '--head-revision', 'head'],
                                           use_subprocess=False)
        self.assertEqual(result, 0)
        self.assertNotIn('Please rebase', summary)

    @staticmethod
    def artifact(identifier=1, branch='master', revision='base', run_id=None, **changes):
        return {'id': identifier, 'created_at': f'2026-10-{identifier:02d}T12:00:00Z',
                'name': cw.REPORT_NAME, 'expired': False, 'size_in_bytes': 100,
                'workflow_run': {'id': identifier if run_id is None else run_id,
                                 'head_branch': branch, 'head_sha': revision}, **changes}

    @staticmethod
    def workflow_run(identifier=1, branch='master', revision='base', **changes):
        return {'id': identifier, 'head_branch': branch, 'head_sha': revision,
                'event': 'push', 'status': 'completed', 'conclusion': 'success',
                'path': '.github/workflows/ci.yml', **changes}

    def discover(self, artifacts, reports, job_conclusion='success', run_changes=None, target='master', runs=None):
        if runs is None:
            candidates = {artifact['workflow_run']['id']: artifact['workflow_run'] for artifact in artifacts}
            runs = [self.workflow_run(identifier, candidate['head_branch'], candidate['head_sha'], **(run_changes or {}))
                    for identifier, candidate in sorted(candidates.items(), reverse=True)]

        def api(url, token):
            page = int(url.rsplit('page=', 1)[1])
            if '/workflows/ci.yml/runs?' in url:
                self.assertNotIn('branch=', url)
                self.assertNotIn('status=', url)
                return {'workflow_runs': runs[(page - 1) * 100:page * 100]}
            if '/actions/artifacts?' in url:
                return {'artifacts': artifacts[(page - 1) * 100:page * 100]}
            if '/jobs?' in url:
                return {'jobs': [{'name': 'Build / Build and test (ubuntu-latest)',
                                  'status': 'completed', 'conclusion': job_conclusion}]}
            raise AssertionError(f'Unexpected API request: {url}')

        def download(url, token, destination):
            identifier = int(url.split('/')[-2])
            report = reports[identifier]
            if isinstance(report, bytes):
                destination.write_bytes(report)
                return
            with zipfile.ZipFile(destination, 'w') as archive:
                archive.writestr('compiler-warnings.json', json.dumps(report))

        with patch.object(cw, 'api_json', side_effect=api) as api_mock, patch.object(cw, 'download_artifact', side_effect=download) as download_mock:
            result = cw.find_baseline('owner/repo', 'read-only', self.root / 'download.zip', self.report(), target)
            return result, api_mock, download_mock

    def test_run_pagination_continues_beyond_one_thousand_entries(self):
        runs = [self.workflow_run(event='pull_request') for _ in range(1000)] + [self.workflow_run(2)]
        result, api, _ = self.discover([self.artifact(2)], {2: self.report()}, runs=runs)
        self.assertIsNotNone(result)
        self.assertTrue(any('/workflows/ci.yml/runs?' in call.args[0] and 'page=11' in call.args[0]
                            for call in api.call_args_list))

    def test_artifact_pagination_continues_beyond_one_thousand_entries(self):
        artifacts = [self.artifact(expired=True) for _ in range(1000)] + [self.artifact(2)]
        result, api, _ = self.discover(artifacts, {2: self.report()})
        self.assertIsNotNone(result)
        self.assertTrue(any('/actions/artifacts?' in call.args[0] and 'page=11' in call.args[0]
                            for call in api.call_args_list))

    def test_latest_successful_run_wins_over_artifact_upload_order(self):
        artifacts = [self.artifact(1, revision='old', created_at='2026-10-20T12:00:00Z'),
                     self.artifact(2, revision='new', created_at='2026-10-19T12:00:00Z')]
        runs = [self.workflow_run(2, revision='new'), self.workflow_run(1, revision='old')]
        result, _, download = self.discover(artifacts, {1: self.report(revision='old'), 2: self.report(revision='new')}, runs=runs)
        self.assertEqual(result['revision'], 'new')
        self.assertEqual(len(download.call_args_list), 1)
        self.assertIn('/artifacts/2/zip', download.call_args.args[0])

    def test_pending_master_build_uses_previous_successful_report(self):
        for has_artifact in (False, True):
            with self.subTest(has_artifact=has_artifact):
                artifacts = [self.artifact(1, revision='previous-success')]
                if has_artifact:
                    artifacts.append(self.artifact(2, revision='latest-pending'))
                runs = [self.workflow_run(2, revision='latest-pending', status='in_progress', conclusion=None),
                        self.workflow_run(1, revision='previous-success')]
                result, _, download = self.discover(artifacts, {1: self.report(revision='previous-success')}, runs=runs)
                self.assertEqual(result['revision'], 'previous-success')
                self.assertEqual(len(download.call_args_list), 1)
                self.assertIn('/artifacts/1/zip', download.call_args.args[0])

    def test_release_target_never_uses_a_master_baseline(self):
        artifacts = [self.artifact(2, revision='master-tip'),
                     self.artifact(1, branch='2.0.X', revision='release-tip')]
        runs = [self.workflow_run(2, revision='master-tip'), self.workflow_run(1, branch='2.0.X', revision='release-tip')]
        result, api, download = self.discover(artifacts, {1: self.report(revision='release-tip')}, target='2.0.X', runs=runs)
        self.assertEqual(result['revision'], 'release-tip')
        self.assertFalse(any('/runs/2/jobs' in call.args[0] for call in api.call_args_list))
        self.assertEqual(len(download.call_args_list), 1)
        result, _, download = self.discover([self.artifact()], {1: self.report()}, target='2.0.X')
        self.assertIsNone(result)
        download.assert_not_called()

    def test_discovery_skips_invalid_newer_reports_and_accepts_prefixed_ubuntu_job_name(self):
        artifacts = [self.artifact(i) for i in (1, 3, 2, 4, 5)]
        reports = {1: self.report(), 2: self.report([self.warning(message=None)]),
                   3: self.report(compiler='different compiler'), 4: b'not a zip',
                   5: self.report(build_succeeded=False)}
        result, _, download = self.discover(artifacts, reports)
        self.assertEqual(result, self.report())
        self.assertEqual(len(download.call_args_list), 5)
        self.assertIn('/artifacts/5/zip', download.call_args_list[0].args[0])

    def test_discovery_rejects_expired_foreign_and_failed_build_artifacts(self):
        for artifacts, conclusion, changes in (
            ([self.artifact(expired=True)], 'success', None),
            ([self.artifact(branch='2.0.X')], 'success', None),
            ([self.artifact()], 'failure', None),
            ([self.artifact()], None, None),
            ([self.artifact()], 'success', {'event': 'pull_request'}),
            ([self.artifact()], 'success', {'status': 'in_progress'}),
            ([self.artifact()], 'success', {'conclusion': 'failure'}),
        ):
            with self.subTest(artifacts=artifacts, conclusion=conclusion, changes=changes):
                result, _, download = self.discover(artifacts, {1: self.report()}, conclusion, changes)
                self.assertIsNone(result)
                download.assert_not_called()

    def test_report_revision_must_match_its_successful_run(self):
        result, _, _ = self.discover([self.artifact(revision='target-build')], {1: self.report(revision='different-build')})
        self.assertIsNone(result)

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
