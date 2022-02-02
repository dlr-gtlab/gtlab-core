/* GTlab - Gas Turbine laboratory
 * Source File: gt_importermetadata.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTIMPORTERMETADATA_H
#define GTIMPORTERMETADATA_H

#include <QString>
#include <QStringList>
#include <QMetaObject>
#include <QIcon>

/**
 * @brief The GtImporterMetaData class
 */
class GtImporterMetaData
{
public:
    /**
     * @brief Default constructor.
     */
    GtImporterMetaData();

    /**
     * @brief Constructor.
     * @param Meta object of importer.
     */
    explicit GtImporterMetaData(QMetaObject& importer);

    /**
     * @brief Returns classname of importer.
     * @return Classname.
     */
    const QString& classname() const;

    /**
     * @brief Returns identification string of importer.
     * @return Identification string.
     */
    const QString& id() const;

    /**
     * @brief Returns list of target classes of importer.
     * @return List of target classes.
     */
    const QStringList& targets() const;

    /**
     * @brief Returns icon of importer.
     * @return Importer icon.
     */
    const QIcon& icon() const;

private:
    /// Importer classname
    QString m_classname;

    /// Importer identification string
    QString m_id;

    /// List of target classes.
    QStringList m_targets;

    /// Importer icon.
    QIcon m_icon;

    /**
     * @brief Initializes importer meta data based on importer meta object.
     * @param Importer meta object.
     */
    void initialize(QMetaObject& importer);

};

#endif // GTIMPORTERMETADATA_H
