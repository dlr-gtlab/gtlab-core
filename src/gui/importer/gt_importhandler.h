/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_importhandler.h
 *
 *  Created on: 07.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTIMPORTHANDLER_H
#define GTIMPORTHANDLER_H

#include "gt_abstractobjectfactory.h"
#include "gt_version.h"

#include "gt_importermetadata.h"
#include "gt_gui_exports.h"

#include <QObject>
#include <QMap>

#define gtImportHandler (GtImportHandler::instance())

class GtAbstractImporter;

/**
 * @brief The GtImportHandler class
 */
class GT_GUI_EXPORT GtImportHandler : public QObject,
        public GtAbstractObjectFactory
{
    friend class GtGuiModuleLoader;

    Q_OBJECT

public:
    /**
     * @brief Returns singleton instance.
     * @return Singleton instance.
     */
    GT_NO_DISCARD
    static GtImportHandler* instance();

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
    QList<GtImporterMetaData> importerMetaData(const QString& classname);

    /**
     * @brief Returns new instance of importer corresponding to given class
     * name.
     * @param className Class name to instantiate.
     * @return New Importer.
     */
    GT_DEPRECATED_REMOVED_IN(2, 2,
                             "use `makeImporter` or `makeObject<T>` instead")
    GtAbstractImporter* newImporter(const QString& className);

    GT_NO_DISCARD
    std::unique_ptr<GtAbstractImporter> makeImporter(const QString& className);

private:
    /// Importer meta data map
    QMap<QString, GtImporterMetaData> m_impMeta;

    /**
     * @brief Constructor.
     * @param Parent object.
     */
    explicit GtImportHandler(QObject* parent = nullptr);

};

#endif // GTIMPORTHANDLER_H
