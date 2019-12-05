/* GTlab - Gas Turbine laboratory
 * Source File: gt_selectfiletoimportpage.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSELECTFILETOIMPORTPAGE_H
#define GTSELECTFILETOIMPORTPAGE_H

#include "gt_mdi_exports.h"

#include "gt_abstractimporterpage.h"

class QFile;
class QLineEdit;
class GtAbstractImporter;

/**
 * @brief The GtSelectFileToImportPage class
 */
class GT_MDI_EXPORT GtSelectFileToImportPage : public GtAbstractImporterPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Data object.
     * @param List of allowed file extensions.
     */
    GtSelectFileToImportPage(GtObject* obj,
                             GtAbstractImporter* importer = Q_NULLPTR);

    /**
     * @brief import
     * @param file
     * @return
     */
    virtual bool import(QFile& file);

    /**
     * @brief Sets allowed file extensions.
     * @param List of allowed file extensions.
     */
    void setFileExtensions(const QStringList& fileExt);

    /**
     * @brief Returns user seleted file name.
     * @return User seleccted file name.
     */
    QString selectedFileName();

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

private:
    /// List of allowed file extensions.
    QStringList m_fileExtensions;

    /// Pointer to importer.
    QPointer<GtAbstractImporter> m_importer;

    /// Line edit for file path
    QLineEdit* m_lineEdit;

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

#endif // GTSELECTFILETOIMPORTPAGE_H
