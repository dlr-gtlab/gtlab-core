/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_gt_processexecutioninfo.h
 */

#ifndef TEST_GT_PROCESSEXECUTIONINFO_H
#define TEST_GT_PROCESSEXECUTIONINFO_H

#include "gtest/gtest.h"

#include <memory>

#include "gt_package.h"
#include "gt_project.h"
#include "gt_coreapplication.h"
#include "gt_coreprocessexecutor.h"
#include "gt_objectlinkproperty.h"
#include "gt_task.h"
#include "gt_calculator.h"
#include "gt_doubleproperty.h"
#include "gt_stringproperty.h"
#include "gt_objectfactory.h"
#include "gt_propertyconnection.h"


class TestPackage : public GtPackage
{
public:
    TestPackage() = default;
};

class TestObjectDouble : public GtObject
{
    Q_OBJECT
public:
    Q_INVOKABLE TestObjectDouble():
        m_x("x", "x")
    {
        registerProperty(m_x);
    };

    void setX(double val)
    {
        m_x.setVal(val);
    }

    double getX()
    {
        return m_x.getVal();
    }

private:
    GtDoubleProperty m_x;
};


class TestCalculatorAPlusB : public GtCalculator
{
    Q_OBJECT
public:
    Q_INVOKABLE TestCalculatorAPlusB()
    {
        setObjectName("TestCalculatorAPlusB");
        m_a = new GtDoubleProperty("a", "a");
        m_b = new GtDoubleProperty("b", "b");
        m_c = new GtDoubleProperty("c", "c");

        registerProperty(*m_a);
        registerProperty(*m_b);

        registerMonitoringProperty(*m_c);
    }


    ~TestCalculatorAPlusB() override
    {
        delete m_a;
        delete m_b;
        delete m_c;
    }

    void setA(double val)
    {
        m_a->setVal(val);
    }

    void setB(double val)
    {
        m_b->setVal(val);
    }

    bool run() override
    {
        auto a = m_a->getVal();
        auto b = m_b->getVal();
        double c = a+b;
        qDebug() << "a+b=" << a << "+" << b << "=" << c;
        m_c->setVal(c);
        qDebug() << "m_c->getVal()=="<< m_c->getVal();
        return true;
    }

private:
    GtDoubleProperty* m_a;
    GtDoubleProperty* m_b;
    GtDoubleProperty* m_c;
};


class TestCalculatorWriteToObject : public GtCalculator
{
    Q_OBJECT
public:
    Q_INVOKABLE TestCalculatorWriteToObject()
    {
        setObjectName("TestCalculatorWriteToObject");
        m_a = new GtDoubleProperty("a", "a");
        m_obj = new GtObjectLinkProperty("obj", "obj", "", this, QStringList() << GT_CLASSNAME(TestObjectDouble), true);
        registerProperty(*m_a);
        registerProperty(*m_obj);
    }

    ~TestCalculatorWriteToObject() override
    {
        delete m_a;
    }

    void setLinkedObj(TestObjectDouble* obj)
    {
        m_obj->setVal(obj->uuid());
        qDebug() << "setLinkedObj" << this;
        qDebug() << "|-> obj:" << obj->uuid();
        qDebug() << "|-> m_obj:" << m_obj->getVal();
    }

    bool run() override
    {
        qDebug() << "run TestCalculatorWriteToObject" << this;
        qDebug() << "m_obj:" << m_obj << m_obj->getVal();

        qDebug() << "proccomp::data";
        qDebug() << "linkedObjects:" << linkedObjects();

        auto obj = data<TestObjectDouble*>(*m_obj);

        if (!obj)
        {
            qDebug() << "TestCalculatorWriteToObject - Linked obj not found!";
            qDebug() << "uuid:" <<  m_obj->getVal();
            return false;
        }
        qDebug() << "obj:" << obj;
        qDebug() << "m_a->getVal():" << m_a->getVal();
        obj->setX(m_a->getVal());
        qDebug() << "obj getX:" << obj->getX();
        return true;
    }

private:
    GtDoubleProperty* m_a;
    GtObjectLinkProperty* m_obj;
};


class TestCalculatorFailing : public GtCalculator
{
    Q_OBJECT
public:
    Q_INVOKABLE TestCalculatorFailing()
    {
    }

    ~TestCalculatorFailing() override
    {
    }

    bool run() override
    {
        return false;
    }
};

class TestTask1 : public GtTask
{
    Q_OBJECT
public:
    Q_INVOKABLE TestTask1()
    {
        m_calc1 = new TestCalculatorAPlusB;
        m_calc2 = new TestCalculatorWriteToObject;
        appendChild(m_calc1);
        appendChild(m_calc2);

        m_conn1 = new GtPropertyConnection;
        m_conn1->setObjectName("Connection1");

        m_conn1->setSourceUuid(m_calc1->uuid());
        m_conn1->setSourceProp("c");
        m_conn1->setTargetUuid(m_calc2->uuid());
        m_conn1->setTargetProp("a");

        appendChild(m_conn1);

        m_conn1->makeConnection();

    }

    ~TestTask1() override
    {
        delete m_calc1;
        delete m_calc2;
        delete m_conn1;
    }

    void setInput(double a, double b, TestObjectDouble* targetobj)
    {
        m_calc1->setA(a);
        m_calc1->setB(b);
        m_calc2->setLinkedObj(targetobj);
    }


private:
    TestCalculatorAPlusB* m_calc1;
    TestCalculatorWriteToObject* m_calc2;
    GtPropertyConnection* m_conn1;

};


#endif // TEST_GT_PROCESSEXECUTIONINFO_H
