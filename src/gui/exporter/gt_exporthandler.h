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
#include "gt_version.h"

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
    GT_NO_DISCARD
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
    GT_NO_DISCARD
    QList<GtExporterMetaData> exporterMetaData(const QString& className);

    /**
     * @brief Returns new instance of importer corresponding to given class
     * name.
     * @param className Class name to instantiate.
     * @return New Exporter.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2,
                             "use `makeExporter` or `makeObject<T>` instead")
    GtAbstractExporter* newExporter(const QString& className);

    std::unique_ptr<GtAbstractExporter> makeExporter(const QString& className);

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
