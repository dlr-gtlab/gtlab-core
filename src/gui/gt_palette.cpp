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

QPalette
GtPalette::darkTheme()
{
    QPalette palette;
    QColor darkColor = basicDarkColor();
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
GtPalette::applyThemeToWidget(QWidget* w, bool dark)
{
    if (dark)
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
GtPalette::basicDarkColor()
{
    return {45, 45, 45};        //QColor(45, 45, 45);
}

QColor
GtPalette::dummyObjectBackgroundColor(bool dark)
{
    if (dark)
    {
        return {255, 130, 25};  //QColor(255, 130, 25);
    }
    else
    {
        return {255, 140, 140}; //QColor(255, 140, 140);
    }
}

QColor
GtPalette::newObjectForgroundColor(bool dark)
{
    if (dark)
    {
        return {Qt::green};
    }
    else
    {
        return {Qt::darkGreen};
    }
}

QColor
GtPalette::changedObjectForgroundColor(bool dark)
{
    if (dark)
    {
        return QColor(Qt::blue).lighter();
    }
    else
    {
        return {Qt::blue};
    }
}

QColor
GtPalette::footprintWarningColor(bool dark)
{
    Q_UNUSED(dark)
    return {255, 255, 0, 100};  //QColor(255, 255, 0, 100);
}

QColor
GtPalette::footprintErrorColor(bool dark)
{
    Q_UNUSED(dark)
    return {255, 0, 0, 100};    //QColor(255, 0, 0, 100);
}

QColor
GtPalette::loggingWarnTextColor(bool dark)
{
    Q_UNUSED(dark)
    return {255, 100, 0};       //QColor(255, 100, 0);
}

QColor
GtPalette::loggingErrorTextColor(bool dark)
{
    Q_UNUSED(dark)
    return {139, 0, 0};         //QColor(139, 0, 0);
}

QColor
GtPalette::loggingFatalTextColor(bool dark)
{
    Q_UNUSED(dark)
    return {255, 255, 255};     //QColor(255, 255, 255);
}

QColor
GtPalette::loggingFatalBackgroundColor(bool dark)
{
    Q_UNUSED(dark)
    return {120, 20, 20};       //QColor(120, 20, 20);
}

QColor
GtPalette::collectionAvailableItemBackground(bool dark)
{
    Q_UNUSED(dark)
    return {180, 229, 190};     //QColor(180, 229, 190);
}

QColor
GtPalette::collectionInstalledItemBackground(bool dark)
{
    Q_UNUSED(dark)
    return {240, 240, 240};     //QColor(240, 240, 240);
}

QColor
GtPalette::environmentModelBack(bool dark)
{
    Q_UNUSED(dark)
    return {214, 170, 170};     //QColor(214, 170, 170);
}
