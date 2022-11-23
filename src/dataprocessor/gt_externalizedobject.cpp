/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizedobject.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 20.07.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_externalizedobject.h"

#include "gt_boolproperty.h"
#include "gt_stringproperty.h"
#include "gt_utilities.h"
#include "gt_variantproperty.h"
#include "gt_externalizationmanager.h"

#include "gt_logging.h"

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

struct GtExternalizedObject::Impl
{
    /// indicates whether the data is fetched (independent of ref count)
    GtBoolProperty pFetched{
        "isFetched", tr("Is Fetched"), tr("Is Object Fetched"), true
    };

    /// inidatces that data should be fetched from original location
    GtBoolProperty pFetchInitialVersion{
        "fetchInitialVersion", tr("Fetch initial version"),
        tr("Fetch initial version"), true
    };

    /// hash of this object, calculated on last externalization
    GtStringProperty pCachedHash{
        "cachedHash", tr("Cached Hash"), tr("Cached Object Hash")
    };

    /// meta data for the externalization process (eg. hdf5 reference)
    GtVariantProperty pMetaData{
        "metaData", tr("Meta Data"), tr("Meta Data"),
        GtUnit::None, QVariant::fromValue(0)
    };

    /// keeps track of number of accesses
    int refCount{0};

    /// object states
    ExternalizeStates states{ ExternalizeOnSave };

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

GtExternalizedObject::GtExternalizedObject() :
    pimpl(std::make_unique<Impl>())
{
    static const QString cat = tr("Externalization");
    registerProperty(pimpl->pFetched, cat, true);
    registerProperty(pimpl->pFetchInitialVersion, cat, true);
    registerProperty(pimpl->pCachedHash, cat, true);
    registerProperty(pimpl->pMetaData, cat, true);

    pimpl->pFetchInitialVersion.hide(true);
    pimpl->pCachedHash.hide(true);
    pimpl->pMetaData.hide(true);

    pimpl->pFetched.setReadOnly(true);
    pimpl->pFetchInitialVersion.setReadOnly(true);
    pimpl->pCachedHash.setReadOnly(true);
    pimpl->pFetched.setReadOnly(true);
    pimpl->pMetaData.setReadOnly(true);
}

GtExternalizedObject::~GtExternalizedObject() = default;

int
GtExternalizedObject::refCount() const
{
    return pimpl->refCount;
}

bool
GtExternalizedObject::isFetched() const
{
    return pimpl->pFetched;
}

void
GtExternalizedObject::setFetchInitialVersion(bool value) const
{
    pimpl->pFetchInitialVersion = value;
}

bool
GtExternalizedObject::hasModifiedData()
{
    return hasModifiedData(calcExtHash());
}

bool
GtExternalizedObject::hasModifiedData(const QString& otherHash) const
{
    return pimpl->pCachedHash.get().isEmpty() ||
           pimpl->pCachedHash != otherHash;
}

QString
GtExternalizedObject::calcExtHash()
{
    // cache properties
    bool fiv{pimpl->pFetchInitialVersion};
    QString hash{pimpl->pCachedHash};
    QVariant metaData{pimpl->pMetaData};
    ExternalizeStates states = pimpl->states;

    // reset original values
    auto finally = gt::finally([&, fiv, states](){
        pimpl->pCachedHash = hash;
        pimpl->pMetaData.setVal(metaData);
        pimpl->states = states;
        pimpl->pFetchInitialVersion = fiv;
    });

    // clear properties for hash calculation
    pimpl->pFetchInitialVersion.revert();
    pimpl->pCachedHash.revert();
    pimpl->pMetaData.revert();
    pimpl->states = {};

    // calc object hash
    return calcHash();
}

void
GtExternalizedObject::onObjectDiffMerged()
{
    /// will automatically check whether the data has changed and whether
    /// the data should be internalized for saving
    fetch();
    release();

    return GtObject::onObjectDiffMerged();
}

bool
GtExternalizedObject::isDataValid() const
{
    // nothing to do here
    return true;
}

bool
GtExternalizedObject::canExternalize() const
{
    // nothing to do here
    return true;
}

bool
GtExternalizedObject::fetch()
{
    pimpl->refCount += 1;

    return fetchHelper();
}

bool
GtExternalizedObject::fetchHelper()
{
    // not checking if externalization is enabled here
    // -> fetching should must always be possible

    if (isFetched())
    {
        return true;
    }

    gtDebug().medium() << "Fetching object...";

    // fetch
    if (!doFetchData(pimpl->pMetaData.get(), pimpl->pFetchInitialVersion))
    {
        gtError() << tr("Fetching object failed!")
                  << tr("(Path: '%1')").arg(objectPath());
        // reset fetched flag
        pimpl->pFetched = false;
        // clear any internalized data
        doClearExternalizedData();
        return false;
    }

    // set fetched flag
    pimpl->pFetched = true;
    return true;
}

bool
GtExternalizedObject::release()
{
    pimpl->refCount -= 1;

    if (!gtExternalizationManager->isExternalizationEnabled())
    {
        return true;
    }

    if (!isFetched())
    {
        // resource is not fetched
        gtError() << tr("Externalizing object failed!")
                  << tr("(Path: '%1')").arg(objectPath());
        gtDebug().medium() << "ref count:" << pimpl->refCount;
        return false;
    }

    // dont externalize if data is still in use
    if (pimpl->refCount > 0 || pimpl->states & KeepInternalized)
    {
        return true;
    }

    assert(pimpl->refCount == 0);

    // scedule for externalization
    if (hasModifiedData() /*|| m_states & ExternalizeOnSave*/)
    {
        setFetchInitialVersion(false);
        pimpl->setExternalizeState(ExternalizeOnSave, true);
        return true;
    }

    // clear fetched state and free internal data
    pimpl->setExternalizeState(ExternalizeOnSave, false);
    pimpl->pFetched = false;
    doClearExternalizedData();

    return true;
}

bool
GtExternalizedObject::externalize()
{
    if (!gtExternalizationManager->isExternalizationEnabled() || !isFetched())
    {
        return true;
    }

    if (!canExternalize())
    {
        gtDebug().medium() << tr("Skipping externalization of invalid object:")
                          << objectName();
        return false;
    }

    // recalculate object hash
    QString hash{calcExtHash()};

    // check if not marked for externalization or has changes
    if (!(pimpl->states & ExternalizeOnSave || pimpl->states & KeepInternalized ||
          hasModifiedData(hash)))
    {
        return true;
    }

    gtDebug().medium() << "Externalizing object...";

    // externalize
    if (!doExternalizeData(pimpl->pMetaData.get()))
    {
        gtError() << tr("Externalizing object failed!")
                  << tr("(Path: '%1')").arg(objectPath());
        return false;
    }

    // save the new hash
    pimpl->pCachedHash = hash;
    // update states
    pimpl->pFetchInitialVersion = false;
    pimpl->setExternalizeState(ExternalizeOnSave, false);
    pimpl->setExternalizeState(KeepInternalized, false);

    // clear data
    if (pimpl->refCount == 0)
    {
        pimpl->pFetched = false;
        doClearExternalizedData();
    }

    return true;
}

bool
GtExternalizedObject::internalize()
{
    if (pimpl->states & KeepInternalized)
    {
        return true;
    }

    if (!fetchHelper())
    {
        return false;
    }

    pimpl->setExternalizeState(KeepInternalized, true);

    return true;
}

GtExternalizedObjectData::GtExternalizedObjectData(GtExternalizedObject* base) :
    m_base{base}
{
    if (m_base)
    {
        m_base->fetch();
    }
}

GtExternalizedObjectData::~GtExternalizedObjectData()
{
    if (m_base)
    {
        m_base->release();
    }
}

bool
GtExternalizedObjectData::isValid() const
{
    return m_base != nullptr && m_base->isFetched() && m_base->isDataValid();
}
