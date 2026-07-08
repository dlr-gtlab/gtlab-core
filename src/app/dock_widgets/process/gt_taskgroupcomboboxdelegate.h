/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GTTASKGROUPCOMBOBOXDELEGATE_H
#define GTTASKGROUPCOMBOBOXDELEGATE_H

#include <QStyledItemDelegate>

class GtTaskGroupModel;

/**
 * @brief The GtTaskGroupComboBoxDelegate class provides a custom delegate for
 * the task group combo box with delete buttons for custom task groups.
 */
class GtTaskGroupComboBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent
     */
    explicit GtTaskGroupComboBoxDelegate(QObject* parent = nullptr);

    /**
     * @brief Creates a widget for editing
     */
    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;

    /**
     * @brief Sets the size of the editor
     */
    void setEditorData(QWidget* editor,
                       const QModelIndex& index) const override;

    /**
     * @brief Updates model data from editor
     */
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override;

    /**
     * @brief Paints the item
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

    /**
     * @brief Handles editor events
     */
    bool editorEvent(QEvent* event,
                     QAbstractItemModel* model,
                     const QStyleOptionViewItem& option,
                     const QModelIndex& index) override;

    /**
     * @brief Returns the size hint for the item
     */
    QSize sizeHint(const QStyleOptionViewItem& option,
                   const QModelIndex& index) const override;

signals:
    /**
     * @brief Emitted when delete button is clicked
     */
    void deleteRequested(const QModelIndex& index);
};

#endif // GTTASKGROUPCOMBOBOXDELEGATE_H