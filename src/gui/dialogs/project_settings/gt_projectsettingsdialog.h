/* GTlab - Gas Turbine laboratory
 * Source File: gt_projectsettingsdialog.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 31.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTPROJECTSETTINGSDIALOG_H
#define GTPROJECTSETTINGSDIALOG_H

#include <QDialog>
#include <memory>

class GtProject;
class QTabWidget;
class QVBoxLayout;
class GtProjectSettingsTab;
/**
 * @brief The GtProjectSettingsDialog class
 */
class GtProjectSettingsDialog : public QDialog
{
    Q_OBJECT

public:

    /**
     * @brief GtProjectSettingsDialog
     * @param project pointer to valid project
     */
    explicit GtProjectSettingsDialog(GtProject* project);

    /**
     * @brief adds a new tab
     * @param tab Tab to add. This object will take ownership.
     */
    void addTab(std::unique_ptr<GtProjectSettingsTab>&& tab);

public slots:

    /**
     * @brief slot for saving all settings
     */
    void saveChanges();

private:

    /// tab widget
    QTabWidget* m_tabWidget{};
    /// base layout
    QVBoxLayout* m_layout{};

signals:

    /**
     * @brief request that the project should be saved
     */
    void projectSaveRequested();

    /**
     * @brief request that the project should be closed
     */
    void projectCloseRequested();
};

#endif // GTPROJECTSETTINGSDIALOG_H
