/* GTlab - Gas Turbine laboratory
 * Source File: gt_processcategoryitem.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 12.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROCESSCATEGORYITEM_H
#define GTPROCESSCATEGORYITEM_H

#include "gt_abstractprocessitem.h"

#include "gt_calculatordata.h"

class GtProcessItem;
class GtCalculatorDataImpl;

/**
 * @brief The GtProcessCategoryItem class
 */
class GtProcessCategoryItem : public GtAbstractProcessItem
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Identification string.
     * @param Parent.
     */
    explicit GtProcessCategoryItem(const QString &id, QObject *parent = nullptr);

    /**
     * @brief addItem
     * @param calcData
     * @return
     */
    bool addItem(GtAbstractProcessItem* newItem);

    /**
     * @brief Returns list of all process items.
     * @return List of process items.
     */
    QList<GtAbstractProcessItem*> items();

    /**
     * @brief collapsed
     * @return
     */
    bool collapsed() const;

    /**
     * @brief setCollapsed
     * @param collapsed
     */
    void setCollapsed(bool collapsed);

private:
    bool m_collapsed;

};

#endif // GTPROCESSCATEGORYITEM_H
