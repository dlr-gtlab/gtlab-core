/* GTlab - Gas Turbine laboratory
 * Source File: gt_loopui.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 13.10.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QIcon>

#include "gt_application.h"
#include "gt_regexp.h"
#include "gt_loop.h"

#include "gt_loopui.h"

GtLoopUI::GtLoopUI()
{

}

QIcon
GtLoopUI::icon(GtObject* /*obj*/) const
{
    return gtApp->icon(QStringLiteral("loopIcon2_16.png"));
}

QVariant
GtLoopUI::specificData(GtObject* obj, int role, int column) const
{
    if (obj == Q_NULLPTR)
    {
        return QVariant();
    }

    if (column == 0)
    {
        switch (role)
        {
            case Qt::DisplayRole:
            {
                GtLoop* loop = qobject_cast<GtLoop*>(obj);

                if (loop == Q_NULLPTR)
                {
                    return QVariant();
                }

                if (loop->currentState() != GtProcessComponent::RUNNING)
                {
                    return QVariant();
                }

                QString str = loop->objectName() + QStringLiteral(" [") +
                              QString::number(loop->currentIterationStep()) +
                              QStringLiteral("/") +
                              QString::number(loop->maxIterationSteps()) +
                              QStringLiteral("]");
                return str;
            }
        }
    }

    return QVariant();
}

bool
GtLoopUI::hasValidationRegExp()
{
    return true;
}

QRegExp
GtLoopUI::validatorRegExp()
{
    return GtRegExp::onlyLettersAndNumbersAndSpace();
}
