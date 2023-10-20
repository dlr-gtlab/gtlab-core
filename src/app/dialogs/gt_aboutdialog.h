/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_aboutdialog.h
 *
 *  Created on: 28.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABOUTDIALOG_H
#define GTABOUTDIALOG_H

#include "gt_dialog.h"

class QTreeWidget;
class QTabWidget;

/**
 * @brief The GtAboutLogo class
 */
class GtAboutLogo : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtAboutLogo
     * @param parent
     */
    explicit GtAboutLogo(QWidget* parent = nullptr);

protected:
    /**
     * @brief paintEvent
     * @param e
     */
    void paintEvent(QPaintEvent* e) override;

    /**
     * @brief mouseDoubleClickEvent
     * @param event
     */
    void mouseDoubleClickEvent(QMouseEvent* event) override;

private:
    /**
     * @brief m_clicks
     */
    int m_clicks;

    /**
     * @brief m_pixmap
     */
    QPixmap m_pixmap;
};

/**
 * @brief The GtAboutDialog class
 */
class GtAboutDialog : public GtDialog
{
    Q_OBJECT

public:
    enum Tabs{
        About = 0,
        ChangeLog = 1,
        Modules = 2
    };

    /**
     * @brief Constructor
     * @param startPage - number of page to start with
     * @param parent widget
     */
    explicit GtAboutDialog(Tabs startPage, QWidget* parent = nullptr);

private slots:
    /**
     * @brief Exports footprint of current framework configuration.
     */
    void exportFootprint();

    /**
     * @brief onModuleClicked
     * Reaction on clicked module in module overview
     * @param index
     */
    void onModuleClicked(const QModelIndex& index);

    /**
     * @brief onDetailsButtonClicked
     * On clicked on the details button of the module overview page
     */
    void onDetailsButtonClicked() const;

    /**
     * @brief openDetails
     * Open a dialog about the modules details based on the index of
     * module selected in the ree widget
     * @param indexOfModule
     */
    void openModuleDetails(const QModelIndex& indexOfModule) const;

    /**
     * @brief openLicenseFolder
     * Open the folder with the licenses files of GTlab
     */
    void openLicenseFolder();
private:
    /**
     * @brief coreAbout
     * @return a widget for the about page of the GTlab core
     */
    QWidget* coreAbout() const;

    /**
     * @brief modulesWidget
     * @return the widget for the module overview
     */
    QWidget* modulesWidget();

    /**
     * @brief changeLogWidget
     * @return widget for the core changelog
     */
    QWidget* changeLogWidget() const;

    /**
     * @brief sharedFunctionsWidget
     * @return widget for the overview of shared functions
     */
    QWidget* sharedFunctionsWidget();

    /**
     * @brief readChangeLog
     * @return the content of the changelog file in the application directory
     */
    QString readChangeLog() const;

    /**
     * @brief addDevelopmentTabs
     * add developer tabs to the widget containing lists af registered
     * calculators, objects,....
     * @param tabs widget to add the developer tabs to
     */
    void addDevelopmentTabs(QTabWidget* tabs);

    /// button for the details
    QPushButton* m_detailsButton;

    /// main treeview for the modules
    QTreeWidget* m_modulesTree;

    QString readApplicationDirFile(QString const& fileName) const;
};

#endif // GTABOUTDIALOG_H
