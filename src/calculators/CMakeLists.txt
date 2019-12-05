#             ______________      __
#            / ____/_  __/ /___  / /_
#           / / __  / / / / __ `/ __ \
#          / /_/ / / / / / /_/ / /_/ /
#          \____/ /_/ /_/\__,_/_.___/

if (NOT GTLAB_BUILD_UTILITIES)
    message(WARNING "GTlabCalculators library requires utilities library")
endif()

if (NOT GTLAB_BUILD_DATAMODEL)
    message(WARNING "GTlabCalculators library requires datamodel library")
endif()

if (NOT GTLAB_BUILD_NETWORK)
    message(WARNING "GTlabCalculators library requires network library")
endif()


add_definitions(-DGT_CALCULATORS_DLL)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel
    ${CMAKE_CURRENT_SOURCE_DIR}/../datamodel/property
    ${CMAKE_CURRENT_SOURCE_DIR}/../network
    ${CMAKE_CURRENT_SOURCE_DIR}/../utilities/logging
)

set (headers
    gt_calculators_exports.h
    gt_processfactory.h
    gt_task.h
    gt_calculator.h
    gt_processdata.h
    gt_processinterface.h
    gt_processmoduleloader.h
    gt_calculatorfactory.h
    gt_taskfactory.h
    gt_tasklink.h
    gt_abstractrunnable.h
    gt_processcomponent.h
    gt_abstractcalculatorexecutor.h
    gt_calculatorexecinterface.h
    gt_calculatorexecutorlist.h
    gt_calculatorprovider.h
    gt_taskprovider.h
    gt_abstractprocessprovider.h
    gt_abstractprocessdata.h
    gt_calculatordata.h
    gt_evaluator.h
    gt_taskrunner.h
    gt_loop.h
    gt_doublemonitoringproperty.h
    gt_monitoringproperty.h
    gt_intmonitoringproperty.h
    gt_monitoringdata.h
    gt_monitoringdatatable.h
    gt_monitoringdataset.h
)

set (sources
    gt_processfactory.cpp
    gt_task.cpp
    gt_calculator.cpp
    gt_processdata.cpp
    gt_processmoduleloader.cpp
    gt_calculatorfactory.cpp
    gt_taskfactory.cpp
    gt_tasklink.cpp
    gt_abstractrunnable.cpp
    gt_processcomponent.cpp
    gt_abstractcalculatorexecutor.cpp
    gt_calculatorexecutorlist.cpp
    gt_calculatorprovider.cpp
    gt_taskprovider.cpp
    gt_abstractprocessprovider.cpp
    gt_abstractprocessdata.cpp
    gt_calculatordata.cpp
    gt_evaluator.cpp
    gt_taskrunner.cpp
    gt_loop.cpp
    gt_doublemonitoringproperty.cpp
    gt_monitoringproperty.cpp
    gt_intmonitoringproperty.cpp
    gt_monitoringdata.cpp
    gt_monitoringdatatable.cpp
    gt_monitoringdataset.cpp
)

add_library(GTlabCalculators SHARED ${headers} ${sources})
target_link_libraries(GTlabCalculators
    Qt5::Core
    Qt5::Xml
    GTlabNumerics
    GTlabPhysics
    GTlabLogging
    GTlabDatamodel
    GTlabNetwork
)

install(TARGETS GTlabCalculators
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

######################################################################
