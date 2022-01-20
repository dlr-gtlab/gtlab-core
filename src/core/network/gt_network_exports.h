/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2017 by DLR
 *
 *  Created on: 23.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_NETWORK_EXPORTS_H
#define GT_NETWORK_EXPORTS_H

#if defined(WIN32)
  #if defined (GT_NETWORK_DLL)
    #define GT_NETWORK_EXPORT __declspec (dllexport)
  #else
    #define GT_NETWORK_EXPORT __declspec (dllimport)
  #endif
#else
    #define GT_NETWORK_EXPORT
#endif

#endif // GT_NETWORK_EXPORTS_H

