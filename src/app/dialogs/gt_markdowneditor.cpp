/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#include <QVBoxLayout>
#include <QQuickWidget>
#include <QQmlContext>

#include "gt_application.h"
#include "gt_logging.h"

#include "gt_markdowneditor.h"

GtMarkdownEditor::GtMarkdownEditor(const QString& content, QWidget* parent) :
    QWidget(parent)
{
    setWindowTitle("Markdown Editor");

    QVBoxLayout* lay = new QVBoxLayout;

    m_qmlwid = new QQuickWidget;
    m_qmlwid->setResizeMode(QQuickWidget::SizeRootObjectToView);
    lay->addWidget(m_qmlwid);
    lay->setContentsMargins(0, 0, 0, 0);

    QQmlContext* context = m_qmlwid->rootContext();
    context->setContextProperty("markdown_text", content);
    m_qmlwid->rootContext()->setContextProperty("gtapp",
                                                  gtApp);

    m_qmlwid->setSource(QUrl("qrc:/qml/markdown_editor.qml"));


    setLayout(lay);
}
