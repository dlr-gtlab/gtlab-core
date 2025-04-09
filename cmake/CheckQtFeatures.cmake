# SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

include(CheckCXXSourceCompiles)

# Comparing two variants is a mess in the Qt version history
#  - First, there was the "<" operator, which was removed in Qt6
#  - In between, there was the member function QVariant::compare(QVariant other)
#  - Now, there is the static function QVariant::compare(QVariant a, QVariant b)
#
# This function checks, if the "compare" functions are available and in which form
#
# The following cmake variables will be set:
#  HAVE_QVARIANT_COMPARE  -  if the static function was found
#  HAVE_QVARIANT_COMPARE_MEMBER  -  if the member function was found
function(check_qvariant_compare)

    # Try to get Qt target properties in version-agnostic way
    if (TARGET Qt::Core)
        get_target_property(QtIncludes Qt::Core INTERFACE_INCLUDE_DIRECTORIES)
        get_target_property(QtLibs Qt::Core INTERFACE_LINK_LIBRARIES)
    elseif (TARGET Qt5::Core)
        get_target_property(QtIncludes Qt5::Core INTERFACE_INCLUDE_DIRECTORIES)
        get_target_property(QtLibs Qt5::Core INTERFACE_LINK_LIBRARIES)
    elseif (TARGET Qt6::Core)
        get_target_property(QtIncludes Qt6::Core INTERFACE_INCLUDE_DIRECTORIES)
        get_target_property(QtLibs Qt6::Core INTERFACE_LINK_LIBRARIES)
    else()
        message(WARNING "No known Qt::Core target found - cannot run QVariant::compare() check")
        return()
    endif()

    # Isolate effect of required variables
    set(_old_required_includes ${CMAKE_REQUIRED_INCLUDES})
    set(_old_required_libraries ${CMAKE_REQUIRED_LIBRARIES})

    set(CMAKE_REQUIRED_INCLUDES ${QtIncludes})
    set(CMAKE_REQUIRED_LIBRARIES ${QtLibs})

    # Member compare: a.compare(b)
    check_cxx_source_compiles("
        #include <QVariant>
        int main() {
            QVariant a(1), b(2);
            return a.compare(b);
        }" HAVE_QVARIANT_COMPARE_MEMBER)

    # Static compare: QVariant::compare(a, b)
    check_cxx_source_compiles("
        #include <QVariant>
        int main() {
            QVariant a(1), b(2);
            return QVariant::compare(a, b);
        }" HAVE_QVARIANT_COMPARE)

    # Restore old CMake required variables
    set(CMAKE_REQUIRED_INCLUDES ${_old_required_includes})
    set(CMAKE_REQUIRED_LIBRARIES ${_old_required_libraries})

    if(NOT HAVE_QVARIANT_COMPARE AND NOT HAVE_QVARIANT_COMPARE_MEMBER)
        message("No form of QVariant::compare() available — will fall back to deprecated operator<")
    endif()

endfunction()
