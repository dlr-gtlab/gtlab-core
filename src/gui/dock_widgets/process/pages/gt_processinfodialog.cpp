/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processinfodialog.cpp
 */

#include "gt_processinfodialog.h"

#include <QMap>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QGridLayout>
#include "gt_stylesheets.h"

GtProcessInfoDialog::GtProcessInfoDialog(
    QList<QPair<QString, QString>> const& processInfos, QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle(tr("Process Information"));

    auto* layout = new QGridLayout(this);

    int i = 0;
    auto addInfo = [&](QString const& labelText, QString const& value){
        layout->addWidget(new QLabel(labelText + ":", this), i, 0);
        layout->addWidget(new QLabel(value, this), i, 1);
        i++;
    };

    for (auto const& i : processInfos)
    {
        addInfo(i.first, i.second);
    }

    auto* m_okButton = new QPushButton(tr("OK"), this);
    m_okButton->setStyleSheet(gt::gui::stylesheet::button());
    connect(m_okButton, &QPushButton::clicked, this,
            &GtProcessInfoDialog::accept);
    layout->addWidget(m_okButton, i, 1, Qt::AlignRight);

    setMinimumWidth(400);
}
