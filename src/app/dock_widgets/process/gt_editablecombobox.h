/* GTlab - Gas Turbine laboratory
 * Source File: gt_editablecombobox.h
 *
 * SPDX-License-Identifier: Apache-2.0
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Created on: 13.11.2024
 * Author: Marvin Noethen (DLR AT-TWK)
 */

#ifndef GTEDITABLECOMBOBOX_H
#define GTEDITABLECOMBOBOX_H

#include <QComboBox>

class GtEditableComboBox : public QComboBox
{
    Q_OBJECT

public:
    explicit GtEditableComboBox(QWidget* parent = nullptr);

    void enableEditing();

    void disableEditing();

protected:
    void keyPressEvent(QKeyEvent* event) override;

    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QString m_textBeforeEditing{};

    void onEditingFinished();

signals:
    void editRequested(int index);

    void editingFinished(int index, const QString& oldText,
                         const QString& newText);
};

#endif // GTEDITABLECOMBOBOX_H
