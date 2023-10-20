# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

source(findFile('scripts', 'python/bdd.py'))
setupHooks('../shared/scripts/bdd_hooks.py')

collectStepDefinitions('./steps', '../shared/steps', '../../../testing_resources/steps')

def main():
    testSettings.throwOnFailure = True
    runFeatureFile('test.feature')
