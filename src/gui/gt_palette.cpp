/* GTlab - Gas Turbine laboratory
 * Source File: gt_palette.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 06.11.2020
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_palette.h"

#include <QSettings>
#include <QWidget>
#include <QStyleFactory>
#include "gt_application.h"


QPalette
GtPalette::currentTheme()
{
    if (gtApp->inDarkMode())
    {
        return darkTheme();
    }

    return standardTheme();
}

QPalette
GtPalette::darkTheme()
{
    QPalette palette;
    QColor darkColor = Color::basicDark();
    QColor disabledColor = QColor(127,127,127);

    palette.setColor(QPalette::Window, darkColor);
    palette.setColor(QPalette::Background, darkColor);
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, darkColor); //QColor(18,18,18));
    palette.setColor(QPalette::AlternateBase, darkColor);   
    palette.setColor(QPalette::ToolTipBase, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::white);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, disabledColor);
    palette.setColor(QPalette::Button, darkColor);
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                     disabledColor);
    return palette;

}

QPalette
GtPalette::standardTheme()
{
    QPalette palette;

    QColor mainColor = QColor(245, 245, 245);
    QColor disabledColor = QColor(127, 127, 127);

    palette.setColor(QPalette::Window, mainColor);
    palette.setColor(QPalette::Background, mainColor);
    palette.setColor(QPalette::WindowText, Qt::black);
    palette.setColor(QPalette::Base, Qt::white);
    palette.setColor(QPalette::AlternateBase, mainColor);
    palette.setColor(QPalette::ToolTipBase, Qt::black);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::black);
    palette.setColor(QPalette::Button, mainColor);
    palette.setColor(QPalette::ButtonText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, disabledColor);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                     disabledColor);

    return palette;
}

void
GtPalette::applyThemeToWidget(QWidget* w)
{
    if (gtApp->inDarkMode())
    {
        w->setStyle(QStyleFactory::create("Fusion"));
        w->setPalette(GtPalette::darkTheme());
        w->setStyleSheet("QToolTip { color: #ffffff; "
                      "background-color: #2a82da; "
                      "border: 1px solid white; }");
    }
    else
    {
        w->setPalette(GtPalette::standardTheme());
        QString style = "Default";
#ifdef Q_OS_WIN
        style = "windowsvista";
#endif
        w->setStyle(QStyleFactory::create(style));
        w->setStyleSheet("QToolTip { color: black; "
                      "background-color: white; "
                      "border: 1px solid black; }");
    }
}

QColor
GtPalette::Color::basicDark()
{
    return {45, 45, 45};        //QColor(45, 45, 45);
}

QColor
GtPalette::Color::dummyObjectBackground()
{
    if (gtApp->inDarkMode())
    {
        return {255, 130, 25};  //QColor(255, 130, 25);
    }
    else
    {
        return {255, 140, 140}; //QColor(255, 140, 140);
    }
}

QColor
GtPalette::Color::newObjectForground()
{
    if (gtApp->inDarkMode())
    {
        return {Qt::green};
    }
    else
    {
        return {Qt::darkGreen};
    }
}

QColor
GtPalette::Color::changedObjectForground()
{
    if (gtApp->inDarkMode())
    {
        return QColor(Qt::blue).lighter();
    }
    else
    {
        return {Qt::blue};
    }
}

QColor
GtPalette::Color::footprintWarning()
{
    return {255, 255, 0, 100};  //QColor(255, 255, 0, 100);
}

QColor
GtPalette::Color::footprintError()
{
    return {255, 0, 0, 100};    //QColor(255, 0, 0, 100);
}

QColor
GtPalette::Color::warningText()
{
    return {255, 100, 0};       //QColor(255, 100, 0);
}

QColor
GtPalette::Color::errorText()
{
    return {139, 0, 0};         //QColor(139, 0, 0);
}

QColor
GtPalette::Color::fatalText()
{
    return {255, 255, 255};     //QColor(255, 255, 255);
}

QColor
GtPalette::Color::fatalTextBackground()
{
    return {120, 20, 20};       //QColor(120, 20, 20);
}

QColor
GtPalette::Color::collectionAvailableItemBackground()
{
    return {180, 229, 190};     //QColor(180, 229, 190);
}

QColor
GtPalette::Color::collectionInstalledItemBackground()
{
    return {240, 240, 240};     //QColor(240, 240, 240);
}

QColor
GtPalette::Color::environmentModelBack()
{
    return {214, 170, 170};     //QColor(214, 170, 170);
}

QColor
GtPalette::Color::infoText()
{
    if (gtApp->inDarkMode())
    {
        return {Qt::white};
    }
    else
    {
        return {Qt::black};
    }
}
