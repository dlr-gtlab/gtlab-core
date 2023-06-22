/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 15.3.2023
 *  Author: Marius Bröcker (AT-TWK)
 *  E-Mail: marius.broecker@dlr.de
 */

#ifndef GTSTYLE_H
#define GTSTYLE_H

#include <QProxyStyle>

/**
 * @brief The GtStyle class.
 * Proxy style to alter the visual appearance of widgets, primitives, default
 * icons etc.
 */
class GtStyle : public QProxyStyle
{
    Q_OBJECT

public:

    /// constructor
    GtStyle();

    /**
     * @brief Returns the prefered palette of this style. Depending on the
     * theme engine this may not be used.
     * @return Palette of this style
     */
    QPalette standardPalette() const override;

    /**
     * @brief General style hints, which can be used to alter the behaviour or
     * visual appearance of widgets
     * @param hint Style hint
     * @param option
     * @param widget
     * @param returnData
     * @return Style hint data
     */
    int styleHint(StyleHint hint,
                  QStyleOption const* option = nullptr,
                  QWidget const* widget = nullptr,
                  QStyleHintReturn* returnData = nullptr) const override;

    /**
     * @brief drawPrimitive
     * @param element
     * @param option
     * @param painter
     * @param widget
     */
    void drawPrimitive(PrimitiveElement element,
                       QStyleOption const* option,
                       QPainter* painter,
                       QWidget const* widget = nullptr) const override;

    /**
     * @brief drawPrimitive
     * @param element
     * @param option
     * @param painter
     * @param widget
     */
    void drawControl(ControlElement element,
                    QStyleOption const* option,
                    QPainter* painter,
                    QWidget const* widget = nullptr) const override;

    /**
     * @brief standardIcon
     * @param standardIcon
     * @param option
     * @param widget
     * @return
     */
    QIcon standardIcon(QStyle::StandardPixmap standardIcon,
                       QStyleOption const* option = nullptr,
                       QWidget const* widget = nullptr) const override;
};

#endif // GTSTYLE_H
