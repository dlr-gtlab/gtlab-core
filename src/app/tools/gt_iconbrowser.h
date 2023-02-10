/* GTlab - Gas Turbine laboratory
 * copyright 2009-2023 by DLR
 *
 *  Created on: 9.2.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTICONBROWSER_H
#define GTICONBROWSER_H

#include "gt_dialog.h"

/**
 * @brief The GtIconOverviewWidget class.
 * Test widget for displaying most if not all GTlab Icons
 */
class GtIconBrowser : public GtDialog
{
    Q_OBJECT

public:

    GtIconBrowser();

protected:

    void keyPressEvent(QKeyEvent* e) override;
};

#endif // GTICONBROWSER_H
