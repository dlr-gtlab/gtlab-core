/* GTlab - Gas Turbine laboratory
 * Source File: gt_projectsettingstab.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 31.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTPROJECTSETTINGSTAB_H
#define GTPROJECTSETTINGSTAB_H

#include <QWidget>
#include <QPointer>

class GtProject;
class QVBoxLayout;
/**
 * @brief The GtProjectSettingsTab class. Base class for tabs in the Project
 * Settings dialog.
 */
class GtProjectSettingsTab : public QWidget
{
    Q_OBJECT

public:

    virtual ~GtProjectSettingsTab() = default;

    /**
     * @brief method called when dialog was accepted
     */
    virtual void saveSettings() = 0;

    /**
     * @brief display name of this tab
     * @return tab name
     */
    virtual QString tabName() const = 0;

protected:

    /**
     * @brief GtProjectSettingsTab
     * @param project pointer to valid project
     * @param parent parent widget
     */
    explicit GtProjectSettingsTab(GtProject* project, QWidget* parent = {});

    /**
     * @brief layout
     * @return layout of this widget
     */
    QVBoxLayout* layout() const;

    /**
     * @brief project
     * @return currently open project
     */
    GtProject* project() const;

private:

    /// pointer to project
    QPointer<GtProject> m_project{};
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

#endif // GTPROJECTSETTINGSTAB_H
