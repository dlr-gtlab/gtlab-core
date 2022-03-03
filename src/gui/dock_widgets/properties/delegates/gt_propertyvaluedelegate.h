/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPROPERTYVALUEDELEGATE_H
#define GTPROPERTYVALUEDELEGATE_H

#include <QStyledItemDelegate>
#include <QPointer>

class GtPropertyTreeView;
class GtProject;

class GtPropertyValueDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    explicit GtPropertyValueDelegate(GtPropertyTreeView* parent = nullptr);

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

    /**
     * @brief displayText
     * @param value
     * @param locale
     * @return
     */
    QString displayText(const QVariant& value,
                        const QLocale& locale) const override;

    /**
     * @brief updateEditorGeometry
     * @param editor
     * @param option
     * @param index
     */
//    void updateEditorGeometry(QWidget* editor,
//                              const QStyleOptionViewItem& option,
//                              const QModelIndex& index) const override;

    /**
     * @brief paint
     * @param painter
     * @param option
     * @param index
     */
    void paint(QPainter* painter,
               const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;

private:
    /// Property tree view.
    GtPropertyTreeView* m_view;

private slots:
    /**
     * @brief onDataChange
     */
    void onDataChange();

};

#endif // GTPROPERTYVALUEDELEGATE_H
