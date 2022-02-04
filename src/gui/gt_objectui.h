/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 06.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTUI_H
#define GTOBJECTUI_H

#include "gt_gui_exports.h"

#include <QObject>
#include <QKeySequence>

#include "gt_objectuiaction.h"
#include "gt_objectuiactiongroup.h"

class QIcon;
class GtObject;

/**
 * @brief The GtObjectUI class
 */
class GT_GUI_EXPORT GtObjectUI : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief GtObjectUI
     */
    Q_INVOKABLE GtObjectUI();

    /**
     * Virtual function to specify object specific icon.
     * @return Object specific icon
     */
    virtual QIcon icon(GtObject* obj) const;

    /** Returns data of specific role.
     *  @param role Data role
     *  @return Requested data as variant
     */
    QVariant data(GtObject* obj, int role, int column = 0) const;

    /** Sets specific data.
     * @param role Data role
     * @param value New data
     * @return Whether data was set correctly or not
     */
    bool setData(GtObject* obj, int role, const QVariant& value);

    /**
     * @brief openWith
     * @param obj
     * @return
     */
    virtual QStringList openWith(GtObject* obj);

    /**
     * @brief actions
     * @return
     */
    const QList<GtObjectUIAction>& actions();

    /**
     * @brief hasActions
     * @return
     */
    bool hasActions();

    /**
     * @brief groupActions
     * @return
     */
    const QHash<QString, QList<GtObjectUIAction> > groupActions();

    /**
     * @brief hasGroupActions
     * @return
     */
    bool hasActionGroups();

    /**
     * @brief actionGroup
     * @return
     */
    const QList<GtObjectUIActionGroup> actionGroups();

    /**
     * @brief doubleClicked
     * @param obj
     */
    virtual void doubleClicked(GtObject* obj);

    /**
     * @brief hasValidationRegExp - validator for filtering names for
     * renaming in models
     * @return true if Object has validator RegExp
     */
    virtual bool hasValidationRegExp();

    /**
     * @brief validatorRegExp - gives the validation RegExp
     * @return
     */
    virtual QRegExp validatorRegExp();
protected:
    /**
     * @brief specificData
     * @param obj
     * @param role
     * @param column
     * @return
     */
    virtual QVariant specificData(GtObject* obj, int role, int column) const;

    /**
     * @brief addAction
     * @param actionText
     * @param actionIcon
     * @param actionMethod
     */
    void addSingleAction(const QString& actionText,
                         const QString& actionIcon,
                         const QString& actionMethod,
                         const QString& actionVerification,
                         const QString& actionVisibility);

    /**
     * @brief addSingleAction
     * @param actionText
     * @param actionIcon
     * @param actionMethod
     * @param actionVerification
     * @param actionVisibility
     * @param shortcut
     */
    void addSingleAction(const QString& actionText,
                         const QString& actionIcon,
                         const QString& actionMethod,
                         const QString& actionVerification,
                         const QString& actionVisibility,
                         const QKeySequence& shortcut);

    /**
     * @brief addAction
     * @param actionText
     * @param actionIcon
     * @param actionMethod
     */
    void addSingleAction(const QString& actionText, const QString& actionIcon,
                         const QString& actionMethod,
                         const QString& actionVerification);

    /**
     * @brief addAction
     * @param actionText
     * @param actionIcon
     * @param actionMethod
     */
    void addSingleAction(const QString& actionText, const QString& actionIcon,
                         const QString& actionMethod,
                         const QString& actionVerification,
                         const QKeySequence& shortcut);

    /**
     * @brief addAction
     * @param actionText
     * @param actionIcon
     * @param actionMethod
     */
    void addSingleAction(const QString& actionText, const QString& actionIcon,
                         const QString& actionMethod);

    /**
     * @brief addAction
     * @param actionText
     * @param actionMethod
     */
    void addSingleAction(const QString& actionText,
                         const QString& actionMethod);

    /**
     * @brief addSingleAction
     * @param actionText
     * @param actionMethod
     * @param shortcut
     */
    void addSingleAction(const QString& actionText,
                         const QString& actionMethod,
                         const QKeySequence &shortcut);

    /**
     * @brief addSingleAction
     * @param actionText
     * @param actionIcon
     * @param actionMethod
     * @param shortcut
     */
    void addSingleAction(const QString &actionText,
                         const QString &actionIcon,
                         const QString &actionMethod,
                         const QKeySequence &shortcut);

    /**
     * @brief addGroupActions
     * @param groupName
     * @param actions
     */
    void addActionGroup(const QString& groupName,
                        const QList<GtObjectUIAction>& actions);

    /**
     * @brief addActionGroup
     * @param groupName
     * @param actions
     */
    void addActionGroup(const QString& groupName, const QString& groupIcon,
                        const QList<GtObjectUIAction>& actions);

    /**
     * @brief addActionGrou
     * @param actionGroup
     */
    void addActionGroup(GtObjectUIActionGroup actionGroup);

    /**
     * @brief addSeparator
     */
    void addSeparator();


private:
    /// List of custom actions
    QList<GtObjectUIAction> m_singleActions;

    /// List of custom menus
    QList<GtObjectUIActionGroup> m_actionGroups;

};

#endif // GTOBJECTUI_H
