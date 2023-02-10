/* GTlab - Gas Turbine laboratory
 * copyright 2023 by DLR
 *
 * Author: Martin Siggel (AT-TWK)
 */

#ifndef GTQMLACTION_H
#define GTQMLACTION_H

#include <QObject>
#include <QUrl>
#include <QString>

#include "gt_gui_exports.h"

/**
 * @brief A QAction like class that can be used to defined buttons
 * for qml
 */
class GT_GUI_EXPORT GtQmlAction : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QUrl iconUrl READ iconUrl WRITE setIconUrl NOTIFY iconUrlChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
public:
    explicit GtQmlAction(QObject* parent = nullptr);
    explicit GtQmlAction(QString text, QObject* parent = nullptr);
    GtQmlAction(QString text, QUrl iconUrl, QObject* parent = nullptr);

    QString text() const;
    void setText(const QString& text);

    QUrl iconUrl() const;
    void setIconUrl(const QUrl&url);

    bool enabled() const;
    void setEnabled(bool);

signals:
    void triggered();
    void iconUrlChanged();
    void textChanged();
    void enabledChanged();

public slots:
    void trigger();

private:
    QString m_text;
    QUrl m_iconUrl{"qrc:/icons/unknownIcon.png"};
    bool m_enabled{true};
};
#endif // GTQMLACTION_H
