/* GTlab - Gas Turbine laboratory
 * Source File: test_externalizedobject.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 01.04.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */
#include "test_externalizedobject.h"

#include "gt_externalizationmanager.h"

#include <QDir>
#include <QDebug>

TestExternalizedObjectData::TestExternalizedObjectData(TestExternalizedObject* base) :
    GtExternalizedObjectData{base}
{

}

const QVector<double>&
TestExternalizedObjectData::values() const
{
    Q_ASSERT(m_base != nullptr);

    return base()->m_values;
}

void
TestExternalizedObjectData::setValues(const QVector<double>  &values)
{
    Q_ASSERT(m_base != nullptr);

    base()->m_values = values;
}

void
TestExternalizedObjectData::setAsInitialValues(const QVector<double>& values)
{
    Q_ASSERT(m_base != nullptr);

    base()->m_values = values;
    base()->m_initialValues = values;
}

void
TestExternalizedObjectData::clearValues()
{
    Q_ASSERT(m_base != nullptr);

    base()->m_values.clear();
}

const QVector<double>&
TestExternalizedObject::internalData() const
{
    return m_values;
}

bool
TestExternalizedObject::doFetchData(QVariant& /*metaData*/,
                                    bool fetchInitialVersion)
{
    if (fetchInitialVersion)
    {
        m_values = m_initialValues;
        return true;
    }

    QDir projectDir{gtExternalizationManager->projectDir()};

    if (!projectDir.exists())
    {
        qDebug() << "project dir does not exist!";
        return false;
    }

    // read data
    QFile file{projectDir.absoluteFilePath("test_ext" + uuid() + ".txt")};

    if (!file.open(QFile::ReadOnly))
    {
        qDebug() << "failed to open file!";
        return false;
    }

    QByteArray data = file.readAll();

    if (data.isEmpty())
    {
        qDebug() << "failed to read the data!";
        return false;
    }

    // deserialize data
    m_values.clear();
    for (auto sValue : data.split(';'))
    {
        m_values.append(sValue.toDouble());
    }

    return true;
}

bool
TestExternalizedObject::doExternalizeData(QVariant& /*metaData*/)
{
    QDir projectDir{gtExternalizationManager->projectDir()};

    if (!projectDir.exists())
    {
        qDebug() << "project dir does not exist!";
        return false;
    }

    // serialize data
    QByteArray data;
    for (auto value : qAsConst(m_values))
    {
        data.append(QByteArray::number(value) + ";");
    }
    data.chop(1); // cop trailing ;

    // write data
    QFile file{projectDir.absoluteFilePath("test_ext" + uuid()) + ".txt"};

    if (!file.open(QFile::WriteOnly))
    {
        qDebug() << "failed to open file!";
        return false;
    }

    if (file.write(data) != data.length())
    {
        qDebug() << "failed to write the data!";
        return false;
    }

    return true;
}

void
TestExternalizedObject::doClearExternalizedData()
{
    m_values.clear();
}
