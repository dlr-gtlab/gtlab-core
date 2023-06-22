/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 23.01.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef TEST_DATAMODEL_INTERFACE_EXPORTS_H
#define TEST_DATAMODEL_INTERFACE_EXPORTS_H

#if defined(WIN32)
  #if defined (TEST_DATAMODEL_INTERFACE_DLL)
    #define TEST_DATAMODEL_INTERFACE_EXPORT __declspec (dllexport)
  #else
    #define TEST_DATAMODEL_INTERFACE_EXPORT __declspec (dllimport)
  #endif
#else
    #define TEST_DATAMODEL_INTERFACE_EXPORT
#endif

#endif // TEST_DATAMODEL_INTERFACE_EXPORTS_H

