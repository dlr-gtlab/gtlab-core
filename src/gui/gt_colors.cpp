/* GTlab - Gas Turbine laboratory
 * Source File: gt_colors.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 06.11.2020
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_colors.h"


#include <QIcon>
#include "gt_application.h"

QColor
GtGUI::Color::basicDark()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {45, 45, 45};        //QColor(45, 45, 45);
}

QColor
GtGUI::Color::dummyObjectBackground()
{
    if (gtApp->inDarkMode())
    {
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        return {255, 130, 25};  //QColor(255, 130, 25);
    }

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {255, 140, 140}; //QColor(255, 140, 140);

}

QColor
GtGUI::Color::newObjectForground()
{
    if (gtApp->inDarkMode())
    {
        return {Qt::green};
    }

    return {Qt::darkGreen};
}

QColor
GtGUI::Color::changedObjectForground()
{
    if (gtApp->inDarkMode())
    {
        return QColor(Qt::blue).lighter();
    }

    return {Qt::blue};
}

QColor
GtGUI::Color::footprintWarning()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {255, 255, 0, 100};  //QColor(255, 255, 0, 100);
}

QColor
GtGUI::Color::footprintError()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {255, 0, 0, 100};    //QColor(255, 0, 0, 100);
}

QColor
GtGUI::Color::warningText()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {255, 100, 0};       //QColor(255, 100, 0);
}

QColor
GtGUI::Color::errorText()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {139, 0, 0};         //QColor(139, 0, 0);
}

QColor
GtGUI::Color::fatalText()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {255, 255, 255};     //QColor(255, 255, 255);
}

QColor
GtGUI::Color::fatalTextBackground()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {120, 20, 20};       //QColor(120, 20, 20);
}

QColor
GtGUI::Color::collectionAvailableItemBackground()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {180, 229, 190};     //QColor(180, 229, 190);
}

QColor
GtGUI::Color::collectionInstalledItemBackground()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {240, 240, 240};     //QColor(240, 240, 240);
}

QColor
GtGUI::Color::environmentModelBack()
{
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    return {214, 170, 170};     //QColor(214, 170, 170);
}

QColor
GtGUI::Color::infoText()
{
    if (gtApp->inDarkMode())
    {
        return {Qt::white};
    }

    return {Qt::black};
}

