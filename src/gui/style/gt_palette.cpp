/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_palette.cpp
 *
 *  Created on: 06.11.2020
 *  Author: Jens Schmeink (AT-TW)
 *  Tel.: +49 2203 601 2191
 */
#include "gt_palette.h"

#include "gt_colors.h"
#include "gt_style.h"
#include "gt_stylesheets.h"
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
    /// QPalette::Dark: Apperantly affects the lower part of a raised frame
    palette.setColor(QPalette::Dark,
                     gt::gui::color::darken(config.main, 40));

    /// QPalette::Mid:
    palette.setColor(QPalette::Mid,
                     gt::gui::color::darken(config.main, 20));

    /// QPalette::Midlight: ???

    /// QPalette::Light: Apperantly affects the top part of a raised frame
    palette.setColor(QPalette::Light,
                     gt::gui::color::lighten(
                         gt::gui::color::desaturate(config.main, 0.5), 20));

    /// QPalette::Shadow: ???

    // highlights
    palette.setColor(QPalette::Highlight, config.highlight);
    palette.setColor(QPalette::HighlightedText, config.textHighlighted);

    // links
    palette.setColor(QPalette::Link, config.link);
    palette.setColor(QPalette::LinkVisited, config.linkVisited);

    // general text
    palette.setColor(QPalette::Text, config.text);
    palette.setColor(QPalette::BrightText, config.text);
    /// QPalette::WindowText: Apperantly also affects the color of QFrame::Plain
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

namespace
{
    GtStyle& getStyle()
    {
        static GtStyle s;
        return s;
    }
}


template <typename Widget>
void setStyle(Widget& w);

template <>
void setStyle(QWidget& w)
{
    w.setStyle(&getStyle());
}

template <>
void setStyle(QApplication& w)
{
    w.setStyle(new GtStyle());
}

template <typename Widget>
inline void applyTheme(Widget& w)
{
    QString stylesheet = gt::gui::stylesheet::toolTip();

    setStyle(w);
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

