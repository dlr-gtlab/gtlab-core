/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizationmanager.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 18.08.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_externalizationmanager.h"

#include "gt_externalizedobject.h"
#include "gt_logging.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>

GtExternalizationManager::GtExternalizationManager() = default;

GtExternalizationManager*
GtExternalizationManager::instance()
{
    static GtExternalizationManager self{};

    return &self;
}

bool
GtExternalizationManager::isExternalizationEnabled() const
{
    return m_isExternalizationEnabled;
}

const QString&
GtExternalizationManager::projectDir() const
{
    assert(!m_projectDir.isEmpty());
    return m_projectDir;
}

void
GtExternalizationManager::enableExternalization(bool value)
{
    m_isExternalizationEnabled = value;
}

void
GtExternalizationManager::enableExternalization(const QVariant& boolValue)
{
    m_isExternalizationEnabled = boolValue.toBool();
}

void
GtExternalizationManager::onProjectLoaded(QString projectDir)
{
    setProjectDir(std::move(projectDir));

    if (m_projectDir.isEmpty())
    {
        return;
    }

    foreach (const auto& interface, m_externalizationInterfaces)
    {
        if (!interface)
        {
            gtError() << "null externalization interface!";
            m_externalizationInterfaces.removeOne(interface);
            continue;
        }

        interface->onProjectLoaded(m_projectDir);
    }
}
void
GtExternalizationManager::registerExernalizationInterface(
        ExternalizationInterface interface)
{
    if (!interface || m_externalizationInterfaces.contains(interface))
    {
        return;
    }

    m_externalizationInterfaces.append(interface);
}

void
GtExternalizationManager::setProjectDir(QString projectDir)
{
    m_projectDir = std::move(projectDir);
}

void
GtExternalizationManager::initExternalizedObjects(GtObject& root)
{
    /// all externalized objects should fetch its initial version
    for (auto* o : root.findChildren<GtExternalizedObject*>())
    {
        assert(o);
        o->setFetchInitialVersion(true);
    }
}

void
GtExternalizationManager::clearProjectDir()
{
    m_projectDir.clear();
}


