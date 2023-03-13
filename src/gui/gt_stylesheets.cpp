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
#include "gt_svgiconengine.h"

#include <QCoreApplication>

QString basicButtonStyleSheet(const QString& size = "min-width: 70px;"
                                                    "min-height:22px;",
                              const QString& border = "border: 1px solid gray;"
                                                      "border-radius: 4px;")
{
    return "QAbstractButton { "
            + border
            + size + " }";

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
    QString text = "QLineEdit {  border: 1px solid gray;"
                   "border-radius: 4px;}";

    return text;
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
    QString text = "QLineEdit {  "
                   "border: 0px solid gray;"
                   "border-radius: 4px;}";

    return text;
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
gt::gui::stylesheet::selectionComboBox(const QString& minWidth,
                                       const QString& maxWidth)
{
    return QStringLiteral(
            "QComboBox {"
            "   border: 1px solid gray;"
            "   border-radius: 4px;"
            "   min-width: %1px;"
            "   max-width: %2px;"
            "   min-height:15px"
            "}"
            "QComboBox::drop-down {"
            "   subcontrol-origin: padding;"
            "   subcontrol-position: top right;"
            "   width: 15px;"
            "   border-left-width: 1px;"
            "   border-left-color: darkgray;"
            "   border-left-style: solid;"
            "   border-top-right-radius: 3px;"
            "   border-bottom-right-radius: 3px;"
            "}"
            "QComboBox::down-arrow {"
            "   image: url(:/icons/stylesheets/arrowDown%3.svg);"
            "   width: 7px;"
            "   height: 7px;"
            "}"
    ).arg(minWidth, maxWidth, gtApp->inDarkMode() ? "_dark":"");
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
    return QStringLiteral("#frame {border-image: url(%1)}")
           .arg(gt::gui::pixmap::backgroundPath());
}
