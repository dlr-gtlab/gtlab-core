/* GTlab - Gas Turbine laboratory
 * Source File: gt_extendedcalculatordata.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 13.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXTENDEDCALCULATORDATA_H
#define GTEXTENDEDCALCULATORDATA_H

#include "gt_gui_exports.h"

#include <QIcon>

#include "gt_calculatordata.h"

class GtExtendedCalculatorDataImpl;
class GtCustomProcessWizard;

#if defined(_MSC_VER) || defined(__APPLE__)
#include <memory>
typedef std::shared_ptr<GtExtendedCalculatorDataImpl> GtExtendedCalculatorData;
#else
#include <tr1/memory>
typedef std::tr1::shared_ptr<GtExtendedCalculatorDataImpl> GtExtendedCalculatorData;
#endif

#define GT_EXTENDED_CALC_DATA(A) GtExtendedCalculatorDataImpl::newExtendedCalcData(A::staticMetaObject);

/**
 * @brief The GtExtendedCalculatorData class
 */
class GT_GUI_EXPORT GtExtendedCalculatorDataImpl : public GtCalculatorDataImpl
{
public:
    /**
     * @brief Constructor.
     * @param metaData Calculator class meta object.
     */
    GtExtendedCalculatorDataImpl(const QMetaObject& metaData);

    /**
     * @brief ~GtExtendedCalculatorDataImpl
     */
    virtual ~GtExtendedCalculatorDataImpl();

    /// Calculator icon
    QIcon icon;

    /// Custom calculator wizard
    GtCustomProcessWizard* wizard;

    /**
     * @brief Identifies if calculator data is valid.
     * @return Returns true if calculator data is valis. Otherwise false is
     * returned.
     */
    bool isValid() const Q_DECL_OVERRIDE;

    /**
     * @brief Generates new extended calculator data based on given meta data.
     * @param metaData Calculator class meta object.
     * @return Extended calculator data.
     */
    static GtExtendedCalculatorData newExtendedCalcData(
            const QMetaObject& metaData);

};

#endif // GTEXTENDEDCALCULATORDATA_H
