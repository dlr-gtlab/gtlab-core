/* GTlab - Gas Turbine laboratory
 * Source File: gt_checkforupdatesdialog.cpp
 * copyright 2009-2017 by DLR
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

#include "gt_icons.h"
#include "gt_logging.h"

#include "gt_confirmdeleteprojectdialog.h"

GtConfirmDeleteProjectDialog::GtConfirmDeleteProjectDialog(
        QWidget* parent, const QString& projectName) :
    QDialog(parent),
    m_checkBox(nullptr)
{
    setWindowTitle(tr("Delete from Session"));
    setWindowIcon(GtGUI::Icon::deleteProject16());

    Qt::WindowFlags flags = windowFlags();
    flags = flags & (~Qt::WindowContextHelpButtonHint);
    setWindowFlags(flags);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    setLayout(layout);

    QHBoxLayout* textLayout = new QHBoxLayout;
    layout->addLayout(textLayout);

    QLabel* infoIcon = new QLabel;

    QPixmap pix(":/icons/questionIcon_24.png");
    infoIcon->setPixmap(pix);

    textLayout->addSpacerItem(new QSpacerItem(15, 10, QSizePolicy::Minimum,
                                              QSizePolicy::Minimum));

    textLayout->addWidget(infoIcon);


    textLayout->addSpacerItem(new QSpacerItem(5, 10, QSizePolicy::Expanding,
                                              QSizePolicy::Minimum));

    QString text = QString(tr("Are your sure you want to remove project '"))
            + projectName + QString(tr("' from session"));

    QLabel* infoText = new QLabel;
    infoText->setText(text);

    textLayout->addWidget(infoText);

    textLayout->addSpacerItem(new QSpacerItem(15, 10, QSizePolicy::Minimum,
                                              QSizePolicy::Minimum));



    QHBoxLayout* checkLay = new QHBoxLayout;

    m_checkBox = new QCheckBox;
    m_checkBox->setChecked(false);



    QLabel* label = new QLabel(tr("Delete project contents on"
                                  " disk (cannot be undone)"));

    layout->addLayout(checkLay);


    checkLay->addSpacerItem(new QSpacerItem(15, 10,
                                            QSizePolicy::Minimum,
                                            QSizePolicy::Minimum));

    checkLay->addWidget(m_checkBox);

    checkLay->addSpacerItem(new QSpacerItem(10, 10,
                                            QSizePolicy::Minimum,
                                            QSizePolicy::Minimum));

    checkLay->addWidget(label);

    checkLay->addSpacerItem(new QSpacerItem(5, 10,
                                            QSizePolicy::Expanding,
                                            QSizePolicy::Minimum));



    QFrame* line = new QFrame;
    line->setObjectName(QStringLiteral("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);

    QHBoxLayout* buttonsLayout = new QHBoxLayout;

    QPushButton* yesButton = new QPushButton(tr("Delete"));
    yesButton->setIcon(GtGUI::Icon::deleteProject16());

    buttonsLayout->addSpacerItem(new QSpacerItem(10, 10,
                                                 QSizePolicy::Expanding,
                                                 QSizePolicy::Minimum));


    buttonsLayout->addWidget(yesButton);

    QPushButton* noButton = new QPushButton(tr("Cancel"));
    noButton->setIcon(GtGUI::Icon::delete16());
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


