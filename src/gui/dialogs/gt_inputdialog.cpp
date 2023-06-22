/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_inputdialog.cpp
 *
 *  Created on: 10.01.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QIcon>

#include "gt_icons.h"

#include "gt_inputdialog.h"

GtInputDialog::GtInputDialog(InputMode mode, QWidget* parent) :
    GtDialog(parent),
    m_currentMode(mode),
    m_textValidator(nullptr),
    m_doubleMax(1000.),
    m_doubleMin(0.),
    m_intMax(1000),
    m_intMin(0),
    m_emptyStringAllowed(false)
{
    QVBoxLayout* lay = new QVBoxLayout;

    m_label = new QLabel(tr("Label Text"));

    lay->addWidget(m_label);

    m_lineEdit = new QLineEdit;

    lay->addWidget(m_lineEdit);

    m_intEdit = new QSpinBox;

    lay->addWidget(m_intEdit);

    m_doubleEdit = new QDoubleSpinBox;

    lay->addWidget(m_doubleEdit);

    // buttons
    QHBoxLayout* btnLay = new QHBoxLayout;

    btnLay->addStretch(1);

    m_acceptBtn = new QPushButton(tr("Ok"));
    m_acceptBtn->setIcon(gt::gui::icon::check());
    m_acceptBtn->setDefault(true);
    btnLay->addWidget(m_acceptBtn);

    m_rejectBtn = new QPushButton(tr("Cancel"));
    m_rejectBtn->setIcon(gt::gui::icon::close());

    btnLay->addWidget(m_rejectBtn);

    lay->addStretch(1);

    lay->addLayout(btnLay);

    setLayout(lay);

    setMode(mode);
    updateInputFields();

    connect(m_lineEdit, SIGNAL(textEdited(QString)), SLOT(validateTextInput()));
    connect(m_acceptBtn, SIGNAL(clicked(bool)), SLOT(accept()));
    connect(m_rejectBtn, SIGNAL(clicked(bool)), SLOT(reject()));
}

void
GtInputDialog::setLabelText(const QString& str)
{
    m_label->setText(str);
}

QString
GtInputDialog::textValue() const
{
    return m_lineEdit->text();
}

void
GtInputDialog::setInitialTextValue(const QString& str)
{
    m_lineEdit->setText(str);
}

double
GtInputDialog::doubleMaximum() const
{
    return m_doubleMax;
}

void
GtInputDialog::setDoubleMaximum(double max)
{
    m_doubleMax = max;
    updateInputFields();
}

double
GtInputDialog::doubleMinimum() const
{
    return m_doubleMin;
}

void
GtInputDialog::setDoubleMinimum(double min)
{
    m_doubleMin = min;
    updateInputFields();
}

double
GtInputDialog::intMaximum() const
{
    return m_intMax;
}

void
GtInputDialog::setIntMaximum(double max)
{
    m_intMax = max;
    updateInputFields();
}

double
GtInputDialog::intMinimum() const
{
    return m_intMin;
}

void
GtInputDialog::setIntMinimum(double min)
{
    m_intMin = min;
    updateInputFields();
}

bool
GtInputDialog::emptyStringAllowed() const
{
    return m_emptyStringAllowed;
}

void
GtInputDialog::setEmptyStringAllowed(bool emptyStringAllowed)
{
    m_emptyStringAllowed = emptyStringAllowed;
}

void
GtInputDialog::setTextValidator(QValidator* validator)
{
    m_textValidator = validator;
    updateInputFields();
}

void
GtInputDialog::showEvent(QShowEvent* event)
{
    switch (m_currentMode)
    {
        case TextInput:
            m_lineEdit->selectAll();
            break;

        default:
            break;
    }

    validateTextInput();

    GtDialog::showEvent(event);
}

QStringList
GtInputDialog::textBlackList() const
{
    return m_textBlackList;
}

void
GtInputDialog::setTextBlackList(const QStringList& textBlackList)
{
    m_textBlackList = textBlackList;
}

void
GtInputDialog::updateInputFields()
{
    // IntInput mode
    m_intEdit->setMinimum(m_intMin);
    m_intEdit->setMaximum(m_intMax);

    // DoubleInput mode
    m_doubleEdit->setMinimum(m_doubleMin);
    m_doubleEdit->setMaximum(m_doubleMax);

    // TextInput mode
    if (m_textValidator)
    {
        m_lineEdit->setValidator(m_textValidator);
    }
}

void
GtInputDialog::setMode(GtInputDialog::InputMode mode)
{
    m_lineEdit->setVisible(false);
    m_intEdit->setVisible(false);
    m_doubleEdit->setVisible(false);

    switch (mode)
    {
        case TextInput:
            m_lineEdit->setVisible(true);
            break;

        case IntInput:
            m_intEdit->setVisible(true);
            break;

        case DoubleInput:
            m_doubleEdit->setVisible(true);
            break;

        default:
            break;
    }
}

void
GtInputDialog::validateTextInput()
{
    if (m_currentMode != TextInput)
    {
        return;
    }

    bool enableAccept = true;

    if (!m_emptyStringAllowed)
    {
       enableAccept =  !m_lineEdit->text().isEmpty();
    }

    if (enableAccept && m_textBlackList.contains(m_lineEdit->text()))
    {
        enableAccept = false;
    }

    m_acceptBtn->setEnabled(enableAccept);
}
