/* GTlab - Gas Turbine laboratory
 * Source File: gt_stylesheets.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 30.01.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */

#include "gt_stylesheets.h"
#include "gt_application.h"
#include "gt_icons.h"
#include "gt_colors.h"
#include "gt_svgiconengine.h"

#include <QCoreApplication>

QString
gt::gui::stylesheet::button(const QColor& baseColor)
{
    return QString(
        "QAbstractButton{"
        " min-width: 70px;"
        " min-height:20px;"
        " border: 1px solid %1;"
        " border-radius: 4px;"
        " background-color: %4;"
        "}"
        "QAbstractButton:flat {"
        " border: none;"
        " border-radius: 0px;"
        "}"
        "QAbstractButton:hover{"
        " background-color: %2"
        "}"
        "QAbstractButton:pressed{"
        " background-color: %3;"
        "}"
        "QAbstractButton:disabled{"
        " background-color: %4;"
        "}"
    ).arg(color::frame().name(),
          color::darken(baseColor, 15).name(),
          color::darken(baseColor, 30).name(),
          baseColor.name());
}

QString
gt::gui::stylesheet::button()
{
    return button(color::main());
}

QString
gt::gui::stylesheet::buttonStyleSheet(const QColor& baseColor)
{
    return button(baseColor);
}

QString
gt::gui::stylesheet::buttonStyleSheet()
{
    return button();
}

QString
gt::gui::stylesheet::buttonStyleSheet2()
{
    return button(color::base());
}

QString
gt::gui::stylesheet::performanceTaskElementDelBtn()
{
    return QStringLiteral(
        "QPushButton { "
        " border: 0px solid %1;"
        " border-radius: 4px;"
        " min-width: 20px;"
        " min-height:20px"
        "}"
    ).arg(color::frame().name());
}

QString
gt::gui::stylesheet::standardLineEdit()
{
    return QStringLiteral(
        "QLineEdit { "
        " border: 1px solid %1;"
        " border-radius: 4px;"
        "}"
        "QLineEdit:no-frame {"
        " border: none;"
        " border-radius: 0px;"
        "}"
    ).arg(color::frame().name());
}

QString
gt::gui::stylesheet::warningLabel()
{
    return QStringLiteral("QLabel{ background-color: %1; color: %2; }")
           .arg(color::infoLabelBackground().name(), color::warningText().name());
}

QString
gt::gui::stylesheet::standardLabel()
{
    return QStringLiteral("QLabel{ background-color: %1; color: %2; }")
           .arg(color::infoLabelBackground().name(), color::text().name());
}

QString
gt::gui::stylesheet::titleLabel()
{
    return QStringLiteral("QLabel{ background-color: %1; color: %2; }")
           .arg(color::titleLabelBackground().name(), color::text().name());
}

QString
gt::gui::stylesheet::resultViewerTitleLabel()
{
    return titleLabel();
}

QString
gt::gui::stylesheet::performanceTaskLineEdit0()
{
    return QStringLiteral(
       "QLineEdit {"
       " border: none;"
       " border-radius: 4px;"
       "}"
    );
}

QString
gt::gui::stylesheet::processRunButton(RunButtonState const& state)
{
    QColor backgroundColor;

    switch (state)
    {
    case RunButtonState::StopProcess:
        backgroundColor = gtApp->inDarkMode() ?
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                              QColor(170, 100,  90) : QColor(255, 230, 230);
        break;
    case RunButtonState::QueueProcess:
        backgroundColor = gtApp->inDarkMode() ?
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                              QColor(190, 170,  90) : QColor(236, 219, 184);
        break;
    case RunButtonState::RunProcess:
        backgroundColor = gtApp->inDarkMode() ?
        // NOLINTNEXTLINE(cppcoreguidelines-avoid-magic-numbers, readability-magic-numbers)
                              QColor(180, 190, 200) : QColor(230, 255, 230);
        break;
    case RunButtonState::NotSelected:
    default:
        backgroundColor = color::main().name();
        break;
    }

    return button(backgroundColor);
}

QString
comboBoxHelper(const QString& width = {})
{
    return QStringLiteral(
            "QComboBox {"
            " border: 1px solid %1;"
            " border-radius: 4px;"
            " min-height: 15px;"
            " %2"
            "}"
            "QComboBox::drop-down {"
            " subcontrol-origin: padding;"
            " subcontrol-position: top right;"
            " width: 15px;"
            " border-left-width: 1px;"
            " border-left-color: %1;"
            " border-left-style: solid;"
            " border-top-right-radius: 3px;"
            " border-bottom-right-radius: 3px;"
            "}"
            "QComboBox::down-arrow {"
            " image: url(:/icons/stylesheets/arrowDown%3.svg);"
            " width: 7px;"
            " height: 7px;"
            "}"
    ).arg(gt::gui::color::frame().name(),
          width, gtApp->inDarkMode() ? "_dark" : QString{});
}

QString
gt::gui::stylesheet::comboBox()
{
    return comboBoxHelper();
}

QString
gt::gui::stylesheet::selectionComboBox(const QString& minWidth,
                                       const QString& maxWidth)
{
    return comboBoxHelper(QStringLiteral(
        "min-width: %1px;"
        "max-width: %2px;"
    ).arg(minWidth, maxWidth));
}

QString
gt::gui::stylesheet::toolTip()
{
    static const auto style = QStringLiteral(
        "QToolTip {"
        " color: %1; "
        " background-color: %2;"
        " border: 1px solid %1;"
        "}"
    );

    if (gtApp->inDarkMode())
    {
        return style.arg("white", gt::gui::color::highlight().name());
    }
    return style.arg("black", "white");
}

QString
gt::gui::stylesheet::spinbox()
{
    return QStringLiteral(
        "QSpinBox {"
        " border: 1px solid %1;"
        " border-radius: 4px;"
        " min-height:15px"
        "}"
        "QSpinBox::up-button {"
        " width: 15px;"
        " border-left-width: 1px;"
        " border-left-color: %1;"
        " border-left-style: solid;"
        " border-top-right-radius: 3px;"
        " border-bottom-width: 1px;"
        " border-bottom-color: %1;"
        " border-bottom-style: solid;"
        "}"
        "QSpinBox::down-button {"
        " width: 15px;"
        " border-left-width: 1px;"
        " border-left-color: %1;"
        " border-left-style: solid;"
        " border-bottom-right-radius: 3px;"
        "}"
        "QSpinBox::down-arrow {"
        " image: url(:/icons/stylesheets/arrowDown%2.svg);"
        " width: 7px;"
        " height: 7px;"
        "}"
        "QSpinBox::up-arrow {"
        " image: url(:/icons/stylesheets/arrowUp%2.svg);"
        " width: 7px;"
        " height: 7px;"
        "}"
    ).arg(color::frame().name(), gtApp->inDarkMode() ? "_dark" : QString{});
}

QString
gt::gui::stylesheet::coloredCarpetPlotBtn(const QColor& col)
{
    QString red = QString::number(col.toRgb().red());
    QString green = QString::number(col.toRgb().green());
    QString blue = QString::number(col.toRgb().blue());

    return QStringLiteral(
        "QPushButton { "
        " border: 0px solid %1;"
        " border-radius: 4px;"
        " background-color:rgb(%2, %3, %4);"
        " min-width: 22px;"
        " max-width: 40px;"
        " min-height:22px;"
        "}"
    ).arg(color::frame().name(), red, green, blue);
}

QString
gt::gui::stylesheet::backgroundFrame()
{
    return QStringLiteral("#frame {border-image: url(%1)}")
           .arg(gt::gui::pixmap::backgroundPath());
}
