/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 19.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTASKARROWENTITY_H
#define GTTASKARROWENTITY_H

#include "gt_graphicsanimatedpathitem.h"

class GtCalculatorEntity;
class GtObject;

/**
 * @brief The GtTaskArrowEntity class
 */
class GtTaskArrowEntity : public GtGraphicsAnimatedPathItem
{
    Q_OBJECT

public:
    enum ArrowTypes
    {
        ARROW_DIRECT,
        ARROW_LEFT_CHILD,
        ARROW_RIGHT_CHILD
    };

    /**
     * @brief GtTaskArrowEntity
     * @param parent
     */
    explicit GtTaskArrowEntity(ArrowTypes type = ARROW_DIRECT,
                               GtCalculatorEntity* parent = nullptr);

    /**
     * @brief link
     * @param from
     * @param to
     */
    void link(GtCalculatorEntity* from, GtCalculatorEntity* to);

    /**
     * @brief updateShape
     */
    void updateShape();

    /**
     * @brief setToPreviewMode
     */
    void setToPreviewMode();

private:
    ///
    ArrowTypes m_type;

    ///
    GtCalculatorEntity* m_from;

    ///
    GtCalculatorEntity* m_to;

    ///
    GtCalculatorEntity* m_parent;

    /// Arrow head
    QGraphicsPathItem* m_head;

private slots:
    /**
     * @brief onItemAdd
     * @param obj
     */
    void onItemAdd(GtObject* obj);

};

#endif // GTTASKARROWENTITY_H
