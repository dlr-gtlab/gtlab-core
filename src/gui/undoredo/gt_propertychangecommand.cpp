/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 02.12.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_propertychangecommand.h"
#include "gt_object.h"
#include "gt_abstractproperty.h"
#include "gt_logging.h"

GtPropertyChangeCommand::GtPropertyChangeCommand(GtObject* obj,
                                                 GtAbstractProperty* prop,
                                                 const QVariant& newVal,
                                                 const QString& unit,
                                                 GtObject* root,
                                                 QUndoCommand* parent) :
    QUndoCommand(parent),
    m_root(root)
{
    // undo text
    QString str = obj->objectName() + QStringLiteral(" - ") +
                  prop->objectName() + QStringLiteral(" ") +
                  QObject::tr("changed");
    setText(str);

    if (!m_root)
    {
        m_root = obj;
    }

    m_uuid = obj->uuid();
    m_id = prop->ident();
    m_unit = unit;
    m_oldValue = prop->valueToVariant(unit);
    m_newValue = newVal;
}

void
GtPropertyChangeCommand::redo()
{
    if (!m_root)
    {
        gtError() << QObject::tr("root object not found!")
                  << QStringLiteral(" ")
                  << QObject::tr("needed by undo command!");
        return;
    }

    GtObject* obj = m_root->getObjectByUuid(m_uuid);

    if (!obj)
    {
        gtError() << QObject::tr("object not found!")
                  << QStringLiteral(" ")
                  << QObject::tr("needed by undo command!");
        return;
    }

    GtAbstractProperty* prop = obj->findProperty(m_id);
    if (!prop)
    {
        gtError() << QObject::tr("property not found!")
                  << QStringLiteral(" ")
                  << QObject::tr("needed by undo command!");
        return;
    }

    if (prop->setValueFromVariant(m_newValue, m_unit))
    {
    }

    QUndoCommand::redo();
}

void
GtPropertyChangeCommand::undo()
{
    if (!m_root)
    {
        gtError() << QObject::tr("root object not found!")
                  << QStringLiteral(" ")
                  << QObject::tr("needed by undo command!");
        return;
    }

    GtObject* obj = m_root->getObjectByUuid(m_uuid);

    if (!obj)
    {
        gtError() << QObject::tr("object not found!")
                  << QStringLiteral(" ")
                  << QObject::tr("needed by undo command!");
        return;
    }

    GtAbstractProperty* prop = obj->findProperty(m_id);
    if (!prop)
    {
        gtError() << QObject::tr("property not found!")
                  << QStringLiteral(" ")
                  << QObject::tr("needed by undo command!");
        return;
    }

    if (prop->setValueFromVariant(m_oldValue, m_unit))
    {
    }

    QUndoCommand::undo();
}

