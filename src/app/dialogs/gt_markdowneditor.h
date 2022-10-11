#ifndef GTMARKDOWNEDITOR_H
#define GTMARKDOWNEDITOR_H

#include <QWidget>

class QQuickWidget;

class GtMarkdownEditor : public QWidget
{
    Q_OBJECT

public:
    GtMarkdownEditor(const QString& content, QWidget* parent = nullptr);

private:
    QQuickWidget* m_qmlwid;

};

#endif // GTMARKDOWNEDITOR_H
