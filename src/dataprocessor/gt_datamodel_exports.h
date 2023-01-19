/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 24.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_DATAMODEL_EXPORTS_H
#define GT_DATAMODEL_EXPORTS_H

#if defined(WIN32)
  #if defined (GT_DATAMODEL_DLL)
    #define GT_DATAMODEL_EXPORT __declspec (dllexport)
  #else
    #define GT_DATAMODEL_EXPORT __declspec (dllimport)
  #endif
#else
    #define GT_DATAMODEL_EXPORT
#endif

#ifdef __has_cpp_attribute
    #if __has_cpp_attribute(nodiscard)
        #define GT_NO_DISCARD [[nodiscard]]
    #else
       #define GT_NO_DISCARD
    #endif
#else
    #define GT_NO_DISCARD
#endif

#endif // GT_DATAMODEL_EXPORTS_H

