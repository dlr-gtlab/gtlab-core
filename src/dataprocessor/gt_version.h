/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 14.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GT_VERSION_H
#define GT_VERSION_H

#define GT_VERSION_MAJOR 2
#define GT_VERSION_MINOR 0
#define GT_VERSION_PATCH 11
#define GT_VERSION_PRE_RELEASE ""
#define GT_VERSION_BUILD ""

/*
   can be used like #if (GT_VERSION >= GT_VERSION_CHECK(4, 4, 0))
*/
#define GT_VERSION_CHECK(major, minor, patch) ((major<<16)|(minor<<8)|(patch))

/*
   GT_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define GT_VERSION      GT_VERSION_CHECK(GT_VERSION_MAJOR, GT_VERSION_MINOR, GT_VERSION_PATCH)


// -----------------------------------------------------------------------------
// String helpers
// -----------------------------------------------------------------------------

#define GT_STR_IMPL(x) #x
#define GT_STR(x) GT_STR_IMPL(x)

// -----------------------------------------------------------------------------
// Deprecation helpers
// -----------------------------------------------------------------------------

#define GT_DEPR_MSG(maj, min, msg) \
 " This deprecated function will be removed in GTlab version " GT_STR(maj) "." GT_STR(min) ": " msg

// Strict mode:
//   Define GTLAB_DEPRECATIONS_AS_ERRORS to make *any* use of
//   GT_DEPRECATED_REMOVED_IN a hard error (for migration builds).
#if defined(Q_MOC_RUN)
    #define GT_REMOVAL_GUARD(maj, min, msg) /* nothing */
#elif defined(GTLAB_DEPRECATIONS_AS_ERRORS)
    #define GT_REMOVAL_GUARD(maj, min, msg)                                    \
    static_assert(false,                                                       \
                  "Deprecated API is forbidden in strict mode: " msg);
#else
    #define GT_REMOVAL_GUARD(maj, min, msg)                                    \
    static_assert(GT_VERSION < GT_VERSION_CHECK((maj), (min), 0),              \
                  "The function needs to be removed, removal version reached (" GT_STR(maj) "." GT_STR(min)     \
                  "): " msg);
#endif


// Use when a function should automatically fail at a certain GTlab version.
//
// IMPORTANT (templates):
//   Attributes cannot appear *before* the `template <...>` keyword.
//   So for function templates use the split form shown below.
//
// Non-template usage:
//   GT_DEPRECATED_REMOVED_IN(3, 0, "Use gtNewFunction instead")
//   void oldApi();
//
// Function-template usage (split form):
//   GT_REMOVAL_GUARD(3, 0, "Use gt::findObject() instead.");
//   template <typename T>
//   GT_DEPRECATED_ATTR(3, 0, "Use gt::findObject() instead.")
//   inline GtObject* findObject(...);

// Attribute only (safe to place after `template <...>`)
#define GT_DEPRECATED_ATTR(maj, min, msg) [[deprecated(GT_DEPR_MSG(maj, min, msg))]]

#define GT_DEPRECATED_REMOVED_IN(maj, min, msg) \
    GT_REMOVAL_GUARD(maj, min, msg);       \
    GT_DEPRECATED_ATTR(maj, min, msg)

#endif // GT_VERSION_H
