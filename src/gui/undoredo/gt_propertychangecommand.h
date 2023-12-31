/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 02.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYCHANGECOMMAND_H
#define GTPROPERTYCHANGECOMMAND_H

#include <QUndoCommand>
#include <QPointer>
#include <QVariant>

class GtObject;
class GtAbstractProperty;
class GtSession;

/**
 * @brief The GtPropertyChangeCommand class
 */
class GtPropertyChangeCommand : public QUndoCommand
{
public:
    /**
     * @brief GtPropertyChangeCommand
     */
    GtPropertyChangeCommand(GtObject* obj,
                            GtAbstractProperty* prop,
                            const QVariant& newVal,
                            const QString& unit,
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
    QString m_uuid;

    ///
    QString m_id;

    ///
    QString m_unit;

    ///
    QVariant m_oldValue;

    ///
    QVariant m_newValue;

};

#endif // GTPROPERTYCHANGECOMMAND_H
