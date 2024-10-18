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
 *
 * Note: the difference to a QAction is that this class uses
 * URLs to reference Icons where QActions uses Icon directly.
 * QtQuick needs URLs to render icons.
 */
class GT_GUI_EXPORT GtQmlAction : public QObject
{
    Q_OBJECT

    /**
     * @brief This property holds the action's descriptive text
     *
     * If the action is added to a toolbar, it will consist
     * of the icon and the text. There is no default text.
     *
     * See also `GtQmlAction::text` and `GtQmlAction::setText`
     */
    Q_PROPERTY(QString text READ text NOTIFY textChanged)

    /**
     * @brief This property holds the URL to the a resource image file
     *        to render the actions icon.
     *
     * See also `GtQmlAction::iconUrl` and `GtQmlAction::setIconUrl`
     */
    Q_PROPERTY(QUrl iconUrl READ iconUrl WRITE setIconUrl NOTIFY iconUrlChanged)

    /**
     * @brief This property holds the state of the action,
     *        whether it is enabled or not.
     *
     * A disabled action is greyed out and cannot be clicked or triggered.
     *
     * See also `GtQmlAction::setEnabled` and `GtQmlAction::setEnabled`
     */
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)


    /**
     * @brief This property holds the tooltip text, which is displayed when
     *        hovering over the button.
     *
     * If the tooltip is empty, nothing will be shown. The default value is an
     * empty string
     *
     * See also `GtQmlAction::toolTip` and `GtQmlAction::setToolTip`
     */
    Q_PROPERTY(QString toolTip READ toolTip NOTIFY toolTipChanged)

    /**
     * @brief This property holds whether the action can be seen
     *        (e.g. in menus and toolbars)
     *
     * If visible is true the action can be seen; if visible is false the
     * action cannot be seen.
     *
     * See also `GtQmlAction::toolTip` and `GtQmlAction::setToolTip`
     */
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible NOTIFY visibleChanged)
public:
    explicit GtQmlAction(QObject* parent = nullptr);
    GtQmlAction(QString text, QUrl icon, QObject* parent = nullptr);

    static GtQmlAction* makeSeparator(QObject* parent = nullptr);

    ~GtQmlAction() override;

    QString text() const;
    void setText(const QString& text);

    QUrl iconUrl() const;
    void setIconUrl(const QUrl& url);


    QString toolTip() const;
    void setToolTip(QString t);

    bool enabled() const;
    void setEnabled(bool);

    bool isVisible() const;
    void setVisible(bool visible);

    /**
     * @brief Returns true if this action acts only as a separator
     */
    Q_INVOKABLE bool isSeparator() const;

signals:
    /**
     * @brief This signal is emitted, when the user clicks
     *        on the action.
     */
    void triggered();


    void iconUrlChanged();
    void textChanged();
    void enabledChanged();
    void visibleChanged();
    void toolTipChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

};
#endif // GTQMLACTION_H
