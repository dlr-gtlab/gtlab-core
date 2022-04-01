/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_processdock
 * copyright 2009-2017 by DLR
 *
 *  Created on: 09.10.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef TEST_GT_PROCESSDOCK_H
#define TEST_GT_PROCESSDOCK_H

#include "gt_calculator.h"
#include "gt_loop.h"
#include "gt_doubleproperty.h"
#include "gt_relativeobjectlinkproperty.h"


class TestGtCalculator : public GtCalculator
{
    Q_OBJECT

public:
    Q_INVOKABLE TestGtCalculator()
    {
        m_doubleProp1 = new GtDoubleProperty("doubleProp1", "TestDouble");

        m_doubleProp2 = new GtDoubleProperty("doubleProp2", "TestDouble");

        m_doubleProp3 = new GtDoubleProperty("doubleProp3", "TestDouble");

        m_relObjLink = new GtRelativeObjectLinkProperty("relLink", "Rel Link",
                                                        "Rel Link", "");

        registerProperty(*m_doubleProp1);
        registerProperty(*m_doubleProp2);
        registerProperty(*m_doubleProp3);
        registerProperty(*m_relObjLink);
    }

    ~TestGtCalculator() override
    {
        delete m_doubleProp1;
        delete m_doubleProp2;
        delete m_doubleProp3;
        delete m_relObjLink;
    }

    bool run() override
    {
        return true;
    }

    GtDoubleProperty* prop1()
    {
        return m_doubleProp1;
    }

    GtDoubleProperty* prop2()
    {
        return m_doubleProp2;
    }

    GtDoubleProperty* prop3()
    {
        return m_doubleProp3;
    }

    GtRelativeObjectLinkProperty* relLink()
    {
        return m_relObjLink;
    }

private:
    GtDoubleProperty* m_doubleProp1;
    GtDoubleProperty* m_doubleProp2;
    GtDoubleProperty* m_doubleProp3;
    GtRelativeObjectLinkProperty* m_relObjLink;
};

class TestGtLoop : public GtLoop
{
    Q_OBJECT

public:
    Q_INVOKABLE TestGtLoop()
    {
        m_doubleProp1 = new GtDoubleProperty("doubleProp1", "TestDouble");

        m_doubleProp2 = new GtDoubleProperty("doubleProp2", "TestDouble");

        m_doubleProp3 = new GtDoubleProperty("doubleProp3", "TestDouble");

        registerProperty(*m_doubleProp1);
        registerProperty(*m_doubleProp2);
        registerProperty(*m_doubleProp3);
    }

    ~TestGtLoop()
    {
        delete m_doubleProp1;
        delete m_doubleProp2;
        delete m_doubleProp3;
    }

    bool run()
    {
        return true;
    }

    GtDoubleProperty* prop1()
    {
        return m_doubleProp1;
    }

    GtDoubleProperty* prop2()
    {
        return m_doubleProp2;
    }

    GtDoubleProperty* prop3()
    {
        return m_doubleProp3;
    }

private:
    GtDoubleProperty* m_doubleProp1;
    GtDoubleProperty* m_doubleProp2;
    GtDoubleProperty* m_doubleProp3;
};

#endif // TEST_GT_PROCESSDOCK_H
