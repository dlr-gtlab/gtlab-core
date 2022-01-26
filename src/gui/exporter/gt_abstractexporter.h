/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractexporter.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTEXPORTER_H
#define GTABSTRACTEXPORTER_H

#include "gt_gui_exports.h"

#include "gt_object.h"

class QFile;
class GtAbstractExporterPage;

/**
 * @brief The GtAbstractExporter class
 */
class GT_GUI_EXPORT GtAbstractExporter : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Returns list of allowed target classes of exporter.
     * @return List of allowed target exporter.
     */
    virtual QStringList targets() const = 0;

    /**
     * @brief Returns icon of exporter.
     * @return Icon of exporter.
     */
    virtual QIcon icon() const = 0;

    /**
     * @brief Main export function.
     * @param Pointer to datamodel member.
     * @param Reference to file selected by user.
     * @return Whether export successfully finished or not.
     */
    virtual bool doExport(GtObject* data, QFile& file) = 0;

    /**
     * @brief Returns file extensions of exporter.
     * @return File extensions of exporter.
     */
    virtual QString extension() const;

    /**
     * @brief Returns wizard pages of exporter based on given object data.
     * @param Object data.
     * @return List of wizard pages.
     */
    virtual QList<GtAbstractExporterPage*> pages(GtObject* data);

protected:
    /**
     * @brief GtAbstractExporter
     */
    GtAbstractExporter();

};

#endif // GTABSTRACTEXPORTER_H
