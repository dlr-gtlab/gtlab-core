/* GTlab - Gas Turbine laboratory
 * Source File: gt_palette.cpp
 * copyright 2009-2020 by DLR
 *
 *  Created on: 06.11.2020
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_palette.h"

#include "gt_colors.h"
#include "gt_application.h"

#include <QSettings>
#include <QWidget>
#include <QStyleFactory>

QPalette
gt::gui::currentTheme()
{
    if (gtApp->inDarkMode())
    {
        return darkTheme();
    }

    return standardTheme();
}

QPalette
gt::gui::darkTheme()
{
    QPalette palette;
    QColor darkColor = color::basicDark();
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    QColor disabledColor = QColor(127, 127, 127);

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
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                     disabledColor);
    return palette;

}

QPalette
gt::gui::standardTheme()
{
    QPalette palette;

    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    QColor mainColor = QColor(245, 245, 245);
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
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
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    palette.setColor(QPalette::Link, QColor(42, 130, 218));
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
    palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    palette.setColor(QPalette::HighlightedText, Qt::black);
    palette.setColor(QPalette::Disabled, QPalette::HighlightedText,
                     disabledColor);

    return palette;
}

void
gt::gui::applyThemeToWidget(QWidget* w)
{
    if (gtApp->inDarkMode())
    {
        w->setStyle(QStyleFactory::create("Fusion"));
        w->setPalette(gt::gui::darkTheme());
        w->setStyleSheet("QToolTip { color: #ffffff; "
                      "background-color: #2a82da; "
                      "border: 1px solid white; }");
    }
    else
    {
        w->setPalette(gt::gui::standardTheme());
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
