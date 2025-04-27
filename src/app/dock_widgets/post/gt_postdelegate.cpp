/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 18.05.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */

#include <QLineEdit>

#include "gt_postdock.h"
#include "gt_regularexpression.h"
#include <gt_logging.h>

#include "gt_postdelegate.h"

GtPostDelegate::GtPostDelegate(GtPostDock* parent) :
    QStyledItemDelegate(parent),
    m_dock(parent)
{

}

QWidget*
GtPostDelegate::createEditor(QWidget* parent,
                             const QStyleOptionViewItem& /*option*/,
                             const QModelIndex& /*index*/) const
{
    QLineEdit* lineEdit = new QLineEdit(parent);


    QValidator* validator = new QRegularExpressionValidator(
        gt::rex::onlyLettersAndNumbers(), parent);

    lineEdit->setValidator(validator);

    return lineEdit;
}

void
GtPostDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return;
    }

    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);

    QValidator* validator = new QRegularExpressionValidator(
                gt::rex::onlyLettersAndNumbers(), parent());

    lineEdit->setValidator(validator);

    QString val = index.data(Qt::DisplayRole).toString();
    lineEdit->setText(val);
}

void
GtPostDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                             const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return;
    }

    if (!m_dock)
    {
        return;
    }

    QStringList entries = m_dock->entries();

    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);

    QValidator* validator = new QRegularExpressionValidator(
                gt::rex::onlyLettersAndNumbers(), parent());

    lineEdit->setValidator(validator);


    QString newId = lineEdit->text();
    QString oldId = index.data(Qt::DisplayRole).toString();

    if (newId == oldId)
    {
        return;
    }

    if (entries.contains(newId))
    {
        gtDebug() << tr("New entry already exists") << QStringLiteral(": ")
                  << newId;
        return;
    }

    if (m_dock->renameFile(oldId, newId))
    {
        model->setData(index, newId);
    }
}
