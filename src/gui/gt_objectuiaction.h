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


namespace gt
{
namespace gui
{

/// Predefined order priority values for context menus in GTlab
struct OrderPriority
{
    // using struct for tighter naming schemes while allowing implicit int conversions
    enum Value : int
    {
        /// default order priority
        Default = 0,

        /// order priority of the "open with" action in the explorer
        OpenWithAction = -50,
        /// order priority of the "import" action
        ImportAction = 50,
        /// order priority of the "export" action
        ExportAction = 100,
        /// order priority of the "rename" action
        RenameAction = 150,
        /// order priority of the "delete" action
        DeleteAction = 200,

        /// denotes that this action should be placed last in a menu.
        Last = 999,
        /// Denotes that this action should be placed first in a meenu.
        First = -999,

        /// order priority to insert before the "open with" actions in the explorer
        /// (but still in the same section)
        BeforeOpenWithAction = OpenWithAction - 1,
        /// order priority to insert after the "open with" actions in the explorer
        /// (in a new section)
        AfterOpenWithAction  = OpenWithAction + 1,

        /// order priority to insert before the "import" action
        /// (but still in the same section)
        BeforeImportAction = ImportAction - 1,
        /// order priority to insert after the "import" action
        /// (in a new section)
        AfterImportAction  = ImportAction + 1,


        /// order priority to insert before the "export" action
        /// (but still in the same section)
        BeforeExportAction = ExportAction - 1,
        /// order priority to insert after the "export" action
        /// (in a new section)
        AfterExportAction  = ExportAction + 1,

        /// order priority to insert before the "rename" action
        /// (but still in the same section)
        BeforeRenameAction = RenameAction - 1,
        /// order priority to insert after the "rename" action
        /// (in a new section)
        AfterRenameAction  = RenameAction + 1,

        /// order priority to insert before the "delete" action
        /// (but still in the same section)
        BeforeDeleteAction = DeleteAction - 1,
        /// order priority to insert after the "delete" action
        /// (in a new section)
        AfterDeleteAction  = DeleteAction + 1,
    };
};

} // namespace gui

} // namespace gt

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
     * @brief Creates an action method from the method name
     * @param methodName The name of the method
     *
     * Example:
     *   GtObjectUIAction("Add point", GtObjectUIAction::fromMethodName("addPoint"))
     */
    static InvokableActionMethod
    fromMethodName(const QString& methodName);

    /**
     * @brief GtObjectUIAction
     */
    GtObjectUIAction();

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
     * @brief Returns whether this action is empty
     * @return is empty
     */
    bool isEmpty() const;

    /**
     * @brief Returns whether this action is a separator
     * @return is separator
     */
    bool isSeparator() const;

    /**
     * @brief Returns the action text
     * @return Action text
     */
    const QString& text() const;

    /**
     * @brief Returns the action icon
     * @return Action icon
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
     * @brief Returns the priority according to which this action is sorted
     * in a menu. An action with a lower priority 'x' prepends all actions
     * with a higher prority > x.
     * @return Order priority
     */
    int orderPriority() const;

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
     * @brief Sets whether the action should be enabled. Overrides any
     * verification method set previously.
     * @param enabled Whether to enable the action
     * @return This
     */
    GtObjectUIAction& setEnabled(bool enabled);

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
     * @brief Sets whether the action should be visible. Overrides any
     * visibility method set previously.
     * @param visible Whether the action should be visible
     * @return This
     */
    GtObjectUIAction& setVisible(bool visible);

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

    /**
     * @brief Sets the order priority according to which the action is sorted
     * in the menu. An action with a lower priority 'x' prepends all actions
     * with a higher prority > x.
     * @param priority Order priority
     * @return This
     */
    GtObjectUIAction& setOrderPriority(int priority);


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

    int m_priority{gt::gui::OrderPriority::Default};

    /**
     * @brief helper function to set action method suing the name of a
     * invokable method
     * @param methodName
     */
    void setActionMethod(const QString& methodName);
};

using GtActionList = QList<GtObjectUIAction>;

namespace gt
{
namespace gui
{

inline GtObjectUIAction
makeAction(const QString& actionText, GtObjectUIAction::ActionMethod actionMethod)
{
    return GtObjectUIAction(actionText, std::move(actionMethod));
}

inline GtObjectUIAction
makeSeparator(int priority = OrderPriority::Default)
{
    return GtObjectUIAction().setOrderPriority(priority);
}

} // namespace gui

} // namespace gt

#endif // GTOBJECTUIACTION_H
