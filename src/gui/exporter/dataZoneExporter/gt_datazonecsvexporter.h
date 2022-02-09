/* GTlab - Gas Turbine laboratory
 * Source File: gt_datazonecsvexporter.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 06.11.2018
 *      Author: Carsten Hollmann (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#ifndef GT_DATAZONECSVEXPORTER_H
#define GT_DATAZONECSVEXPORTER_H

#include "gt_gui_exports.h"

#include <QFile>
#include <QTextStream>

#include "gt_abstractexporter.h"

class GtAbstractDataZone;
class GtDataZone;
class GtDataZone0D;

/**
 * @brief The GtDataZoneCsvExporter class
 * Exporter to write the content of GtDataZones to .csv-files
 *
 * Currently its working for up to 4 dimensional datazones
 */
class GT_GUI_EXPORT GtDataZoneCsvExporter : public GtAbstractExporter
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     */
    Q_INVOKABLE GtDataZoneCsvExporter();

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
     * @param Pointer to datamodel member.
     * @param Reference to file selected by user.
     * @return Whether export successfully finished or not.
     */
    bool doExport(GtObject* data, QFile& file) override;

    /**
     * @brief Returns file extensions of exporter.
     * @return File extensions of exporter.
     */
    QString extension() const override;

    /**
     * @brief Returns wizard pages of importer based on given object data.
     * @param Object data.
     * @return List of wizard pages.
     */
    QList<GtAbstractExporterPage*> pages(GtObject* data) override;

    /**
     * @brief writing of multidimensional tables to a .csv file
     * dependent on the dimension the following helper functions will be used
     * @param data data zone for export
     * @param file file to write in
     * @return
     */
    bool writeMultiDimData(GtDataZone* data, QFile& file);

private:
    /**
     * @brief writing of 0-dimensional tables to a .csv file
     * @param data data zone for export
     * @param file file to write in
     * @return
     */
    bool write0Ddata(GtDataZone0D* data, QFile& file);

    /**
     * @brief writing of 1D tables to a .csv file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write1Ddata(GtDataZone* data, QTextStream& out);

    /**
     * @brief writing of 2D tables to a .csv file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write2Ddata(GtDataZone* data, QTextStream& out);

    /**
     * @brief writing of 3D tables to a .csv file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write3Ddata(GtDataZone* data, QTextStream& out);

    /**
     * @brief writing of 4D tables to a .csv file
     * @param out textstream to output file
     * @param data data zone chosen for export
     * @return
     */
    bool write4Ddata(GtDataZone* data, QTextStream& out);

    /**
     * @brief writing the appropriate header to the file
     * @param out textstream to output file
     * @param dataZone data zone chosen for export
     * @return
     */
    bool writeHeaderLine(QTextStream& out, GtAbstractDataZone* dataZone);

    /**
     * @brief Helper function to write the CSV headers
     * @param out out textstream to output file
     * @param p parameter name
     * @param v
     */
    void writeHeaderLineHelper(QTextStream& out, const QString& p,
                               const QString& v, int nParams);
};

#endif // GT_DATAZONECSVEXPORTER_H
