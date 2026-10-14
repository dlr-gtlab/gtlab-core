/*
 * GTlab IntelliGraph
 *
 *  SPDX-License-Identifier: BSD-3-Clause
 *  SPDX-FileCopyrightText: 2026 German Aerospace Center
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_GUILAYOUTADAPTER_H
#define GT_GUILAYOUTADAPTER_H

#include <QObject>

class GtMdiItem;
class GtGuiLayoutAdapter : public QObject
{
    Q_OBJECT

public:

    virtual void focus(QString const& itemId) = 0;

    virtual void addItem(GtMdiItem* item) = 0;

    virtual bool hasItem(QString const& itemId) const = 0;

    virtual GtMdiItem* currentItem() = 0;

protected:

    GtGuiLayoutAdapter();
};

#endif // GT_GUILAYOUTADAPTER_H
