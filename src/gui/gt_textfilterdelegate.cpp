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
#include <QToolTip>

#include "gt_palette.h"
#include "gt_regexp.h"
#include "gt_coredatamodel.h"
#include "gt_application.h"
#include "gt_project.h"
#include "gt_objectui.h"
#include "gt_colors.h"

#include "gt_regexpvalidator.h"
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
    auto* lineEdit = new QLineEdit(parent);

    /// Standart regExp with hint and strict chcking rule set to active
    QRegExp regExp = gt::re::onlyLettersAndNumbers();
    QString hint = gt::re::onlyLettersAndNumbersHint();

    updateRegExpSetupByObject(index, regExp, hint);

    lineEdit->setValidator(new GtRegExpValidator(regExp, /*strict*/ false,
                                                 this->parent()));

    lineEdit->setToolTip(hint);

    connect(lineEdit, &QLineEdit::textChanged,
            lineEdit, [lineEdit, regExp, hint](
            const QString& text)
    {
        QPalette pal = gt::gui::currentTheme();

        if (!regExp.exactMatch(text))
        {
            pal.setColor(QPalette::Text, gt::gui::color::warningText());
            QToolTip::showText(lineEdit->mapToGlobal(QPoint(0, 0)), hint,
                               lineEdit);
        }
        else
        {
            QToolTip::hideText();
        }

        lineEdit->setPalette(pal);
    });

    return lineEdit;
}

void
GtTextFilterDelegate::setEditorData(QWidget* editor,
                                    const QModelIndex &index) const
{
    if (!index.isValid()) return;

    auto* lineEdit = static_cast<QLineEdit*>(editor);
    QString val = index.data(Qt::DisplayRole).toString();
    lineEdit->setText(val);
}

void
GtTextFilterDelegate::updateRegExpSetupByObject(const QModelIndex& index,
                                                QRegExp& regExp,
                                                QString& hint) const
{
    if (GtProject* proj = gtApp->currentProject())
    {
        QString uuid = index.data(GtCoreDatamodel::UuidRole).toString();

        if (GtObject* obj = proj->getObjectByUuid(uuid))
        {
            if (m_validatorflag == uiFilter)
            {
                static GtObjectUI fallbackUI;

                GtObjectUI* oui = gtApp->defaultObjectUI(obj);
                if (!oui) oui = &fallbackUI;

                if (oui->hasValidationRegExp(obj))
                {
                    regExp = oui->validatorRegExp(obj);
                    hint = oui->regExpHint(obj);
                }
            }
            else if (m_validatorflag == allowSpaces)
            {
                regExp = gt::re::onlyLettersAndNumbersAndSpace();
            }
        }
    }
}
