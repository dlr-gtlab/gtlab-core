/* GTlab - Gas Turbine laboratory
 * Source File: gt_calculatoroverviewmodel.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_application.h"
#include "gt_calculatorfactory.h"
#include "gt_processcategoryitem.h"
#include "gt_processcalculatoritem.h"
#include "gt_extendedcalculatordata.h"
#include "gt_icons.h"

#include "gt_calculatoroverviewmodel.h"

GtCalculatorOverviewModel::GtCalculatorOverviewModel(QObject* parent) :
    GtProcessOverviewModel(parent)
{

}

void
GtCalculatorOverviewModel::setUpModel()
{
    foreach (GtCalculatorData calcData,
             gtCalculatorFactory->calculatorDataList())
    {
        if (!gtApp->devMode() &&
            calcData->status != GtCalculatorDataImpl::RELEASE)
        {
            continue;
        }

        QString catId = calcData->category;

        if (catId.isEmpty())
        {
            catId = tr("Uncategorized");
        }

        GtProcessCategoryItem* catItem = categoryItem(catId);

        if (!catItem)
        {
            continue;
        }

        GtProcessCalculatorItem* pItem = new GtProcessCalculatorItem(calcData,
                                                                     catItem);

        if (!catItem->addItem(pItem))
        {
            delete pItem;
        }
    }
}

QString
GtCalculatorOverviewModel::id(GtAbstractProcessItem* item) const
{
    if (!item)
    {
        return QString();
    }

    GtProcessCalculatorItem* calcItem =
            qobject_cast<GtProcessCalculatorItem*>(item);

    if (!calcItem)
    {
        return QString();
    }

    GtCalculatorData calcData = calcItem->calculatorData();

    QString tmpStr = calcData->id;

    if (gtApp->devMode())
    {
        tmpStr = tmpStr + QStringLiteral(" [") +
                 calcData->statusToString(calcData->status) +
                 QStringLiteral("]");
    }

    return tmpStr;
}

QString
GtCalculatorOverviewModel::version(GtAbstractProcessItem* item) const
{
    if (!item)
    {
        return QString();
    }

    GtProcessCalculatorItem* calcItem =
            qobject_cast<GtProcessCalculatorItem*>(item);

    if (!calcItem)
    {
        return QString();
    }

    GtCalculatorData calcData = calcItem->calculatorData();

    return calcData->version.toString();
}

QIcon
GtCalculatorOverviewModel::icon(GtAbstractProcessItem* item) const
{
    if (!item)
    {
        return QIcon();
    }

    GtProcessCalculatorItem* calcItem =
            qobject_cast<GtProcessCalculatorItem*>(item);

    if (!calcItem)
    {
        return QIcon();
    }

    GtCalculatorData calcData = calcItem->calculatorData();

    GtExtendedCalculatorDataImpl* extendedData =
            dynamic_cast<GtExtendedCalculatorDataImpl*>(calcData.get());

    if (!extendedData || extendedData->icon.isNull())
    {
         return gt::gui::icon::calculator();
    }

    return extendedData->icon;
}

QString
GtCalculatorOverviewModel::description(GtAbstractProcessItem* item) const
{
    if (!item)
    {
        return QString();
    }

    GtProcessCalculatorItem* calcItem =
            qobject_cast<GtProcessCalculatorItem*>(item);

    if (!calcItem)
    {
        return QString();
    }

    GtCalculatorData calcData = calcItem->calculatorData();

    if (calcData->description.isNull())
    {
         return calcData->id;
    }

    return calcData->description;

}
