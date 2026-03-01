/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 */

#ifndef GT_MODULEDIRECTORIESTAB_H
#define GT_MODULEDIRECTORIESTAB_H

#include <QWidget>
#include <QStringList>

namespace Ui
{
    class GtModuleDirectoriesTab;
}

class QListWidgetItem;

/**
 * @brief Widget to manage module search directories for GTlab.
 *
 * The list shows two fixed, informational entries at the bottom (default
 * install path and default user path). All preceding entries are user-defined
 * and can be added/removed/edited. User entries are checkable to enable/disable
 * them.
 *
 * @note The last two rows are non-interactive and never returned by userPaths().
 */
class GtModuleDirectoriesTab : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Construct the widget.
     * @param parent Optional parent QWidget.
     */
    explicit GtModuleDirectoriesTab(QWidget* parent = nullptr);

    /**
     * @brief Destructor.
     */
    ~GtModuleDirectoriesTab() override;

    /**
     * @brief Set the default (built-in) install modules path.
     *
     * Shown as a fixed row near the bottom, greyed out and non-interactive.
     * This is a visual hint only and is never included in userPaths().
     *
     * @param path Absolute or relative path; will be normalized for display.
     */
    void setDefaultInstallPath(const QString& path);

    /**
     * @brief Set the default user modules path.
     *
     * Shown as a fixed row at the bottom, greyed out and non-interactive.
     * This is a visual hint only and is never included in userPaths().
     *
     * @param path Absolute or relative path; will be normalized for display.
     */
    void setDefaultUserPath(const QString& path);

    /**
     * @brief Return only enabled, user-defined module directories.
     *
     * The last two fixed rows are excluded. Unchecked (disabled) user entries
     * are also excluded.
     *
     * @return QStringList of normalized, enabled user paths.
     */
    QStringList userPaths() const;

    /**
     * @brief Replace all user-defined module directories (enabled by default).
     *
     * Clears existing user entries (all rows except the last two) and appends
     * the given paths as editable, checkable items (checked). Paths are
     * normalized and deduplicated.
     *
     * @param paths List of paths to set as user entries.
     */
    void setUserPaths(const QStringList& paths);

signals:
    /**
     * @brief Emitted whenever the set of enabled user paths changes.
     *
     * Emitted after add/remove, edit, check/uncheck, or bulk updates.
     *
     * @param paths The current enabled user paths (same as userPaths()).
     */
    void userPathsChanged(const QStringList& paths);

private slots:
    /**
     * @brief Open a directory chooser and append a new user entry.
     */
    void onAddDirectory();

    /**
     * @brief Remove the currently selected user entries (rows above headers).
     */
    void onRemoveDirectory();

    /**
     * @brief Move the selected user entries up by one row (user rows only).
     */
    void onMoveUp();

    /**
     * @brief Move the selected user entries down by one row (user rows only).
     */
    void onMoveDown();

    /**
     * @brief Keep UI state (e.g., remove button enabled) in sync with selection.
     */
    void onSelectionChanged();

    /**
     * @brief React to edits or checkbox toggles on list items.
     *
     * Restores header rows, normalizes paths, deduplicates, and updates styling.
     *
     * @param item The item that changed.
     */
    void onItemChanged(QListWidgetItem* item);

    /**
     * @brief Show a context menu for the list at the given position.
     *
     * Provides actions like "Show in Explorer" and "Rename / Edit Path".
     *
     * @param pos Position in viewport coordinates.
     */
    void onListContextMenu(const QPoint& pos);

    /**
     * @brief Open the system file manager at the selected entry's path.
     */
    void onShowInExplorer();

    /**
     * @brief Start inline editing of the currently selected user entry.
     */
    void onEditItemInline();

    /**
     * @brief Replace the currently selected user entry via a directory chooser.
     */
    void onChangeToDirectory();

private:
    /**
     * @brief Apply visual appearance to a user item.
     *
     * Sets strike-through/gray when unchecked (disabled), normal style when checked.
     *
     * @param item Target list item (must be a user row, i.e., row >= 2).
     */
    void applyUserItemAppearance(QListWidgetItem* item);

    /**
     * @brief Open a directory in the system file manager.
     * @param path Target path (file or directory). Files open their parent dir.
     * @return true on successful launch, false otherwise.
     */
    bool openInFileManager(const QString& path);

    /**
     * @brief Return the currently selected user item or nullptr if selection is invalid.
     */
    QListWidgetItem* currentUserItem() const;

    /**
     * @brief Ensure the two fixed header rows exist and are styled/locked.
     */
    void ensureHeaderItems();

    /**
     * @brief Set text and tooltip for one of the two header rows.
     *
     * @param row install path row or user path row (last two rows).
     * @param text Display text (path).
     * @param about Tooltip/help text explaining the row.
     */
    void setHeaderText(int row, const QString& text, const QString& about);

    /**
     * @brief Whether a given row index is one of the two fixed headers.
     */
    bool isHeaderRow(int row) const;

    /**
     * @brief Enable/disable the Remove button based on current selection.
     */
    void updateRemoveButton();

    /**
     * @brief Enable/disable move buttons based on current selection.
     */
    void updateMoveButtons();

    //! Auto-generated UI.
    Ui::GtModuleDirectoriesTab* ui;

    //! Display text for the default install modules path (row 0).
    QString m_defaultInstallPath;

    //! Display text for the default user modules path (row 1).
    QString m_defaultUserPath;

    //! Prevent re-entrant signal handling during bulk updates.
    bool m_blockSignals = false;
};

#endif // GT_MODULEDIRECTORIESTAB_H
