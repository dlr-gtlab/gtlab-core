<!--
SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

# System tests

This directory contains end-to-end tests for GTlab Core functionality that is
exercised through public executables rather than in-process unit-test APIs.
Their CTest names start with `SystemTest.` and have the `SystemTest` label, so
they can be selected independently from unit tests.

## Prerequisites

Configure with test modules enabled. The Python test driver requires Python 3
and Pytest; install the specified dependency when it is not already
available:

```bash
python3 -m pip install -r tests/systemtests/requirements.txt
```

## Run the system tests

Build the console and test modules, then run only system tests from the build
directory:

```bash
make systemtests -j$(nproc)
ctest -R "^SystemTest\." --output-on-failure
```

CTest uses Pytest as the driver. Each test owns its temporary working directory
and must not write result files into the source-tree fixtures.

## Adding a test

Create a dedicated directory below `tests/systemtests/` for each feature. Keep
its fixtures, Pytest file, and short module-level documentation together. Add
a small CMake registration in `CMakeLists.txt` with a `SystemTest.` name and
`SystemTest` label.
