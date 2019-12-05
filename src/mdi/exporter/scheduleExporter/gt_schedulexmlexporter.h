/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2019 by DLR
 *
 *  Created on: 01.08.2019
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 2579
 */
#ifndef GTP_EXPRESSIONXMLEXPORTER_H
#define GTP_EXPRESSIONXMLEXPORTER_H

#include <QFile>

#include "gt_abstractexporter.h"

class GtControlSchedule;

/**
 * @brief The GtpScheduleXmlExporter class
 */
class GtpScheduleXmlExporter : public GtAbstractExporter
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    Q_INVOKABLE GtpScheduleXmlExporter();

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

    /**
     * @brief writeData
     * @param data
     * @param file
     * @return
     */
    bool writeData(GtControlSchedule* data, QFile& file);
};

#endif // GTP_EXPRESSIONXMLEXPORTER_H
