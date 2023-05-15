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
  #if defined (GT_DATAMODEL_DLL) // Build gui dll
    #define GT_DATAMODEL_EXPORT __declspec (dllexport)
  #else
    #if defined (GT_DATAMODEL_LIB) // use static lib
      #define GT_DATAMODEL_EXPORT
    #else // use dll
      #define GT_DATAMODEL_EXPORT __declspec (dllimport)
    #endif
  #endif
#else
    #define GT_DATAMODEL_EXPORT
#endif

#endif // GT_DATAMODEL_EXPORTS_H

