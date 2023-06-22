/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GTMARKDOWNEDITOR_H
#define GTMARKDOWNEDITOR_H

#include <QWidget>

class QQuickWidget;

class GtMarkdownEditor : public QWidget
{
    Q_OBJECT

public:
    explicit GtMarkdownEditor(const QString& content, QWidget* parent = nullptr);

private:
    QQuickWidget* m_qmlwid;

};

#endif // GTMARKDOWNEDITOR_H
