/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 30.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSETTINGS_H
#define GTSETTINGS_H

#include "gt_core_exports.h"

#include "gt_abstractsettings.h"

/**
 * @brief The GtSettings class
 */
class GT_CORE_EXPORT GtSettings : public GtAbstractSettings
{
    friend class GtCoreApplication;

public:
    /**
     * @brief lastSession
     * @return
     */
    QString lastSession();

    /**
     * @brief setLastSession
     * @param val
     */
    void setLastSession(const QString& val);

    /**
     * @brief lastPerspective
     * @return
     */
    QString lastPerspective();

    /**
     * @brief setLastPerspective
     * @param val
     */
    void setLastPerspective(const QString& val);

    /**
     * @brief lastPath
     * @return
     */
    QString lastPath();

    /**
     * @brief setLastPath
     * @param val
     */
    void setLastPath(const QString& val);

    /**
     * @brief openLastSession
     * @return
     */
    bool openLastSession();

    /**
     * @brief setOpenLastSession
     * @param val
     */
    void setOpenLastSession(bool val);

    /**
     * @brief language
     * @return
     */
    QString language();

    /**
     * @brief setLanguage
     * @param val
     */
    void setLanguage(const QString& val);

    /**
     * @brief firstApplicationRun
     * @return
     */
    bool firstApplicationRun();

    /**
     * @brief setShowStartupPage
     * @param val
     */
    void setShowStartupPage(bool val);

    /**
     * @brief showStartupPage
     * @return
     */
    bool showStartupPage();

    /**
     * @brief setSearchForUpdate
     * @param val
     */
    void setSearchForUpdate(bool val);

    /**
     * @brief searchForUpdate
     * @return
     */
    bool searchForUpdate();

    /**
     * @brief lastProject
     * @return
     */
    QString lastProject();

    /**
     * @brief setLastProject
     * @param val
     */
    void setLastProject(const QString& val);

    /**
     * @brief openLastProject
     * @return
     */
    bool openLastProject();

    /**
     * @brief setOpenLastProject
     * @param val
     */
    void setOpenLastProject(bool val);

    /**
     * @brief setMaxLogLength
     * @param val
     */
    void setMaxLogLength(int val);

    /**
     * @brief maxLogLength
     * @return
     */
    int maxLogLength();

    /**
     * @brief Sets last used process elements.
     * @param list List of last used process element class names.
     */
    void setLastProcessElements(const QStringList& list);

    /**
     * @brief Returns list of last used process elements.
     * @return List of last used process element class names.
     */
    QStringList lastProcessElements();

    /**
     * @brief Stores given expand state list of explorer dock widget.
     * @param list List of expand states.
     */
    void setExplorerExpandStates(const QStringList& list);

    /**
     * @brief Returns expand states of explorer dock widget.
     * @return List including expand states of explorer dock widget.
     */
    QStringList explorerExpandStates();

    /**
     * @brief shortcutsTable
     * @return combination of current and default short cuts as a table
     * The default short cuts are added to the returned map if they are not part of
     * the map from the settings
     */
    QMap<QString, QStringList> shortcutsTable() const;

    /**
     * @brief setShortcutsTable
     * @param shortcutsTable - table to set in the settings
     */
    void setShortcutsTable(QMap<QString, QStringList> shortcutsTable);

    /**
     * @brief initialShortCuts
     * @return the default shortcuts as a variant (QMap<QString, QStringList>).
     * QStringList contains two elements (keysequence and category)
     */
    QVariant initialShortCuts() const;

    /**
     * @brief intialShortCutsMap
     * @return the dafault short cuts as a map. QStringList contains two
     * elements (keysequence and category)
     */
    QMap<QString, QStringList> intialShortCutsMap();

    QString themeMode();

    bool darkMode();

    void setThemeMode(const QString& theme);
protected:
    /**
     * @brief GtSettings
     */
    GtSettings();

private:
    ///
    GtSettingsItem* m_lastSession;

    ///
    GtSettingsItem* m_openSession;

    ///
    GtSettingsItem* m_language;

    ///
    GtSettingsItem* m_firstRun;

    ///
    GtSettingsItem* m_lastPath;

    ///
    GtSettingsItem* m_lastPerspective;

    ///
    GtSettingsItem* m_showStartupPage;

    ///
    GtSettingsItem* m_searchForUpdate;

    ///
    GtSettingsItem* m_lastProject;

    ///
    GtSettingsItem* m_openProject;

    ///
    GtSettingsItem* m_maxLogLength;

    ///
    GtSettingsItem* m_lastProcessElements;

    /// Explorer dock widget expand states setting
    GtSettingsItem* m_explorerExpandStates;

    /// Settings for short cuts in GTlab
    GtSettingsItem* m_shortcutsTable;

    /// Settings for theme selection
    GtSettingsItem* m_themeSelection;
};

#endif // GTSETTINGS_H
