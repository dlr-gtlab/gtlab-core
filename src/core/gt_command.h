/* GTlab - Gas Turbine laboratory
 * Source File: gt_command.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 28.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COMMAND_H
#define GT_COMMAND_H

#include "gt_core_exports.h"

#include <QString>

class GtCoreApplication;

/**
 * @brief The GtCommand class
 */
class GT_CORE_EXPORT GtCommand
{
    friend class GtCoreApplication;

public:
    /**
     * @brief GtCommand
     */
    GtCommand();

    /**
     * @brief Returns command identification string.
     * @return Command identification string
     */
    QString id() const;

    /**
     * @brief Returns true if command id is not empty.
     * @return Whether command is valid or not
     */
    bool isValid() const;

private:
    /// Command identification string
    QString m_id;

    /**
     * @brief Constructor.
     * @param uuid Command identification string
     */
    explicit GtCommand(const QString& uuid);

};

#endif // GT_COMMAND_H
