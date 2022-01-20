/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractimporter.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 07.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTIMPORTER_H
#define GTABSTRACTIMPORTER_H

#include "gt_gui_exports.h"

#include "gt_object.h"

class QFile;
class GtAbstractImporterPage;

/**
 * @brief The GtAbstractImporter class
 */
class GT_GUI_EXPORT GtAbstractImporter : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Returns list of allowed target classes of importer.
     * @return List of allowed target classes.
     */
    virtual QStringList targets() const = 0;

    /**
     * @brief Returns icon of importer.
     * @return Icon of importer.
     */
    virtual QIcon icon() const = 0;

    /**
     * @brief Main import function.
     * @param Pointer to datamodel member.
     * @param Reference to file selected by user.
     * @return Whether import successfully finished or not.
     */
    virtual bool import(GtObject* data, QFile& file) = 0;

    /**
     * @brief Returns list of allowed file extensions of importer.
     * @return List of allowed file extensions of importer.
     */
    virtual QStringList extensions() const;

    /**
     * @brief Returns wizard pages of importer based on given object data.
     * @param Object data.
     * @return List of wizard pages.
     */
    virtual QList<GtAbstractImporterPage*> pages(GtObject* data);

protected:
    /**
     * @brief Constructor.
     */
    GtAbstractImporter();

};

#endif // GTABSTRACTIMPORTER_H
