/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */


#ifndef GTQMLTOOLBAR_H
#define GTQMLTOOLBAR_H

#include <QObject>
#include <QWidget>

#include <memory>

#include <gt_qvariantlistmodel.h>
#include <gt_qmlaction.h>
#include <gt_qmltoolbargroup.h>

#include <gt_gui_exports.h>

/**
 * @brief Class for controlling the QML Toolbar.
 */
class GT_GUI_EXPORT GtQmlToolbar : public QWidget
{
    Q_OBJECT

    /**
     * @brief A property holding all toolbar groups to be used
     *        by QtQuick
     */
    Q_PROPERTY(QObject* groups READ groups NOTIFY groupsChanged FINAL)

    /**
     * @brief A property holding the status actions, that will be
     *        rendered at the right side of the toolbar
     *
     *  See also `GtQmlToolbar::statusActions`
     */
    Q_PROPERTY(GtQmlToolbarGroup* statusActions READ statusActions NOTIFY statusActionsChanged FINAL)

    /**
     * @brief A property, that contains the color theme of the toolbar
     */
    Q_PROPERTY(QVariantMap theme READ themeMap NOTIFY themeChanged FINAL)
public:
    explicit GtQmlToolbar(QWidget *parent = nullptr);
    ~GtQmlToolbar() override;

    /**
     * @brief Adds a new toolbar group
     *
     * Note: the toolbar will not become parent or owner. The ownership
     * remains at the caller.
     */
    Q_INVOKABLE void addToolbarGroup(GtQmlToolbarGroup* group);

    /**
     * @brief Removes a action group from the toolbar
     */
    Q_INVOKABLE void removeToolbarGroup(GtQmlToolbarGroup * group);

    /**
     * @brief Adds a status action to the toolbar, which will be rendered at
     *        the right side of the toolbar.
     *
     * The ownership is not transferred.
     */
    Q_INVOKABLE void addStatusAction(GtQmlAction* action);

    /**
     * @brief Returns all toolbar groups
     */
    QVariantListModel* groups();
    GtQmlToolbarGroup* statusActions();

    struct GT_GUI_EXPORT Theme
    {
        Theme();

        /// Color of the text and icons
        QColor foreground;

        /// First background color
        QColor base;

        /// Second background color
        QColor background;

        /// Color of the button, when hovered
        QColor buttonHover;

        /// True, if this theme is a dark mode
        bool darkmode;
    };

    void setColorTheme(const Theme& colors);

signals:
    void groupsChanged();
    void statusActionsChanged();
    void themeChanged();

private:
    QVariantMap themeMap() const;

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GTQMLTOOLBAR_H
