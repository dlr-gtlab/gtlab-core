/* GTlab - Gas Turbine laboratory
 * Source File: gt_loopui.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 13.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLOOPUI_H
#define GTLOOPUI_H

#include "gt_objectui.h"

/**
 * @brief The GtLoopUI class
 */
class GtLoopUI : public GtObjectUI
{
    Q_OBJECT

public:
    /**
     * @brief GtLoopUI
     */
    Q_INVOKABLE GtLoopUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    virtual QIcon icon(GtObject* obj) const Q_DECL_OVERRIDE;

    /**
     * @brief hasValidationRegExp
     * @return true, because element has validator
     */
    bool hasValidationRegExp();

    /**
     * @brief validatorRegExp
     * @return Regexp to accept letters, digits, -, _, and space
     */
    QRegExp validatorRegExp();

protected:
    /**
     * @brief specificData
     * @param obj
     * @param role
     * @param column
     * @return
     */
    QVariant specificData(GtObject* obj,
                          int role,
                          int column) const Q_DECL_OVERRIDE;

};

#endif // GTLOOPUI_H
