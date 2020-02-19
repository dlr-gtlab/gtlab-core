/* GTlab - Gas Turbine laboratory
 * Source File: gt_importhandler.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 07.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTIMPORTHANDLER_H
#define GTIMPORTHANDLER_H

#include "gt_abstractobjectfactory.h"

#include "gt_importermetadata.h"

#include <QObject>
#include <QMap>

#define gtImportHandler (GtImportHandler::instance())

class GtAbstractImporter;

/**
 * @brief The GtImportHandler class
 */
class GtImportHandler : public QObject,
        public GtAbstractObjectFactory
{
    friend class GtGuiModuleLoader;

    Q_OBJECT

public:
    /**
     * @brief Returns singleton instance.
     * @return Singleton instance.
     */
    static GtImportHandler* instance();

    /**
     * @brief registerClass
     * @param metaObject
     * @return
     */
    bool registerClass(QMetaObject metaObj) Q_DECL_OVERRIDE;

    /**
     * @brief Returns list of importer which can handle given classname.
     * @param Classname.
     * @return List of importer identification strings.
     */
    QList<GtImporterMetaData> importerMetaData(const QString& classname);

    /**
     * @brief Returns new instance of importer corresponding to given classname.
     * @param Classname.
     * @return New Importer.
     */
    GtAbstractImporter* newImporter(const QString& classname);

private:
    /// Importer meta data map
    QMap<QString, GtImporterMetaData> m_impMeta;

    /**
     * @brief Constructor.
     * @param Parent object.
     */
    GtImportHandler(QObject* parent = NULL);

};

#endif // GTIMPORTHANDLER_H