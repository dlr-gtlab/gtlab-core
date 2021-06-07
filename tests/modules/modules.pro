#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

######################################################################
#### DO NOT CHANGE!
######################################################################

TEMPLATE = subdirs
CONFIG += ordered console
CONFIG += c++11

SUBDIRS += datamodel_interface
SUBDIRS += mdi_interface
SUBDIRS += mdi_interface_ext
SUBDIRS += module_interface

RESOURCES += ../../src/resources/icons/icons.qrc

######################################################################
