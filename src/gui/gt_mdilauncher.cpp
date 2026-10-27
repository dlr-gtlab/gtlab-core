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

#include "gt_mdilauncher.h"
#include "gt_mdiitem.h"
#include "gt_collectioninterface.h"
#include "gt_algorithms.h"
#include "gt_logging.h"
#include "gt_tabwidget.h"
#include "gt_collectionregistry.h"
#include "gt_guifactory.h"
#include "gt_guilayoutadapter.h"
#include "gt_guitablayoutadapter.h"

struct GtMdiLauncher::Impl
{
    QPointer<GtGuiLayoutAdapter> adapter{};
};

GtMdiLauncher::GtMdiLauncher(QObject* parent) : QObject(parent),
    pimpl(std::make_unique<Impl>())
{
}

GtMdiLauncher::~GtMdiLauncher() = default;

bool
GtMdiLauncher::isItemAllowed(GtMdiItem* mdiItem)
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
GtMdiLauncher::closeAll()
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

void
GtMdiLauncher::setMdiArea(QTabWidget* area)
{
    auto* tab = qobject_cast<GtTabWidget*>(area);
    if (!tab) return;

    setAdapter(std::make_unique<GtGuiTabLayoutAdapter>(*tab));
}

QString
GtMdiLauncher::mdiOpenWithName(const QString& className) const
{
    QString displayName = GtGuiFactory::instance().displayName(className);
    return displayName.isEmpty() ? className : displayName;
}

QStringList
GtMdiLauncher::collectionIds()
{
    return GtCollectionRegistry::instance().collectionIds();
}

QIcon
GtMdiLauncher::collectionIcon(const QString& id)
{
    return GtCollectionRegistry::instance().collectionIcon(id);
}

QMap<QString, QMetaType::Type>
GtMdiLauncher::collectionStructure(const QString& id)
{
    return GtCollectionRegistry::instance().collectionStructure(id);
}

GtCollectionInterface*
GtMdiLauncher::collection(const QString& id)
{
    return GtCollectionRegistry::instance().collection(id);
}

bool
GtMdiLauncher::registerCollection(const QString& str,
                                  GtCollectionInterface* coll)
{
    return GtCollectionRegistry::instance().registerCollection(str, coll);
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
GtMdiLauncher::isCurrentWindowPrintable() const
{
    assert(pimpl->adapter);

    auto item = pimpl->adapter->currentItem();
    return item && item->isPrintable();
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

GtMdiItem*
GtMdiLauncher::open(const QString& id, GtObject* data, const QString& customId)
{
    assert(pimpl->adapter);

    auto& factory = GtGuiFactory::instance();
    if (!factory.isKnownGuiItem(id))
    {
        gtWarning().medium() << tr("Unknown GUI item '%1'!").arg(id);
        return nullptr;
    }

    auto item = factory.makeGuiItem(id);
    if (!item) return nullptr;

    // add parent to this class for now
    item->setParent(this);

    item->setCurrentObject(data, customId);

    if (!isItemAllowed(item.get()))
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


QStringList
GtMdiLauncher::dockWidgetIds()
{
    return GtGuiFactory::instance().knownDockWidgets();
}

QMetaObject
GtMdiLauncher::dockWidget(const QString& id)
{
    QMetaObject const* dock = GtGuiFactory::instance().dockWidget(id);
    return dock ? *dock : QMetaObject{};
}

bool
GtMdiLauncher::registerDockWidget(QMetaObject metaObj)
{
    return GtGuiFactory::instance().registerDockWidget(metaObj, QString{});
}

bool
GtMdiLauncher::dockWidgetsExist(const QList<QMetaObject>& metaData)
{
    return std::any_of(metaData.begin(),
                       metaData.end(),
                       [](const QMetaObject& mobj){
        QString className = mobj.className();
        return GtGuiFactory::instance().isKnownDockWidget(className);
    });
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
        registerDockWidget(metaObj);
    }
    return true;
}

GtObject*
GtMdiLauncher::newObject(const QString& className, GtObject* parent)
{
    return GtAbstractObjectFactory::newObject(className, parent);
}

QString
GtMdiLauncher::superClassName(const QString& className) const
{
    return GtAbstractObjectFactory::superClassName(className);
}

QStringList
GtMdiLauncher::classHierarchy(const QString& className)
{
    return GtAbstractObjectFactory::classHierarchy(className);
}

bool
GtMdiLauncher::knownClass(const QString& className) const
{
    return GtAbstractObjectFactory::knownClass(className);
}

QStringList
GtMdiLauncher::knownClasses() const
{
    return GtAbstractObjectFactory::knownClasses();
}

bool
GtMdiLauncher::registerClass(QMetaObject metaObj)
{
    return registerClass(metaObj, QString{});
}

bool
GtMdiLauncher::registerClass(QMetaObject metaObj, QString const& moduleId)
{
    GtGuiFactory::instance().registerGuiItem(metaObj, moduleId);
    return GtAbstractObjectFactory::registerClass(metaObj, moduleId);
}

bool
GtMdiLauncher::unregisterClass(const QMetaObject& metaObj)
{
    GtGuiFactory::instance().unregisterGuiItem(metaObj.className());
    return GtAbstractObjectFactory::unregisterClass(metaObj);
}

bool
GtMdiLauncher::registerClasses(const QList<QMetaObject>& metaData)
{
    return registerClasses(metaData, QString{});
}

bool
GtMdiLauncher::registerClasses(const QList<QMetaObject>& metaData, const QString& moduleId)
{
    return std::all_of(metaData.begin(),
                       metaData.end(),
                       [this, &moduleId](QMetaObject const& mobj) {
                           return registerClass(mobj, moduleId);
                       });
}

QString
GtMdiLauncher::moduleId(const QString& className) const
{
    return GtGuiFactory::instance().moduleId(className);
}

bool
GtMdiLauncher::containsDuplicates(const QList<QMetaObject>& metaData) const
{
    return GtAbstractObjectFactory::containsDuplicates(metaData);
}

bool
GtMdiLauncher::invokable(const QMetaObject& metaObj) const
{
    return GtAbstractObjectFactory::invokable(metaObj);
}

bool
GtMdiLauncher::allInvokable(const QList<QMetaObject>& metaData)
{
    return GtAbstractObjectFactory::allInvokable(metaData);
}
