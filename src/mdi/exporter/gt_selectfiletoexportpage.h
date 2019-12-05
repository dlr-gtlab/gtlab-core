/* GTlab - Gas Turbine laboratory
 * Source File: gt_selectfiletoexportpage.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSELECTFILETOEXPORTPAGE_H
#define GTSELECTFILETOEXPORTPAGE_H

#include "gt_mdi_exports.h"

#include "gt_abstractexporterpage.h"

class QFile;
class QLineEdit;
class GtAbstractExporter;

/**
 * @brief The GtSelectFileToExportPage class
 */
class GT_MDI_EXPORT GtSelectFileToExportPage : public GtAbstractExporterPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Data object.
     * @param List of allowed file extensions.
     */
    GtSelectFileToExportPage(GtObject* obj,
                             GtAbstractExporter* exporter = Q_NULLPTR);

    /**
     * @brief export
     * @param file
     * @return
     */
    virtual bool doExport(QFile& file);

    /**
     * @brief Sets allowed file extension.
     * @param Allowed file extension.
     */
    void setFileExtension(const QString& fileExt);

    /**
     * @brief validatePage
     * @return
     */
    bool validatePage() Q_DECL_OVERRIDE;

    /**
     * @brief isComplete
     * @return
     */
    bool isComplete() const Q_DECL_OVERRIDE;

protected:
    /// Line edit for file path
    QLineEdit* m_lineEdit;

    /// Pointer to exporter.
    QPointer<GtAbstractExporter> m_exporter;

private:
    /// allowed file extension.
    QString m_fileExtension;

private slots:
    /**
     * @brief selectFilePath
     */
    void selectFilePath();

    /**
     * @brief onPathSelected
     */
    void onPathSelected();

signals:

    /**
     * @brief pathSelected
     */
    void pathSelected();

};

#endif // GTSELECTFILETOEXPORTPAGE_H
