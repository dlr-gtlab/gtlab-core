/* GTlab - Gas Turbine laboratory
 * Source File: gt_sessionviewer.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSESSIONVIEWER_H
#define GTSESSIONVIEWER_H

#include "gt_mdiitem.h"

/**
 * @brief The GtSessionViewer class
 */
class GtSessionViewer : public GtMdiItem
{
    Q_OBJECT

public:
    /**
      * @brief GtSessionViewer
      */
    Q_INVOKABLE GtSessionViewer();

    /**
     * @brief Virtual function to specify item specific icon.
     * @return Object specific icon.
     */
    virtual QIcon icon() const;

};

#endif // GTSESSIONVIEWER_H
