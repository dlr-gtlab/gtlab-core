/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QMdiSubWindow>
#include <QFrame>

#include "gt_mdiwidget.h"
#include "gt_mdilauncher.h"
#include "gt_mdiitem.h"
#include "gt_application.h"
#include "gt_collectioninterface.h"
#include "gt_algorithms.h"
#include "gt_icons.h"
#include "gt_logging.h"
#include "gt_tabwidget.h"
#include "gt_guifactory.h"
#include "gt_guilayoutadapter.h"

struct GtMdiLauncher::Impl
{
    QPointer<GtGuiLayoutAdapter> adapter{};

    /// Registered dock widget meta objects
    QHash<QString, QMetaObject> dockWidgets;

    /// Collections
    QHash<QString, GtCollectionInterface*> collections;
};

GtMdiLauncher::GtMdiLauncher(QObject* parent) : QObject(parent),
    pimpl(std::make_unique<Impl>())
{
}

GtMdiLauncher::~GtMdiLauncher() = default;

QString
GtMdiLauncher::generateMdiItemId(GtMdiItem* mdiItem)
{
    if (!mdiItem) return {};

    mdiItem->regenerateId();
    return mdiItem->currentId();
}

bool
GtMdiLauncher::mdiItemAllowed(GtMdiItem* mdiItem)
{
    if (!mdiItem) return false;

    if (mdiItem->allowsMultipleInstances()) return true;

    QString mdiId = mdiItem->currentId();

    if (mdiId.isEmpty()) return false;

    assert(pimpl->adapter);
    return !pimpl->adapter->hasItem(mdiId);
}

void
GtMdiLauncher::setFocus(const QString& mdiId)
{
    assert(pimpl->adapter);
    pimpl->adapter->focus(mdiId);
}

void
GtMdiLauncher::close()
{
    assert(pimpl->adapter);
    pimpl->adapter->closeAll();
}

GtMdiLauncher*
GtMdiLauncher::instance()
{
    static GtMdiLauncher* retval = nullptr;
    if (!retval)
    {
        retval = new GtMdiLauncher(qApp);
    }
    return retval;
}

void
GtMdiLauncher::setAdapter(std::unique_ptr<GtGuiLayoutAdapter> adapter)
{
    pimpl->adapter = adapter.get();
    if (adapter) adapter.release()->setParent(this);
}

QString
GtMdiLauncher::mdiOpenWithName(const QString& className) const
{
    QString displayName = GtGuiFactory::instance().displayName(className);
    return displayName.isEmpty() ? className : displayName;
}

QStringList
GtMdiLauncher::dockWidgetIds()
{
    return pimpl->dockWidgets.keys();
}

QMetaObject
GtMdiLauncher::dockWidget(const QString& id)
{
    return pimpl->dockWidgets.value(id);
}

QStringList
GtMdiLauncher::collectionIds()
{
    return pimpl->collections.keys();
}

QIcon
GtMdiLauncher::collectionIcon(const QString& id)
{
    if (!pimpl->collections.contains(id))
    {
        return {};
    }

    GtCollectionInterface* coll = pimpl->collections.value(id);

    if (!coll)
    {
        return {};
    }

    return coll->collectionIcon();
}

QMap<QString, QMetaType::Type>
GtMdiLauncher::collectionStructure(const QString& id)
{
    if (!pimpl->collections.contains(id))
    {
        return {};
    }

    GtCollectionInterface* coll = pimpl->collections.value(id);

    if (!coll)
    {
        return {};
    }

    return coll->collectionStructure();
}

GtCollectionInterface*
GtMdiLauncher::collection(const QString& id)
{
    if (!pimpl->collections.contains(id))
    {
        return nullptr;
    }

    return pimpl->collections.value(id);
}

void
GtMdiLauncher::printCurrentWindow()
{
    assert(pimpl->adapter);

    auto item = pimpl->adapter->currentItem();
    if (item) item->print();
}

void
GtMdiLauncher::print(QWidget* widget)
{
    if (!widget) return;

    assert(pimpl->adapter);
    auto* item = pimpl->adapter->itemFromWidget(*widget);
    if (item) item->print();
}

bool
GtMdiLauncher::isPrintable(QWidget* widget) const
{
    if (!widget) return false;

    assert(pimpl->adapter);
    auto* item = pimpl->adapter->itemFromWidget(*widget);
    return item && item->isPrintable();
}

std::vector<GtQmlAction*>
GtMdiLauncher::toolbarActions(QWidget* widget) const
{
    if (!widget) return {};

    assert(pimpl->adapter);
    auto* item = pimpl->adapter->itemFromWidget(*widget);
    if (!item) return {};

    return item->toolbarActions();
}

bool
GtMdiLauncher::registerDockWidget(QMetaObject metaObj)
{
    GtGuiFactory::instance().registerDockWidget(metaObj, GT_MODULENAME());

    if (pimpl->dockWidgets.contains(metaObj.className()))
    {
        gtWarning().medium()
                << tr("Dockwidget '%1' was already registered")
                   .arg(metaObj.className());
        return false;
    }

    pimpl->dockWidgets.insert(metaObj.className(), metaObj);

    return true;
}

bool
GtMdiLauncher::registerClass(QMetaObject metaObj)
{
    GtGuiFactory::instance().registerGuiItem(metaObj, GT_MODULENAME());
    return GtAbstractObjectFactory::registerClass(metaObj);
}

bool
GtMdiLauncher::dockWidgetsExist(const QList<QMetaObject>& metaData)
{
    for (const QMetaObject& mobj : metaData)
    {
        QString classname = mobj.className();
        if (pimpl->dockWidgets.contains(classname))
        {
            gtWarning().medium()
                    << tr("Dockwidget '%1' was already registered")
                       .arg(classname);
            return true;
        }
    }
    return false;
}

bool
GtMdiLauncher::registerDockWidgets(const QList<QMetaObject>& metaData)
{
    if (dockWidgetsExist(metaData))
    {
        return false;
    }

    for (const QMetaObject& metaObj : metaData)
    {
        registerDockWidget(metaObj); // checks if widget exists twice
    }

    return true;
}

bool
GtMdiLauncher::registerCollection(const QString& str,
                                  GtCollectionInterface* coll)
{
    if (pimpl->collections.contains(str))
    {
        return false;
    }

    pimpl->collections.insert(str, coll);

    return true;
}

GtMdiItem*
GtMdiLauncher::open(const QString& id, GtObject* data, const QString& customId)
{
    assert(pimpl->adapter);

    auto& factory = GtGuiFactory::instance();
    if (!factory.isGuiItem(id))
    {
        gtWarning().medium() << tr("Unknown GUI item '%1'!").arg(id);
        return nullptr;
    }

    auto item = factory.makeGuiItem(id);
    if (!item) return nullptr;

    // add parent to this class for now
    item->setParent(this);

    item->setCurrentObject(data, customId);

    if (!mdiItemAllowed(item.get()))
    {
        auto activeItem = pimpl->adapter->item(item->currentId());
        if (!activeItem) return nullptr;

        if (data && activeItem->flags().testFlag(GtMdiItem::AllowsReopening))
        {
            activeItem->setCurrentObject(data);
        }
        pimpl->adapter->focus(activeItem->currentId());
        return activeItem;
    }

    if (!pimpl->adapter->addItem(*item)) return nullptr;

    item->setData(data);

    return item.release();
}

GtMdiItem*
GtMdiLauncher::open(const QString& id, const QString& customId)
{
    return open(id, nullptr, customId);
}
