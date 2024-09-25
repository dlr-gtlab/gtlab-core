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

    Q_PROPERTY(QObject* groups READ groups NOTIFY groupsChanged FINAL)
    Q_PROPERTY(GtQmlToolbarGroup* statusActions READ statusActions NOTIFY statusActionsChanged FINAL)
    Q_PROPERTY(bool darkmode READ darkmode NOTIFY darkmodeChanged FINAL)

public:
    explicit GtQmlToolbar(QWidget *parent = nullptr);
    ~GtQmlToolbar() override;

    Q_INVOKABLE QVariantListModel* toolbarGroups();

    Q_INVOKABLE void addToolbarGroup(GtQmlToolbarGroup* group);
    Q_INVOKABLE void removeToolbarGroup(const QString &groupId);

    Q_INVOKABLE void addStatusAction(GtQmlAction* action);

    Q_INVOKABLE bool darkmode() const;
    Q_INVOKABLE void setDarkmode(bool d);

    QVariantListModel* groups();
    GtQmlToolbarGroup* statusActions();

signals:
    void groupsChanged();
    void statusActionsChanged();
    void darkmodeChanged();

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GTQMLTOOLBAR_H
