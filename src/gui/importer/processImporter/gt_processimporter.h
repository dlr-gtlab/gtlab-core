/* GTlab - Gas Turbine laboratory
 * Source File: gt_processimporter.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 04.05.2020
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */
#ifndef GTPROCESSIMPORTER_H
#define GTPROCESSIMPORTER_H

#include "gt_abstractimporter.h"

/**
 * @brief The GtProcessImporter class
 * Importer Process elements
 */
class GtProcessImporter : public GtAbstractImporter
{
    Q_OBJECT
public:
    /**
     * @brief GtProcessImporter
     */
    Q_INVOKABLE GtProcessImporter();


    /**
     * @brief Returns list of allowed target classes of importer.
     * @return List of allowed target classes.
     */
    QStringList targets() const override;

    /**
     * @brief Returns icon of importer.
     * @return Icon of importer.
     */
    QIcon icon() const override;

    /**
     * @brief Main import function.
     * @param Pointer to datamodel member.
     * @param Reference to file selected by user.
     * @note The imported object and all its children get new uuids
     * to avoid doubled usage
     * @return Whether import successfully finished or not.
     */
    bool import(GtObject* data, QFile& file) override;

    /**
     * @brief Returns list of allowed file extensions of importer.
     * @return List of allowed file extensions of importer.
     */
    QStringList extensions() const override;
};

#endif // GTPROCESSIMPORTER_H
