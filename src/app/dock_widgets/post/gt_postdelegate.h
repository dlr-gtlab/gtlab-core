/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 18.05.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */
#ifndef GTPOSTDELEGATE_H
#define GTPOSTDELEGATE_H

#include <QStyledItemDelegate>

class GtPostDock;


/**
 * @brief The GtPostDelegate class
 */
class GtPostDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief GtPostDelegate
     * @param parent
     */
    explicit GtPostDelegate(GtPostDock* parent);

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
    /// Label dock widget
    GtPostDock* m_dock;
};

#endif // GTPOSTDELEGATE_H
