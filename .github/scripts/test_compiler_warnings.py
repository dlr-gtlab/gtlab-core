import json
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

import compiler_warnings


class CompilerWarningTests(unittest.TestCase):
    def test_parses_and_normalizes_gcc_diagnostics(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            source = root / "src" / "file.cpp"
            source.parent.mkdir()
            source.write_text("")
            log = root / "build.log"
            log.write_text(
                f"{source}:17:4: warning:   unused   variable  [-Wunused-variable]\n"
                "unrelated compiler output\n"
            )

            warnings = compiler_warnings.parse_log(log, root)

        self.assertEqual(
            warnings,
            [{"file": "src/file.cpp", "line": 17, "column": 4, "message": "unused variable", "option": "-Wunused-variable"}],
        )

    def test_line_number_changes_do_not_create_new_warning(self):
        baseline = {"warnings": [self.warning(line=10)]}
        current = {"warnings": [self.warning(line=99)]}

        changes = compiler_warnings.compare_reports(baseline, current)

        self.assertEqual((len(changes["new"]), len(changes["existing"]), len(changes["resolved"])), (0, 1, 0))

    def test_changed_warning_identity_is_new_even_when_total_is_unchanged(self):
        baseline = {"warnings": [self.warning(message="unused variable") ]}
        current = {"warnings": [self.warning(message="unused parameter") ]}

        changes = compiler_warnings.compare_reports(baseline, current)

        self.assertEqual(len(changes["new"]), 1)
        self.assertEqual(len(changes["resolved"]), 1)
        self.assertEqual(len(changes["existing"]), 0)

    def test_repeated_diagnostics_are_compared_by_occurrence(self):
        baseline = {"warnings": [self.warning(), self.warning()]}
        current = {"warnings": [self.warning()]}

        changes = compiler_warnings.compare_reports(baseline, current)

        self.assertEqual((len(changes["new"]), len(changes["existing"]), len(changes["resolved"])), (0, 1, 1))

    def test_report_validation_rejects_incompatible_schema(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            report_path = Path(temporary_directory) / "warnings.json"
            report_path.write_text(json.dumps({"schema_version": 2}))

            with self.assertRaises(ValueError):
                compiler_warnings.load_report(report_path)

    def test_missing_baseline_is_reported_as_unverified_without_failing(self):
        with tempfile.TemporaryDirectory() as temporary_directory:
            root = Path(temporary_directory)
            current = root / "current.json"
            current.write_text(json.dumps({"schema_version": 1, "revision": "abc", "compiler": "gcc", "platform": "ubuntu-24.04", "warnings": [self.warning()]}))
            summary = root / "summary.md"
            checkstyle = root / "new-warnings.xml"
            arguments = ["compiler_warnings.py", "compare", "--current", str(current), "--summary", str(summary), "--checkstyle", str(checkstyle)]

            with patch("sys.argv", arguments):
                result = compiler_warnings.main()

            self.assertEqual(result, 0)
            self.assertIn("Comparison **not verified**", summary.read_text())
            self.assertTrue(checkstyle.exists())

    @staticmethod
    def warning(line=10, message="unused variable"):
        return {"file": "src/file.cpp", "line": line, "column": 4, "message": message, "option": "-Wunused-variable"}


if __name__ == "__main__":
    unittest.main()
