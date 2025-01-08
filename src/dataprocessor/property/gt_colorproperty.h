/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.12.2024
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#ifndef GT_COLORPROPERTY_H
#define GT_COLORPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_stringproperty.h"

namespace gt
{
    // GTlab struct to store rgb color information with the values for
    // red, green and blue fraction of the color.
    // The values have a valid range from 0 to 255
    struct GT_DATAMODEL_EXPORT rgb
    {
        rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255);

        explicit rgb(QString const& hexString);

        explicit rgb(Qt::GlobalColor qCol);

        // red
        uint8_t m_r{255};
        // green
        uint8_t m_g{255};
        // blue
        uint8_t m_b{255};
        // alpha
        uint8_t m_alpha{255};

        // sets the values of the rgb struct from the information
        // of a hex string
        bool fromString(QString const& hexCodeInput);

        // sets the values of the rgb struct from the qt namespace definitions
        void fromQtColor(Qt::GlobalColor qCol);

        // write a hex code color from the its values of rgb
        QString toHexString() const;
    };
}

/**
 * @brief The GtColorProperty class
 * Property to store color information.
 * It uses the hex code as string to store color information
 */
class GT_DATAMODEL_EXPORT GtColorProperty : public GtStringProperty
{
    Q_OBJECT

public:
    /**
     * @brief GtColorProperty
     * @param ident - identifier
     * @param name - name of the property (to show in properties dock)
     * @param brief - description to give futher information
     * @param rgb - rgb value struct
     */
    Q_INVOKABLE GtColorProperty(QString const& ident,
                                QString const& name,
                                QString const& brief,
                                gt::rgb const& rgb);

    /**
     * @brief GtColorProperty
     * @param ident - identifier
     * @param name - name of the property (to show in properties dock)
     * @param brief - description to give futher information
     * @param r - red rgb value (between 0 and 255 is valid)
     * @param g - green rgb value (between 0 and 255 is valid)
     * @param b - blue rgb value (between 0 and 255 is valid)
     * @param alpha - transparency value (between 0 and 255) where
     * 0 is fully transparent while 255 represents a fully opaque color
     */
    Q_INVOKABLE GtColorProperty(QString const& ident,
                                QString const& name,
                                QString const& brief,
                                uint8_t r, uint8_t g, uint8_t b,
                                uint8_t alpha = 255);

    /**
     * @brief GtColorProperty
     * @param ident - identifier
     * @param name - name of the property (to show in properties dock)
     * @param brief - description to give futher information
     * @param col - GlobalColor
     */
    Q_INVOKABLE GtColorProperty(QString const& ident,
                                QString const& name,
                                QString const& brief,
                                Qt::GlobalColor const& col);

    /**
     * @brief Overloaded function to convert internal property value to
     * QVariant. Value is converted based on given unit.
     * @param unit Unit into which the value is to be converted.
     * @param ok Whether conversion was successfull or not.
     * @return Value as QVariant
     */
    QVariant valueToVariant(QString const& unit, bool* ok = nullptr) const override;

    /**
     * @brief Overloaded function to set internal property value from given
     * QVariant. Value of QVariant is converted based on given unit.
     * @param val New value in form of QVariant.
     * @param unit Unit of the input value.
     * @param success Whether the value could be set or not
     * @return
     */
    GT_NO_DISCARD
    bool setValueFromVariant(QVariant const& val, QString const& unit) override;

    GtColorProperty& operator=(const QString& val);
    GtColorProperty& operator=(const gt::rgb& val);

    /**
     * @brief toRGB
     * @return a rgb struct holding the rgb values.
     * For an invalid property the rgb code is set
     * to an invalid value (-1, -1, -1)
     */
    gt::rgb toRGB() const;

    /**
     * @brief hexString
     * @return the color as hex string.
     * If the transparancy value alpha is set to 255 (no transparency) it is not
     * added to the string.
     * The string is defined as #AARRGGBB or respectivly #RRGGBB
     */
    QString hexString() const;

    /**
     * @brief setFromRGB
     * Sets property from the values for rgb as three int values of the
     * gt::rgb struct
     * @param r - red
     * @param g - green
     * @param b - blue
     * @param alpha - transparency value (between 0 and 255) where
     * 0 is fully transparent while 255 represents a fully opaque color
     */
    void setFromRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha = 255);
    void setFromRGB(const gt::rgb& rgb);

protected:
    /**
     * @brief GtColorProperty
     * @param ident - identifier
     * @param name - name of the property (to show in properties dock)
     * @param brief - description to give futher information
     * @param color to initialize. This has to be a hex string.
     * Easiest way is to define a QColor and call the function QColor::name()
     */
    Q_INVOKABLE GtColorProperty(QString const& ident,
                                QString const& name,
                                QString const& brief,
                                QString const& color);

};


#endif // GT_COLORPROPERTY_H
