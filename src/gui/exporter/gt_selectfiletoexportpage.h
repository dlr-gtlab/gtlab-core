/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_selectfiletoexportpage.h
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSELECTFILETOEXPORTPAGE_H
#define GTSELECTFILETOEXPORTPAGE_H

#include "gt_gui_exports.h"

#include "gt_abstractexporterpage.h"

class QFile;
class QLineEdit;
class GtAbstractExporter;

/**
 * @brief The GtSelectFileToExportPage class
 */
class GT_GUI_EXPORT GtSelectFileToExportPage : public GtAbstractExporterPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Data object.
     * @param List of allowed file extensions.
     */
    explicit GtSelectFileToExportPage(GtObject* obj,
                                      GtAbstractExporter* exporter = nullptr);

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
    bool validatePage() override;

    /**
     * @brief isComplete
     * @return
     */
    bool isComplete() const override;

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
