/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectuiactiongroup.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 28.09.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTOBJECTUIACTIONGROUP_H
#define GTOBJECTUIACTIONGROUP_H

#include "gt_gui_exports.h"

#include "gt_objectuiaction.h"

#include <QString>
#include <QList>

/**
 * @brief The GtObjectUIActionGroup class
 */
class GT_GUI_EXPORT GtObjectUIActionGroup
{
public:
    /**
     * @brief GtObjectUIActionGroup
     */
    GtObjectUIActionGroup();

    /**
     * @brief GtObjectUIActionGroup
     */
    GtObjectUIActionGroup(const QString& groupName,
                          const QList<GtObjectUIAction> actions,
                          const QString& icon = QString());
    /**
     * @brief actions
     * @return list of actions
     */
    const QList<GtObjectUIAction> actions();

    /**
     * @brief name
     * @return group name
     */
    const QString name();

    /**
     * @brief icon
     * @return icon
     */
    const QString icon();

private:
    QList<GtObjectUIAction> m_actions;

    QString m_name;

    QString m_icon;
};

#endif // GTOBJECTUIACTIONGROUP_H
