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
#include "gt_mdilauncher.h"
#include "gt_mdiitem.h"
#include "gt_collectioninterface.h"
#include "gt_tabwidget.h"
#include "gt_collectionregistry.h"
#include "gt_guifactory.h"
#include "gt_guilayoutadapter.h"
#include "gt_guilauncher.h"
#include "gt_guitablayoutadapter.h"

struct GtMdiLauncher::Impl
{
};

GtMdiLauncher::GtMdiLauncher(QObject* parent) :
    QObject(parent),
    pimpl(nullptr)
{ }

GtMdiLauncher::~GtMdiLauncher() = default;

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
GtMdiLauncher::closeAll()
{
    GtGuiLauncher::instance().closeAll();
}

void
GtMdiLauncher::setMdiArea(QTabWidget* area)
{
    auto* tab = qobject_cast<GtTabWidget*>(area);
    if (!tab) return;

    GtGuiLauncher::instance().setAdapter(
        std::make_unique<GtGuiTabLayoutAdapter>(*tab));
}

QString
GtMdiLauncher::mdiOpenWithName(const QString& className) const
{
    return GtGuiFactory::instance().openWithMenuName(className);
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
    return GtGuiLauncher::instance().printCurrentWindow();
}

void
GtMdiLauncher::print(QWidget* widget)
{
    return GtGuiLauncher::instance().print(widget);
}

bool
GtMdiLauncher::isCurrentWindowPrintable() const
{
    return GtGuiLauncher::instance().isCurrentWindowPrintable();
}

bool
GtMdiLauncher::isPrintable(QWidget* widget) const
{
    return GtGuiLauncher::instance().isPrintable(widget);
}

std::vector<GtQmlAction*>
GtMdiLauncher::toolbarActions(QWidget* widget) const
{
    return GtGuiLauncher::instance().toolbarActions(widget);
}

GtMdiItem*
GtMdiLauncher::open(const QString& id, GtObject* data, const QString& customId)
{
    return GtGuiLauncher::instance().open(id, data, customId);
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
    const QMetaObject* dock = GtGuiFactory::instance().dockWidget(id);
    return dock ? *dock : QMetaObject{};
}

bool
GtMdiLauncher::registerDockWidget(QMetaObject metaObj,
                                  const QString& moduleIdent)
{
    return GtGuiFactory::instance().registerDockWidget(metaObj, moduleIdent);
}

bool
GtMdiLauncher::dockWidgetsExist(const QList<QMetaObject>& metaData)
{
    return std::any_of(
        metaData.begin(), metaData.end(), [](const QMetaObject& mobj) {
            QString className = mobj.className();
            return GtGuiFactory::instance().isKnownDockWidget(className);
        });
}

bool
GtMdiLauncher::registerDockWidgets(const QList<QMetaObject>& metaData,
                                   const QString& moduleId)
{
    return GtGuiFactory::instance().registerDockWidgets(metaData, moduleId);
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
GtMdiLauncher::registerClass(QMetaObject metaObj, const QString& moduleIdent)
{
    GtGuiFactory::instance().registerGuiItem(metaObj, moduleIdent);
    return GtAbstractObjectFactory::registerClass(metaObj, moduleIdent);
}

bool
GtMdiLauncher::unregisterClass(const QMetaObject& metaObj)
{
    GtGuiFactory::instance().unregisterGuiItem(metaObj.className());
    return GtAbstractObjectFactory::unregisterClass(metaObj);
}

bool
GtMdiLauncher::registerClasses(const QList<QMetaObject>& metaData,
                               const QString& moduleIdent)
{
    return std::all_of(metaData.begin(), metaData.end(),
                       [this, &moduleIdent](const QMetaObject& mobj) {
                           return registerClass(mobj, moduleIdent);
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
