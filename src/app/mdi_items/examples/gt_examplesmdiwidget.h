/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 10.11.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTEXAMPLESMDIWIDGET_H
#define GTEXAMPLESMDIWIDGET_H

#include "gt_mdiitem.h"
#include "gt_examplesentry.h"
#include <QMap>

class QDir;
class QScrollArea;
class QWidget;

class GtExamplesTabWidget;
class GtExampleGraphicalItem;

/**
 * @brief The GtExamplesMdiWidget class
 * Main Widget for visulaization of example-selection
 */
class GtExamplesMdiWidget : public GtMdiItem
{
    Q_OBJECT

public:
    /**
     * @brief GtExamplesMdiWidget - Constructor
     */
    Q_INVOKABLE GtExamplesMdiWidget();


    /** Virtual function to specify item specific icon.
     *  @return Object specific icon
     */
    QIcon icon() const override;

    /**
     * @brief allowsMultipleInstances
     * @return
     */
    bool allowsMultipleInstances() const override;

    /**
     * @brief showEvent
     */
    void showEvent() override;

private:
    /**
     * @brief Initializes examples path. Path remains empty if initialization
     * fails
     */
    void initializeExamplesPath();

    /**
     * @brief initializeDirectories. Searchs for example projects and adds them
     * to the example entries list.
     */
    void initializeDirectories();

    /**
     * @brief validateExampleDiretory
     * validate directory if all necessary files are inside
     * @param dir
     * @returnd
     */
    bool validateExampleDiretory(const QDir& dir) const;

    /**
     * @brief readDirectoryContentToExampleEntry
     * Read Content of directory to GtExampleEntry object
     * @param dir
     * @param entry
     * @return success
     */
    bool readDirectoryContentToExampleEntry(const QDir& dir,
                                            GtExamplesEntry& entry);
    /**
     * @brief initializeWidget
     * initialize the tabView
     */
    void initializeWidget();

    QList<GtExamplesEntry> m_examplesEntries;

    QList<GtExampleGraphicalItem*> m_graphicalItems;

    QString m_examplesPath;

    GtExamplesTabWidget* m_tabWidget;

    QMap<QString, QWidget*> m_tabs;

    /**
     * @brief getAllcategories
     * Read all categories from the example-entries
     * @return
     */
    QStringList getAllCategories();

    /**
     * @brief setBasicLayoutToTabPage
     *  Set some general basic-layout stuff to the tab-page widget
     * @param tabPage
     */
    void setBasicLayoutToTabPage(QScrollArea* tabPage, QString const& category);

    /**
     * @brief getCurrentPage
     * Get the tab-view page by the name of the category
     * @param category
     * @return
     */
    QWidget* getCurrentPage(const QString& category) const;

    /**
     * @brief sortItems
     *  Sort the elements to current tabWidget-size
     */
    void sortItems();

public slots:
    /**
     * @brief onResized
     *  Reaction on resizing the tabwidget is the repeated ues of sortItems
     */
    void onResized();

    /**
     * @brief onOpenProject
     * Copy the data from Examples-Dir to dir which is choosen inside and
     * opens the project in the current Session
     */
    void onOpenProject(const QString& exampleName);

};

#endif // GTEXAMPLESMDIWIDGET_H
