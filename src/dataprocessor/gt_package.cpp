/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 21.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_package.h"
#include "gt_objectmemento.h"
#include "gt_objectfactory.h"
#include "gt_logging.h"

#include <QDir>
#include <QDomDocument>
#include <QScopeGuard>
#include <QTemporaryDir>

#include <utility>

namespace
{
// The base hooks acknowledge the probe. An override that handles auxiliary
// data itself must not be assumed to be safe just because it returns true.
struct MiscDataProbe
{
    const GtPackage* package = nullptr;
    bool calledBase = false;
};

thread_local MiscDataProbe miscDataProbe;
} // namespace

GtPackage::GtPackage()
{
    setObjectName("Package");
    setFlag(UserDeletable);
}

bool
GtPackage::readData(const QDomElement& root)
{
    QDomElement oe = root.firstChildElement();
    while (!oe.isNull())
    {
        if (oe.tagName() != "object" && oe.tagName() != "objectref")
        {
            oe = oe.nextSiblingElement();
            continue;
        }

        GtObjectMemento memento(oe);

        if (!memento.isNull())
        {
            // default object
            GtObject* dobj = findDirectChild<GtObject*>(memento.ident());

            if (dobj)
            {
                // TODO: warning for unknown data element
                //gtDebug() << tr("default object found!") << " ("
                //          << dobj->objectName() << ") " << tr("restoring...");
                dobj->setFactory(gtObjectFactory);
                dobj->fromMemento(memento);
            }
            else
            {
                auto cobj = memento.restore_unique(gtObjectFactory);

                if (cobj)
                {
                    appendChild(std::move(cobj));
                }
                else
                {
                    gtWarning() << tr("unknown data element skipped!")
                                << "(" << memento.className() << ")";
                }
            }
        }

        oe = oe.nextSiblingElement();
    }

    return true;
}

bool
GtPackage::readMiscData(const QDir& /*projectDir*/)
{
    if (miscDataProbe.package == this) miscDataProbe.calledBase = true;
    return true;
}

bool
GtPackage::saveData(QDomElement& root, QDomDocument& /*doc*/)
{
    foreach (GtObject* obj, findDirectChildren<GtObject*>())
    {
        GtObjectMemento memento = obj->toMemento();
        root.appendChild(memento.documentElement());
    }

    return true;
}

bool
GtPackage::saveMiscData(const QDir& /*projectDir*/)
{
    if (miscDataProbe.package == this) miscDataProbe.calledBase = true;
    return true;
}

bool
GtPackage::hasStandaloneData(const QDir* readProbeDir)
{
    auto memento = toMemento();
    auto copy = memento.restore_unique<GtPackage>(factory());
    if (!copy) return false;
    const auto sameData = [&] {
        auto restored = copy->toMemento();
        memento.calculateHashes();
        restored.calculateHashes();
        return memento.fullHash() == restored.fullHash();
    };
    if (!sameData()) return false;

    QTemporaryDir scratch;
    if (!scratch.isValid()) return false;

    const QDir directory(scratch.path());
    const auto empty = [&] {
        return directory.isEmpty(QDir::AllEntries | QDir::NoDotAndDotDot |
                                 QDir::Hidden | QDir::System);
    };
    const auto previousProbe =
        std::exchange(miscDataProbe, MiscDataProbe{copy.get()});
    const auto restoreProbe =
        qScopeGuard([previousProbe] { miscDataProbe = previousProbe; });

    if (!copy->readMiscData(readProbeDir ? *readProbeDir : directory) ||
        !miscDataProbe.calledBase || !sameData() || !empty())
    {
        return false;
    }

    QDomDocument originalDocument;
    QDomDocument restoredDocument;
    auto originalRoot =
        originalDocument.createElement(QStringLiteral("GTLABMODULE"));
    auto restoredRoot =
        restoredDocument.createElement(QStringLiteral("GTLABMODULE"));
    originalDocument.appendChild(originalRoot);
    restoredDocument.appendChild(restoredRoot);
    if (!saveData(originalRoot, originalDocument) ||
        !copy->saveData(restoredRoot, restoredDocument) ||
        originalDocument.toByteArray() != restoredDocument.toByteArray())
    {
        return false;
    }

    // Probe the live data as well. An override can store values outside the
    // Memento that only become visible when it writes them to the directory.
    miscDataProbe.calledBase = false;
    if (!copy->saveMiscData(directory) || !miscDataProbe.calledBase || !empty())
        return false;
    miscDataProbe = {this};
    return saveMiscData(directory) && miscDataProbe.calledBase && empty();
}
