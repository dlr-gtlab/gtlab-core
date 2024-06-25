/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 21.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPACKAGE_H
#define GTPACKAGE_H

#include "gt_datamodel_exports.h"

#include "gt_object.h"

class QDomElement;
class QDomDocument;
class QDir;

/**
 * @brief The GtPackage class
 */
class GT_DATAMODEL_EXPORT GtPackage : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief Module specific data read method.
     *
     * @return Returns true if data was successfully read.
     */
    virtual bool readData(const QDir& projectDir, const QDomElement& root);

    /**
     * @brief Module specific data save method.
     *
     * @return Returns true if data was successfully read.
     */
    virtual bool saveData(const QDir& projectDir, QDomElement& root, QDomDocument& doc);

protected:
    /**
     * @brief GtPackage
     */
    GtPackage();

};

#endif // GTPACKAGE_H
