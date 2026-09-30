# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

"""Pytest hooks for import-to-session system tests."""

import pytest


@pytest.hookimpl(hookwrapper=True)
def pytest_runtest_makereport(item, call):
    outcome = yield
    report = outcome.get_result()
    if report.when == "call":
        item.rep_call = report
