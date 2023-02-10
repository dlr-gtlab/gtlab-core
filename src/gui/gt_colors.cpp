/* GTlab - Gas Turbine laboratory
 * Source File: gt_colors.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 06.11.2020
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_colors.h"
#include "gt_application.h"
#include "gt_palette.h"

#include <QPainter>
#include <QRandomGenerator>

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

QColor gt::gui::color::highlight()
{
    return currentTheme().color(QPalette::Highlight);
}

QColor gt::gui::color::textHighlight()
{
    return currentTheme().color(QPalette::HighlightedText);
}

QColor
gt::gui::color::basicDark()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor{21,34, 49};
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
gt::gui::color::newObjectForground()
{
    if (gtApp->inDarkMode())
    {
        return {Qt::green};
    }

    return {Qt::darkGreen};
}

QColor
gt::gui::color::changedObjectForground()
{
    if (gtApp->inDarkMode())
    {
        return QColor(Qt::blue).lighter();
    }

    return {Qt::blue};
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
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor(200, 200, 255, 125);
}

QColor
gt::gui::color::gridPointColor()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return QColor(100, 100, 155);
}
