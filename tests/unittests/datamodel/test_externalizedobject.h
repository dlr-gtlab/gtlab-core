/* GTlab - Gas Turbine laboratory
 * Source File: test_externalizedobject.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 01.04.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef TEST_EXTERNALOBECT_H
#define TEST_EXTERNALOBECT_H

#include "gt_abstractproperty.h"
#include "gt_externalizedobject.h"

#include <QVariant>
#include <QVector>

class TestExternalizedObject;
class TestExternalizedObjectData : public GtExternalizedObjectData
{    
    GT_DECL_BASECLASS(TestExternalizedObject)

public:

    explicit TestExternalizedObjectData(TestExternalizedObject* base);

    const QVector<double>& values() const;

    void setValues(const QVector<double>& values);

    const QStringList& params() const;

    void setParams(const QStringList& params);

    void setInitialValues(const QVector<double>& values);

    void setInitialParams(const QStringList& params);

    void clearValues();
};


class TestExternalizedObject : public GtExternalizedObject
{
    Q_OBJECT

    Q_PROPERTY(QVector<double> values MEMBER m_values)

    Q_PROPERTY(QStringList params MEMBER m_params)

    GT_DECL_DATACLASS(TestExternalizedObjectData)

public:

    Q_INVOKABLE TestExternalizedObject() = default;

    /// access to internal data for testing purpose
    const QVector<double>& internalValues() const;
    const QStringList& internalParams() const;

    /// check if the data was used with params
    bool hasParams() const;

    /// forwarding internal methods and member for easier testing
    bool fetchInitialVersion() const
    {
        auto prop = findProperty("fetchInitialVersion");
        assert(prop);
        return prop->value<bool>();
    }

    void setFetchInitialVersion(bool value)
    {
        GtExternalizedObject::setFetchInitialVersion(value);
    }

    bool hasModifiedData()
    {
        return GtExternalizedObject::hasModifiedData();
    }

    bool hasModifiedData(const QString& other) const
    {
        return GtExternalizedObject::hasModifiedData(other);
    }

    QString calcExtHash()
    {
        return GtExternalizedObject::calcExtHash();
    }

protected:

    /**
     * @brief fetches the intData.
     * @param metaData meta data
     * @return success
     */
    bool doFetchData(QVariant& metaData, bool fetchInitialVersion) override;

    /**
     * @brief externalizes the intData.
     * @param metaData meta data for externalization
     * @return success
     */
    bool doExternalizeData(QVariant& metaData) override;

    /**
     * @brief clears intData.
     */
    void doClearExternalizedData() override;

private:

    /// simple double data to test externalization
    QVector<double> m_values;
    /// optional string data to test externalization for compound data
    QStringList m_params;

    /// simulates initial values
    QVector<double> m_initialValues;
    /// simulates initial params
    QStringList m_initialParams;
};

#endif // TEST_EXTERNALOBECT_H
