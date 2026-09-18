/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 * Source File: test_copy_gt_object_with_uuidmap.h
 *
 *  Created on: 18.09.2026
 *  Author: Matthias Schuff (SG-VTM)
 */

#ifndef TEST_COPY_GTOBJECT_WITH_UUIDMAP_H
#define TEST_COPY_GTOBJECT_WITH_UUIDMAP_H

#include "gt_object.h"



class MyGtObject : public GtObject
{
    Q_OBJECT
public:
    Q_INVOKABLE MyGtObject() {};
};



#endif // TEST_COPY_GTOBJECT_WITH_UUIDMAP_H
