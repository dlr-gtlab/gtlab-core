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
#include <QApplication>

QPalette
gt::gui::currentTheme()
{
    if (gtApp->inDarkMode())
    {
        return darkTheme();
    }

    return standardTheme();
}

struct ColorConfig
{
    QColor main;
    QColor base;
    QColor text;
    QColor disabled;
    QColor highlight;
    QColor textHighlighted;
    QColor link;
    QColor linkVisited;
};

QPalette
generateTheme(ColorConfig const& config)
{
    QPalette palette = qApp->palette();

    // window stuff
    palette.setColor(QPalette::Window, config.main);
    palette.setColor(QPalette::Base, config.base);
    palette.setColor(QPalette::AlternateBase, config.main);

    // widgets
    palette.setColor(QPalette::Button, config.main);

    // tones
    palette.setColor(QPalette::Light, config.main); // used for text shadows
    // QPalette::Dark = QFrame::Sunken
    // QPalette::Mid  = QWizard: QFrame between page and buttons

    // highlights
    palette.setColor(QPalette::Highlight, config.highlight);
    palette.setColor(QPalette::HighlightedText, config.textHighlighted);

    // links
    palette.setColor(QPalette::Link, config.link);
    palette.setColor(QPalette::LinkVisited, config.linkVisited);

    // general text
    palette.setColor(QPalette::Text, config.text);
    palette.setColor(QPalette::BrightText, config.text);
    palette.setColor(QPalette::WindowText, config.text);
    palette.setColor(QPalette::ToolTipText, config.text);
    palette.setColor(QPalette::ButtonText, config.text);
    palette.setColor(QPalette::PlaceholderText, config.disabled);

    // disabled text
    palette.setColor(QPalette::Disabled, QPalette::Text, config.disabled);
    palette.setColor(QPalette::Disabled, QPalette::BrightText, config.disabled);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, config.disabled);

    return palette;
}


QPalette
gt::gui::darkTheme()
{
    static const QPalette p = [](){

        ColorConfig config;
        config.main = color::basicDark();
        config.base = config.main.lighter(105);
        config.text = Qt::white;
        config.disabled = Qt::gray;
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        config.highlight =  QColor{42, 130, 218};
        config.textHighlighted = config.text;
        config.link = config.highlight;
        config.linkVisited = config.link.darker();

        return generateTheme(config);
    }();

    return p;
}

QPalette
gt::gui::standardTheme()
{
    static const QPalette p = [](){

        ColorConfig config;
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        config.main = QColor{245, 245, 245};
        config.base = Qt::white;
        config.text = Qt::black;
        config.disabled = Qt::gray;
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
        config.highlight = QColor{42, 130, 218};
        config.textHighlighted = config.text;
        config.link = config.highlight;
        config.linkVisited = config.link.darker();

        return generateTheme(config);
    }();

    return p;
}

template <typename Widget>
inline void applyTheme(Widget& w)
{
    QString stylesheet;

#ifndef Q_OS_WIN
    QString style = QStringLiteral("Default");
#else
    QString style = QStringLiteral("windowsvista");
#endif

    if (gtApp->inDarkMode())
    {
        style = QStringLiteral("Fusion");
        stylesheet.append(QStringLiteral(
            "QToolTip {"
            " color: white; "
            " background-color: #2a82da;"
            " border: 1px solid white;"
            "}"
//            "QCheckBox::indicator {"
//            " border: 1px solid gray;"
//            "}"
        ));
    }
    else
    {
        stylesheet.append(QStringLiteral(
            "QToolTip {"
            " color: black; "
            " background-color: white;"
            " border: 1px solid black;"
            "}"
        ));
    }

    w.setStyle(QStyleFactory::create(style));
    w.setStyleSheet(stylesheet);
    w.setPalette(gt::gui::currentTheme());
}

void
gt::gui::applyThemeToWidget(QWidget* w)
{
    assert(w);
    applyTheme(*w);
}

void
gt::gui::applyThemeToApplication()
{
    assert(qApp);
    applyTheme(*qApp);
}

