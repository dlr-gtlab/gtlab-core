/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 06.12.2024
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_colorproperty.h"

#include "gt_regexp.h"

GtColorProperty::GtColorProperty(const QString& ident,
                                 const QString& name,
                                 const QString& brief,
                                 const QString& color) :
    GtStringProperty(ident, name, brief, color,
                     QRegularExpression(gt::re::forHexColorCode().pattern()))
{
    m_id = ident;
    setObjectName(name);
    m_brief = brief;
    m_initValue = color;
    m_value = color;
}

GtColorProperty::GtColorProperty(const QString& ident, const QString& name,
                                 const QString& brief, const gt::rgb& rgb) :
    GtColorProperty(ident, name, brief, rgb.toHexString())
{
}

GtColorProperty::GtColorProperty(const QString& ident, const QString& name,
                                 const QString& brief, uint8_t r, uint8_t g,
                                 uint8_t b, uint8_t alpha) :
    GtColorProperty(ident, name, brief, gt::rgb(r, g, b, alpha).toHexString())
{
}

GtColorProperty::GtColorProperty(const QString& ident, const QString& name,
                                 const QString& brief,
                                 const Qt::GlobalColor& col) :
    GtColorProperty(ident, name, brief, gt::rgb(col).toHexString())
{
}


QVariant
GtColorProperty::valueToVariant(const QString &unit, bool *ok) const
{
    return gt::valueSuccess(QVariant::fromValue(m_value), ok);
}

bool
GtColorProperty::setValueFromVariant(const QVariant& val, const QString& unit)
{
    QString name = val.toString();
    if (name.isEmpty()) return false;

    QRegularExpressionMatch match = validator().match(name);

    if (!match.hasMatch()) return false;

    setVal(name);

    return true;
}

GtColorProperty&
GtColorProperty::operator=(const QString& val)
{
    setVal(val);
    emit changed();
    return *this;
}

GtColorProperty&
GtColorProperty::operator=(const gt::rgb& val)
{
    setFromRGB(val);
    emit changed();
    return *this;
}

gt::rgb
GtColorProperty::toRGB() const
{
    return gt::rgb(m_value);
}

QString
GtColorProperty::hexString() const
{
    gt::rgb rgb(m_value);

    return rgb.toHexString();
}

void
GtColorProperty::setFromRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha)
{
    setFromRGB({r, g, b, alpha});
}

void
GtColorProperty::setFromRGB(gt::rgb const& rgb)
{
    QString val = rgb.toHexString();

    if (val.isEmpty())
    {
        gtWarning() << QObject::tr("Cannot use invalid rgb color definition "
                                   "(Values need to be between 0 and 255");
        return;
    }

    setVal(val);
}

gt::rgb::rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t alpha) :
    m_r(r), m_g(g), m_b(b), m_alpha(alpha)
{
}

gt::rgb::rgb(const QString& hexString)
{
    fromString(hexString);
}

gt::rgb::rgb(Qt::GlobalColor qCol)
{
    fromQtColor(qCol);
}

bool
gt::rgb::fromString(const QString& hexCodeInput)
{    
    std::string hexCode = hexCodeInput.toStdString();
    if (hexCode[0] == '#')
    {
        hexCode = hexCode.substr(1);
    }

    // check for shortend hexcode version
    if (hexCode.length() == 3)
    {
        // doubleing of entries to match long version of definition
        hexCode = {hexCode[0], hexCode[0],
                   hexCode[1], hexCode[1],
                   hexCode[2], hexCode[2]};
    }
    else if (hexCode.length() == 4)
    {
        // doubleing of entries to match long version of definition
        hexCode = {hexCode[0], hexCode[0],
                   hexCode[1], hexCode[1],
                   hexCode[2], hexCode[2],
                   hexCode[3], hexCode[3]};
    }

    if (hexCode.length() != 6 && hexCode.length() != 8)
    {
        gtError() << QObject::tr("Invalid hex code in "
                                 "property (%1)").arg(hexCodeInput);
        return false;
    }

    int shift = 0;

    std::stringstream ss;

    // Check size of string
    if (hexCode.length() == 8)
    {
        ss << std::hex << hexCode.substr(0, 2); // alpha
        ss >> m_alpha;
        ss.clear();
        shift = 2;
    }
    else
    {
        m_alpha = 255;
    }


    ss << std::hex << hexCode.substr(0 + shift, 2); // red
    ss >> m_r;
    ss.clear();

    ss << std::hex << hexCode.substr(2 + shift, 2); // green
    ss >> m_g;
    ss.clear();

    ss << std::hex << hexCode.substr(4 + shift, 2); // blue
    ss >> m_b;
    ss.clear();

    return true;
}

void
gt::rgb::fromQtColor(Qt::GlobalColor qCol)
{
    m_alpha = 255;
    switch (qCol)
    {
    case Qt::white:
        m_r = 255;
        m_g = 255;
        m_b = 255;
        break;
    case Qt::black:
        m_r = 0;
        m_g = 0;
        m_b = 0;
        break;
    case Qt::red: // #ff0000
        m_r = 255;
        m_g = 0;
        m_b = 0;
        break;
    case Qt::darkRed: // #800000
        m_r = 128;
        m_g = 0;
        m_b = 0;
        break;
    case Qt::green: // #00ff00
        m_r = 0;
        m_g = 255;
        m_b = 0;
        break;
    case Qt::darkGreen: // #008000
        m_r = 0;
        m_g = 128;
        m_b = 0;
        break;
    case Qt::blue:  // #0000ff
        m_r = 0;
        m_g = 0;
        m_b = 255;
        break;
    case Qt::darkBlue:  // #000080
        m_r = 0;
        m_g = 0;
        m_b = 128;
        break;
    case Qt::cyan:  // #00ffff
        m_r = 0;
        m_g = 255;
        m_b = 255;
        break;
    case Qt::darkCyan:  // #008080
        m_r = 0;
        m_g = 128;
        m_b = 128;
        break;
    case Qt::magenta:  // #ff00ff
        m_r = 255;
        m_g = 0;
        m_b = 255;
        break;
    case Qt::darkMagenta:  // #800080
        m_r = 128;
        m_g = 0;
        m_b = 128;
        break;
    case Qt::yellow:  // #ffff00
        m_r = 255;
        m_g = 255;
        m_b = 0;
        break;
    case Qt::darkYellow:  // #808000
        m_r = 128;
        m_g = 128;
        m_b = 0;
        break;
    case Qt::gray:  // #a0a0a4
        m_r = 160;
        m_g = 160;
        m_b = 164;
        break;
    case Qt::darkGray:  // #808080
        m_r = 128;
        m_g = 128;
        m_b = 128;
        break;
    case Qt::lightGray:  // #c0c0c0
        m_r = 192;
        m_g = 192;
        m_b = 192;
        break;
    case Qt::transparent:  // #00000000
        m_r = 0;
        m_g = 0;
        m_b = 0;
        m_alpha = 0;
        break;
    default:
        gtError() << QObject::tr("Invalid Qt namespace color value."
                                 "Color is set to white as default");
        m_r = 255;
        m_g = 255;
        m_b = 255;
        break;
    }
}

QString
gt::rgb::toHexString() const
{
    QString res = "#";

    if (m_alpha != 255)
    {
        res += QString("%1").arg(m_alpha, 2, 16, QChar('0'));
    }

    res += QString("%1%2%3")
               .arg(m_r, 2, 16, QChar('0'))
               .arg(m_g, 2, 16, QChar('0'))
               .arg(m_b, 2, 16, QChar('0'));

    return res;
}
