/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatorhelperfactory.h
 *
 *  Created on: 09.08.2019
 *  Author: Marvin Noethen (AT-TW)
 *  Tel.: +49 2203 601 2692
 */

#ifndef GT_CALCULATORHELPERFACTORY_H
#define GT_CALCULATORHELPERFACTORY_H

#include "gt_core_exports.h"

#include <QObject>

#include "gt_abstractobjectfactory.h"
#include "gt_calculatordata.h"

class GtObject;

#define gtCalculatorHelperFactory (GtCalculatorHelperFactory::instance())

/**
 * @brief The GtCalculatorHelperFactory class
 */
class GT_CORE_EXPORT GtCalculatorHelperFactory : public QObject,
        public GtAbstractObjectFactory
{
    Q_OBJECT

public:
    /**
     * @brief Returns instance of calculator helper factory based
     * on singleton pattern.
     * @return Instance of calculator helper factory
     */
    static GtCalculatorHelperFactory* instance();

    /**
     * @brief Generates a calculator helper object based on given helper class
     * name and appends it to the given parent. That only happens if the helper
     * class name is connected as helper of the given parent.
     * @param helperClassName Class name of the helper class.
     * @param objName Object name of helper object.
     * @param parent The parent of the new generated helper object.
     * @return Returns pointer to generated helper object or NULL if generation
     *  fails.
     */
    Q_INVOKABLE GtObject* newCalculatorHelper(const QString& helperClassName,
                                  const QString& objName,
                                  GtObject* parent);

    /**
     * @brief Connects calculator helper to parent class and
     * registers helper to factory.
     * @param calcClassName Class name of calculator class that uses the
     * registered helper.
     * @param helper New helper meta object.
     * @return  Returns true if calculstor helper was successfully added.
     */
    bool connectHelper(const QString& parentClassName,
                                  QMetaObject helper);

    /**
     * @brief Returns list of helper class names which are connected with
     * given parent class name.
     * @param parentClassName Name of parent class parent whose
     * helpers are to be found.
     * @return List of helper class names.
     */
    Q_INVOKABLE QStringList connectedHelper(const QString& parentClassName);
private:
    /// Calculator data
    QHash<QString, QList<QString>> m_calcHelperConnection;

    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    explicit GtCalculatorHelperFactory(QObject* parent);
};

#endif // GT_CALCULATORHELPERFACTORY_H
