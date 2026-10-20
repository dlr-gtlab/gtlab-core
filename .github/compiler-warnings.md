# Compiler warning check

The Ubuntu leg of the existing **Build and test** job records compiler output
while it performs the normal build. The workflow parses GCC/Clang warning
diagnostics into `compiler-warnings.json`, including the compiler, runner, and
source revision, then uploads the report as the `compiler-warnings-ubuntu`
artifact for 60 days. The build log is piped through `tee` with `pipefail`, so
the build's exit status remains authoritative.

For pull requests, the check looks through `master` push runs of the CI
workflow, newest first, and selects the latest one with a successful Ubuntu
build and a non-expired, structurally valid warning report matching that run's
source revision. It compares normalized
source paths, warning options, and messages; source line numbers do not decide
whether a warning is new. Repeated diagnostics are counted by occurrence.
New warnings fail the Ubuntu job, unchanged warnings are allowed, and resolved
warnings are shown as improvements. If there is no usable baseline, the PR job
passes this comparison with **not verified** in its summary so the report can
be reviewed manually. A failed analyzer is reported separately and does not
become an empty report.

Warnings introduced on the PR are sent to reviewdog when GitHub allows review
comments. Failure to publish annotations does not alter the comparison result;
fork pull requests still receive the comparison and job summary.

To inspect a report, open the workflow run in the Actions tab, expand
**Build and test**, and download the `compiler-warnings-ubuntu` artifact. The
JSON's `warnings` array contains the normalized path, line, column, message,
and compiler warning option for each occurrence.
