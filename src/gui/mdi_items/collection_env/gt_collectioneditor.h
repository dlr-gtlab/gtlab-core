/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectioneditor.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 02.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONEDITOR_H
#define GT_COLLECTIONEDITOR_H

#include "gt_gui_exports.h"

#include "gt_mdiitem.h"

#include "gt_accessdata.h"

class QComboBox;
class QPushButton;
class QTabWidget;
class QCheckBox;
class GtCollectionLoadingWidget;
class GtAbstractCollectionSettings;
class GtCollectionInterface;
class GtCollectionReply;
class GtCollectionLoader;
class GtCollectionNetworkItem;
class GtAbstractBrowserWidget;

/**
 * @brief The GtCollectionEditor class
 */
class GT_GUI_EXPORT GtCollectionEditor : public GtMdiItem
{
    Q_OBJECT

public:
    /**
     * @brief GtCollectionEditor
     */
    Q_INVOKABLE GtCollectionEditor();

    /**
     * @brief setCollection
     * @param collection
     */
    void setCollection(GtCollectionInterface* collection);

private:
    /// Collection widget.
    QWidget* m_collectionWidget;

    /// Collection specific widget
    QWidget* m_specificCollectionWidget;

    /// Browser widget.
    QWidget* m_browserWidget;

    /// Collection specific browser
    GtAbstractBrowserWidget* m_specificBrowserWidget;

    /// Collection.
    GtCollectionInterface* m_collection;

    /// Collection settings.
    GtAbstractCollectionSettings* m_collectionSettings;

    /// Source combo box.
    QComboBox* m_srcCombo;

    /// Download button.
    QPushButton* m_downloadButton;

    /// Download all updates button.
    QPushButton* m_updateAllButton;

    /// Select all valid items check box.
    QCheckBox* m_selectAllCheckBox;

    /// Tab widget.
    QTabWidget* m_tabWidget;

    /// Collection replies.
//    QList<GtCollectionReply*> m_replies;

    /// Collection loader.
    GtCollectionLoader* m_loader;

    /// Queue of access data
    QList<GtAccessData> m_accessQueue;

    /// Fetch list animation.
    GtCollectionLoadingWidget* m_fetchMovie;

    /**
     * @brief generateCollectionSettings
     * @param id
     * @param parent
     * @return
     */
    void generateCollectionSettings();

    /**
     * @brief setSettings
     * @param meta
     */
    void generateCollectionSpecificWidgets();

    /**
     * @brief loadLocalCollection
     */
    void loadLocalCollection();

    /**
     * @brief fillSourceList
     */
    void fillSourceList();

    /**
     * @brief loadNextAccessData
     */
//    void loadNextAccessData();

    /**
     * @brief setBrowserCollection
     * @param installedItems
     * @param availableItems
     * @param updataAvailableItems
     */
    void setBrowserCollection(const QList<GtCollectionNetworkItem>& installedItems,
                              const QList<GtCollectionNetworkItem>& availableItems,
                           const QList<GtCollectionNetworkItem>& updataAvailableItems);

private slots:
    /**
     * @brief onTabChange
     * @param tab
     */
    void onTabChange(int tab);

    /**
     * @brief reloadNetworkCollection
     */
    void reloadNetworkCollection();

    /**
     * @brief onCollectionReply
     */
    void onCollectionReply();

    /**
     * @brief onBrowserSelectionChange
     */
    void onBrowserSelectionChange();

    /**
     * @brief onSelectAllCheckToggled
     * @param checked
     */
    void onSelectAllCheckToggled(bool checked);

    /**
     * @brief downloadSelectedItems
     */
    void downloadSelectedItems();

    /**
     * @brief updateAllItems
     */
    void updateAllItems();

};

#endif // GT_COLLECTIONEDITOR_H
