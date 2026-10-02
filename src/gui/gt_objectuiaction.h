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
#include "gt_globals.h"

#include <functional>
#include <QString>

class GtObject;
class QIcon;
class QKeySequence;

namespace gt
{
namespace gui
{

/// Predefined order priority values for context menus in GTlab
struct OrderPriority
{
    // using struct for tighter naming schemes while allowing implicit
    // int conversions
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
        Last  =  999,
        /// Denotes that this action should be placed first in a meenu.
        First = -999,

        /// inserts before the "open with" actions in a separate section
        BeforeOpenWithSection = OpenWithAction - 2,
        /// inserts before the "open with" actions in the same section
        BeforeOpenWithAction  = OpenWithAction - 1,
        /// inserts after the "open with" actions in the same section
        AfterOpenWithAction   = OpenWithAction,
        /// inserts after the "open with" actions in a separate section
        AfterOpenWithSection  = OpenWithAction + 1,

        /// inserts before the "import" actions in a separate section
        BeforeImportSection = ImportAction - 2,
        /// inserts before the "import" actions in the same section
        BeforeImportAction  = ImportAction - 1,
        /// inserts after the "import" actions in the same section
        AfterImportAction   = ImportAction,
        /// inserts after the "import" actions in a separate section
        AfterImportSection  = ImportAction + 1,

        /// inserts before the "export" actions in a separate section
        BeforeExportSection = ExportAction - 2,
        /// inserts before the "export" actions in the same section
        BeforeExportAction  = ExportAction - 1,
        /// inserts after the "export" actions in the same section
        AfterExportAction   = ExportAction,
        /// inserts after the "export" actions in a separate section
        AfterExportSection  = ExportAction + 1,

        /// inserts before the "rename" actions in a separate section
        BeforeRenameSection = RenameAction - 2,
        /// inserts before the "rename" actions in the same section
        BeforeRenameAction  = RenameAction - 1,
        /// inserts after the "rename" actions in the same section
        AfterRenameAction   = RenameAction,
        /// inserts after the "rename" actions in a separate section
        AfterRenameSection  = RenameAction + 1,

        /// inserts before the "delete" actions in a separate section
        BeforeDeleteSection = DeleteAction - 2,
        /// inserts before the "delete" actions in the same section
        BeforeDeleteAction  = DeleteAction - 1,
        /// inserts after the "delete" actions in the same section
        AfterDeleteAction   = DeleteAction,
        /// inserts after the "delete" actions in a separate section
        AfterDeleteSection  = DeleteAction + 1,
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
     * @brief Constructor, creates a separator action
     */
    GtObjectUIAction() noexcept;

    /**
     * @brief Constructor
     * @param text Action text
     * @param method Method to execute when action was triggered
     */
    GtObjectUIAction(QString name, ActionMethod method);

    /**
     * @brief Overload. Constructor.
     * @param text Action text
     * @param method Method to execute when action was triggered.
     * Function requires a parent object
     */
    GtObjectUIAction(QString name, InvokableActionMethod method);

    GtObjectUIAction(GtObjectUIAction const&) noexcept;
    GtObjectUIAction(GtObjectUIAction&&) noexcept;
    GtObjectUIAction& operator=(GtObjectUIAction const&) noexcept;
    GtObjectUIAction& operator=(GtObjectUIAction&&) noexcept;
    ~GtObjectUIAction() noexcept;

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
    GT_DEPRECATED_REMOVED_IN(2, 2, "Use `name()` instead")
    const QString& text() const { return name(); }
    /**
     * @brief Returns the action name
     * @return Action text
     */
    const QString& name() const;

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

    /**
     * @brief Swaps this action with `other`
     * @param other Other
     */
    void swap(GtObjectUIAction& other) noexcept;

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

inline void swap(GtObjectUIAction& a, GtObjectUIAction& b) noexcept { a.swap(b); }

using GtActionList = QList<GtObjectUIAction>;

namespace gt
{
namespace gui
{

inline GtObjectUIAction
makeAction(QString actionText, GtObjectUIAction::ActionMethod actionMethod)
{
    return GtObjectUIAction(std::move(actionText), std::move(actionMethod));
}

inline GtObjectUIAction
makeSeparator(int priority = OrderPriority::Default)
{
    GtObjectUIAction sep;
    sep.setOrderPriority(priority);
    return sep;
}

} // namespace gui

} // namespace gt

#endif // GTOBJECTUIACTION_H
