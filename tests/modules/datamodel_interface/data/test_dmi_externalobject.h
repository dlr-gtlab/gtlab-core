/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 21.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef TESTDMIEXTERNALOBJECT_H
#define TESTDMIEXTERNALOBJECT_H

#include "test_datamodel_interface_exports.h"
#include "gt_externalizedobject.h"

class TestDmiExternalObject;

/**
 * @brief The TestDmiExternalObject class.
 * Helper class for external test object
 */
class TEST_DATAMODEL_INTERFACE_EXPORT TestDmiExternalObjectData :
        public GtExternalizedObjectData
{
    GT_DECL_BASECLASS(TestDmiExternalObject)

public:

    explicit TestDmiExternalObjectData(TestDmiExternalObject* base);

    /*
     * GETTER
     */

    QStringList const& params() const;

    QVector<double> const& values() const;

    double value(int idx) const;

    QString const& param(int idx) const;

    /*
     * SETTER
     */

    void setParams(QStringList params) const;

    void setValues(QVector<double> vals) const;
};

/**
 * @brief The TestDmiExternalObject class.
 * Simple data class to test externalization of an object
 */
class TEST_DATAMODEL_INTERFACE_EXPORT TestDmiExternalObject :
        public GtExternalizedObject
{
    Q_OBJECT

    GT_DECL_DATACLASS(TestDmiExternalObjectData)

    Q_PROPERTY(QStringList params MEMBER m_params)

    Q_PROPERTY(QVector<double> values MEMBER m_values)

public:

    /**
      * @brief Constructor.
      */
    Q_INVOKABLE TestDmiExternalObject();

    /**
     * @brief Returns whether the object data is valid.
     * This is a delegating method and should not be called directly.
     * @return is valid
     */
    bool isDataValid() const override;

    /**
     * @brief Returns whether the object can be externalized.
     * @return can externalize
     */
    bool canExternalize() const override;

    /**
     * @brief Method for fetching the externalized data
     * @param metaData Data that may need to be saved in between sessions to
     * help fetch/externalize the dataset.
     * @param fetchInitialVersion Whether to fetch the intial version of the
     * data (i.e. the original data that has not changed)
     * @return success
     */
    bool doFetchData(QVariant& metaData, bool fetchInitialVersion) override;

    /**
     * @brief Method to implement for externalizing the data. Will only be
     * called if canExternalize returns true.
     * @param metaData Data that may need to be saved in between sessions to
     * help fetch/externalize the dataset.
     * @return success
     */
    bool doExternalizeData(QVariant& metaData) override;

    /**
     * @brief doClearExternalizedData method to implement for clearing the
     * externalized data.
     */
    void doClearExternalizedData() override;

protected:

    void onObjectDiffMerged() override;

private:

    /// example params
    QStringList m_params;

    /// example values
    QVector<double> m_values;
};

#endif // TESTDMIEXTERNALOBJECT_H
