/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizedobject.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 20.07.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_externalizedobject.h"

#include "gt_externalizationsettings.h"

#include "gt_logging.h"

GtExternalizedObject::GtExternalizedObject() :
    m_refCount(1),
    m_cachedHash(QString()),
    m_states(Fetched | Standalone)
{
}

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

void
GtExternalizedObject::setIsFetched(bool value)
{
    setObjectState(Fetched, value);
}

bool
GtExternalizedObject::isTemporary() const
{
    return m_states & Temporary;
}

void
GtExternalizedObject::setIsTemporary(bool enable)
{
    setObjectState(Temporary, enable);
}

bool
GtExternalizedObject::hasChanges()
{
    return m_cachedHash.isEmpty() || m_cachedHash != recalculateHash();
}

bool
GtExternalizedObject::hasChanges(QString otherHash) const
{
    return m_cachedHash.isEmpty() || m_cachedHash != otherHash;
}

QString
GtExternalizedObject::cachedHash() const
{
    return m_cachedHash;
}

QString
GtExternalizedObject::recalculateHash()
{
    // cache properties
    QString hash(m_cachedHash);
    // access hdf5 specific properties
    QVariant h5ref(property("h5Reference"));

    // clear properties for hash calculation
    m_cachedHash.clear();
    setProperty("h5Reference", QVariant(0));

    // calc hash
    QString newHash(calcHash());

    // set original values
    m_cachedHash = hash;
    setProperty("h5Reference", h5ref);

    return newHash;
}

void
GtExternalizedObject::updateCachedHash(const QString& hash)
{
    m_cachedHash = hash;
}

bool
GtExternalizedObject::fetchData()
{    
    if (!gtExternalizationSettings->isExternalizationEnabled())
    {
        return true;
    }

    m_refCount += 1;

    if (m_states & Fetched)
    {
        // resource is already fetched
        gtWarning() << "resource is already fetched!" << m_refCount;
        return true;
    }

    // resource must be fetched
    if (!doFetchData())
    {
        // fetching failed
        gtError() << "could not fetch externalized resource!";
        // reset ref count
        m_refCount = 0;
        // reset fetched flag
        m_states = m_states & ~Fetched;
        // clear any internalized data
        doClearExternalizedData();
        return false;
    }

    // set fetched flag
    m_states = m_states | Fetched;
    // data can be cleared
    m_states = m_states & ~Standalone;
    // ref count should equal 1 now
    m_refCount = 1;

    return true;
}

bool
GtExternalizedObject::releaseData(ReleaseFlag mode)
{
    if (!gtExternalizationSettings->isExternalizationEnabled())
    {
        return true;
    }

    m_refCount -= 1;

    if (!(m_states & Fetched))
    {
        // resource is not fetched
        gtWarning() << "resource is no longer fetched!" << m_refCount;
        // reset ref count
        m_refCount = 0;
        return true;
    }

    if (mode == ForceExternalize || (mode == Externalize && hasChanges()))
    {
        // recalculate the hash
        QString hash(recalculateHash());

        if (!doExternalizeData())
        {
            // externalizing failed
            gtError() << "could not externalize resource!";
            // restore ref count
            m_refCount += 1;
            return false;
        }

        // object can be cleared and is no longer standalone
        m_states = m_states & ~Standalone;
        // save the new hash
        updateCachedHash(hash);
    }

    // ref count should not be below 0
    if (m_refCount < 0)
    {
        m_refCount = 0;
    }

    // dont clear if data is in use or was not externalized before
    if (m_refCount > 0 || m_states & Standalone)
    {
        return true;
    }

    // clear fetched state
    m_states = m_states & ~Fetched;
    // data can be cleared
    doClearExternalizedData();

    return true;
}

GtExternalizedObject::ObjectStates
GtExternalizedObject::objectStates() const
{
    return m_states;
}

void
GtExternalizedObject::setObjectState(ObjectState state, bool enable)
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

void
GtExternalizedObject::resetRefCount()
{
    m_refCount = 0;
}
