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

public:
    /**
     * @brief ~GtMdiItem
     */
    ~GtMdiItem() override;

    bool setCurrentObject(GtObject* object, QString customId = {});

    const QString& currentId() const noexcept;

    enum GuiItemFlag : int
    {
        NoFlag = 0,
        /// Indicates that the gui item may be instantiated multiple times
        /// if the custom id of the corresponding object is the same.
        /// Is overriden by `allowsMultipleInstances`.
        AllowsMultipleInstances = 1 << 0,
        /// Indicates that the gui item may react to updated data if the custom
        /// id of the corresponding object is the same.
        /// TODO: where is it called?
        AllowsReopening = 1 << 1,
        /// Indicates that the gui item is deleted once the data object is deleted
        BoundToObject = 1 << 2,
    };
    using GuiItemFlags = QFlags<GuiItemFlag>;

    void setItemFlag(GuiItemFlag flag, bool enable = true) noexcept;
    void setItemFlags(GuiItemFlags flags) noexcept;
    GuiItemFlags itemFlags() const noexcept;

    /**
     * @brief Returns this gui items widget
     * @return Widget
     */
    QWidget* widget();

    GtObject* currentObject() noexcept;
    const GtObject* currentObject() const noexcept;

    /**
     * @brief Virtual function to specify item specific icon.
     * @return Object specific icon.
     */
    virtual QIcon icon() const;

    /**
     * @brief setData
     * @param obj
     */
    virtual void setData(GtObject* obj);

    /**
     * @brief Triggered once when the mdi item is shown
     */
    virtual void showEvent();

    /**
      * @brief subWin
      * @return
      */
    GT_DEPRECATED_REMOVED_IN(2, 2, "No replacement planned. Use `widget()`.")
    QWidget* subWin()
    {
        return widget();
    }

    /**
     * @brief Returns the widget containing this gui item's widget
     * @return Containing widget
     */
    QWidget* containingWidget();

    /**
     * @brief Virtual function to specify if this gui item allows multiple
     * instances. By default multiple instances are not allowed.
     * @return Multiple instances indicator.
     */
    virtual bool allowsMultipleInstances() const;

    /**
     * @brief Virtual function to specify if this gui item is printable.
     * @return Printable indicator.
     */
    virtual bool isPrintable() const;

    /**
     * @brief Returns actions installed into the toolbar when the item gets active
     */
    const std::vector<GtQmlAction*>& toolbarActions() const;

public slots:
    /**
     * @brief print
     */
    virtual void print();

    /**
     * @brief onThemeChanged
     * Implementation to hande the change betewwn dark and bright mode
     */
    virtual void onThemeChanged();

protected:
    /**
     * @brief GtMdiItem
     */
    GtMdiItem();

    /**
     * @brief initialized
     */
    virtual void initialized();

    /**
     * @brief Virtual function to return custom id based on the given object.
     * Default implementation returns empty string.
     * @param obj Given object
     * @return Custom identification string.
     */
    virtual QString customId(GtObject* obj);

    /** Event called after current project has changed.
        @param new current project */
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

    bool regenerateId();

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

signals:
    /**
     * @brief try to reopening mdi item (Result View)
     * @param pointer to existing data object
     */
    // TODO: whats the replacement?
    GT_DEPRECATED_REMOVED_IN(2, 2, "...")
    void tryReopening(GtObject* existDataObj);

};

#endif // GTMDIITEM_H
