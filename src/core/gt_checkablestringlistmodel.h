/* GTlab - Gas Turbine laboratory
 * Source File: gt_checkablestringlistmodel.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 03.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTCHECKABLESTRINGLISTMODEL_H
#define GTCHECKABLESTRINGLISTMODEL_H

#include "gt_core_exports.h"

#include <QAbstractListModel>

typedef QPair<QString, Qt::CheckState> StringBoolPair;

/**
 * @brief The GtCheckableStringListModel class
 */
class GT_CORE_EXPORT GtCheckableStringListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum SelectionModes {
        MultipleSelection,
        SingleSelection
    };

    /**
     * @brief GtCheckableStringListModel
     * @param strings
     * @param parent
     */
    explicit GtCheckableStringListModel(const QStringList& strings,
                                        QObject* parent = nullptr);

    /**
     * @brief GtCheckableStringListModel
     * @param parent
     */
    explicit GtCheckableStringListModel(QObject* parent = nullptr);

    /**
     * @brief stringList
     * @return
     */
    QStringList stringList() const;

    /**
     * @brief setStringList
     * @param strings
     */
    void setStringList(const QStringList &strings,
                       Qt::CheckState state = Qt::Checked);

    /**
     * @brief setSelectionMode
     * @param mode
     */
    void setSelectionMode(GtCheckableStringListModel::SelectionModes mode);

    /**
     * @brief selectionMode
     * @return
     */
    GtCheckableStringListModel::SelectionModes selectionMode();

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex& parent =
            QModelIndex()) const override;

    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex& index,
                 const QVariant& value,
                 int role = Qt::EditRole) override;

    /**
     * @brief selectItems
     * @param strings
     */
    void selectItems(const QStringList& strings);

    /**
     * @brief selectItem
     * @param string
     */
    void selectItem(const QString& string);

    /**
     * @brief clearSelection
     */
    void clearSelection();

    /**
     * @brief selectAll
     */
    void selectAll();

    /**
     * @brief selectedStringList
     * @return
     */
    QStringList selectedStringList();

    /**
     * @brief insertRows
     * @param row
     * @param count
     * @param parent
     * @return
     */
    bool insertRows(int row, int count,
                    const QModelIndex& parent = QModelIndex()) override;

    /**
     * @brief removeRows
     * @param row
     * @param count
     * @param parent
     * @return
     */
    bool removeRows(int row, int count,
                    const QModelIndex& parent = QModelIndex()) override;

    /**
     * @brief supportedDropActions
     * @return
     */
    Qt::DropActions supportedDropActions() const override;

private:
    /// Model data
    QList<StringBoolPair> m_data;

    /// Selection mode
    SelectionModes m_mode;

private slots:
    /**
     * @brief onDataChanged
     */
    void onDataChanged();

signals:
    /**
     * @brief noItemSelected
     */
    void noItemSelected(bool);

    void selectionChanged();

};

#endif // GTCHECKABLESTRINGLISTMODEL_H
