/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_colorpropertyeditor.cpp
 *
 *  Created on: 06.12.2024
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFile>
#include <QDir>
#include <QColorDialog>

#include "gt_colorpropertyeditor.h"
#include "gt_colorproperty.h"

#include "gt_application.h"
#include "gt_command.h"
#include "gt_project.h"

#include "gt_propertyfilechoosereditor.h"

GtColorPropertyEditor::GtColorPropertyEditor(QWidget* parent) :
    QWidget(parent)
{
    auto* lay = new QHBoxLayout;
    m_colorLineEdit = new QLineEdit;
    lay->addWidget(m_colorLineEdit);

    m_selectButton = new QPushButton();
    m_selectButton->setFlat(true);
    m_selectButton->setMaximumWidth(16);
    m_selectButton->setAutoDefault(false);
    m_selectButton->setToolTip(tr("Select Color"));

    lay->addWidget(m_selectButton);
    setLayout(lay);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    m_colorLineEdit->setFrame(false);
    m_colorLineEdit->setReadOnly(true);

    connect(m_selectButton, SIGNAL(clicked(bool)),
            SLOT(selectColor()));
}

void
GtColorPropertyEditor::setColorProperty(GtColorProperty* prop)
{
    m_prop = prop;

    if (m_prop)
    {
        connect(m_prop.data(), SIGNAL(changed()), SLOT(propertyValueChanged()));
    }

    update();
}

void
GtColorPropertyEditor::update()
{
    QColor c;

    if (m_prop)
    {
        c = m_prop->getVal();
    }

    m_colorLineEdit->setText(c.name());

    m_selectButton->setToolTip(tr("Select Color"));

    QString styleSheet = QString("background-color: %1;").arg(c.name());
    m_selectButton->setStyleSheet(styleSheet);
}

void
GtColorPropertyEditor::setPropertyValue(const QColor &val)
{
    if (!m_prop) return;

    const QString commandMsg = m_prop->objectName() + QStringLiteral(" ") +
            QObject::tr("changed");

    auto cmd = gtApp->makeCommand(gtApp->currentProject(), commandMsg);
    Q_UNUSED(cmd)

    m_prop->setVal(val);
}

void
GtColorPropertyEditor::selectColor()
{
    if (!m_prop) return;

    QColor c = QColorDialog::getColor(m_prop->getVal());

    if (c.isValid()) setPropertyValue(c);

    update();

    emit colorSelected(c);
}

void
GtColorPropertyEditor::propertyValueChanged()
{
    update();
}
