/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_labeldelegate.h
 *
 *  Created on: 22.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLABELDELEGATE_H
#define GTLABELDELEGATE_H

#include <QStyledItemDelegate>

class GtLabelsDock;

/**
 * @brief The GtLabelDelegate class
 */
class GtLabelDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief GtLabelDelegate
     * @param parent
     */
    explicit GtLabelDelegate(GtLabelsDock* parent);

    /**
     * @brief createEditor
     * @param parent
     * @param option
     * @param index
     * @return
     */
    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const override;

    /**
     * @brief setEditorData
     * @param editor
     * @param index
     */
    void setEditorData(QWidget* editor,
                       const QModelIndex& index) const override;

    /**
     * @brief setModelData
     * @param editor
     * @param model
     * @param index
     */
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const override;

private:
    /// Label dock widget.
    GtLabelsDock* m_dock;

};

#endif // GTLABELDELEGATE_H
