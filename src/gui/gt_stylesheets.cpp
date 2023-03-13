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

#define BORDER             "border: 1px solid gray;"
#define NO_BORDER          "border: none;"

#define BORDER_RADIUS      "border-radius: 0px;"
#define NO_BORDER_RADIUS   "border-radius: 0px;"

#define CHECKBOX_INDICATOR_SIZE "width: 12px; height: 12px;"

#define SUBCONTROL_WIDTH "width: 15px;"
#define SUBCONTROL_INDICATOR_SIZE "width: 7px; height: 7px;"

#define SUBCONTROL_BORDER(DIR)  \
    "border-" #DIR "-width: 1px;" \
    "border-" #DIR "-color: darkgray;" \
    "border-" #DIR "-style: solid;"

#define SUBCONTROL_BORDER_RADIUS(DIR) \
    " border-" #DIR "-radius: 0px;"

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
gt::gui::stylesheet::lineEdit()
{
    return QStringLiteral(
        "QLineEdit {"
        BORDER
        BORDER_RADIUS
        "}"
        "QLineEdit:no-frame {"
        NO_BORDER
        NO_BORDER_RADIUS
        "}"
     );
}

QString
gt::gui::stylesheet::checkBox()
{
    return QStringLiteral(
        "QCheckBox::indicator {"
            BORDER
            CHECKBOX_INDICATOR_SIZE
        "}"
        "QCheckBox::indicator:checked {"
        " image: url(:/icons/stylesheets/checked%1.svg);"
            CHECKBOX_INDICATOR_SIZE
        "}"
        "QCheckBox::indicator:checked:pressed {"
        " background-color: %2;"
        " image: url(:/icons/stylesheets/checked%1.svg);"
            CHECKBOX_INDICATOR_SIZE
        "}"
        "QCheckBox::indicator:unchecked:pressed {"
        " background-color: %2;"
        "}"
        "QCheckBox::indicator:indeterminate {"
        " image: url(:/icons/stylesheets/checked_intermediate%1.svg);"
            CHECKBOX_INDICATOR_SIZE
        "}"
        "QCheckBox::indicator:indeterminate:pressed {"
        " background-color: %2;"
        " image: url(:/icons/stylesheets/checked_intermediate%1.svg);"
            CHECKBOX_INDICATOR_SIZE
        "}"
    ).arg(gtApp->inDarkMode() ? "_dark":"",
          gt::gui::color::disabled().darker(120).name());
}

QString comboBoxHelper(const QString& width = {})
{
    return QStringLiteral(
        "QComboBox {"
            BORDER
            BORDER_RADIUS
//        " min-height: 15px;"
        " %1"
        "}"
        "QComboBox::drop-down {"
//        " subcontrol-origin: padding;"
//        " subcontrol-position: top right;"
            SUBCONTROL_WIDTH
            SUBCONTROL_BORDER(left)
            SUBCONTROL_BORDER_RADIUS(top-right)
            SUBCONTROL_BORDER_RADIUS(bottom-right)
        "}"
        "QComboBox::down-arrow {"
        " image: url(:/icons/stylesheets/arrowDown%2.svg);"
            SUBCONTROL_INDICATOR_SIZE
        "}"
    ).arg(width, gtApp->inDarkMode() ? "_dark":"");
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
gt::gui::stylesheet::spinbox()
{
    return QStringLiteral(
        "QSpinBox {"
            BORDER
            BORDER_RADIUS
//        " min-height: 15px"
        "}"
        "QSpinBox::up-button {"
            SUBCONTROL_WIDTH
            SUBCONTROL_BORDER(left)
            SUBCONTROL_BORDER(bottom)
            SUBCONTROL_BORDER_RADIUS(top-right)
//        " width: 15px;"
//        " border-left-width: 1px;"
//        " border-left-color: darkgray;"
//        " border-left-style: solid;"
//        " border-top-right-radius: 4px;"
//        " border-bottom-width: 1px;"
//        " border-bottom-color: darkgray;"
//        " border-bottom-style: solid;"
        "}"
        "QSpinBox::down-button {"
            SUBCONTROL_WIDTH
            SUBCONTROL_BORDER(left)
            SUBCONTROL_BORDER_RADIUS(bottom-right)
//        " width: 15px;"
//        " border-left-width: 1px;"
//        " border-left-color: darkgray;"
//        " border-left-style: solid;"
//        " border-bottom-right-radius: 4px;"
        "}"
        "QSpinBox::down-arrow {"
        " image: url(:/icons/stylesheets/arrowDown%2.svg);"
            SUBCONTROL_INDICATOR_SIZE
        "}"
        "QSpinBox::up-arrow {"
        " image: url(:/icons/stylesheets/arrowUp%2.svg);"
            SUBCONTROL_INDICATOR_SIZE
        "}"
    ).arg(gtApp->inDarkMode() ? "_dark":"");
}

QString
gt::gui::stylesheet::pushButton()
{
    return QStringLiteral(
        "QPushButton {"
        " min-width: 70px;"
        " min-height: 20px;"
            BORDER
            BORDER_RADIUS
//        " border: 1px solid gray;"
//        " border-radius: 4px;"
        "}"
        "QPushButton:flat {"
            NO_BORDER
            NO_BORDER_RADIUS
        " min-width: 0px;"
        "}"
    );
}

QString basicButtonStyleSheet(
        const QString& size = QStringLiteral("min-width: 70px;"
                                             "min-height:22px;"),
        const QString& border = QStringLiteral("border: 1px solid gray;"
                                               "border-radius: 4px;"))
{
    return QStringLiteral(
        "QAbstractButton {"
        " %1"
        " %2"
        "}"
    ).arg(border, size);
}

QString basicHoverButtonStyleSheet()
{
    return "QAbstractButton:hover{background-color:"
           "rgb(180,200,200)}"
           "QAbstractButton:disabled {background-color:"
           "rgb(180,180,180)}"
           "QAbstractButton:pressed{background-color:"
           "rgb(180,213,213)}";
}

QString
gt::gui::stylesheet::buttonStyleSheet()
{
    return basicButtonStyleSheet() + basicHoverButtonStyleSheet();
}

QString
gt::gui::stylesheet::buttonStyleSheet2()
{
    QString text = "QPushButton { "
                   "border: 1px solid gray;"
                   "border-radius: 4px;"
                   "background-color:white;"
                   "min-width: 70px;"
                   "min-height:20px}";

    if (gtApp->inDarkMode())
    {
        text = basicButtonStyleSheet();
    }

    return text;
}

QString
gt::gui::stylesheet::performanceTaskElementDelBtn()
{
    QString text = "QPushButton { "
                   "border: 0px solid gray;"
                   "border-radius: 4px;"
                   "min-width: 22px;"
                   "min-height:22px}";
    return text;
}

QString
gt::gui::stylesheet::standardLineEdit()
{
    return lineEdit();
}

QString
gt::gui::stylesheet::warningLabel()
{
    if (gtApp->inDarkMode())
    {
        return "QLabel { background-color : black; color : red; }";
    }

    return "QLabel { background-color : white; color : red; }";
}

QString
gt::gui::stylesheet::standardLabel()
{
    if (gtApp->inDarkMode())
    {
        return "QLabel { background-color : black; color : white; }";
    }

    return "QLabel { background-color : white; color : black; }";
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
    QString backgroundColor;

    switch (state)
    {
    case RunButtonState::StopProcess:
        backgroundColor = "rgb(255,230,230)";
        break;
    case RunButtonState::QueueProcess:
        backgroundColor = "rgb(236,219,184)";
        break;
    case RunButtonState::NotSelected:
    default:
        backgroundColor = gtApp->inDarkMode() ? "rgb(180,200,200)" :
                                                "rgb(230,255,230)";
        break;
    }

    QString text = "QPushButton { border: 1px solid gray;"
                   "border-radius: 4px;"
                   "background-color:"+ backgroundColor +";"
                   "min-width: 70px;min-height:22px}";

    return text;
}

QString
gt::gui::stylesheet::resultViewerTitleLabel()
{
    if (gtApp->inDarkMode())
    {
        return "QLabel { background-color : black; color : white; }";
    }

    return "QLabel { background-color : #f2f3f5; color : black; }";
}

QString
gt::gui::stylesheet::coloredCarpetPlotBtn(const QColor& col)
{
    QString red = QString::number(col.toRgb().red());
    QString green = QString::number(col.toRgb().green());
    QString blue = QString::number(col.toRgb().blue());

    QString text = "QPushButton { border: 0px solid gray;"
                   "border-radius: 4px;"
                   "background-color:rgb(" + red + "," + green +
                                        "," + blue + ");"
                   "min-width: 22px;"
                   "max-width: 40px;"
                   "min-height:22px}";

    return text;
}

QString
gt::gui::stylesheet::backgroundFrame()
{
    return QStringLiteral("#frame {border-image: url(%1);}")
           .arg(gt::gui::pixmap::backgroundPath());
}
