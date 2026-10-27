/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUILAYOUTADAPTER_H
#define GT_GUILAYOUTADAPTER_H

#include <gt_gui_exports.h>
#include <QObject>

class GtMdiItem;
class GT_GUI_EXPORT GtGuiLayoutAdapter : public QObject
{
    Q_OBJECT

public:
    virtual void focus(const QString& itemId) = 0;

    virtual bool addItem(GtMdiItem& item) = 0;

    bool hasItem(const QString& itemId)
    {
        return item(itemId);
    };

    virtual GtMdiItem* item(const QString& itemId) = 0;

    virtual GtMdiItem* itemFromWidget(QWidget& widget) = 0;

    virtual GtMdiItem* currentItem() = 0;

    virtual void closeAll() = 0;

protected:
    GtGuiLayoutAdapter();
};

#endif // GT_GUILAYOUTADAPTER_H
