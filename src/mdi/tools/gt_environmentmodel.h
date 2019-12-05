/* GTlab - Gas Turbine laboratory
 * Source File: gt_environmentmodel.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 05.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_ENVIRONMENTMODEL_H
#define GT_ENVIRONMENTMODEL_H

#include "gt_mdi_exports.h"

#include <QAbstractTableModel>

#include <QMap>

/**
 * @brief The GtEnvironmentModel class
 */
class GT_MDI_EXPORT GtEnvironmentModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param vars Environment variable identification strings
     * @param parent Parent object
     */
    GtEnvironmentModel(const QStringList& vars, QObject* parent = Q_NULLPTR);

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;

    /**
     * @brief columnCount
     * @param parent
     * @return
     */
    int columnCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    /**
     * @brief setData
     * @param index
     * @param value
     * @param role
     * @return
     */
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role) Q_DECL_OVERRIDE;

    /**
     * @brief headerData
     * @param section
     * @param orientation
     * @param role
     * @return
     */
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role) const Q_DECL_OVERRIDE;

    /**
     * @brief flags
     * @param index
     * @return
     */
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

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
