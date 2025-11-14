/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_object
 *
 *  Created on: 20.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef TEST_GT_OBJECT_H
#define TEST_GT_OBJECT_H

#include <QPointF>
#include <QRegExpValidator>

#include "gt_objectgroup.h"

#include "gt_boolproperty.h"
#include "gt_doubleproperty.h"
#include "gt_openfilenameproperty.h"
#include "gt_groupproperty.h"
#include "gt_intproperty.h"
#include "gt_labelproperty.h"
#include "gt_modeproperty.h"
#include "gt_modetypeproperty.h"
#include "gt_objectlinkproperty.h"
#include "gt_stringproperty.h"
#include "gt_variantproperty.h"
#include "gt_doublelistproperty.h"
#include "gt_existingdirectoryproperty.h"
#include "gt_openfilenameproperty.h"
#include "gt_regularexpression.h"
#include "test_propertycontainerobject.h"

class TestSpecialGtObject : public GtObjectGroup
{
    Q_OBJECT

//    Q_PROPERTY(bool boolProp READ getBool WRITE setBool)
//    Q_PROPERTY(double doubleProp READ getDouble WRITE setDouble)
//    Q_PROPERTY(QString fileProp READ getFile WRITE setFile)
//    Q_PROPERTY(int intProp READ getInt WRITE setInt)
//    Q_PROPERTY(QString labelProp READ getLabel WRITE setLabel)
//    Q_PROPERTY(QString modeProp READ getMode WRITE setMode)
//    Q_PROPERTY(QString linkProp READ getLink WRITE setLink)
//    Q_PROPERTY(QString strProp READ getString WRITE setString)
    Q_PROPERTY(QVector<double> dVec READ getDoubleVec WRITE setDoubleVec)
//    Q_PROPERTY(QVector<int> iVec MEMBER intVec)
//    Q_PROPERTY(QList<QPointF> pVec MEMBER pointVec)
//    Q_PROPERTY(QList<int> iList MEMBER intList)
    Q_PROPERTY(QStringList strList MEMBER stringList)
//    Q_PROPERTY(QList<bool> bList MEMBER boolList)

public:
    Q_INVOKABLE TestSpecialGtObject() :
        m_boolProp("boolProp", "Test Bool", QString()),
        m_doubleProp("doubleProp", "Test Double",
                                            QString(), GtUnit::Category::Angle),
        m_fileProp("fileProp", "Test File", QString(), QStringList()),
        m_groupProp("Test Group", QString()),
        m_intProp("intProp", "Test Int", QString(), GtUnit::Category::Angle),
        m_labelProp("labelProp", "Test Label", QString(), this),
        m_modeProp("modeProp", "Test Mode", QString()),
        m_modeTypeProp("Test Type", QString()),
        m_linkProp("linkProp", "Test Link", QString(), QString(), this, {}),
        m_strProp("strProp", "Test String",  QString(), "Test",
                   new QRegularExpressionValidator(gt::rex::onlyLettersAndNumbers(), this)),
        m_varProp("variantProp", "Test Variant", QString()),
        m_doubleListProp("dblList", "Double List Property"),
        m_exDirProp("exDir", "Existing Directory", "Existing Directory Property"),
        m_fileChooser("fileChooser", "File Chooser", "File Chooser", {"dat"})
    {
        m_intProp.setOptional(true);

        boolList << true << false << true;

        registerProperty(m_boolProp);
        registerProperty(m_doubleProp);
        registerProperty(m_fileProp);
        registerProperty(m_groupProp);
        m_groupProp.registerSubProperty(m_intProp);
        registerProperty(m_intProp);
        registerProperty(m_labelProp);
        registerProperty(m_modeProp);
        m_modeProp.registerSubProperty(m_modeTypeProp);
        registerProperty(m_linkProp);
        registerProperty(m_strProp);
        registerProperty(m_varProp);
        registerProperty(m_doubleListProp);
        registerProperty(m_exDirProp);
        registerProperty(m_fileChooser);
    }

    bool getBool()
    {
        return m_boolProp.get();
    }

    void setBool(bool val)
    {
        m_boolProp.setVal(val);
    }

    double getDouble()
    {
        return m_doubleProp.get();
    }

    void setDouble(double val)
    {
        m_doubleProp.setVal(val);
    }

    QString getFile()
    {
        return m_fileProp.get();
    }

    void setFile(const QString& val)
    {
        m_fileProp.setVal(val);
    }

    int getInt()
    {
        return m_intProp.get();
    }

    void setInt(int val)
    {
        m_intProp.setVal(val);
    }

    QString getLabel()
    {
        return m_labelProp.get();
    }

    void setLabel(const QString& val)
    {
        m_labelProp.setVal(val);
    }

    QString getMode()
    {
        return m_modeProp.get();
    }

    void setMode(const QString& val)
    {
        m_modeProp.setVal(val);
    }

    QString getLink()
    {
        return m_linkProp.get();
    }

    void setLink(const QString& val)
    {
        m_linkProp.setVal(val);
    }

    QString getString()
    {
        return m_strProp.get();
    }

    void setString(const QString& str)
    {
        m_strProp.setVal(str);
    }

    QVariant getVariant()
    {
        return m_varProp.get();
    }

    void setVariant(const QVariant& var)
    {
        m_varProp.setVal(var);
    }

    QVector<double> getDoubleVec() const
    {
        return doubleVec;
    }

    void setDoubleVec(const QVector<double>& value)
    {
        doubleVec = value;
    }

    bool childAccepted(GtObject* child) override
    {
        return qobject_cast<GtObjectGroup*>(child) != nullptr ||
               qobject_cast<GtPropertyConnection*>(child) != nullptr ||
               qobject_cast<TestObject*>(child) != nullptr;
    }

    GtBoolProperty m_boolProp;

    GtDoubleProperty m_doubleProp;

    GtOpenFileNameProperty m_fileProp;

    GtGroupProperty m_groupProp;

    GtIntProperty m_intProp;

    GtLabelProperty m_labelProp;

    GtModeProperty m_modeProp;

    GtModeTypeProperty m_modeTypeProp;

    GtObjectLinkProperty m_linkProp;

    GtStringProperty m_strProp;

    GtVariantProperty m_varProp;

    GtDoubleListProperty m_doubleListProp;

    GtExistingDirectoryProperty m_exDirProp;

    GtOpenFileNameProperty m_fileChooser;

    QVector<double> doubleVec;

    QList<QPointF> pointVec;

    QList<int> intList;

    QVector<int> intVec;

    QStringList stringList;

    QList<bool> boolList;

};

#endif // TEST_GT_OBJECT_H
