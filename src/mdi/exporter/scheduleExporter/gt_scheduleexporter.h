/* GTlab - Gas Turbine laboratory
 * Source File: gt_scheduleexporter.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 15.03.2018
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GTSCHEDULEEXPORTER_H
#define GTSCHEDULEEXPORTER_H

#include <QFile>

#include "gt_abstractexporter.h"

class GtControlSchedule;

/**
 * @brief The GtScheduleExporter class
 */
class GtScheduleExporter : public GtAbstractExporter
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    Q_INVOKABLE GtScheduleExporter();

    /**
     * @brief Returns list of allowed target classes of exporter.
     * @return List of allowed target exporter.
     */
    QStringList targets() const Q_DECL_OVERRIDE;

    /**
     * @brief Returns icon of exporter.
     * @return Icon of exporter.
     */
    QIcon icon() const Q_DECL_OVERRIDE;

    /**
     * @brief Main export function.
     * @param Pointer to datamodel member.
     * @param Reference to file selected by user.
     * @return Whether export successfully finished or not.
     */
    bool doExport(GtObject* data, QFile& file) Q_DECL_OVERRIDE;

    /**
     * @brief Returns file extensions of exporter.
     * @return File extensions of exporter.
     */
    QString extension() const Q_DECL_OVERRIDE;

    /**
     * @brief Returns wizard pages of importer based on given object data.
     * @param Object data.
     * @return List of wizard pages.
     */
    QList<GtAbstractExporterPage*> pages(GtObject* data) Q_DECL_OVERRIDE;

private:
    bool writeData(GtControlSchedule* data, QFile& file);

};

#endif // GTSCHEDULEEXPORTER_H
