/* GTlab - Gas Turbine laboratory
 * Source File: gt_abstractimporterpage.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTIMPORTERPAGE_H
#define GTABSTRACTIMPORTERPAGE_H

#include "gt_mdi_exports.h"

#include <QWizardPage>
#include <QPointer>

class GtObject;

/**
 * @brief The GtAbstractImporterPage class
 */
class GT_MDI_EXPORT GtAbstractImporterPage : public QWizardPage
{
    Q_OBJECT

protected:
    /**
     * @brief Constructor.
     * @param Object.
     */
    GtAbstractImporterPage(GtObject* obj);

    /**
     * @brief Returns data object.
     * @return Data object.
     */
    GtObject* object();

private:
    /// Pointer to object
    QPointer<GtObject> m_obj;

};

#endif // GTABSTRACTIMPORTERPAGE_H
