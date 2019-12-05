/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATORFACTORY_H
#define GTCALCULATORFACTORY_H

#include "gt_calculators_exports.h"

#include <QObject>

#include "gt_abstractobjectfactory.h"
#include "gt_calculatordata.h"
#include "gt_calculatorhelperfactory.h"

#define gtCalculatorFactory (GtCalculatorFactory::instance())
#define REGISTER_HELPER(A, B) gtCalculatorHelperFactory->connectHelper(A::staticMetaObject.className(), B::staticMetaObject)
/**
 * @brief The GtCalculatorFactory class
 */
class GT_CALCULATORS_EXPORT GtCalculatorFactory : public QObject,
        public GtAbstractObjectFactory
{
public:
    /**
     * @brief Returns instance of calculator factory based on singleton pattern.
     * @return Instance of calculator factory
     */
    static GtCalculatorFactory* instance();

    /**
     * @brief Registers calculator data to factory.
     * @param taskData New calculstor data.
     * @return Returns true if calculstor data was successfully added.
     */
    virtual bool registerCalculatorData(GtCalculatorData calcData);

    /**
     * @brief Returns full list of registered calculator data.
     * @return List of calculator data.
     */
    QList<GtCalculatorData> calculatorDataList() const;

    /**
     * @brief Returns specific calculator data based on given classname.
     * Returns invalid data if no matching data could be found.
     * @param className Classname of requested calculator data.
     * @return Calculator data.
     */
    GtCalculatorData calculatorData(const QString& className);

    /**
     * @brief Checks if calculator data exists for given class name.
     * @param className Calculator class name.
     * @return True if calculator data for given class name exists.
     */
    bool calculatorDataExists(const QString& className);

private:
    /// Calculator data
    QHash<QString, GtCalculatorData> m_calcData;

    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    GtCalculatorFactory(QObject* parent = Q_NULLPTR);

};

#endif // GTCALCULATORFACTORY_H
