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

/**
 * @brief The FilterPopupWidget class
 * 
 * Simple popup widget for multi-select filtering.
 * Contains checkbox list with "Select All/None" buttons.
 * Changes apply immediately (no apply/cancel buttons).
 */
class FilterPopupWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent widget
     */
    explicit FilterPopupWidget(QWidget* parent = nullptr);

    /**
     * @brief Set filter items
     * @param items List of filter items to display
     * @param selected Set of currently selected items
     */
    void setItems(const QStringList& items, const QSet<QString>& selected);

    /**
     * @brief Set filter items for integer values (levels)
     * @param items List of item names
     * @param values List of corresponding integer values
     * @param selected Set of currently selected values
     */
    void setItems(const QStringList& items, const QList<int>& values,
                  const QSet<int>& selected);

    /// Get currently selected values (string version)
    QSet<QString> selectedValues() const;

    /// Get currently selected values (int version)
    QSet<int> selectedIntValues() const;

signals:
    /// Emitted when selection changes
    void selectionChanged(const QSet<QString>& selected);
    void selectionChangedInt(const QSet<int>& selected);

private:
    void createCheckBoxes(const QStringList& items);
    void updateSelection();

    QList<QCheckBox*> m_checkBoxes;
    QMap<QString, int> m_itemToInt;
    bool m_updating{false};

    QWidget* m_contentWidget{nullptr};
    QVBoxLayout* m_contentLayout{nullptr};
};

#endif // FILTERPOPUPWIDGET_H
