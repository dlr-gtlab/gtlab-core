/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_processcomponentmodel.h
 *
 *  Created on: 05.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTPROCESSCOMPONENTMODEL_H
#define GTPROCESSCOMPONENTMODEL_H

#include "gt_gui_exports.h"
#include "gt_styledmodel.h"
#include "gt_processcomponent.h"

class GtObject;

/**
 * @brief The GtProcessComponentModel class
 */
class GT_GUI_EXPORT GtProcessComponentModel : public GtStyledModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param parent Parent object.
     */
    explicit GtProcessComponentModel(QObject* parent = nullptr);

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex& index,
                  int role = Qt::DisplayRole) const override;

    /**
     * @brief Returns process component state specific icon.
     * @param state Process component state.
     * @return Proccess componentn state specific icon.
     */
    static QIcon stateToIcon(GtProcessComponent::STATE state);

    /**
     * @brief Returns process component state specific icon.
     * @param state Process component state.
     * @param progress Progress of the current running processelement
     * @return Proccess componentn state specific icon.
     */
    static QIcon stateToIcon(GtProcessComponent::STATE state, int progress);

    /**
     * @brief Returns process component state specific string.
     * @param state Process component state.
     * @param progress Progress of the current running processelement
     * @return Proccess componentn state specific string.
     */
    static QString stateToString(GtProcessComponent::STATE state, int progress);

protected:
    /**
     * @brief moveRows
     * @param sourceParent
     * @param sourceRow
     * @param count
     * @param destinationParent
     * @param destinationChild
     * @return
     */
    bool moveRows(const QModelIndex& sourceParent, int sourceRow,
                  int count, const QModelIndex& destinationParent,
                  int destinationChild) override;

    /**
     * @brief canDropMimeData
     * @param data
     * @param action
     * @param row
     * @param column
     * @param parent
     * @return
     */
    bool canDropMimeData(const QMimeData* data, Qt::DropAction action,
                         int row, int column,
                         const QModelIndex& parent) const override;

    /**
     * @brief dropMimeData
     * @param mimeData
     * @param action
     * @param row
     * @param column
     * @param destinationIndex
     * @return
     */
    bool dropMimeData(const QMimeData* mimeData, Qt::DropAction action,
                      int row, int column,
                      const QModelIndex& destinationIndex) override;

    /**
     * @brief Returns the item flags for the given index.
     * @param Model index
     * @return Item Flags
     */
    Qt::ItemFlags flags(const QModelIndex& index) const override;

private:
    /**
     * @brief toolTip
     * @param obj
     * @return
     */
    QString toolTip(GtObject* obj) const;

    /**
     * @brief objectLinkToolTip
     * @param id
     * @param obj
     * @return
     */
    QString objectLinkToolTip(const QString& id, GtObject* obj,
                              bool first = false) const;

};

#endif // GTPROCESSCOMPONENTMODEL_H
