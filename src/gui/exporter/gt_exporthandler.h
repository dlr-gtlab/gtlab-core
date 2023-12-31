/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_exporthandler.h
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXPORTHANDLER_H
#define GTEXPORTHANDLER_H

#include "gt_abstractobjectfactory.h"

#include "gt_exportermetadata.h"
#include "gt_gui_exports.h"
#include <QObject>
#include <QMap>

#define gtExportHandler (GtExportHandler::instance())

class GtAbstractExporter;

/**
 * @brief The GtExportHandler class
 */
class GT_GUI_EXPORT GtExportHandler : public QObject,
        public GtAbstractObjectFactory
{
    friend class GtGuiModuleLoader;

    Q_OBJECT

public:/**
     * @brief Returns singleton instance.
     * @return Singleton instance.
     */
    static GtExportHandler* instance();

    /**
     * @brief registerClass
     * @param metaObject
     * @return
     */
    bool registerClass(QMetaObject metaObj) override;

    /**
     * @brief Returns list of importer which can handle given classname.
     * @param Classname.
     * @return List of importer identification strings.
     */
    QList<GtExporterMetaData> exporterMetaData(const QString& classname);

    /**
     * @brief Returns new instance of importer corresponding to given classname.
     * @param Classname.
     * @return New Importer.
     */
    GtAbstractExporter* newExporter(const QString& classname);

private:
    /// Importer meta data map
    QMap<QString, GtExporterMetaData> m_expMeta;

    /**
     * @brief GtExportHandler
     * @param parent
     */
    explicit GtExportHandler(QObject* parent = nullptr);

};

#endif // GTEXPORTHANDLER_H
