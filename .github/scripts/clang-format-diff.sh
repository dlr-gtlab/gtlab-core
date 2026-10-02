#!/usr/bin/env bash
# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
# SPDX-License-Identifier: MPL-2.0+

set -euo pipefail

# Usage: clang-format-diff.sh <target-ref> <head-ref> <output-patch>
# Override these paths to use the same tools locally as in CI.
format_binary=${CLANG_FORMAT_BINARY:-clang-format-19}
format_driver=${GIT_CLANG_FORMAT:-git-clang-format-19}
format_base=$(git merge-base "$1" "$2")
format_output=$(mktemp)
trap 'rm -f "$format_output"' EXIT

format_status=0
"$format_driver" --binary="$format_binary" --style=file \
    --extensions=c,cc,cpp,cxx,h,hh,hpp,hxx \
    --diff "$format_base" "$2" > "$format_output" || format_status=$?

# git-clang-format 19 returns 1 when it produces a formatting diff.
if (( format_status > 1 )); then
    cat "$format_output" >&2
    exit "$format_status"
fi

if [[ $(head -n 1 "$format_output") == 'diff --git '* ]]; then
    cp "$format_output" "$3"
elif grep -qxE 'no modified files to format|clang-format did not modify any files' "$format_output"; then
    : > "$3"
else
    cat "$format_output" >&2
    echo 'Unexpected git-clang-format output.' >&2
    exit 2
fi
