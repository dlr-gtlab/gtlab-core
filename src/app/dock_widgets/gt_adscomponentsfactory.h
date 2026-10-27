/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#ifndef GT_ADSCOMPONENTSFACTORY_H
#define GT_ADSCOMPONENTSFACTORY_H

#include <qtadvanceddocking-qt5/DockComponentsFactory.h>

class GtAdsComponentsFactory : public ads::CDockComponentsFactory
{
public:

    ads::CDockAreaTitleBar*
        createDockAreaTitleBar(ads::CDockAreaWidget* area) const override;
};

#endif // GT_ADSCOMPONENTSFACTORY_H
