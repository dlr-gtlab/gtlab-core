/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labeldelegate.cpp
 *
 *  Created on: 22.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QLineEdit>
#include <QMessageBox>

#include "gt_labelsdock.h"
#include "gt_project.h"
#include "gt_label.h"
#include "gt_application.h"
#include "gt_datamodel.h"
#include "gt_command.h"
#include "gt_regularexpression.h"
#include "gt_icons.h"

#include "gt_labeldelegate.h"

GtLabelDelegate::GtLabelDelegate(GtLabelsDock* parent) :
    QStyledItemDelegate(parent),
    m_dock(parent)
{

}

QWidget*
GtLabelDelegate::createEditor(QWidget* parent,
                              const QStyleOptionViewItem& /*option*/,
                              const QModelIndex& /*index*/) const
{
    QLineEdit* lineEdit = new QLineEdit(parent);

    QValidator* validator = new QRegularExpressionValidator(
                gt::rex::onlyLettersAndNumbers(), this->parent());

    lineEdit->setValidator(validator);

    return lineEdit;
}

void
GtLabelDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (!index.isValid())
    {
        return;
    }

    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);
    QString val = index.data(Qt::DisplayRole).toString();
    lineEdit->setText(val);

    QValidator* validator = new QRegularExpressionValidator(
                gt::rex::onlyLettersAndNumbers(), this->parent());
    lineEdit->setValidator(validator);
}

void
GtLabelDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
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

    GtLabel* label = m_dock->labelFromIndex(index);

    if (!label)
    {
        return;
    }

    GtProject* project = m_dock->project();

    if (!project)
    {
        return;
    }

    QLineEdit* lineEdit = static_cast<QLineEdit*>(editor);

    QValidator* validator = new QRegularExpressionValidator(
                gt::rex::onlyLettersAndNumbers(), this->parent());
    lineEdit->setValidator(validator);

    QString newId = lineEdit->text();
    QString oldId = index.data(Qt::DisplayRole).toString();

    if (newId == oldId)
    {
        return;
    }

    QList<GtLabel*> const labels = project->findLabelUsages(label);

    int nou = labels.size();

    QString msg;

    gtDebug().verbose().nospace() << __FUNCTION__ << ": " << nou;

    if (nou <= 0)
    {
        model->setData(index, newId);
    }
    else
    {
        msg = tr("Rename all") + QStringLiteral(" <b>") + QString::number(nou) +
              QStringLiteral("</b> ") + tr("data entries of ") +
              label->objectName() + QStringLiteral("?");

        QMessageBox mb;
        mb.setIcon(QMessageBox::Question);
        mb.setWindowTitle(tr("Rename Label"));
        mb.setWindowIcon(gt::gui::icon::rename());
        mb.setText(msg);
        mb.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        mb.setDefaultButton(QMessageBox::Yes);
        int ret = mb.exec();

        switch (ret)
        {
            case QMessageBox::Yes:
            {
                QString delMsg = tr("Rename Label") + QStringLiteral(" - ") +
                                 label->objectName();
                auto command = gtApp->makeCommand(project, delMsg);
                Q_UNUSED(command)

                model->setData(index, newId);

                for (GtLabel* l : labels)
                {
                    QModelIndex tmpIndex = gtDataModel->indexFromObject(l);
                    gtDataModel->setData(tmpIndex, newId);
                }

                break;
            }

            case QMessageBox::Cancel:
            {
                break;
            }
        }
    }


}
