/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_MDI_EXPORTS_H
#define GT_MDI_EXPORTS_H

#if defined(WIN32)
  #if defined (GT_MDI_DLL)
    #define GT_MDI_EXPORT __declspec (dllexport)
  #else
    #define GT_MDI_EXPORT __declspec (dllimport)
  #endif
#else
    #define GT_MDI_EXPORT
#endif

#endif // GT_MDI_EXPORTS_H

