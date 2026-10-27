/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUITABLAYOUTADAPTER_H
#define GT_GUITABLAYOUTADAPTER_H

#include <gt_guilayoutadapter.h>
#include <gt_tabwidget.h>
#include <gt_mdiitem.h>

#include <QPointer>

class GT_GUI_EXPORT GtGuiTabLayoutAdapter final : public GtGuiLayoutAdapter
{
    Q_OBJECT

public:

    GtGuiTabLayoutAdapter(GtTabWidget& tabWidget);

    bool isNull() const noexcept;

    void focus(QString const& itemId) override;

    bool addItem(GtMdiItem& item) override;

    GtMdiItem* item(QString const& itemId) override;

    GtMdiItem* itemFromWidget(QWidget& widget) override;

    GtMdiItem* currentItem() override;

    void closeAll() override;

private:
    
    QPointer<GtTabWidget> m_tab;
    QHash<QWidget*, QPointer<GtMdiItem>> m_openItems;

    void onTabCloseRequested(int idx);

    void onObjectNameChanged(GtMdiItem* item);
};

#endif // GT_GUITABLAYOUTADAPTER_H
