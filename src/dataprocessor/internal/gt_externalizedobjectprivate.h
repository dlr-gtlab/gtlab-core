/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 20.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GT_INTERNAL_EXTERNALIZEDOBJECTPRIVATE_H
#define GT_INTERNAL_EXTERNALIZEDOBJECTPRIVATE_H

#include "gt_boolproperty.h"
#include "gt_stringproperty.h"
#include "gt_variantproperty.h"

namespace gt
{

namespace internal
{

/**
 * @brief The ExternalizeState enum
 */
enum ExternalizeState
{
    /// indicates whether the data should be externalized on next save
    ExternalizeOnSave = 2,
    /// inidatces that data should not be externalized indirectly
    /// (ref count reaches 0)
    KeepInternalized = 8
};
Q_DECLARE_FLAGS(ExternalizeStates, ExternalizeState)

static QString const S_EXT_OBJECT_PROP_IS_FETCHED = QStringLiteral("isFetched");
static QString const S_EXT_OBJECT_PROP_HASH = QStringLiteral("cachedHash");

} // namespace internal

} // namespace gt

/**
 * @brief The GtExternalizedObjectPrivate struct.
 * Impl class for GtExternalizedObject
 */
struct GtExternalizedObjectPrivate
{
    using ExternalizeStates = gt::internal::ExternalizeStates;
    using ExternalizeState = gt::internal::ExternalizeState;

    /// indicates whether the data is fetched (independent of ref count)
    GtBoolProperty pFetched{
        gt::internal::S_EXT_OBJECT_PROP_IS_FETCHED,
        QObject::tr("Is Fetched"),
        QObject::tr("Is Object Fetched"),
        true
    };

    /// inidatces that data should be fetched from original location
    GtBoolProperty pFetchInitialVersion{
        "fetchInitialVersion",
        QObject::tr("Fetch initial version"),
        QObject::tr("Fetch initial version"),
        true
    };

    /// hash of this object, calculated on last externalization
    GtStringProperty pCachedHash{
        gt::internal::S_EXT_OBJECT_PROP_HASH,
        QObject::tr("Cached Hash"),
        QObject::tr("Cached Object Hash")
    };

    /// meta data for the externalization process (eg. hdf5 reference)
    GtVariantProperty pMetaData{
        "metaData",
        QObject::tr("Meta Data"),
        QObject::tr("Meta Data"),
        GtUnit::None,
        QVariant::fromValue(0)
    };

    /// keeps track of number of accesses
    int refCount{0};

    /// object states
    ExternalizeStates states{ ExternalizeState::ExternalizeOnSave };

    /**
     * @brief Sets the desired state
     * @param state state to set
     * @param enable whether state should be set or cleared
     */
    inline void setExternalizeState(ExternalizeState state, bool enable)
    {
        if (enable)
        {
            states |= state;
        }
        else
        {
            states &= ~state;
        }
    }
};

#endif // GT_INTERNAL_EXTERNALIZEDOBJECTPRIVATE_H
