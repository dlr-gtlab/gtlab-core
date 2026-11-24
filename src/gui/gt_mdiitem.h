/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMDIITEM_H
#define GTMDIITEM_H

#include "gt_gui_exports.h"

#include <QPointer>

#include "gt_object.h"
#include "gt_shortcutsettingsdata.h"

#include <memory>

class GtProject;
class GtState;
class GtQmlAction;

/**
 * @brief The GtMdiItem class
 */
class GT_GUI_EXPORT GtMdiItem : public GtObject
{
    Q_OBJECT

    friend class GtProjectChangedEvent;
    friend class GtObjectChangedEvent;
    friend class GtGuiLayoutAdapter;
    friend class GtGuiLauncher;

public:
    /**
     * @brief ~GtMdiItem
     */
    ~GtMdiItem() override;

    /// Item Flags
    enum GuiItemFlag : int
    {
        /// Default
        NoFlag = 0,
        /// Indicates that the gui item may be instantiated multiple times
        /// if the custom id of the corresponding object is the same.
        /// Is overriden by `allowsMultipleInstances`.
        AllowsMultipleInstances = 1 << 0,
        /// Indicates that the gui item may react to updated data if the custom
        /// id of the corresponding object is the same. The `setData` method
        /// will be called with the new object
        AllowsReopening = 1 << 1,
        /// Indicates that the gui item should be deleted once the data object
        /// is deleted. If no data object is given, no instance is allowed.
        BoundToObject = 1 << 2,
    };
    using GuiItemFlags = QFlags<GuiItemFlag>;

    /**
     * @brief Sets or unsets the given item flag. Flags must be set within the
     * constructor to take an effect.
     * @param flag Item flag to set
     * @param enable Whether to set or reset the given flag
     */
    void setItemFlag(GuiItemFlag flag, bool enable = true) noexcept;
    /**
     * @brief Sets the given item flags. Flags must be set within the
     * constructor to take an effect.
     * @param flags Item flags to set
     */
    void setItemFlags(GuiItemFlags flags) noexcept;
    /**
     * @brief Returns the item flags currently set.
     * @return Flags
     */
    GuiItemFlags itemFlags() const noexcept;

    /**
     * @brief Returns this gui items widget
     * @return Widget
     */
    QWidget* widget();

    /**
     * @brief Returns the current data object
     * @return Current data object. May be null.
     */
    GtObject* currentObject() noexcept;
    const GtObject* currentObject() const noexcept;

    /**
     * @brief Returns the id of this gui item. The id is used to indentify the
     * gui item. A gui item may yield the same id for different objects
     * (by using `customId`). If another gui item is being requested that has
     * the same id as an exisiting gui item, the `AllowsMultipleInstances` flag
     * will decide wehther another item is opened.
     * @return Current id
     */
    const QString& currentId() const noexcept;

    /**
     * @brief Virtual function to specify item specific icon.
     * @return Object specific icon.
     */
    virtual QIcon icon() const;

    /**
      * @brief subWin
      * @return
      */
    GT_DEPRECATED_REMOVED_IN(2, 2, "No replacement planned. Use `widget()`.")
    QWidget* subWin();

    /**
     * @brief Returns the widget containing this gui item's widget
     * @return Containing widget
     */
    QWidget* containingWidget();

    /**
     * @brief Denotes whether multiple instances of this gui item are allowed.
     * This is determined by the id of the gui item (see `currentId`). If an
     * existing gui item has the same id and multiple instances are not allowed,
     * a new instance is not created. By default multiple instances are not
     * allowed.
     * @return Whether multiple instances for gui items with the same id are
     * allowed.
     */
    virtual bool allowsMultipleInstances() const;

    /**
     * @brief Virtual function to specify if this gui item is printable.
     * @return Printable indicator.
     */
    virtual bool isPrintable() const;

    /**
     * @brief Returns actions installed into the toolbar when the item gets
     * active
     */
    const std::vector<GtQmlAction*>& toolbarActions() const;

public slots:
    /**
     * @brief print
     */
    virtual void print();

protected:
    /**
     * @brief GtMdiItem
     */
    GtMdiItem();

    /**
     * @brief Triggered once the widget is about to show, before `showEvent`.
     * `currentObject` and `customId` are set and `widget` is .
     */
    virtual void initialized();

    /**
     * @brief Triggered once when the gui item is shown, before `setData`
     */
    virtual void showEvent();

    /**
     * @brief Triggered once the item is initialized and the widget gets
     * visible. Main method to apply the data to the view/editor. If the flag
     * `AllowsReopening` is set, this method is used to apply new data too.
     * @param obj Object (same as currentObject). May be null.
     */
    virtual void setData(GtObject* obj);

    /**
     * @brief Allows to return a custom id based on the given object. It can be
     * used to link multiple objects onto the same gui item instance by
     * returning the same id for each object. The custom id is class specific.
     * By default no custom id is used and each gui item has a unique id for
     * each unique data object.
     * @param obj Given object
     * @return Custom identification string.
     */
    virtual QString customId(GtObject* obj);

    /**
     * Event called after current project has changed.
     * @param new current project
     */
    virtual void projectChangedEvent(GtProject* project);

    /**
     * @brief Event called after object selection has changed.
     * @param current object
     */
    virtual void objectChangedEvent(GtObject* obj);

    /**
     * @brief Event called after project was saved.
     * @param project Savend project.
     */
    virtual void projectSavedEvent(GtProject* project);

    /**
     * @brief onThemeChanged
     * Implementation to hande the change betewwn dark and bright mode
     */
    virtual void onThemeChanged();

    /**
     * @brief setSubWin
     * @param subWin
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "Bi replacement planned. Edit `widget()`.")
    void setSubWin(QWidget* subWin);

    /**
     * @brief registerShortCut
     * @param id - id of the shortcut to use
     * @param k - key sequence to use
     * @param readOnly - optional flag to define it as read only
     * @return the key sequence used for this id
     */
    QKeySequence registerShortCut(QString const& id,
                                  QKeySequence const& k,
                                  bool readOnly = false);

    /**
     * @brief registerShortCut
     * @param data - short cut data to register
     * @return the key sequence used for this id
     */
    GT_DEPRECATED_REMOVED_IN(
        2, 2,
        "Use registerShortCut(QString const& id, "
        "QKeySequence const& k, bool readOnly = false) instead")
    QKeySequence registerShortCut(GtShortCutSettingsData const& data);

    /**
     * @brief registerShortCuts
     * @param list - list of short cuts to register
     */
    GT_DEPRECATED_REMOVED_IN(
        2, 2,
        "Use registerShortCut(QString const& id, "
        "QKeySequence const& k, bool readOnly = false) instead")
    void registerShortCuts(QList<GtShortCutSettingsData> const& list);

    /**
     * @brief Initializes a new global state.
     * @param State identification string.
     * @param State specific path.
     * @param Initial value.
     * @param Force initialization indicator.
     * @return New state.
     */
    GtState* initializeState(const QString& id, const QString& path,
                             const QVariant& initValue,
                             bool forceInitialization = false);

    /**
     * @brief Initializes a new project specific state.
     * @param Project.
     * @brief Initializes a new global state.
     * @param State identification string.
     * @param State specific path.
     * @param Initial value.
     * @param Force initialization indicator.
     * @return New state.
     */
    GtState* initializeState(GtProject* project, const QString& id,
                             const QString& path, const QVariant& initValue,
                             bool forceInitialization = false);

    /**
     * @brief Adds a toolbar action for the gui item
     *
     * The item will be the owner / parent of the created action
     *
     * @param text    The text of the action
     * @param iconUrl The icon of the action
     * @return A newly created action
     */
    GtQmlAction* addToolbarAction(const QString& text, const QUrl& iconUrl);

    /**
     * @brief Adds a separator to the toolbar context
     */
    void addToolbarSeparator();

    /**
     * @brief getShortCut
     * @param id - identification string of the short cut to read
     * @return short cut for this id registered in this object
     */
    QKeySequence getShortCut(const QString& id);

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;

    /**
     * @brief Sets the containing widget. The item may be embedded into another
     * widget which then becomes the containing widget (e.g. a dock-like widget)
     * @param cwidget Containing widget
     */
    void setContainingWidget(QWidget* cwidget);

    /**
     * @brief Sets the current object internall and regenertes the id using the
     * custom id, if valid.
     * @param object New data object
     * @param customId optional custom id
     * @return Whether object is accepted
     */
    bool setCurrentObject(GtObject* object, QString customId = {});

private slots:

    /** Called after current project has changed.
        @param project new current project */
    void onProjectChanged(GtProject* project);

    /**
     * @brief Called after project was saved.
     * @param project Saved project.
     */
    void onProjectSaved(GtProject* project);

    /** Called after current project has changed.
        @param project new current project */
    void onObjectChanged(GtObject* obj);

    /**
     * @brief windowStateChanged
     * @param oldState
     * @param newState
     */
    void windowStateChanged(Qt::WindowStates oldState,
                            Qt::WindowStates newState);

    /**
     * @brief windowAboutToActive
     */
    void windowAboutToActive();
};

/// forward decl
using GtGuiItem = GtMdiItem;

#endif // GTMDIITEM_H
