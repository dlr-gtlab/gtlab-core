/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractloadinghelper.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 29.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GT_ABSTRACTLOADINGHELPER_H
#define GT_ABSTRACTLOADINGHELPER_H

#include "gt_core_exports.h"

#include <QObject>

/**
 * @brief The GtAbstractLoadingHelper class
 */
class GT_CORE_EXPORT GtAbstractLoadingHelper : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GtAbstractLoadingHelper
     */
    GtAbstractLoadingHelper();

    /**
     * @brief ~GtAbstractLoadingHelper
     */
    ~GtAbstractLoadingHelper() override;

    /**
     * @brief run
     */
    virtual void run() = 0;

public slots:
    /**
     * @brief exec
     */
    void exec();

signals:
    /**
     * @brief finished
     */
    void finished();

};

#endif // GT_ABSTRACTLOADINGHELPER_H
