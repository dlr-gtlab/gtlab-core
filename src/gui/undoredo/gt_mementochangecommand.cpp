/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_mementochangecommand.cpp
 *
 *  Created on: 11.08.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_object.h"
#include "gt_datamodel.h"
#include "gt_logging.h"

#include "gt_mementochangecommand.h"

GtMementoChangeCommand::GtMementoChangeCommand(const GtObjectMementoDiff& diff,
                                               const QString& msg,
                                               GtObject* root,
                                               QUndoCommand* parent) :
    QUndoCommand(parent),
    m_root(root),
    m_diff(diff),
    m_firstTime(true)
{
    // undo text
    setText(msg);
}

void
GtMementoChangeCommand::redo()
{
    if (!m_root)
    {
        gtWarning() << QObject::tr("Root object not found!") <<
                       QStringLiteral(" (change command)");
        return;
    }

    const bool objTreeChanges = m_diff.hasObjectTreeChanges();

    if (objTreeChanges && !m_firstTime)
    {
        gtDataModel->beginResetModelView();
    }

    if (!m_firstTime)
    {
        if (!m_root->applyDiff(m_diff))
        {
            gtWarning() << QObject::tr("Could not apply diff!") <<
                           QStringLiteral(" (change command)");
//            gtDebug() << m_diff.toString();
        }
    }
    else
    {
        m_firstTime = false;
    }

    if (objTreeChanges && !m_firstTime)
    {
        gtDataModel->endResetModelView();
    }
}

void
GtMementoChangeCommand::undo()
{
    if (!m_root)
    {
        gtWarning() << QObject::tr("Root object not found!") <<
                       QStringLiteral(" (change command)");
        return;
    }

    const bool objTreeChanges = m_diff.hasObjectTreeChanges();

    if (objTreeChanges)
    {
        gtDataModel->beginResetModelView();
    }

    if (!m_root->revertDiff(m_diff))
    {
        gtWarning() << QObject::tr("Could not revert diff!") <<
                       QStringLiteral(" (change command)");
    }

    if (objTreeChanges)
    {
        gtDataModel->endResetModelView();
    }
}
