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
                                 const QString& brief, int r, int g, int b) :
    GtColorProperty(ident, name, brief, gt::rgb(r, g, b).toHexString())
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

void
GtColorProperty::setFromRGB(int r, int g, int b)
{
    setFromRGB({r, g, b});
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

gt::rgb::rgb(int r, int g, int b) :
    m_r(r), m_g(g), m_b(b)
{
}

gt::rgb::rgb(const QString& hexString)
{
    fromString(hexString);
}

bool
gt::rgb::valid() const
{
    if (m_r < 0 || m_r > 255 || m_g < 0 || m_g > 255 || m_b < 0 || m_b > 255) return false;

    return true;
}

void
gt::rgb::fromString(const QString& hexCodeInput)
{
    std::string hexCode = hexCodeInput.toStdString();
    if (hexCode[0] == '#')
    {
        hexCode = hexCode.substr(1);
    }

    // check for shortend hexcode version
    if (hexCode.length() == 3) {
        // doubleing of entries to match long version of definition
        hexCode = { hexCode[0], hexCode[0],
                   hexCode[1], hexCode[1],
                   hexCode[2], hexCode[2] };
    }

    // Check size of string
    if (hexCode.length() != 6)
    {
        gtError() << QObject::tr("Invalid hex code in "
                                 "property (%1)").arg(hexCodeInput);
        m_r = -1;
        m_g = -1;
        m_b = -1;
        return;
    }

    int r, g, b;

    std::stringstream ss;
    ss << std::hex << hexCode.substr(0, 2); // red
    ss >> r;
    ss.clear();

    ss << std::hex << hexCode.substr(2, 2); // green
    ss >> g;
    ss.clear();

    ss << std::hex << hexCode.substr(4, 2); // blue
    ss >> b;
}

QString
gt::rgb::toHexString() const
{
    if (!valid()) return {};

    std::stringstream ss;
    ss << "#"
       << std::setw(2) << std::setfill('0') << std::hex << m_r
       << std::setw(2) << std::setfill('0') << std::hex << m_g
       << std::setw(2) << std::setfill('0') << std::hex << m_b;
    std::string help = ss.str();

    return QString::fromStdString(help);
}
