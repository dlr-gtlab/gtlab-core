/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractprocessdata.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 11.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTPROCESSDATA_H
#define GTABSTRACTPROCESSDATA_H

#include "gt_core_exports.h"

class QString;

/**
 * @brief The GtAbstractProcessData class
 */
class GT_CORE_EXPORT GtAbstractProcessData
{
public:
    /**
     * @brief Status defines whether a process item can be use in
     * gtlab release mode(RELEASE) or not (PROTOTYPE, DEPRECATED)
     */
    enum DEV_STATUS
    {
        RELEASE = 0,
        PROTOTYPE = 1,
        DEPRECATED = 2
    };

    /**
     * @brief Destructor.
     */
    virtual ~GtAbstractProcessData();

    /**
     * @brief isValid
     * @return
     */
    virtual bool isValid() const = 0;

    /**
     * @brief Converts enumeration status to string.
     * @param s Dev status from enumeration
     * @return Converted status string
     */
    QString statusToString(GtAbstractProcessData::DEV_STATUS s);

    /// status variable
    GtAbstractProcessData::DEV_STATUS status;

protected:
    /**
     * @brief Constructor.
     */
    GtAbstractProcessData();

};

#endif // GTABSTRACTPROCESSDATA_H
