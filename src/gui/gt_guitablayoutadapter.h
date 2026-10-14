/*
 * GTlab IntelliGraph
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: 2026 German Aerospace Center
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUITABLAYOUTADAPTER_H
#define GT_GUITABLAYOUTADAPTER_H

#include <gt_guilayoutadapter.h>

#include <gt_tabwidget.h>
#include <gt_mdiitem.h>

class GtTabWidget;
class GtGuiTabLayoutAdapter : public GtGuiLayoutAdapter
{
    Q_OBJECT

public:

    GtGuiTabLayoutAdapter(GtTabWidget& tabWidget);

    void focus(QString const& itemId) override;

    void addItem(GtMdiItem* item) override;

    bool hasItem(QString const& itemId) const override;

    GtMdiItem* currentItem() override;

private:
    
    QPointer<GtTabWidget> m_tab;
    QHash<QWidget*, QPointer<GtMdiItem>> m_openItems;
};

#endif // GT_GUITABLAYOUTADAPTER_H
