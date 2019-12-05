/* GTlab - Gas Turbine laboratory
 * Source File: gt_posttemplatepath.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 26.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPOSTTEMPLATEPATH_H
#define GTPOSTTEMPLATEPATH_H

#include "gt_object.h"

/**
 * @brief The GtPostTemplatePath class
 */
class GtPostTemplatePath : public GtObject
{
    Q_OBJECT

public:
    /**
     * @brief GtPostTemplatePath
     */
    GtPostTemplatePath(const QString& path);

    /**
     * @brief path
     * @return
     */
    QString path();

};

#endif // GTPOSTTEMPLATEPATH_H
