/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_checkforupdatesdialog.cpp
 *
 *  Created on: 10.11.2017
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QStyle>

#include "gt_icons.h"

#include "gt_confirmdeleteprojectdialog.h"

GtConfirmDeleteProjectDialog::GtConfirmDeleteProjectDialog(
        QWidget* parent, const QString& projectName) :
    GtDialog(parent),
    m_checkBox(nullptr)
{
    setWindowTitle(tr("Delete from Session"));
    setWindowIcon(gt::gui::icon::projectRemove());

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);

    QHBoxLayout* textLayout = new QHBoxLayout;
    layout->addLayout(textLayout);

    QLabel* infoIcon = new QLabel;
    infoIcon->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // cheesy way to access message box icons
    {
        QMessageBox dummy;
        // set the appropriate icon
        dummy.setIcon(QMessageBox::Question);
        infoIcon->setPixmap(dummy.iconPixmap());
    }

    textLayout->addSpacerItem(new QSpacerItem(15, 10, QSizePolicy::Minimum,
                                              QSizePolicy::Minimum));

    textLayout->addWidget(infoIcon);

    QString text = tr("Are your sure you want to remove project '")
                   + projectName + tr("' from the current session?");

    QLabel* infoText = new QLabel;
    infoText->setWordWrap(true);
    infoText->setText(text);

    textLayout->addSpacing(10);
    textLayout->addWidget(infoText);

    auto* checkLay = new QHBoxLayout;

    QString checkBoxText = tr("Delete project contents on"
                              " disk (cannot be undone)");
    m_checkBox = new QCheckBox(checkBoxText);
    m_checkBox->setChecked(false);

    layout->addLayout(checkLay);


    checkLay->addSpacerItem(new QSpacerItem(15, 10,
                                            QSizePolicy::Minimum,
                                            QSizePolicy::Minimum));

    checkLay->addWidget(m_checkBox);


    checkLay->addSpacerItem(new QSpacerItem(5, 10,
                                            QSizePolicy::Expanding,
                                            QSizePolicy::Minimum));

    layout->addSpacerItem(new QSpacerItem(10, 30,
                                          QSizePolicy::Fixed,
                                          QSizePolicy::Fixed));


    QFrame* line = new QFrame;
    line->setObjectName(QStringLiteral("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;

    QPushButton* yesButton = new QPushButton(tr("Delete"));
    yesButton->setIcon(gt::gui::icon::projectDelete());

    buttonsLayout->addSpacerItem(new QSpacerItem(10, 10,
                                                 QSizePolicy::Expanding,
                                                 QSizePolicy::Minimum));


    buttonsLayout->addWidget(yesButton);

    QPushButton* noButton = new QPushButton(tr("Cancel"));
    noButton->setIcon(gt::gui::icon::cancel());
    buttonsLayout->addWidget(noButton);

    buttonsLayout->setContentsMargins(9, 9, 9, 9);

    connect(yesButton, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(noButton, SIGNAL(clicked(bool)), SLOT(close()));

    layout->addLayout(buttonsLayout);

    setMaximumWidth(450);
    setMinimumWidth(450);
    setMinimumHeight(180);
    setMaximumHeight(180);
}

bool
GtConfirmDeleteProjectDialog::deleteFromHardDisk()
{
    return m_checkBox->isChecked();
}


