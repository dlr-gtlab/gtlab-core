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
#include <QColorDialog>

#include "gt_colorpropertyeditor.h"
#include "gt_colorproperty.h"

#include "gt_application.h"
#include "gt_command.h"
#include "gt_project.h"


GtColorPropertyEditor::GtColorPropertyEditor(QWidget* parent) :
    QWidget(parent)
{
    auto* lay = new QHBoxLayout;
    m_colorLineEdit = new QLineEdit;
    lay->addWidget(m_colorLineEdit);

    m_selectButton = new QPushButton();
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

    if (m_prop) c = m_prop->getVal();

    m_colorLineEdit->setText(c.name(QColor::HexArgb));

    m_selectButton->setAutoFillBackground(true);

    static const QString styleBase = QStringLiteral(R"(
      QAbstractButton{
        margin: 0px;
        max-width: 40px;
        border: 1px solid #777777; border-radius: 2px;
        background: qlineargradient( x1:0 y1:0, x2:0 y2:1, stop:0 %3, stop:1 '%1');
      }
      QAbstractButton:hover{ background-color: %2}
      QAbstractButton:pressed{ background-color: %2;})");

    QColor highlightColor = c.lighter();
    QColor gradientUpColor = c.lighter(110);
    QColor gradientLoColor = c.darker(110);

    QString style = QString(styleBase).arg(QVariant(gradientLoColor).toString(),
                                           QVariant(highlightColor).toString(),
                                           QVariant(gradientUpColor).toString());

    m_selectButton->setStyleSheet(style);
}

void
GtColorPropertyEditor::setPropertyValue(const QColor& val)
{
    if (!m_prop) return;

    const QString commandMsg = m_prop->objectName() + QStringLiteral(" ") +
            QObject::tr("changed");

    auto cmd = gtApp->makeCommand(gtApp->currentProject(), commandMsg);
    Q_UNUSED(cmd)

    m_prop->setVal(val.name(QColor::HexArgb));
}

void
GtColorPropertyEditor::selectColor()
{
    if (!m_prop) return;

    QColorDialog::ColorDialogOptions options = {QColorDialog::ShowAlphaChannel};
    QColor c = QColorDialog::getColor(m_prop->getVal(), nullptr,
                                      tr("Color selection"), options);

    if (c.isValid()) setPropertyValue(c.name(QColor::HexArgb));

    update();

    emit colorSelected(c);
}

void
GtColorPropertyEditor::propertyValueChanged()
{
    update();
}
