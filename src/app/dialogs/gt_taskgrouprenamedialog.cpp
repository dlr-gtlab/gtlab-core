/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: gt_taskgrouprenamedialog.cpp
 *
 */

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QMessageBox>

#include "gt_taskgrouprenamedialog.h"

#include "gt_colors.h"

GtTaskGroupRenameDialog::GtTaskGroupRenameDialog(
    const QString& currentName, const QStringList& existingNames,
    QWidget* parent) :
    QDialog(parent),
    m_lineEdit(new QLineEdit(currentName)),
    m_warningLabel(new QLabel()),
    m_okButton(nullptr),
    m_existingNames(existingNames),
    m_currentName(currentName)
{
    setWindowTitle(tr("Rename Task Group"));

    auto* layout = new QVBoxLayout(this);

    auto* inputLayout = new QHBoxLayout;
    inputLayout->addWidget(new QLabel(tr("New name:")));
    inputLayout->addWidget(m_lineEdit);
    layout->addLayout(inputLayout);

    m_warningLabel->setVisible(false);
    layout->addWidget(m_warningLabel);

    layout->addStretch();

    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok |
                                               QDialogButtonBox::Cancel, this);
    layout->addWidget(buttonBox);

    m_okButton = buttonBox->button(QDialogButtonBox::Ok);

    connect(buttonBox, &QDialogButtonBox::accepted,
            this, &GtTaskGroupRenameDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected,
            this, &GtTaskGroupRenameDialog::reject);

    connect(m_lineEdit, &QLineEdit::textChanged,
            this, &GtTaskGroupRenameDialog::validateName);
    validateName(currentName);
}

QString
GtTaskGroupRenameDialog::newName() const
{
    return m_lineEdit->text();
}

void
GtTaskGroupRenameDialog::validateName(const QString& text)
{
    if (text.isEmpty())
    {
        m_warningLabel->setText(tr("Name cannot be empty."));
        setLabelColor(gt::gui::color::errorText());
        m_warningLabel->setVisible(true);
        m_okButton->setEnabled(false);
        return;
    }

    if (text == m_currentName)
    {
        m_warningLabel->setText(tr("New name is identical to current name."));
        setLabelColor(gt::gui::color::warningText());
        m_warningLabel->setVisible(true);
        m_okButton->setEnabled(true);
        return;
    }

    if (m_existingNames.contains(text, Qt::CaseInsensitive))
    {
        m_warningLabel->setText(tr("A task group with this name already exists."));
        setLabelColor(gt::gui::color::errorText());
        m_warningLabel->setVisible(true);
        m_okButton->setEnabled(false);
        return;
    }

    m_warningLabel->setVisible(false);
    m_okButton->setEnabled(true);
}

void
GtTaskGroupRenameDialog::setLabelColor(QColor newColor)
{
    QPalette pal = m_warningLabel->palette();
    pal.setColor(QPalette::WindowText, newColor);
    m_warningLabel->setPalette(pal);
}
