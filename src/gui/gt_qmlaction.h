/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#ifndef GTQMLACTION_H
#define GTQMLACTION_H

#include <QObject>
#include <QUrl>
#include <QString>

#include "gt_gui_exports.h"

#include <memory>

/**
 * @brief A QAction like class that can be used to defined buttons
 * for qml
 */
class GT_GUI_EXPORT GtQmlAction : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QUrl iconUrl READ iconUrl WRITE setIconUrl NOTIFY iconUrlChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString tooltip READ tooltip NOTIFY tooltipChanged)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)
public:
    explicit GtQmlAction(QObject* parent = nullptr);
    GtQmlAction(QString text, QUrl icon, QObject* parent = nullptr);

    ~GtQmlAction() override;

    QString text() const;
    void setText(const QString& text);

    QUrl iconUrl() const;
    void setIconUrl(const QUrl& url);


    QString tooltip() const;
    void setTooltip(QString t);

    bool enabled() const;
    void setEnabled(bool);

    bool visible() const;
    void setVisible(bool visible);

signals:
    void triggered();
    void iconUrlChanged();
    void textChanged();
    void enabledChanged();
    void visibleChanged();
    void tooltipChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

};
#endif // GTQMLACTION_H
