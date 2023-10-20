/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractimporterpage.h
 *
 *  Created on: 08.11.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABSTRACTIMPORTERPAGE_H
#define GTABSTRACTIMPORTERPAGE_H

#include "gt_gui_exports.h"

#include <QWizardPage>
#include <QPointer>

class GtObject;

/**
 * @brief The GtAbstractImporterPage class
 */
class GT_GUI_EXPORT GtAbstractImporterPage : public QWizardPage
{
    Q_OBJECT

protected:
    /**
     * @brief Constructor.
     * @param Object.
     */
    explicit GtAbstractImporterPage(GtObject* obj);

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
