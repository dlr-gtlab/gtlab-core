/* GTlab - Gas Turbine laboratory
 * Source File: gt_editablecombobox.cpp
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 13.11.2024
 * Author: Marvin Noethen (DLR AT-TWK)
 */

#include <QKeyEvent>
#include <QLineEdit>

#include "gt_stylesheets.h"

#include "gt_editablecombobox.h"


GtEditableComboBox::GtEditableComboBox(QWidget* parent) : QComboBox(parent)
{
    setStyleSheet(gt::gui::stylesheet::comboBox());
    installEventFilter(this);
}

void
GtEditableComboBox::enableEditing()
{
    if (isEditable()) return;

    m_textBeforeEditing = currentText();

    setEditable(true);

    lineEdit()->selectAll();
    lineEdit()->setFocus();
}

void
GtEditableComboBox::disableEditing()
{
    setEditable(false);
}

void
GtEditableComboBox::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
        case Qt::Key_F2:

            if (currentIndex() >= 0)
            {
                emit editRequested(currentIndex());
                return;
            }

            break;

        case Qt::Key_Enter:
        case Qt::Key_Return:

            if (isEditable())
            {
                onEditingFinished();
                return;
            }

            break;

        default:
            break;
    }

    QComboBox::keyPressEvent(event);
}

bool
GtEditableComboBox::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() == QEvent::FocusOut && isEditable())
    {
        onEditingFinished();
    }

    return QComboBox::eventFilter(watched, event);
}

void
GtEditableComboBox::onEditingFinished()
{
    if (!isEditable()) return;

    auto newText = lineEdit()->text();

    disableEditing();
    emit editingFinished(currentIndex(), m_textBeforeEditing, newText);

    m_textBeforeEditing.clear();
}
