/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef TEST_IMPORTER_INTERFACE_EXPORTS_H
#define TEST_IMPORTER_INTERFACE_EXPORTS_H

#if defined(WIN32)
  #if defined(TEST_IMPORTER_INTERFACE_DLL)
    #define TEST_IMPORTER_INTERFACE_EXPORT __declspec(dllexport)
  #else
    #define TEST_IMPORTER_INTERFACE_EXPORT __declspec(dllimport)
  #endif
#else
  #define TEST_IMPORTER_INTERFACE_EXPORT
#endif

#endif // TEST_IMPORTER_INTERFACE_EXPORTS_H
