/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_externalizedobject.cpp
 *
 *  Created on: 01.04.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */
#include "test_externalizedobject.h"

#include "gt_externalizationmanager.h"

#include "gt_logging.h"

#include <QDir>
#include <QDebug>

TestExternalizedObjectData::TestExternalizedObjectData(TestExternalizedObject* base) :
    GtExternalizedObjectData{base}
{

}

const QVector<double>&
TestExternalizedObjectData::values() const
{
    assert(m_base);
    return base()->m_values;
}

void
TestExternalizedObjectData::setValues(const QVector<double>  &values) &
{
    assert(m_base);
    base()->m_values = values;
}

const QStringList&
TestExternalizedObjectData::params() const
{
    assert(m_base);
    return base()->m_params;
}

void
TestExternalizedObjectData::setParams(const QStringList& params) &
{
    assert(m_base);
    base()->m_params = params;
}

void
TestExternalizedObjectData::setInitialValues(const QVector<double>& values) &
{
    assert(m_base);
    base()->m_values = values;
    base()->m_initialValues = values;
}


void
TestExternalizedObjectData::setInitialParams(const QStringList& params) &
{
    assert(m_base);
    base()->m_params = params;
    base()->m_initialParams = params;
}

void
TestExternalizedObjectData::clearValues() &
{
    assert(m_base);
    base()->m_values.clear();
}

const QVector<double>&
TestExternalizedObject::internalValues() const
{
    return m_values;
}

const QStringList&
TestExternalizedObject::internalParams() const
{
    return m_params;
}

bool
TestExternalizedObject::hasParams() const
{
    return !m_initialParams.isEmpty();
}

bool
TestExternalizedObject::doFetchData(QVariant& /*metaData*/,
                                    bool fetchInitialVersion)
{
    const auto readDataHelper = [uuid = uuid()](const QString& suffix,
                                                const auto& deserializer) {
        QDir projectDir{gtExternalizationManager->projectDir()};
        if (!projectDir.exists())
        {
            gtError() << "Project dir does not exist!";
            return false;
        }

        // read data
        QFile file{projectDir.absoluteFilePath("test_ext" + uuid + "_" +
                                               suffix + ".txt")};

        if (!file.open(QFile::ReadOnly))
        {
            gtError() << "Failed to open file!";
            return false;
        }

        QByteArray data = file.readAll();
        // deserialize data
        return deserializer(data);
    };

    if (fetchInitialVersion)
    {
        m_values = m_initialValues;
        m_params = m_initialParams;
        return true;
    }

    // read values and params consecutively
    bool success = true;
    success &= readDataHelper("values", [&](const QByteArray& data){
        m_values.clear();
        for (auto sValue : data.split(';'))
        {
            m_values.append(sValue.toDouble());
        }
        return true;
    });
    success &= readDataHelper("params", [&](const QByteArray& data){
        m_params.clear();
        for (auto p : data.split(';'))
        {
            m_params.append(p);
        }
        return true;
    });
    return success;
}

bool
TestExternalizedObject::doExternalizeData(QVariant& /*metaData*/)
{
    const auto writeDataHelper = [uuid = uuid()](const QString& suffix,
                                                 const auto& serializer) {
        QDir projectDir{gtExternalizationManager->projectDir()};
        if (!projectDir.exists())
        {
            gtError() << "Project dir does not exist!";
            return false;
        }

        // read data
        QFile file{projectDir.absoluteFilePath("test_ext" + uuid + "_" +
                                               suffix + ".txt")};

        if (!file.open(QFile::ReadWrite | QIODevice::Truncate))
        {
            gtError() << "Failed to open file!";
            return false;
        }

        // serialize and write data
        QByteArray data = serializer();
        if (file.write(data) != data.length())
        {
            qDebug() << "Failed to write the data!";
            return false;
        }

        return true;
    };

    // write values and params consecutively
    bool success = true;
    success &= writeDataHelper("values", [&](){
        QByteArray data;

        if (m_values.isEmpty()) return data;

        for (auto value : qAsConst(m_values))
        {
            data.append(QByteArray::number(value) + ";");
        }
        return data.chopped(1); // chop trailing ;
    });
    success &= writeDataHelper("params", [&](){
        QByteArray data;

        if (m_params.isEmpty()) return data;

        for (const auto& p : qAsConst(m_params))
        {
            data.append(p.toUtf8() + ";");
        }
        return data.chopped(1); // chop trailing ;
    });
    return success;
}

void
TestExternalizedObject::doClearExternalizedData()
{
    m_values.clear();
    m_params.clear();
}
