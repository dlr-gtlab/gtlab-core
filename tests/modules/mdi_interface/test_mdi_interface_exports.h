/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_mdi_interface_exports

 * Created on: 12.08.2020
 * Author: M. Bröcker
 */

#ifndef TEST_MDI_INTERFACE_EXPORTS_H
#define TEST_MDI_INTERFACE_EXPORTS_H

#if defined(WIN32)
  #if defined (TEST_MDI_INTERFACE_DLL)
    #define TEST_MDI_INTERFACE_EXPORT __declspec (dllexport)
  #else
    #define TEST_MDI_INTERFACE_EXPORT __declspec (dllimport)
  #endif
#else
    #define TEST_MDI_INTERFACE_EXPORT
#endif

#endif // TEST_MDI_INTERFACE_EXPORTS_H

