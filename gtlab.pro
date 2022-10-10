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
CONFIG += c++14

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
    SUBDIRS += src/dataprocessor
}

#### CORE
contains(BUILD_CORE, true) {
    message("BUILD_CORE = true")
    SUBDIRS += src/core
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

#### TEST MODULES ####
contains(BUILD_TESTMODULES, true) {
    message("BUILD_TESTMODULES = true")
    SUBDIRS += tests/modules
}

#### TRANSLATIONS ####
TRANSLATIONS = translations/gtlab_de.ts

######################################################################
