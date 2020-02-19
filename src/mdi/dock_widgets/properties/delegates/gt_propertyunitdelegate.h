/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 27.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYUNITDELEGATE_H
#define GTPROPERTYUNITDELEGATE_H

#include <QStyledItemDelegate>

class QComboBox;

/**
 * @brief The GtPropertyUnitDelegate class
 */
class GtPropertyUnitDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    GtPropertyUnitDelegate(QObject* parent = Q_NULLPTR);

    /**
     * @brief createEditor
     * @param parent
     * @param option
     * @param index
     * @return
     */
    QWidget* createEditor(QWidget* parent,
                          const QStyleOptionViewItem& option,
                          const QModelIndex& index) const Q_DECL_OVERRIDE;

    /**
     * @brief setEditorData
     * @param editor
     * @param index
     */
    void setEditorData(QWidget* editor,
                       const QModelIndex& index) const Q_DECL_OVERRIDE;

    /**
     * @brief setModelData
     * @param editor
     * @param model
     * @param index
     */
    void setModelData(QWidget* editor,
                      QAbstractItemModel* model,
                      const QModelIndex& index) const Q_DECL_OVERRIDE;

    /**
     * @brief updateEditorGeometry
     * @param editor
     * @param option
     * @param index
     */
    void updateEditorGeometry(QWidget* editor,
                              const QStyleOptionViewItem& option,
                              const QModelIndex&) const Q_DECL_OVERRIDE;

    /**
     * @brief paint
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const Q_DECL_OVERRIDE;

private:
    /// Combobox
    mutable QComboBox* m_comboBox;

    mutable int m_width;

    mutable int m_heigth;

private slots:
    /**
     * @brief Commits data to the view.
     * @param Current index.
     */
    void setData(int val);

};

#endif // GTPROPERTYUNITDELEGATE_H