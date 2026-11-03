/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "gt_guilauncher.h"

#include "gt_guifactory.h"
#include "gt_guilayoutadapter.h"
#include "gt_application.h"
#include "gt_mdiitem.h"
#include "gt_logging.h"

struct GtGuiLauncher::Impl
{
    QPointer<GtGuiLayoutAdapter> adapter{};

}; // struct Impl

GtGuiLauncher::GtGuiLauncher() :
    QObject(GtApplication::instance()),
    impl{std::make_unique<Impl>()}
{
    assert(parent() && "GtApplication was not setup yet!");
}

GtGuiLauncher::~GtGuiLauncher() noexcept = default;

GtGuiLauncher&
GtGuiLauncher::instance() noexcept
{
    static GtGuiLauncher self;
    return self;
}

bool
GtGuiLauncher::isNull() const noexcept
{
    return !pimpl()->adapter;
}

bool
GtGuiLauncher::isNewItemAllowed(GtMdiItem const& guiItem) const
{
    if (guiItem.allowsMultipleInstances()) return true;

    QString mdiId = guiItem.currentId();

    if (mdiId.isEmpty()) return false;

    assert(!isNull());
    return !pimpl()->adapter->hasItem(mdiId);
}

void
GtGuiLauncher::setAdapter(std::unique_ptr<GtGuiLayoutAdapter> adapter)
{
    pimpl()->adapter = adapter.get();
    if (adapter) adapter.release()->setParent(this);
}

void
GtGuiLauncher::focus(const QString& itemId)
{
    assert(!isNull());
    pimpl()->adapter->focus(itemId);
}

void
GtGuiLauncher::printCurrentWindow()
{
    assert(!isNull());
    auto item = pimpl()->adapter->currentItem();
    if (item) item->print();
}

void
GtGuiLauncher::print(QWidget* widget)
{
    if (!widget) return;

    assert(!isNull());
    auto* item = pimpl()->adapter->itemFromWidget(*widget);
    if (item && item->isPrintable()) item->print();
}

bool
GtGuiLauncher::isCurrentWindowPrintable() const
{
    assert(!isNull());

    auto item = pimpl()->adapter->currentItem();
    return item && item->isPrintable();
}

bool
GtGuiLauncher::isPrintable(QWidget* widget) const
{
    if (!widget) return false;

    assert(pimpl()->adapter);
    auto* item = pimpl()->adapter->itemFromWidget(*widget);
    return item && item->isPrintable();
}

std::vector<GtQmlAction*>
GtGuiLauncher::toolbarActions(QWidget* widget) const
{
    if (!widget) return {};

    assert(!isNull());
    auto* item = pimpl()->adapter->itemFromWidget(*widget);
    if (!item) return {};

    return item->toolbarActions();
}

void
GtGuiLauncher::closeAll()
{
    assert(!isNull());
    pimpl()->adapter->closeAll();
}

GtMdiItem*
GtGuiLauncher::open(const QString& className, GtObject* data, const QString& customId)
{
    assert(!isNull());

    auto& factory = GtGuiFactory::instance();
    if (!factory.isKnownGuiItem(className))
    {
        gtWarning().medium() << tr("Unknown GUI item '%1'!").arg(className);
        return nullptr;
    }

    auto item = factory.makeGuiItem(className);
    if (!item) return nullptr;

    // add parent to this class for now
    item->setParent(this);

    item->setCurrentObject(data, customId);

    if (!isNewItemAllowed(*item))
    {
        auto activeItem = pimpl()->adapter->item(item->currentId());
        if (!activeItem) return nullptr;

        if (data && activeItem->flags().testFlag(GtMdiItem::AllowsReopening))
        {
            activeItem->setCurrentObject(data);
        }
        pimpl()->adapter->focus(activeItem->currentId());
        return activeItem;
    }

    if (!pimpl()->adapter->addItem(*item)) return nullptr;

    item->setData(data);

    return item.release();
}

GtMdiItem*
GtGuiLauncher::open(const QString& className, const QString& customId)
{
    return open(className, nullptr, customId);
}

GtGuiLauncher::Impl*
GtGuiLauncher::pimpl() noexcept
{
    return impl.get();
}

const GtGuiLauncher::Impl*
GtGuiLauncher::pimpl() const noexcept
{
    return impl.get();
}

