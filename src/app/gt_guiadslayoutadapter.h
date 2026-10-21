/*
 * GTlab IntelliGraph
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: 2026 German Aerospace Center
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUIADSLAYOUTADAPTER_H
#define GT_GUIADSLAYOUTADAPTER_H

#include <gt_guilayoutadapter.h>
#include <gt_mdiitem.h>

#include <QPointer>

#include <qtadvanceddocking-qt5/DockAreaWidget.h>
#include <qtadvanceddocking-qt5/DockManager.h>

class GtGuiAdsLayoutAdapter final : public GtGuiLayoutAdapter
{
    Q_OBJECT

public:

    GtGuiAdsLayoutAdapter(ads::CDockAreaWidget& centralWidget);

    bool isNull() const noexcept;

    void focus(QString const& itemId) override;

    bool addItem(GtMdiItem& item) override;

    GtMdiItem* item(QString const& itemId) override;

    GtMdiItem* itemFromWidget(QWidget& widget) override;

    GtMdiItem* currentItem() override;

    void closeAll() override;

private:

    QPointer<ads::CDockManager> m_manager;
    QPointer<ads::CDockAreaWidget> m_area;
    QHash<QWidget*, QPointer<GtMdiItem>> m_openItems;
};

#endif // GT_GUIADSLAYOUTADAPTER_H
