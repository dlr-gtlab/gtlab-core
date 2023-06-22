/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_mementochangecommand.h
 *
 *  Created on: 11.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMEMENTOCHANGECOMMAND_H
#define GTMEMENTOCHANGECOMMAND_H

#include <QUndoCommand>
#include <QPointer>
#include "gt_objectmementodiff.h"

class GtObject;

/**
 * @brief The GtMementoChangeCommand class
 */
class GtMementoChangeCommand : public QUndoCommand
{
public:
    GtMementoChangeCommand(const GtObjectMementoDiff& diff,
                           const QString& msg,
                           GtObject* root = nullptr,
                           QUndoCommand* parent = nullptr);

    /**
     * @brief redo
     */
    void redo() override;

    /**
     * @brief undo
     */
    void undo() override;

private:
    ///
    QPointer<GtObject> m_root;

    ///
    GtObjectMementoDiff m_diff;

    ///
    bool m_firstTime;

};

#endif // GTMEMENTOCHANGECOMMAND_H
