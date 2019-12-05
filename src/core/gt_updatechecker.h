/* GTlab - Gas Turbine laboratory
 * Source File: gt_updatechecker.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 20.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_UPDATECHECKER_H
#define GT_UPDATECHECKER_H

#include "gt_core_exports.h"

#include <QObject>

/**
 * @brief The GtUpdateChecker class
 */
class GT_CORE_EXPORT GtUpdateChecker : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent object
     */
    GtUpdateChecker(QObject* parent = Q_NULLPTR);

public slots:
    /**
     * @brief Checks whether update exists or not.
     */
    void checkForUpdate();

signals:
    /**
     * @brief Emitted if check update process ends up with error.
     * @param errorCode Error Code
     * 1    Maintenance Tool no found
     * 2    Timed out
     * 3    Update file not found
     * 4    Could not open update file
     * 5    Connection refused
     * 6    No updates available
     * 7    Failure
     * 8    Error
     * @param errorStr Error string
     */
    void error(int errorCode, const QString& errorStr);

    /**
     * @brief Emitted if update exists.
     */
    void updateAvailable();

};

#endif // GT_UPDATECHECKER_H
