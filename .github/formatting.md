<!--
SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
SPDX-License-Identifier: MPL-2.0+
-->

# Formatting check

The `Formatting / clang-format` check uses clang-format 19 and the repository's
`.clang-format`. It checks changed C/C++ lines from the merge-base with the PR's
target branch, including stacked PRs. clang-format may also adjust surrounding
lines in the same statement to produce consistent formatting.

Run the same check locally (replace `origin/master` with the PR target):

```bash
bash .github/scripts/clang-format-diff.sh origin/master HEAD /tmp/clang-format.patch
```

An empty patch means no formatting changes are needed. To apply corrections:

```bash
git apply /tmp/clang-format.patch
```

The script defaults to `clang-format-19` and `git-clang-format-19`. Set
`CLANG_FORMAT_BINARY` and `GIT_CLANG_FORMAT` if they are installed elsewhere.
The check compares committed changes; commit your edits before running it again.

CI uploads a correction patch and fails when formatting changes are needed.
For branches in this repository, reviewdog also posts inline suggestions on
added lines. Fork PRs receive the same check and patch, without review comments.
A failure to post suggestions does not prevent the formatting gate from running.

To block merging on formatting errors, add the `clang-format` check to the
required status checks in the target branch's protection rule or ruleset.
