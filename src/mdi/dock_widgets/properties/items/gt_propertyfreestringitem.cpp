/* GTlab - Gas Turbine laboratory
 * Source File: gt_propertyfreestringitem.cpp
 * copyright 2009-2018 by DLR
 *
 *  Created on: 20.11.2018
 *  Author: Björn Schneider (AT-TWK)
 *  Tel.: +49 2203 601 2642
 */

#include "gt_propertyfreestringitem.h"

#include <QLineEdit>
#include <QRegExpValidator>
#include <QSpinBox>
#include <QDebug>

#include "gt_propertyvaluedelegate.h"
#include "gt_abstractproperty.h"
#include "gt_regexp.h"

GtPropertyFreeStringItem::GtPropertyFreeStringItem()
{

}

QWidget*
GtPropertyFreeStringItem::editorWidget(QWidget* parent,
                                               const GtPropertyValueDelegate*
                                               delegate) const
{
    QVariant var = m_property->valueToVariant();

    switch (var.type())
    {
        case QVariant::Double:
        {
            QLineEdit* lineEdit = new QLineEdit(parent);
            QDoubleValidator* validator = new QDoubleValidator(lineEdit);
            lineEdit->setValidator(validator);
            return lineEdit;
        }

        case QVariant::Int:
        {
            QSpinBox* spinbox = new QSpinBox(parent);
            spinbox->setRange(std::numeric_limits<int>::min(),
                              std::numeric_limits<int>::max());
            connect(spinbox, SIGNAL(valueChanged(int)),
                    delegate, SLOT(onDataChange()));
            return spinbox;
        }

        case QVariant::String:
        {
            QLineEdit* lineEdit = new QLineEdit(parent);

            qDebug() << " \t  GtPropertyFreeStringItem::editorWidget()::"
                        "case QVariant::String:";
            QValidator* validator = new QRegExpValidator(
                GtRegExp::woUmlauts(), lineEdit);
            lineEdit->setValidator(validator);

            return lineEdit;
        }

        default:
            return GtAbstractPropertyItem::editorWidget(parent, delegate);
            break;
    }

    return Q_NULLPTR;
}
