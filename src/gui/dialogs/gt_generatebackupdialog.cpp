/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_generatebackupdialog.h
 *
 *  Created on: 17.11.2022
 *  Author: Jens Schmeinkr (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_generatebackupdialog.h"
#include "gt_icons.h"
#include "gt_textedit.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QTabWidget>

GtGenerateBackUpDialog::GtGenerateBackUpDialog(QWidget* parent) :
    GtDialog(parent)
{
    setWindowTitle(tr("Generate Backup"));
    setWindowIcon(gt::gui::icon::data());

    auto* layout = new QVBoxLayout;
    setLayout(layout);

    auto* title = new QLabel(tr("Please add a note for the backup"));
    layout->addWidget(title);


    auto* tab = new QTabWidget;
    layout->addWidget(tab);

    m_textEdit = new QTextEdit;
    m_textEdit->setPlaceholderText("Insert message to explain backup reason");
    tab->addTab(m_textEdit, tr("Edit"));

    auto* preview = new GtTextEdit("", GtTextEdit::MD, this);
    preview->setReadOnly(true);
    tab->addTab(preview, tr("Preview"));


    auto* btnLayOut = new QHBoxLayout;
    layout->addLayout(btnLayOut);

    btnLayOut->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Expanding,
                                             QSizePolicy::Expanding));

    auto* btnOk = new QPushButton("Ok");
    btnLayOut->addWidget(btnOk);
    auto* cancelBtn = new QPushButton("Cancel");
    btnLayOut->addWidget(cancelBtn);

    connect(m_textEdit, SIGNAL(textChanged()), SLOT(onTextChanged()));

    connect(this, SIGNAL(textChanged(QString)),
            preview , SLOT(updateText(QString)));

    connect(btnOk, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(cancelBtn, SIGNAL(clicked(bool)), SLOT(close()));
}

QString
GtGenerateBackUpDialog::message() const
{
    return m_message;
}

void
GtGenerateBackUpDialog::onTextChanged()
{
    m_message = m_textEdit->toPlainText();
    emit textChanged(m_textEdit->toPlainText());
}
