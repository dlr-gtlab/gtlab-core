/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 28.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_CORE_EXPORTS_H
#define GT_CORE_EXPORTS_H

#if defined(WIN32)
  #if defined (GT_CORE_DLL) // Build gui dll
    #define GT_CORE_EXPORT __declspec (dllexport)
  #else
    #if defined (GT_CORE_LIB) // use static lib
      #define GT_CORE_EXPORT
    #else // use dll
      #define GT_CORE_EXPORT __declspec (dllimport)
    #endif
  #endif
#else
    #define GT_CORE_EXPORT
#endif

#endif // GT_CORE_EXPORTS_H

