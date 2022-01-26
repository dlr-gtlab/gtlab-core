/* GTlab - Gas Turbine laboratory
 * Source File: gt_labeldata.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 20.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTLABELDATA_H
#define GTLABELDATA_H

#include "gt_datamodel_exports.h"
#include "gt_objectgroup.h"

class GtLabel;

/**
 * @brief The GtLabelData class
 */
class GT_DATAMODEL_EXPORT GtLabelData : public GtObjectGroup
{
    Q_OBJECT

public:
    /**
     * @brief GtLabelData
     */
    Q_INVOKABLE GtLabelData();

    /**
     * @brief Returns a list of all label identification strings.
     * @return List of label identification strings.
     */
    QStringList labelIds() const;

    /**
     * @brief Returns whether label data is empty or not.
     * @return Whether label data is empty or not.
     */
    bool isEmpty() const;

    /**
     * @brief Adds a new label to project.
     * @param Identification string of new label.
     * @return Whether label could be added or not.
     */
    bool addLabel(const QString& id);

    /**
     * @brief addLabel
     * @param label
     * @return
     */
    bool addLabel(GtLabel* label);

    /**
     * @brief Returns list of all label items.
     * @return Label items.
     */
    QList<GtLabel*> labels();

    /**
     * @brief Adds a default label if label data is empty.
     * @return Whether default label could be added.
     */
    bool addDefaultLabel();

    /**
     * @brief Checks the existance of a label for given identification string.
     * @param Label identification string.
     * @return Whether label was found or not.
     */
    bool labelExists(const QString& id) const;

    /**
     * @brief Renames a given label.
     * @param Existing label identification string.
     * @param New label identification string.
     * @return Whether label was renamed or not.
     */
    bool renameLabel(const QString& oldId, const QString& newId);

};

#endif // GTLABELDATA_H
