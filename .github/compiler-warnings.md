<!--
SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
SPDX-License-Identifier: MPL-2.0+
-->

# Compiler warning check

The Ubuntu leg of **Build and test** records the normal compilation output
without another build. Dependency installation and CMake configuration remain
shared with Windows; only Ubuntu compilation uses `tee` and `pipefail`, preserving
the compiler's exit status. The runner remains `ubuntu-latest`; the report records
the actual Ubuntu version from `/etc/os-release`.

After compilation, `compiler-warnings.json` is uploaded as the
`compiler-warnings-ubuntu` artifact for 60 days, subject to repository retention
limits. Schema version 2 records the source revision, compiler identity/version,
platform, compilation success, and each diagnostic's file, line, column, message,
and warning option. Failed compilations can produce reports for inspection, but
these reports are never accepted as baselines.

For PRs targeting `master`, the check enumerates all retained warning artifacts,
newest first, and chooses the latest valid, compatible report from a completed
`master` push run of `ci.yml` with a successful Ubuntu job. Discovery paginates
artifacts to exhaustion, avoiding the 1,000-result workflow-search limit. A
compatible report must represent the **exact PR base revision** and use the same
Ubuntu version, compiler identity/version and report schema. If `ubuntu-latest`
moves to a different Ubuntu version, comparisons remain not verified until a
compatible master report is available. An older report can otherwise flag
warnings already present on the PR base as new.

The comparison matches normalized source paths, warning options and messages,
ignoring line numbers. Repeated diagnostics are compared by occurrence; new
occurrences retain current source locations. Summaries list new, existing and
resolved warnings. Warning comparison and best-effort reviewdog annotations run
**after** the existing tests and binary artifact upload. A final step fails the
job for new warnings, broken analysis or a failed warning-report upload.

When a compatible report is missing, expired, malformed or unavailable, the
summary says **comparison not verified** and requests manual review without
blocking the PR. PRs targeting `2.0.X` still publish their reports, but comparison
is **not verified / unsupported target**. Enforcement currently supports `master`
only. A detected regression advises rebasing only when the PR head is actually
behind its target branch.

The build job has only `contents: read` and `actions: read`; checkout credentials
are not persisted and no secrets are inherited by the build workflow. Reviewdog
uses the local `github-annotations` reporter with a read-only token for diff
lookup. It does not post PR review comments or require a write token, and works
for fork PRs. Annotation failures do not change the comparison result.

The parser supports GCC/Clang text diagnostics in `file:line:column: warning:`
and `file:line: warning:` form, including headers, ANSI colors, warning options,
and include-stack/source/note context. Missing columns are recorded as zero.
Ubuntu compilation uses `LC_ALL=C.UTF-8` to keep compiler messages in English.
The parser was checked against the Ubuntu/Ninja job of PR #1635 (CI run
39425512709, job 118636943309): all 25 compiler warning occurrences were captured.
Representative excerpts are retained as a regression fixture. Diagnostics with
no source line (for example linker or compiler-driver warnings), MSVC output and
localized diagnostics are not supported. Unrecognized `warning:` diagnostics
fail analysis explicitly instead of producing an incomplete or misleading zero
report. CMake configuration warnings and runtime warnings are outside the
compiler-log scope.

To inspect a report, open the workflow run in the Actions tab and download
`compiler-warnings-ubuntu`. The Ubuntu job summary shows counts and up to 100
locations per category; the JSON contains all occurrences. To run the focused
regression tests locally:

```sh
python3 -m unittest discover -s .github/scripts -p 'test_*.py' -v
```
