#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

include( local_settings.pri )

TEMPLATE = subdirs
CONFIG += ordered console
CONFIG += c++11

#### ARTISTIC STYLE FILE
DISTFILES  += style.astylerc

#### CODE COVERAGE ####
CONFIG(debug, debug|release) {
    contains(ENABLE_COVERAGE, true) {
        message("Coverage enabled")
    }
}

#### DATAMODEL
contains(BUILD_DATAMODEL, true) {
    message("BUILD_DATAMODEL = true")
    SUBDIRS += src/datamodel
}

#### NETWORK
contains(BUILD_NETWORK, true) {
    message("BUILD_NETWORK = true")
    SUBDIRS += src/network
}

#### CALCULATOR INTERFACE
contains(BUILD_CALCULATOR_INTERFACE, true) {
    message("BUILD_CALCULATOR_INTERFACE = true")
    SUBDIRS += src/calculators
}

#### CORE
contains(BUILD_CORE, true) {
    message("BUILD_CORE = true")
    SUBDIRS += src/core
}

#### MDI
contains(BUILD_MDI, true) {
    message("BUILD_MDI = true")
    SUBDIRS += src/mdi
}

#### GUI
contains(BUILD_GUI, true) {
    message("BUILD_GUI = true")
    SUBDIRS += src/gui
}

#### BATCH
contains(BUILD_BATCH, true) {
    message("BUILD_BATCH = true")
    SUBDIRS += src/batch
}

#### APP
contains(BUILD_APP, true) {
    message("BUILD_APP = true")
    SUBDIRS += src/app
}

#### UNIT TESTS ####
contains(BUILD_UNITTESTS, true) {
    message("BUILD_UNITTESTS = true")
    SUBDIRS += tests/unittests
}

#### REGRESSION TESTS ####
contains(BUILD_REGRESSIONTESTS, true) {
    message("BUILD_REGRESSIONTESTS = true")
    SUBDIRS += tests/regressiontests
}

#### TEST MODULES ####
contains(BUILD_TESTMODULES, true) {
    message("BUILD_TESTMODULES = true")
    SUBDIRS += tests/modules
}

#### TRANSLATIONS ####
TRANSLATIONS = translations/gtlab_de.ts

######################################################################
