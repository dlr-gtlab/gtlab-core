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

The check uses the PR target branch from `pull_request.base.ref`, including
`master` and `2.0.X`. It selects the newest successful push run of `ci.yml` on
that branch with a successful Ubuntu job and a valid, compatible warning report,
then compares it with the current PR merge build. If the newest target-branch
build is pending, failed or has no usable report, discovery falls back to the
newest available successful build with a usable report on the same branch.

Discovery indexes retained warning artifacts and paginates workflow runs newest
first, filtering branches and statuses locally to avoid the 1,000-result API
search limit. Reports from other target branches or failed/incomplete builds
are rejected. Each report's revision must match its own workflow run's source
revision; it need not match `pull_request.base.sha`, the current target-branch
tip or a merge base. No additional or historical builds are performed.

A compatible report must use the same Ubuntu version, compiler identity/version
and report schema. If `ubuntu-latest` moves to a different Ubuntu version,
comparisons remain not verified until a compatible target-branch report exists.

The comparison matches normalized source paths, warning options and messages,
ignoring line numbers. Repeated diagnostics are compared by occurrence; new
occurrences retain current source locations. Summaries list new, existing and
resolved warnings. Warning comparison and best-effort reviewdog annotations run
**after** the existing tests and binary artifact upload. A final step fails the
job for new warnings, broken analysis or a failed warning-report upload.

When a compatible report is missing, expired, malformed or unavailable, the
summary says **comparison not verified** and requests manual review without
blocking the PR. New warnings fail the gate even when the PR is behind its target
branch. In that case, the summary also says `Please rebase onto the latest
<target branch> and rerun CI`. The hint appears only when new warnings were found
and the PR head is actually behind the current target-branch tip.

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
