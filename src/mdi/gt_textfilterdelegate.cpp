/* GTlab - Gas Turbine laboratory
 * Source File: gt_labeldelegate.h
 * copyright 2009-2017 by DLR
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
    QRegExp regExp = GtRegExp::onlyLettersAndNumbers();

    if (proj != Q_NULLPTR)
    {
        GtObject* obj = proj->getObjectByUuid(uuid);

        if (obj != Q_NULLPTR)
        {
            if (m_validatorflag == uiFilter)
            {
                GtObjectUI* oui = gtApp->defaultObjectUI(obj);

                if (oui != Q_NULLPTR)
                {
                    if (oui->hasValidationRegExp())
                    {
                        regExp = oui->validatorRegExp();
                    }
                }
            }
            else if (m_validatorflag == allowSpaces)
            {
                regExp = GtRegExp::onlyLettersAndNumbersAndSpace();
            }
        }
    }

    QValidator* validator = new QRegExpValidator(regExp, this->parent());;

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
