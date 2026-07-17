/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_command.h
 *
 *  Created on: 28.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_RECORDING_H
#define GT_RECORDING_H

#include <qobject.h>
#include "gt_core_exports.h"

#include <QString>

class GtCoreApplication;

/**
 * @brief The GtRecording class
 */
class GT_CORE_EXPORT GtRecording
{
    friend class GtCoreApplication;

public:
    /**
     * @brief GtCommand
     */
    GtRecording();

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
    QString m_actvityUuid;

    /**
     * @brief Constructor.
     * @param uuid Command identification string
     */
    explicit GtRecording(const QString& uuid);

};

#endif // GT_RECORDING_H
