/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizedobject.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 20.07.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_externalizedobject.h"

#include "gt_externalizationmanager.h"

#include "gt_logging.h"

GtExternalizedObject::GtExternalizedObject() = default;

int
GtExternalizedObject::refCount() const
{
    return m_refCount;
}

bool
GtExternalizedObject::isFetched() const
{
    return m_states & Fetched;
}

bool
GtExternalizedObject::fetchInitialVersion() const
{
    return m_states & FetchInitialVersion;
}

void
GtExternalizedObject::setFetched(bool value)
{
    setExternalizeState(Fetched, value);
}

void
GtExternalizedObject::setFetchInitialVersion(bool value)
{
    setExternalizeState(FetchInitialVersion, value);
}

bool
GtExternalizedObject::hasModifiedData()
{
    return m_cachedHash.isEmpty() || m_cachedHash != calcExtHash();
}

bool
GtExternalizedObject::hasModifiedData(const QString& otherHash) const
{
    return m_cachedHash.isEmpty() || m_cachedHash != otherHash;
}

QString
GtExternalizedObject::calcExtHash()
{
    // cache properties
    QString hash{m_cachedHash};
    QVariant metaData{m_metaData};
    auto states = m_states;

    // clear properties for hash calculation
    m_cachedHash.clear();
    m_metaData.clear();
    m_states = {};

    // calc object hash
    QString newHash(calcHash());

    // set original values
    m_cachedHash = hash;
    m_metaData = metaData;
    m_states = states;

    return newHash;
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
    m_refCount += 1;

    return fetchHelper();
}

bool
GtExternalizedObject::fetchHelper()
{
    if (!gtExternalizationManager->isExternalizationEnabled())
    {
        return true;
    }

    if (isFetched())
    {
        return true;
    }

    // fetch
    if (!doFetchData(m_metaData, fetchInitialVersion()))
    {
        gtError() << tr("Fetching object failed!")
                  << "(object path: '" + objectPath() + "')";
        // reset fetched flag
        setExternalizeState(Fetched, false);
        // clear any internalized data
        doClearExternalizedData();
        return false;
    }

    // set fetched flag
    setExternalizeState(Fetched, true);
    return true;
}

bool
GtExternalizedObject::release()
{    
    m_refCount -= 1;

    if (!gtExternalizationManager->isExternalizationEnabled())
    {
        return true;
    }

    if (!isFetched())
    {
        // resource is not fetched
        gtError() << tr("Externalizing object failed!")
                  << tr("(Object is not fetched,")
                  << "ref count:" << m_refCount
                  << "object path: '" +  this->objectPath() + "')";
        return false;
    }

    // dont externalize if data is still in use
    if (m_refCount > 0 || m_states & KeepInternalized)
    {
        return true;
    }

    Q_ASSERT(m_refCount == 0);

    // scedule for externalization
    if (hasModifiedData() /*|| m_states & ExternalizeOnSave*/)
    {
        setExternalizeState(FetchInitialVersion, false);
        setExternalizeState(ExternalizeOnSave, true);
        return true;
    }

    // clear fetched state and free internal data
    setExternalizeState(ExternalizeOnSave, false);
    setExternalizeState(Fetched, false);
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
        gtWarning() << tr("Cannot externalize invalid object:") << objectName();
        return false;
    }

    // recalculate object hash
    QString hash{calcExtHash()};

    // check if not marked for externalization or has changes
    if (!(m_states & ExternalizeOnSave || m_states & KeepInternalized ||
          hasModifiedData(hash)))
    {
        return true;
    }

    // externalize
    if (!doExternalizeData(m_metaData))
    {
        gtError() << tr("Externalizing object failed!")
                  << "(object path: '" + objectPath() + "')";
        return false;
    }

    // save the new hash
    m_cachedHash = hash;
    // update states
    setExternalizeState(FetchInitialVersion, false);
    setExternalizeState(ExternalizeOnSave, false);
    setExternalizeState(KeepInternalized, false);

    // clear data
    if (m_refCount == 0)
    {
        setExternalizeState(Fetched, false);
        doClearExternalizedData();
    }

    return true;
}

bool
GtExternalizedObject::internalize()
{
    if (m_states & KeepInternalized)
    {
        return true;
    }

    if (!fetchHelper())
    {
        return false;
    }

    setExternalizeState(KeepInternalized, true);

    return true;
}

void
GtExternalizedObject::setExternalizeState(ExternalizeState state, bool enable)
{
    if (enable)
    {
        m_states = m_states | state;
    }
    else
    {
        m_states = m_states & ~state;
    }
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
