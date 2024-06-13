/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_colors.cpp
 *
 *  Created on: 06.11.2020
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_colors.h"
#include "gt_application.h"
#include "gt_palette.h"
#include "gt_utilities.h"

#include <QPainter>
#include <QRandomGenerator>

/// helper function to desaturate a color
QColor
gt::gui::color::desaturate(QColor const& color, double multiplier)
{
    int blackness = (color.red() + color.green() + color.blue()) / 3;

    int dr = blackness - color.red();
    int dg = blackness - color.green();
    int db = blackness - color.blue();

    return QColor{
        color.red() +   static_cast<int>(dr * multiplier),
        color.green() + static_cast<int>(dg * multiplier),
        color.blue() +  static_cast<int>(db * multiplier)
    };
}

QColor
gt::gui::color::lighten(const QColor& color, int amount)
{
    constexpr int limit = std::numeric_limits<uint8_t>::max();
    int offset = (gtApp->inDarkMode() ? -1 : 1) * amount;

    QColor c = color;
    c.setRed(gt::clamp(c.red() + offset, 0, limit));
    c.setGreen(gt::clamp(c.green() + offset, 0, limit));
    c.setBlue(gt::clamp(c.blue() + offset, 0, limit));

    return c;
}

QColor
gt::gui::color::main()
{
    return currentTheme().color(QPalette::Window);
}

QColor
gt::gui::color::base()
{
    return currentTheme().color(QPalette::Base);
}

QColor
gt::gui::color::text()
{
    return currentTheme().color(QPalette::Text);
}

QColor
gt::gui::color::disabled()
{
    return currentTheme().color(QPalette::Disabled, QPalette::Text);
}

QColor
gt::gui::color::highlight()
{
    return currentTheme().color(QPalette::Highlight);
}

QColor
gt::gui::color::textHighlight()
{
    return currentTheme().color(QPalette::HighlightedText);
}

QColor
gt::gui::color::frame()
{
    return lighten(currentTheme().color(QPalette::WindowText),
                   gtApp->inDarkMode() ? 80 : 110);
}

QColor
gt::gui::color::lightFrame()
{
    return currentTheme().color(QPalette::Dark);
}

QColor
gt::gui::color::titleLabelBackground()
{
    if (gtApp->inDarkMode())
    {
        return Qt::black;
    }
    return main();
}

QColor
gt::gui::color::infoLabelBackground()
{
    if (gtApp->inDarkMode())
    {
        return Qt::black;
    }
    return base();
}

QColor
gt::gui::color::basicDark()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{21, 34, 49};
}

QColor
gt::gui::color::dummyObjectBackground()
{
    if (gtApp->inDarkMode())
    {
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        return QColor{255, 130, 25};
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{255, 140, 140};
}

QColor
gt::gui::color::newObjectForeground()
{
    if (gtApp->inDarkMode())
    {
        return {Qt::green};
    }

    return {Qt::darkGreen};
}

QColor
gt::gui::color::changedObjectForeground()
{
    if (gtApp->inDarkMode())
    {
        return QColor(Qt::blue).lighter();
    }

    return {Qt::blue};
}


QColor
gt::gui::color::newObjectForground()
{
    return newObjectForeground();
}

QColor
gt::gui::color::changedObjectForground()
{
    return changedObjectForeground();
}

QColor
gt::gui::color::footprintWarning()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{255, 255, 0, 100};
}

QColor
gt::gui::color::footprintError()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{255, 0, 0, 100};
}

QColor
gt::gui::color::warningText()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{255, 100, 0};
}

QColor
gt::gui::color::errorText()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{190, 0, 0};
}

QColor
gt::gui::color::fatalText()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{255, 255, 255};
}

QColor
gt::gui::color::fatalTextBackground()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{120, 20, 20};
}

QColor
gt::gui::color::collectionAvailableItemBackground()
{
    if (gtApp->inDarkMode())
    {
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        return QColor{89, 110, 93};
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{180, 229, 190};
}

QColor
gt::gui::color::collectionInstalledItemBackground()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{240, 240, 240};
}

QColor
gt::gui::color::environmentModelBack()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{214, 170, 170};
}

QColor
gt::gui::color::debugText()
{
    if (gtApp->inDarkMode())
    {
        return QColor(Qt::gray).lighter(120);
    }
    return QColor(Qt::gray).darker();
}

QColor
gt::gui::color::infoText()
{
    return text();
}

void
gt::gui::color::setPaintertoGray(QPainter* painter)
{
    if (painter)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        painter->setPen(QPen(QColor(70, 70, 70), 1.5));
        painter->setBrush(Qt::white);

        if (gtApp->inDarkMode())
        {
            // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
            painter->setPen(QPen(QColor(170, 170, 170), 1.5));
            painter->setBrush(QBrush(gt::gui::color::basicDark()));
        }
    }
}

QColor
gt::gui::color::randomColor()
{
    QRandomGenerator randg;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor::fromHsv(randg.generate() % 256, 255, 190);
}

QColor
gt::gui::color::gridLineColor()
{
    return gridLine();
}

QColor
gt::gui::color::gridLineMinor()
{

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor(150, 150, 150, 20);
}

QColor
gt::gui::color::gridPointColor()
{
    return gridPoint();
}

QColor
gt::gui::color::gridLine()
{
    if (gtApp->inDarkMode())
    {
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        return QColor(25, 25, 25, 255);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor(200, 200, 255, 125);
}

QColor
gt::gui::color::gridPoint()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor(100, 100, 155);
}

QColor
gt::gui::color::gridAxis()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor(150, 150, 150);
}

QColor
gt::gui::color::code_editor::highlightLine()
{
    if (gtApp->inDarkMode())
    {
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        return QColor(160, 160, 0);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor(Qt::yellow).lighter(160);
}

QColor
gt::gui::color::xml_highlight::syntaxChar()
{
    if (gtApp->inDarkMode())
    {
        return QColor(235, 160, 50);
    }

    return Qt::blue;
}

QColor
gt::gui::color::xml_highlight::elementName()
{
    if (gtApp->inDarkMode())
    {
        return QColor(190, 35, 35);
    }

    return Qt::darkRed;
}

QColor
gt::gui::color::xml_highlight::comment()
{
    if (gtApp->inDarkMode())
    {
        return Qt::green;
    }

    return Qt::darkGreen;
}

QColor
gt::gui::color::xml_highlight::attributeName()
{
    if (gtApp->inDarkMode())
    {
        return Qt::red;
    }

    return Qt::red;
}

QColor
gt::gui::color::xml_highlight::attributeValue()
{
    if (gtApp->inDarkMode())
    {
        return QColor(100, 200, 255);
    }

    return Qt::blue;
}

QColor
gt::gui::color::xml_highlight::error()
{
    if (gtApp->inDarkMode())
    {
        return Qt::magenta;
    }

    return Qt::darkMagenta;
}

QColor
gt::gui::color::xml_highlight::other()
{
    if (gtApp->inDarkMode())
    {
        return QColor(200, 200, 200);
    }

    return Qt::black;
}

QColor
gt::gui::color::js_highlight::normal()
{
    return QColor(0, 0, 0);
}

QColor
gt::gui::color::js_highlight::comment()
{
    return QColor(0, 128, 0);
}

QColor
gt::gui::color::js_highlight::number()
{
    return QColor(255, 0, 0);
}

QColor
gt::gui::color::js_highlight::string()
{
    return QColor(0, 0, 255);
}

QColor
gt::gui::color::js_highlight::operator_()
{
    return QColor(0, 0, 0);
}

QColor
gt::gui::color::js_highlight::identifier()
{
    return QColor(0, 0, 32);
}

QColor
gt::gui::color::js_highlight::keyword()
{
    return QColor(0, 0, 128);
}

QColor
gt::gui::color::js_highlight::builtIn()
{
    return QColor(0, 128, 128);
}

QColor
gt::gui::color::js_highlight::marker()
{
    return QColor(255, 255, 0);
}

QColor
gt::gui::color::connection_editor::connection()
{
    if (gtApp->inDarkMode())
    {
        return Qt::lightGray;
    }
    return Qt::black;
}

QColor
gt::gui::color::connection_editor::connectionDraft()
{
    if (gtApp->inDarkMode())
    {
        return QColor(98, 182, 230);
    }
    return Qt::blue;
}

QColor
gt::gui::color::connection_editor::portBrush()
{
    if (gtApp->inDarkMode())
    {
        return QColor(69, 130, 162);
    }
    return Qt::darkGray;
}

QColor
gt::gui::color::connection_editor::portHoverEnter()
{
    if (gtApp->inDarkMode())
    {
        return Qt::lightGray;
    }
    return Qt::white;
}

QColor
gt::gui::color::connection_editor::connectionHighlight()
{
    if (gtApp->inDarkMode())
    {
        return Qt::red;
    }
    return Qt::red;
}
