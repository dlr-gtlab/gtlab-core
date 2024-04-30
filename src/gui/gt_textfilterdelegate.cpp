/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labeldelegate.h
 *
 *  Created on: 05.12.2017
 *  Author: jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include <QLineEdit>

#include "gt_regexp.h"
#include "gt_logging.h"
#include "gt_coredatamodel.h"
#include "gt_application.h"
#include "gt_project.h"
#include "gt_objectui.h"

#include "gt_textfilterdelegate.h"

GtTextFilterDelegate::GtTextFilterDelegate(QObject* parent,
                                           validatorflag filter) :
    QStyledItemDelegate(parent),
    m_validatorflag(filter)
{

}

QWidget*
GtTextFilterDelegate::createEditor(QWidget* parent,
                                   const QStyleOptionViewItem& /*option*/,
                                   const QModelIndex& index) const
{
    QLineEdit* lineEdit = new QLineEdit(parent);

    QString uuid = index.data(GtCoreDatamodel::UuidRole).toString();

    GtProject* proj = gtApp->currentProject();

    /// Standart regExp
    QRegExp regExp = gt::re::onlyLettersAndNumbers();

    if (proj)
    {
        GtObject* obj = proj->getObjectByUuid(uuid);

        if (obj)
        {
            if (m_validatorflag == uiFilter)
            {
                static GtObjectUI fallbackUI;

                GtObjectUI* oui = gtApp->defaultObjectUI(obj);
                if (!oui) oui = &fallbackUI;

                if (oui->hasValidationRegExp())
                {
                    regExp = oui->validatorRegExp();
                }
            }
            else if (m_validatorflag == allowSpaces)
            {
                regExp = gt::re::onlyLettersAndNumbersAndSpace();
            }
        }
    }

    QValidator* validator = new QRegExpValidator(regExp, this->parent());

    lineEdit->setValidator(validator);

    return lineEdit;
}

void
GtTextFilterDelegate::setEditorData(QWidget* editor,
                                    const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return;
    }

    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    QString val = index.data(Qt::DisplayRole).toString();
    lineEdit->setText(val);
}
