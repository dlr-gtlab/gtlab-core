/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2024 German Aerospace Center (DLR)
 *
 * Author: Martin Siggel (AT-TWK)
 */

#ifndef GTQMLTOOLBARGROUP_H
#define GTQMLTOOLBARGROUP_H

#include <gt_qmlaction.h>

#include <gt_listmodel.h>

using ActionModel = GtListModel<GtQmlAction*>;

template <>
inline QVariant
gt::asVariant(GtQmlAction* const& action)
{
    return QVariant::fromValue(action);
}

/**
 * @brief A logical group of actions that can be added
 *        and removed from the toolbar
 */
class GtQmlToolbarGroup : public ActionModel
{

    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged FINAL)
    Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged FINAL)
public:
    GtQmlToolbarGroup();

    explicit GtQmlToolbarGroup(QString name, QObject* parent = nullptr);
    GtQmlToolbarGroup(QString name, const std::vector<GtQmlAction*>& actions,
                    QObject* parent = nullptr);

    QString name() const;
    void setName(QString name);

    bool visible() const;
    void setVisible(bool visible);

    // adds an action to the group
    bool append(GtQmlAction* action);

signals:
    void nameChanged();
    void visibleChanged();

private:
    QString m_groupName;
    bool m_visible{true};
};

#endif // GTQMLTOOLBARGROUP_H
