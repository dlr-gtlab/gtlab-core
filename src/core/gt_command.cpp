/* GTlab - Gas Turbine laboratory
 * Source File: gt_command.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 28.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QUuid>

#include "gt_command.h"

GtCommand::GtCommand(const QString& uuid) :
    m_id(uuid)
{

}

GtCommand::GtCommand()
{

}

QString
GtCommand::id() const
{
    return m_id;
}

bool
GtCommand::isValid() const
{
    return !m_id.isEmpty();
}
