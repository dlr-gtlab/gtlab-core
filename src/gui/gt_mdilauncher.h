/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 11.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTMDILAUNCHER_H
#define GTMDILAUNCHER_H

#include "gt_gui_exports.h"

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

/**
 * @brief The GtMdiLauncher class
 */
class GT_GUI_EXPORT GtMdiLauncher : public QObject,
        public GtAbstractObjectFactory
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
     * @brief setMdiArea
     * @param area
     */
    void setMdiArea(QTabWidget* area);

    /**
     * @brief dockWidgetIds
     * @return
     */
    QStringList dockWidgetIds();

    /**
     * @brief Returns meta object of dock widget.
     * @param dock widget identification string
     * @return dock widget meta object
     */
    QMetaObject dockWidget(const QString& id);

    /**
     * @brief collectionIds
     * @return
     */
    QStringList collectionIds();

    /**
     * @brief collectionIcon
     * @param id
     * @return
     */
    QIcon collectionIcon(const QString& id);

    /**
     * @brief collectionStructure
     * @param id
     * @return
     */
    QMap<QString, QMetaType::Type> collectionStructure(const QString& id);

    /**
     * @brief collection
     * @param id
     * @return
     */
    GtCollectionInterface* collection(const QString& id);

    /**
     * @brief Calls print routine of current mdi window.
     */
    void printCurrentWindow();

    /**
     * @brief print
     * @param subWindow
     */
    void print(QWidget *subWindow);

    /**
     * @brief Returns true in mdi item for given subwindow is printable.
     * @param subWindow
     */
    bool isPrintable(QWidget* subWindow) const;

    /**
     * @brief registerDockWidget
     * @param metaObj - meta object of dockwidget to add
     * @return true in case of success, else false
     */
    bool registerDockWidget(QMetaObject metaObj);

protected:
    /**
     * @brief dockWidgetsExists
     * @param metaData
     * @return
     */
    bool dockWidgetsExist(const QList<QMetaObject>& metaData);

    /**
     * @brief registerDockWidgets
     * @param metaData
     * @return
     */
    bool registerDockWidgets(const QList<QMetaObject>& metaData);

    /**
     * @brief registerCollection
     * @param str
     * @param meta
     * @return
     */
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

    /**
     * @brief changeTabTitle
     * This slot changes the title of the current selected mdi item
     * or if it is called by a connection it renames the mdi item
     * which was the sender of the signal
     * @param newTitle -  new title
     */
    void changeTabTitle(QString const& newTitle);

private:
    /**
     * @brief GtMdiLauncher
     * @param parent
     */
    explicit GtMdiLauncher(QObject* parent = nullptr);

    /// Mdi area widget
    QPointer<QTabWidget> m_area;

    /// Registered dock widget meta objects
    QHash<QString, QMetaObject> m_dockWidgets;

    /// List of all open mdi items
    QMap<const QObject*, GtMdiItem*> m_openItems;

    /// Collections
    QHash<QString, GtCollectionInterface*> m_collections;

    /**
     * @brief generateMdiItemId
     * @param data
     * @return
     */
    QString generateMdiItemId(GtMdiItem* mdiItem);

    /**
     * @brief mdiItemAllowed
     * @param mdiItem
     * @param data
     * @return
     */
    bool mdiItemAllowed(GtMdiItem* mdiItem);

    /**
     * @brief openItemIds
     * @return
     */
    QStringList openItemIds();

    /**
     * @brief setFocus
     * @param mdiId
     */
    void setFocus(const QString& mdiId);

private slots:
    /**
     * @brief onSubWindowClose
     * @param obj
     */
    void onSubWindowClose(QObject* obj);

    void onCloseTabRequest(int i);
};

#endif // GTMDILAUNCHER_H
