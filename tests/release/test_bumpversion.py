"""Smoke-test bump2version release transitions against the real version files."""

import shutil
import subprocess
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
RELEASE_FILES = (
    ".bumpversion.cfg",
    "CMakeLists.txt",
    "CHANGELOG.md",
    "conanfile.py",
    "docs/dev/conf.py",
    "docs/user/conf.py",
    "src/dataprocessor/gt_version.h",
)


class BumpVersionTest(unittest.TestCase):
    def setUp(self):
        self.assertIsNotNone(shutil.which("bump2version"), "Install bump2version==1.0.1")
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.workdir = Path(self.tmp.name)
        for name in RELEASE_FILES:
            dest = self.workdir / name
            dest.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(ROOT / name, dest)
        self.original_changelog = (self.workdir / "CHANGELOG.md").read_bytes()

    def bump(self, *args):
        subprocess.run(
            ["bump2version", "--no-commit", "--no-tag", *args],
            cwd=self.workdir,
            capture_output=True,
            text=True,
            check=True,
        )

    def check_version(self, version, cmake_version):
        config = (self.workdir / ".bumpversion.cfg").read_text()
        self.assertIn("current_version = " + version, config)
        self.assertIn('version = "' + version + '"', (self.workdir / "conanfile.py").read_text())
        self.assertIn('#define GT_VERSION_STRING "' + version + '"', (self.workdir / "src/dataprocessor/gt_version.h").read_text())
        for path in ("docs/dev/conf.py", "docs/user/conf.py"):
            self.assertIn('release = "' + version + '"', (self.workdir / path).read_text())
        self.assertIn("project(GTlab-Core VERSION " + cmake_version + ")", (self.workdir / "CMakeLists.txt").read_text())
        self.assertEqual(self.original_changelog, (self.workdir / "CHANGELOG.md").read_bytes())

    def test_alpha_beta_rc_final_and_patch(self):
        steps = (
            (("--new-version", "2.1.0-alpha.1", "stage"), "2.1.0-alpha.1"),
            (("serial",), "2.1.0-alpha.2"),
            (("--new-version", "2.1.0-beta.1", "stage"), "2.1.0-beta.1"),
            (("--new-version", "2.1.0-rc.1", "stage"), "2.1.0-rc.1"),
            (("stage",), "2.1.0"),
            (("patch",), "2.1.1"),
            (("patch",), "2.1.2"),
        )
        for args, version in steps:
            with self.subTest(version=version):
                self.bump(*args)
                self.check_version(version, version.split("-")[0])

    def test_opt_in_to_patch_prerelease(self):
        self.bump("--new-version", "2.1.0", "stage")
        self.bump("--new-version", "2.1.1-alpha.1", "patch")
        self.check_version("2.1.1-alpha.1", "2.1.1")
        self.bump("serial")
        self.check_version("2.1.1-alpha.2", "2.1.1")
        self.bump("--new-version", "2.1.1-rc.1", "stage")
        self.bump("stage")
        self.check_version("2.1.1", "2.1.1")


if __name__ == "__main__":
    unittest.main()
