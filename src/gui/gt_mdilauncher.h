/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMDILAUNCHER_H
#define GTMDILAUNCHER_H

#include "gt_gui_exports.h"
#include "gt_globals.h"

#include "gt_abstractobjectfactory.h"

#include <QObject>
#include <QPointer>
#include <QMap>

#define gtMdiLauncher (GtMdiLauncher::instance())

class QTabWidget;
class QMdiSubWindow;
class GtMdiItem;
class GtCollectionInterface;
class GtAbstractCollectionSettings;
class GtQmlAction;
class GtGuiLayoutAdapter;

/**
 * @brief The GtMdiLauncher class
 */
class GT_GUI_EXPORT GtMdiLauncher : public QObject,
                                    protected GtAbstractObjectFactory
{
    friend class GtGuiModuleLoader;

    Q_OBJECT

public:
    /**
     * @brief instance
     * @return
     */
    static GtMdiLauncher* instance();

    /**
     * @brief Sets the adapter responsible for laying out the gui items
     * @param adapter Adapter
     */
    void setAdapter(std::unique_ptr<GtGuiLayoutAdapter> adapter);

    /**
     * @brief setMdiArea
     * @param area
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `setAdapter` instead")
    void setMdiArea(QTabWidget* area);

    /**
     * @brief Returns the "open with" menu entry name for the launcher
     * @param classname The name of the MDI class
     *
     * @return The name. Returns the class name, if the menu name is not known
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::displayName`")
    QString mdiOpenWithName(const QString& classname) const;

    /**
     * @brief dockWidgetIds
     * @return
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::knownDockWidgets`")
    QStringList dockWidgetIds();

    /**
     * @brief Returns meta object of dock widget.
     * @param dock widget identification string
     * @return dock widget meta object
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::dockWidget`")
    QMetaObject dockWidget(const QString& id);

    /**
     * @brief collectionIds
     * @return
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtCollectionRegistry::collectionIds`")
    QStringList collectionIds();

    /**
     * @brief collectionIcon
     * @param id
     * @return
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtCollectionRegistry::collectionIcon`")
    QIcon collectionIcon(const QString& id);

    /**
     * @brief collectionStructure
     * @param id
     * @return
     */
    GT_DEPRECATED_REMOVED_IN(2, 2,
                             "use `GtCollectionRegistry::collectionStructure`")
    QMap<QString, QMetaType::Type> collectionStructure(const QString& id);

    /**
     * @brief collection
     * @param id
     * @return
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtCollectionRegistry::collection`")
    GtCollectionInterface* collection(const QString& id);

    /**
     * @brief Calls print routine of current mdi window.
     */
    void printCurrentWindow();

    /**
     * @brief print
     * @param subWindow
     */
    void print(QWidget* widget);

    /**
     * @brief Returns true if current gui item is printable.
     * @return Is printable
     */
    bool isCurrentWindowPrintable() const;

    /**
     * @brief Returns true if mdi item for given subwindow is printable.
     * @param subWindow
     * @return Is printable
     */
    bool isPrintable(QWidget* widget) const;

    /**
     * @brief Returns the toolbar actions of the currently opened MDI window,
     *        which need to be placed into the editor context of the main toolbar
     *
     * @param subWindow The widget of the currently opened tab (from the MDIItem)
     *
     * @return The list of actions
     */
    std::vector<GtQmlAction*> toolbarActions(QWidget* widget) const;

    /**
     * @brief registerDockWidget
     * @param metaObj - meta object of dockwidget to add
     * @return true in case of success, else false
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::registerDockWidget`")
    bool registerDockWidget(QMetaObject metaObj);

    /**
     * @brief Closes all active MDI items.
     */
    void closeAll();

    /** DEPRECATED API **/

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::makeGuiItem()`")
    GtObject* newObject(const QString& className,
                        GtObject* parent = nullptr) override;

    GT_DEPRECATED_REMOVED_IN(2, 2,
                             "use `GtGuiFactory::guiItem()"
                             "->superClass()->className()`")
    QString superClassName(const QString& className) const override;

    GT_DEPRECATED_REMOVED_IN(2, 2,
                             "use `gt::classHierarchy` "
                             "(gt_qtuitilities.h)")
    QStringList classHierarchy(const QString& className) override;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::isKnownGuiItem`")
    bool knownClass(const QString& className) const override;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::knownGuiItems`")
    QStringList knownClasses() const override;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::registerGuiItem`")
    bool registerClass(QMetaObject metaObj) override;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::registerGuiItem`")
    bool registerClass(QMetaObject metaObj, const QString& moduleId) override;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::unregisterGuiItem`")
    bool unregisterClass(const QMetaObject& metaObj);

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::registerGuiItem`")
    bool registerClasses(const QList<QMetaObject>& metaData) override;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::registerGuiItem`")
    bool registerClasses(const QList<QMetaObject>& metaData,
                         const QString& moduleId) override;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::moduleId`")
    QString moduleId(const QString& className) const override;

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::isKnownGuiItem`")
    bool containsDuplicates(const QList<QMetaObject>& metaData) const override;

    GT_DEPRECATED_REMOVED_IN(2, 2,
                             "use `gt::isDefaultConstructible` "
                             "(gt_qtuitilities.h)")
    bool invokable(const QMetaObject& metaObj) const override;

    GT_DEPRECATED_REMOVED_IN(2, 2,
                             "use `gt::areDefaultConstructible` "
                             "(gt_qtuitilities.h)")
    bool allInvokable(const QList<QMetaObject>& metaData);

    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::guiItem`")
    const QMetaObject* metaObject(const QString& clzName) const;

protected:
    /**
     * @brief dockWidgetsExists
     * @param metaData
     * @return
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::isKnownDockWidget`")
    bool dockWidgetsExist(const QList<QMetaObject>& metaData);

    /**
     * @brief registerDockWidgets
     * @param metaData
     * @return
     */
    GT_DEPRECATED_REMOVED_IN(2, 2, "use `GtGuiFactory::registerDockWidget`")
    bool registerDockWidgets(const QList<QMetaObject>& metaData);

    /**
     * @brief registerCollection
     * @param str
     * @param meta
     * @return
     */
    GT_DEPRECATED_REMOVED_IN(2, 2,
                             "use `GtCollectionRegistry::registerCollection`")
    bool registerCollection(const QString& str, GtCollectionInterface* coll);

public slots:
    /**
     * @brief open
     * @param id
     * @param data
     * @param customId
     * @return
     */
    GtMdiItem* open(const QString& id,
                    GtObject* data = nullptr,
                    const QString& customId = QString());

    /**
     * @brief open
     * @param id
     * @param customId
     * @return
     */
    GtMdiItem* open(const QString& id, const QString& customId);

private:
    /**
     * @brief GtMdiLauncher
     * @param parent
     */
    explicit GtMdiLauncher(QObject* parent = nullptr);
    ~GtMdiLauncher();

    struct Impl;
    std::unique_ptr<Impl> pimpl;

    /**
     * @brief isItemAllowed
     * @param mdiItem
     * @param data
     * @return
     */
    bool isItemAllowed(GtMdiItem* mdiItem);

    /**
     * @brief setFocus
     * @param mdiId
     */
    void setFocus(const QString& mdiId);
};

#endif // GTMDILAUNCHER_H
