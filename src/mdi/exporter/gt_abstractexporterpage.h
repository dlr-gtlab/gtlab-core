/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractexporterpage.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 10.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTEXPORTERPAGE_H
#define GTABSTRACTEXPORTERPAGE_H

#include "gt_mdi_exports.h"

#include <QWizardPage>
#include <QPointer>

class GtObject;

/**
 * @brief The GtAbstractExporterPage class
 */
class GT_MDI_EXPORT GtAbstractExporterPage : public QWizardPage
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param Object.
     */
    GtAbstractExporterPage(GtObject* obj);

    /**
     * @brief Returns data object.
     * @return Data object.
     */
    GtObject* object();

private:
    /// Pointer to object
    QPointer<GtObject> m_obj;

};

#endif // GTABSTRACTEXPORTERPAGE_H
