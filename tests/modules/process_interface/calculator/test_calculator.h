#ifndef TESTCALCULATOR_H
#define TESTCALCULATOR_H

#include "gt_calculator.h"
#include "gt_doubleproperty.h"
#include "gt_doublemonitoringproperty.h"
#include "gt_objectlinkproperty.h"
#include "gt_objectpathproperty.h"


class TestCalculator : public GtCalculator
{
    Q_OBJECT
public:
    Q_INVOKABLE TestCalculator();

    bool run() override;

    GtDoubleProperty m_value;

    GtDoubleMonitoringProperty m_result;

    GtObjectLinkProperty m_objectLink;

    GtObjectPathProperty m_objectPath;

};

#endif // TESTCALCULATOR_H
