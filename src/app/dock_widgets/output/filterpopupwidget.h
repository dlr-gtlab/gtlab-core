/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef FILTERPOPUPWIDGET_H
#define FILTERPOPUPWIDGET_H

#include <QWidget>
#include <QSet>
#include <QStringList>
#include <QMap>

class QCheckBox;
class QPushButton;
class QVBoxLayout;

namespace gt
{

/**
 * @brief The FilterPopupWidget class
 * 
 * Popup widget for multi-select filtering with scrollable content.
 * Contains a checkbox list with "Select All/None" buttons and a scrollbar
 * that limits visible items to 5 for compact UI.
 * Changes apply immediately (no apply/cancel buttons).
 * 
 * The widget supports three item formats:
 * - Simple string lists for general filtering
 * - Integer value lists for level-based filtering
 * - String pairs (display, storage) for ID/category filtering
 */
class FilterPopupWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     * 
     * Creates a popup widget with a scrollable checkbox list area
     * and action buttons. The scroll area is configured to display
     * a maximum of 5 items at a time.
     */
    explicit FilterPopupWidget(QWidget* parent = nullptr);

    /**
     * @brief Set filter items
     * @param items List of filter items to display
     * @param selected Set of currently selected items
     * 
     * Sets filter items as a simple string list. Used for general
     * filtering where display and storage values are the same.
     */
    void setItems(const QStringList& items, const QSet<QString>& selected);

    /**
     * @brief Set filter items for integer values (levels)
     * @param items List of item names
     * @param values List of corresponding integer values
     * @param selected Set of currently selected values
     * 
     * Sets filter items for integer-based filtering (e.g., log levels).
     * The items are displayed to users while values are used internally.
     */
    void setItems(const QStringList& items, const QList<int>& values,
                  const QSet<int>& selected);

    /// Get currently selected values (string version)
    QSet<QString> selectedValues() const;

    /// Get currently selected values (int version)
    QSet<int> selectedIntValues() const;

    /// Get currently selected storage values (for string pairs)
    QSet<QString> selectedStorageValues() const;

    /**
     * @brief setItems
     * @param displayItems List of display strings
     * @param storageItems List of corresponding storage strings
     * @param selectedStorageValues Set of currently selected storage values
     * 
     * Sets filter items as string pairs where display items are shown
     * to users but storage values are used internally for filtering.
     * This is used for filtering by log IDs and categories.
     */
    void setItems(const QStringList& displayItems,
                  const QStringList& storageItems,
                  const QSet<QString>& selectedStorageValues);

signals:
    /// Emitted when selection changes (display values)
    void selectionChanged(const QSet<QString>& selected);
    void selectionChangedInt(const QSet<int>& selected);
    void selectionChangedStorage(const QSet<QString>& selectedStorageValues);

private:
    /**
     * @brief createCheckBoxes
     * @param items List of item names to create checkboxes for
     * 
     * Creates checkbox widgets for the given items and connects
     * them to the selection update mechanism.
     */
    void createCheckBoxes(const QStringList& items);

    /**
     * @brief createCheckBoxesForStrings
     * @param displayItems List of display strings
     * @param storageItems List of corresponding storage strings
     * 
     * Creates checkbox widgets for string pairs where display items
     * are shown to users but storage values are used for filtering.
     */
    void createCheckBoxesForStrings(const QStringList& displayItems,
                                    const QStringList& storageItems);

    /**
     * @brief updateSelection
     * 
     * Updates the internal selection state and emits selectionChanged
     * signals when the user modifies checkbox states.
     */
    void updateSelection();

    QList<QCheckBox*> m_checkBoxes;
    QMap<QString, int> m_itemToInt;
    bool m_updating{false};

    QWidget* m_contentWidget{nullptr};
    QVBoxLayout* m_contentLayout{nullptr};

    QMap<QString, QString> m_displayToStorage;
    QMap<QString, QString> m_storageToDisplay;
};
} // namespace gt
#endif // FILTERPOPUPWIDGET_H
