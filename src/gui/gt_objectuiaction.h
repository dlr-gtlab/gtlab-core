/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 24.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTUIACTION_H
#define GTOBJECTUIACTION_H

#include "gt_gui_exports.h"
#include "gt_object.h"
#include "gt_globals.h"

#include <functional>
#include <QString>
#include <QIcon>
#include <QKeySequence>

/**
 * @brief The GtObjectUIAction class
 */
class GtObject;
class GT_GUI_EXPORT GtObjectUIAction
{
public:

    /// Method signatures for action, visibility and verification.
    /// Must be called with a parent and target objects
    using InvokableActionMethod       = std::function<void (QObject*, GtObject*)>;
    using InvokableVerificationMethod = std::function<bool (QObject*, GtObject*)>;
    using InvokableVisibilityMethod   = std::function<bool (QObject*, GtObject*)>;

    using ActionMethod       = std::function<void (GtObject*)>;
    using VerificationMethod = std::function<bool (GtObject*)>;
    using VisibilityMethod   = std::function<bool (GtObject*)>;

    /**
     * @brief GtObjectUIAction
     */
    GtObjectUIAction();

    /**
     * @brief GtObjectUIAction
     */
    [[deprecated("Set action properties using dedicated setters instead")]]
    GtObjectUIAction(const QString& text,
                     const QString& method,
                     const QString& icon = {},
                     const QString& verification = {},
                     const QString& visibility = {},
                     const QKeySequence& shortCut = {});

    /**
     * @brief Constructor
     * @param text Action text
     * @param method Method to execute when action was triggered
     */
    GtObjectUIAction(const QString& text, ActionMethod method);

    /**
     * @brief Overload. Constructor.
     * @param text Action text
     * @param method Method to execute when action was triggered.
     * Function requires a parent object
     */
    GtObjectUIAction(const QString& text, InvokableActionMethod method);

    /**
     * @brief text
     * @return
     */
    const QString& text() const;

    /**
     * @brief icon
     * @return
     */
    const QIcon& icon() const;

    /**
     * @brief Action method. Must be called with a parent and target objet as
     * parameters
     * @return Action method
     */
    const InvokableActionMethod& method() const;

    /**
     * @brief Verification method used to check if action should be enabled.
     * Must be called with a parent and target objet as
     * parameters
     * @return Verification method
     */
    const InvokableVerificationMethod& verificationMethod() const;

    /**
     * @brief Visibility method used to check if action should be visible.
     * Must be called with a parent and target objet as
     * parameters
     * @return Visibility method
     */
    const InvokableVisibilityMethod& visibilityMethod() const;

    /**
     * @brief Shortcut
     * @return Short cut connected to the action
     */
    const QKeySequence& shortCut() const;

    /**
     * @brief Dedicated setter for the UI icon
     * @param icon Icon
     * @return This
     */
    GtObjectUIAction& setIcon(const QIcon& icon);

    /**
     * @brief Overload. Accepts a string instead
     * @param icon Icon name or path
     * @return This
     */
    GtObjectUIAction& setIcon(const QString& icon);

    /**
     * @brief Dedicated setter for the verification method. Function signature
     * must accept a pointer of the parent object to invoke method on and the
     * target object.
     * @param method Method
     * @return This
     */
    GtObjectUIAction& setVerificationMethod(InvokableVerificationMethod method);

    /**
     * @brief Overload. Function signature must accept a pointer of the target
     * object.
     * @param method Method
     * @return This
     */
    GtObjectUIAction& setVerificationMethod(VerificationMethod method);

    /**
     * @brief Overload. Accepts the string of a invokable method.
     * @param method Method
     * @return This
     */
    GtObjectUIAction& setVerificationMethod(const QString& methodName);

    /**
     * @brief Dedicated setter for the visibility method. Function signature
     * must accept a pointer of the parent object to invoke method on and the
     * target object.
     * @param method Method
     * @return This
     */
    GtObjectUIAction& setVisibilityMethod(InvokableVisibilityMethod method);

    /**
     * @brief Overload. Function signature must accept a pointer of the target
     * object.
     * @param method Method
     * @return This
     */
    GtObjectUIAction& setVisibilityMethod(VisibilityMethod method);

    /**
     * @brief Overload. Accepts the string of a invokable method.
     * @param method Method
     * @return This
     */
    GtObjectUIAction& setVisibilityMethod(const QString& methodName);

    /**
     * @brief Dedicated setter for the shortcut. Shortcut must be registered
     * to make it visible to the application.
     * @param shortCut Shortcut
     * @return This
     */
    GtObjectUIAction& setShortCut(const QKeySequence& shortCut);

    /**
     * @brief Registers the shortcut in the application and sets the shortcut
     * for this action.
     * @param id Ident of the shortcut
     * @param cat Shortcut category (e.g. class name in which its used)
     * @param k Initial keysequence
     * @param readonly Read only
     * @return This
     */
    GtObjectUIAction& registerShortCut(QString const& id,
                                       QString const& cat,
                                       QKeySequence const& k,
                                       bool readOnly = false);

    /**
     * @brief Overload. Shortcut category will be set using the module name.
     * @param id Ident of the shortcut
     * @param k Initial keysequence
     * @param readonly Read only
     * @return This
     */
    // dummy template to make sure its generated once its used thus setting
    // the category using the module name
    template<typename Dummy = void>
    GtObjectUIAction& registerShortCut(QString const& id,
                                       QKeySequence const& k,
                                       bool readOnly = false)
    {
        return registerShortCut(id, GT_MODULENAME(), k, readOnly);
    }

private:
    /// Action text
    QString m_text{};

    /// Action icon
    QIcon m_icon{};

    /// Invokable method
    InvokableActionMethod m_method{};

    /// Verification method
    InvokableVerificationMethod m_verification{};

    /// Visibility method
    InvokableVisibilityMethod m_visibility{};

    /// Shortcut
    QKeySequence m_shortCut{};

    /**
     * @brief helper function to set action method suing the name of a
     * invokable method
     * @param methodName
     */
    void setActionMethod(const QString& methodName);
};

using GtActionList = QList<GtObjectUIAction>;

#endif // GTOBJECTUIACTION_H
