/* GTlab - Gas Turbine laboratory
 * Source File: gt_mementochangecommand.h
 * copyright 2009-2016 by DLR
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
    virtual void redo();

    /**
     * @brief undo
     */
    virtual void undo();

private:
    ///
    QPointer<GtObject> m_root;

    ///
    GtObjectMementoDiff m_diff;

    ///
    bool m_firstTime;

};

#endif // GTMEMENTOCHANGECOMMAND_H
