/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_calculatordata.h
 *
 *  Created on: 11.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCALCULATORDATA_H
#define GTCALCULATORDATA_H

#include "gt_core_exports.h"

#include "gt_abstractprocessdata.h"

#include <QStringList>
#include <QMetaObject>

class GtCalculatorDataImpl;

#if defined(_MSC_VER) || defined(__APPLE__)
#include <memory>
typedef std::shared_ptr<GtCalculatorDataImpl> GtCalculatorData;
#else
#include <tr1/memory>
typedef std::tr1::shared_ptr<GtCalculatorDataImpl> GtCalculatorData;
#endif

#define GT_CALC_DATA(A) GtCalculatorDataImpl::newCalcData(A::staticMetaObject)

/**
 * @brief The GtCalculatorData class
 */
class GT_CORE_EXPORT GtCalculatorDataImpl : public GtAbstractProcessData
{
public:
    /**
     * @brief Constructor.
     * @param metaData
     */
    explicit GtCalculatorDataImpl(const QMetaObject& metaData);

    /**
     * @brief Returns calculator meta data.
     * @return Calculator meta data.
     */
    const QMetaObject& metaData() const;

    /**
     * @brief Returns whether calculator data is valid or not.
     * @return Validation.
     */
    bool isValid() const override;

    /**
     * @brief newCalcData
     * @param metaData
     * @return
     */
    static GtCalculatorData newCalcData(const QMetaObject& metaData);

    /**
     * @brief Adds new environment variable to GTlab.
     * @param Environment variable identification string
     */
    void addEnvironmentVariable(const QString& var);

    /**
     * @brief Returns calculator specific environment variables.
     * @return List of environment variable identification strings
     */
    const QStringList& environmentVariables();

private:
    /// Calculator meta data.
    QMetaObject m_metaData;

    /// Environment variables.
    QStringList m_envVars;

};



#endif // GTCALCULATORDATA_H
