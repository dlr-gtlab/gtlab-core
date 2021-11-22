/* GTlab - Gas Turbine laboratory
 * Source File: gt_datazonedatexporter.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 26.11.2018
 *      Author: Carsten Hollmann (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GT_DATAZONEDATEXPORTER_H
#define GT_DATAZONEDATEXPORTER_H

#include <QFile>
#include <QTextStream>

#include "gt_abstractexporter.h"

class GtDataZone;
class GtDataZone0D;

class GtDataZoneDatExporter : public GtAbstractExporter
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    Q_INVOKABLE GtDataZoneDatExporter();

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
     * @brief writing of 0-dimensional tables to a .csv file
     * @param data data zone for export
     * @param file file to write in
     * @return
     */
    bool write0Ddata(GtDataZone0D* data, QFile& file);

    /**
     * @brief writing of multidimensional tables to a .dat file
     * dependent on the dimension the following helper functions will be used
     * @param data data zone for export
     * @param file file to write in
     * @return
     */
    bool writeMultiDimData(GtDataZone* data, QFile& file);

    /**
     * @brief writing of 1D tables to a .dat file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write1Ddata(GtDataZone* data, QTextStream& out);

    /**
     * @brief writing of 2D tables to a .dat file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write2Ddata(GtDataZone* data, QTextStream& out);

    /**
     * @brief writing of 3D tables to a .dat file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write3Ddata(GtDataZone* data, QTextStream& out);

    /**
     * @brief writing of 4D tables to a .dat file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write4Ddata(GtDataZone* data, QTextStream& out);

    /**
     * @brief writeHeaderLine
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool writeHeaderLine0D(QTextStream& out, GtDataZone0D* dataZone);

    /**
     * @brief writing the appropriate tecplot header to the file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool writeHeaderLine(QTextStream& out, GtDataZone* dataZone);
};

#endif // GT_DATAZONEDATEXPORTER_H
