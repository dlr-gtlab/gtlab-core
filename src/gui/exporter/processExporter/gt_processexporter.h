/* GTlab - Gas Turbine laboratory
 * Source File: gt_processexporter.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 04.05.2020
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */
#ifndef GTPROCESSEXPORTER_H
#define GTPROCESSEXPORTER_H

#include <QFile>

#include "gt_abstractexporter.h"

/**
 * @brief The GtProcessExporter class
 */
class GtProcessExporter : public GtAbstractExporter
{
    Q_OBJECT

public:
    /**
     * @brief GtProcessExporter
     * Constructor
     */
    Q_INVOKABLE GtProcessExporter();

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
};

#endif // GTPROCESSEXPORTER_H
