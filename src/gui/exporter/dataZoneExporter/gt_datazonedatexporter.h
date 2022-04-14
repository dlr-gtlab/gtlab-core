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

#include "gt_gui_exports.h"

#include "gt_abstractexporter.h"

class QFile;
class QTextStream;
class GtDataZone0D;
class GtDataZone0DData;
class GtDataZone;
class GtDataZoneData;
/**
 * @brief The GtDataZoneDatExporter class
 * Exporter to write the content of GtDataZones to .dat-files
 *
 * Currently its working for up to 4 dimensional datazones
 */
class GT_GUI_EXPORT GtDataZoneDatExporter : public GtAbstractExporter
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
    QStringList targets() const override;

    /**
     * @brief Returns icon of exporter.
     * @return Icon of exporter.
     */
    QIcon icon() const override;

    /**
     * @brief Main export function.
     * @param object pointer to datamodel member.
     * @param file Reference to file selected by user.
     * @return Whether export successfully finished or not.
     */
    bool doExport(GtObject* object, QFile& file) override;

    /**
     * @brief Returns file extensions of exporter.
     * @return File extensions of exporter.
     */
    QString extension() const override;

    /**
     * @brief Returns wizard pages of importer based on given object data.
     * @param data data.
     * @return List of wizard pages.
     */
    QList<GtAbstractExporterPage*> pages(GtObject* data) override;

private:
    /**
     * @brief writing of 0-dimensional tables to a .csv file
     * @param dataZone data zone for export
     * @param file file to write in
     * @return
     */
    bool write0Ddata(GtDataZone0D& dataZone, QFile& file);

    /**
     * @brief writing of multidimensional tables to a .dat file
     * dependent on the dimension the following helper functions will be used
     * @param dataZone data zone for export
     * @param file file to write in
     * @return
     */
    bool writeMultiDimData(GtDataZone& dataZone, QFile& file);

    /**
     * @brief writing of 1D tables to a .dat file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write1Ddata(const GtDataZone& dataZone,
                     const GtDataZoneData& data,
                     QTextStream& out);

    /**
     * @brief writing of 2D tables to a .dat file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write2Ddata(const GtDataZone& dataZone,
                     const GtDataZoneData& data,
                     QTextStream& out);

    /**
     * @brief writing of 3D tables to a .dat file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write3Ddata(const GtDataZone& dataZone,
                     const GtDataZoneData& data,
                     QTextStream& out);

    /**
     * @brief writing of 4D tables to a .dat file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write4Ddata(const GtDataZone& dataZone,
                     const GtDataZoneData& data,
                     QTextStream& out);

    /**
     * @brief writeHeaderLine
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool writeHeaderLine0D(const GtDataZone0D& dataZone,
                           const GtDataZone0DData& data,
                           QTextStream& out);

    /**
     * @brief writing the appropriate tecplot header to the file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool writeHeaderLine(const GtDataZone& dataZone,
                         const GtDataZoneData& data,
                         QTextStream& out);
};

#endif // GT_DATAZONEDATEXPORTER_H
