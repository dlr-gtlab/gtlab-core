/*
 * GTlab IntelliGraph
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: 2026 German Aerospace Center
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "gt_guifactory.h"

#include "gt_qtutilities.h"
#include "gt_finally.h"

#include <QObject>
#include <QHash>
#include <QWidget>

using ClassName = QString;

struct GtGuiFactory::Impl
{
    struct DockWidgetEntry
    {
        QMetaObject metaObject;
        QString moduleId;
        QString displayName{};
    };

    struct GuiItemEntry
    {
        QMetaObject metaObject;
        QString moduleId;
        QString displayName{};
    };

    QHash<ClassName, DockWidgetEntry> dockWidgets;

    QHash<ClassName, GuiItemEntry> guiItems;

    mutable QString lastError{};

    template<typename T, typename Hash>
    std::unique_ptr<T> make(Hash const& map, QString const& className) const
    {
        lastError.clear();

        auto getClassType = [&map, this](){
            if ((void*)&map == &guiItems) return QStringLiteral("GUI-Item");
            if ((void*)&map == &dockWidgets) return QStringLiteral("DockWidget");
            return QStringLiteral("object");
        };

        auto entry = map.find(className);
        if (entry == map.end())
        {
            lastError = QObject::tr("Failed to instantiate %1, "
                                    "class '%2' not found!")
                            .arg(getClassType(), className);
            return nullptr;
        }

        std::unique_ptr<QObject> base{entry->metaObject.newInstance()};
        if (!base)
        {
            lastError = QObject::tr("Failed to instantiate %1, "
                                    "class not invokable?")
                            .arg(getClassType(), className);
            return nullptr;
        }

        auto t = gt::unique_qobject_cast<T>(std::move(base));
        if (!t)
        {
            lastError = QObject::tr("Failed to instantiate %1, "
                                    "invalid class type!")
                            .arg(getClassType(), className);
            return nullptr;
        }

        return t;
    }

    template<typename Hash>
    auto find(Hash const& map, QString const& className) noexcept
    {
        auto iter = map.find(className);
        return (iter == map.end()) ? nullptr : &*iter;
    };

}; // struct Impl

GtGuiFactory::GtGuiFactory() :
    impl{std::make_unique<Impl>()}
{ }

GtGuiFactory::~GtGuiFactory() noexcept = default;

GtGuiFactory::Impl*
GtGuiFactory::pimpl() noexcept
{
    return impl.get();
}

const GtGuiFactory::Impl*
GtGuiFactory::pimpl() const noexcept
{
    return impl.get();
}

GtGuiFactory*
GtGuiFactory::instance() noexcept
{
    static GtGuiFactory self;
    return &self;
}

std::unique_ptr<QWidget>
GtGuiFactory::makeDockWidget(QString const& className) const
{
    return pimpl()->make<QWidget>(pimpl()->dockWidgets, className);
}

std::unique_ptr<QWidget>
GtGuiFactory::makeGuiItem(const QString& className) const
{
    return pimpl()->make<QWidget>(pimpl()->guiItems, className);
}

bool
GtGuiFactory::registerDockWidget(QMetaObject metaObject, const QString& moduleId)
{
    pimpl()->lastError.clear();

    QString className = metaObject.className();
    if (isKnownClass(className))
    {
        pimpl()->lastError = QObject::tr("Failed to register DockWidget, "
                                         "class '%1' is already registered!")
                                 .arg(className);
        return false;
    }

    auto iter = pimpl()->dockWidgets.insert(
        className, Impl::DockWidgetEntry{std::move(metaObject), className});

    auto unregister = gt::finally([this, iter](){
        pimpl()->dockWidgets.erase(iter);
    });

    auto tmp = makeDockWidget(className);
    if (!tmp)
    {
        return false;
    }

    iter->displayName = tmp->objectName();
    unregister.clear();

    return true;
}

bool
GtGuiFactory::registerGuiItem(QMetaObject metaObject, const QString& moduleId)
{
    pimpl()->lastError.clear();

    QString className = metaObject.className();
    if (isKnownClass(className))
    {
        pimpl()->lastError = QObject::tr("Failed to register GUI-Item, "
                                         "class '%1' is already registered!")
                                 .arg(className);
        return false;
    }

    auto iter = pimpl()->guiItems.insert(
        className, Impl::GuiItemEntry{std::move(metaObject), className});

    auto unregister = gt::finally([this, iter](){
        pimpl()->guiItems.erase(iter);
    });

    auto tmp = makeGuiItem(className);
    if (!tmp)
    {
        return false;
    }

    iter->displayName = tmp->objectName();
    unregister.clear();

    return true;
}

QString
GtGuiFactory::moduleId(const QString& className) noexcept
{
    if (auto* dock = pimpl()->find(pimpl()->dockWidgets, className))
    {
        return dock->moduleId;
    }
    if (auto* item = pimpl()->find(pimpl()->guiItems, className))
    {
        return item->moduleId;
    }
    return {};
}

QString
GtGuiFactory::displayName(const QString& className) noexcept
{
    if (auto* dock = pimpl()->find(pimpl()->dockWidgets, className))
    {
        return dock->displayName;
    }
    if (auto* item = pimpl()->find(pimpl()->guiItems, className))
    {
        return item->displayName;
    }
    return {};
}

bool
GtGuiFactory::isDockWidget(const QString& className) noexcept
{
    return pimpl()->find(pimpl()->dockWidgets, className);
}

bool
GtGuiFactory::isGuiItem(const QString& className) noexcept
{
    return pimpl()->find(pimpl()->guiItems, className);
}

bool
GtGuiFactory::isKnownClass(const QString& className) noexcept
{
    return isGuiItem(className) || isDockWidget(className);
}

QStringList
GtGuiFactory::knownDockWidgets() const
{
    QStringList classNames;
    classNames.reserve(pimpl()->dockWidgets.size());
    std::copy(pimpl()->dockWidgets.keyBegin(),
              pimpl()->dockWidgets.keyEnd(),
              std::back_inserter(classNames));
    return classNames;
}

QStringList
GtGuiFactory::knownGuiItems() const
{
    QStringList classNames;
    classNames.reserve(pimpl()->guiItems.size());
    std::copy(pimpl()->guiItems.keyBegin(),
              pimpl()->guiItems.keyEnd(),
              std::back_inserter(classNames));
    return classNames;
}

const QString&
GtGuiFactory::lastError() const noexcept
{
    return pimpl()->lastError;
}
