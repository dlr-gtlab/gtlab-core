# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

# QProcess in this test triggers Valgrind's internal clone syscall assertion.
valgrind --leak-check=yes --xml=yes --xml-file=valgrind-report.xml --suppressions=./tests/memcheck/valgrind_memcheck.supp ./build/$UNITTESTSNAME --gtest_filter=-GtApplicationPaths.TemporaryDirectoryProcessLifecycle
