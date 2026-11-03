# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

import os
import shutil

source(findFile('scripts', 'python/bdd.py'))
setupHooks('../shared/scripts/bdd_hooks.py')

collectStepDefinitions('./steps', '../shared/steps', '../../../testing_resources/steps')

def main():
    testSettings.throwOnFailure = True
    testProjectsDir = os.path.abspath(
        os.path.join(os.path.dirname(__file__), "../../../")
    )
    for project in (
        "issue1602_no_current",
        "issue1602_current",
        "issue1602_added",
        "issue1602_switch_added",
        "issue1602_import_current",
    ):
        projectDir = os.path.join(testProjectsDir, project)
        if os.path.exists(projectDir):
            shutil.rmtree(projectDir)

    runFeatureFile('test.feature')
