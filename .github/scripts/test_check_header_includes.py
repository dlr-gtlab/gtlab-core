"""Tests for check_header_includes.py: python -m unittest -v test_check_header_includes.py"""

import contextlib
import io
import json
import tempfile
import unittest
from pathlib import Path

from check_header_includes import collect_headers, find_issues, main


class HeaderIncludeChecks(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.src = Path(self.tmp.name) / "src"
        self.src.mkdir()

    def write(self, path, content):
        dest = self.src / path
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_text(content, encoding="utf-8")
        return dest

    def test_includes_and_comments(self):
        self.write("properties/gt_doublelistproperty.h", "#pragma once\n")
        self.write("other/gt_object.h", '''\
#include "gt_doublelistproperty.h"
# include "properties/gt_doublelistproperty.h"
#include <../properties/gt_doublelistproperty.h>
#include "..\\properties\\gt_doublelistproperty.h"
// #include "properties/gt_doublelistproperty.h"
/*
#include "properties/gt_doublelistproperty.h"
*/
#include <QtCore/QObject>
''')
        results = list(find_issues(collect_headers(self.src, [])))
        self.assertEqual([issue[1] for issue in results], [2, 3, 4])
        self.assertTrue(all(issue[-1] == "gt_doublelistproperty.h" for issue in results))

    def test_exclusions_apply_to_both_index_and_scan(self):
        self.write("thirdparty/gt_vendor.h", "#include \"properties/gt_property.h\"\n")
        self.write("properties/gt_property.h", "#pragma once\n")
        self.write("other/gt_object.h", '#include "thirdparty/gt_vendor.h"\n')
        self.write("internal/gt_secret.h", "#pragma once\n")
        self.write("other/gt_other.h", '#include "internal/gt_secret.h"\n')
        headers = collect_headers(self.src, ["internal/**", "**/thirdparty/**"])
        self.assertEqual(len(list(find_issues(headers))), 0)
        self.assertEqual(len(headers), 3)

    def test_cli_rdjsonl_and_exit_codes(self):
        self.write("properties/gt_foo.h", "#pragma once\n")
        self.write("gt_bar.h", '#include "properties/gt_foo.h"\n')
        stdout, stderr = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(stdout), contextlib.redirect_stderr(stderr):
            code = main(["--root", str(self.src), "--format", "rdjsonl"])
        self.assertEqual(code, 0)
        diagnostics = [json.loads(s) for s in stdout.getvalue().splitlines()]
        self.assertEqual(len(diagnostics), 1)
        self.assertEqual(diagnostics[0]["severity"], "ERROR")
        self.assertEqual(diagnostics[0]["code"]["value"], "GTLAB001")
        self.assertEqual(diagnostics[0]["suggestions"][0]["text"], "gt_foo.h")
        with contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(main(["--root", str(self.src)]), 1)

    def test_no_header_is_configuration_error(self):
        with contextlib.redirect_stderr(io.StringIO()):
            self.assertEqual(main(["--root", str(self.src)]), 2)

    def test_json_config_sets_source_root_and_excludes(self):
        config_dir = Path(self.tmp.name) / "config"
        config_root = config_dir / "headers"
        config_root.mkdir(parents=True)
        (config_root / "gt_public.h").write_text("#pragma once\n", encoding="utf-8")
        (config_root / "gt_wrapper.h").write_text(
            '#include "internal/gt_private.h"\n', encoding="utf-8"
        )
        internal = config_root / "internal"
        internal.mkdir()
        (internal / "gt_private.h").write_text("#pragma once\n", encoding="utf-8")
        config = config_dir / "header-includes.json"
        config.write_text(
            json.dumps(
                {"source_root": "headers", "exclude": ["internal/**", "**/thirdparty/**"]}
            ),
            encoding="utf-8",
        )

        stdout, stderr = io.StringIO(), io.StringIO()
        with contextlib.redirect_stdout(stdout), contextlib.redirect_stderr(stderr):
            code = main(["--config", str(config)])

        self.assertEqual(code, 0)
        self.assertIn("Checked 2 headers; found 0 invalid includes.", stderr.getvalue())


if __name__ == "__main__":
    unittest.main()
