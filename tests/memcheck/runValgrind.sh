# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

valgrind --leak-check=yes --xml=yes --xml-file=valgrind-report.xml --suppressions=./tests/memcheck/valgrind_memcheck.supp ./build/$UNITTESTSNAME
