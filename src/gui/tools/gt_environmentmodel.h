/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_environmentmodel.h
 *
 *  Created on: 05.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ENVIRONMENTMODEL_H
#define GT_ENVIRONMENTMODEL_H

#include "gt_gui_exports.h"

#include <QAbstractTableModel>

#include <QMap>

/**
 * @brief The GtEnvironmentModel class
 */
class GT_GUI_EXPORT GtEnvironmentModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param vars Environment variable identification strings
     * @param parent Parent object
     */
    explicit GtEnvironmentModel(const QStringList &vars, QObject *parent = nullptr);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex& parent) const override;

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex& parent) const override;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const override;

    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role) override;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const override;

    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * @brief Saves environment variables.
     */
    void saveVariables();

private:
    /// List of environment variables
    QMap<QString, QVariant> m_vars;

    /**
     * @brief Initialization.
     * @param vars Environment variable identification strings.
     */
    void init(const QStringList& vars);

};

#endif // GT_ENVIRONMENTMODEL_H
