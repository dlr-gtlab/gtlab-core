/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_platform.h
 *
 *  Created on: 11.04.2022
 *  Author: Martin Siggel (AT-TWK)
 *  Tel.: +49 2203 601 2264
 */

#ifndef GT_PLATFORM_H
#define GT_PLATFORM_H

#ifdef _MSVC_LANG
#define CPP_STANDARD_VERSION _MSVC_LANG
#else
#define CPP_STANDARD_VERSION __cplusplus
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

#endif // GT_PLATFORM_H
