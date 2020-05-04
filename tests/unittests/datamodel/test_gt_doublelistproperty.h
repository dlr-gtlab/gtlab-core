/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_doublelistproperty.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 01.02.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef TEST_GT_DOUBLELISTPROPERTY_H
#define TEST_GT_DOUBLELISTPROPERTY_H

#include "gt_object.h"
#include "gt_doublelistproperty.h"

class TestDoubleListObject : public GtObject
{
    Q_OBJECT


public:
    Q_INVOKABLE TestDoubleListObject() {
        doubleList = new GtDoubleListProperty("doubleList", "Double List");

        registerProperty(*doubleList);
    };

    QVector<double> getDoubleList() const
    {
        return doubleList->getVal();
    }

    void setDoubleList(const QVector<double>& value)
    {
        doubleList->setVal(value);
    }

private:
    GtDoubleListProperty* doubleList;

};

#endif // TEST_GT_DOUBLELISTPROPERTY_H
