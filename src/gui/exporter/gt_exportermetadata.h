/* GTlab - Gas Turbine laboratory
 * Source File: gt_exportermetadata.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTEXPORTERMETADATA_H
#define GTEXPORTERMETADATA_H

#include <QString>
#include <QStringList>
#include <QMetaObject>
#include <QIcon>

/**
 * @brief The GtExporterMetaData class
 */
class GtExporterMetaData
{
public:
    /**
     * @brief Default constructor.
     */
    GtExporterMetaData();

    /**
     * @brief Constructor.
     * @param Meta object of exporter.
     */
    explicit GtExporterMetaData(QMetaObject& exporter);

    /**
     * @brief Returns classname of exporter.
     * @return Classname.
     */
    const QString& classname() const;

    /**
     * @brief Returns identification string of exporter.
     * @return Identification string.
     */
    const QString& id() const;

    /**
     * @brief Returns list of target classes of exporter.
     * @return List of target classes.
     */
    const QStringList& targets() const;

    /**
     * @brief Returns icon of exporter.
     * @return Importer icon.
     */
    const QIcon& icon() const;

private:
    /// Exporter classname
    QString m_classname;

    /// Exporter identification string
    QString m_id;

    /// List of target classes.
    QStringList m_targets;

    /// Exporter icon.
    QIcon m_icon;

    /**
     * @brief Initializes exporter meta data based on exporter meta object.
     * @param Exporter meta object.
     */
    void initialize(QMetaObject& exporter);

};

#endif // GTEXPORTERMETADATA_H
