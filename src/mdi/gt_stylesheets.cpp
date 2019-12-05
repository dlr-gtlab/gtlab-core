/* GTlab - Gas Turbine laboratory
 * Source File: gt_stylesheets.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 30.01.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */

#include "gt_stylesheets.h"


QString
GtStyleSheets::buttonStyleSheet()
{
    QString text = "QPushButton { border: 1px solid gray;"
                   "border-radius: 4px;"
                   "background-color:white;"
                   "min-width: 70px;"
                   "min-height:22px}"
                   "QPushButton:hover{background-color:"
                   "rgb(220,235,250)}"
                   "QPushButton:disabled {background-color:"
                   "rgb(240,240,240)}"
                   "QPushButton:pressed{background-color:"
                   "rgb(180,213,246)}";

    return text;
}

QString
GtStyleSheets::buttonStyleSheet2()
{
    QString text = "QPushButton { border: 1px solid gray;"
                   "border-radius: 4px;"
                   "background-color:white;"
                   "min-width: 70px;"
                   "min-height:20px}"
                   "QPushButton:hover{background-color:"
                   "rgb(220,235,250)}"
                   "QPushButton:disabled {background-color:"
                   "rgb(240,240,240)}"
                   "QPushButton:pressed{background-color:"
                   "rgb(180,213,246)}";

    return text;
}

QString
GtStyleSheets::performanceTaskElementDelBtn()
{
    QString text = "QPushButton { border: 0px solid gray;"
                   "border-radius: 4px;"
                   "background-color: white;"
                   "min-width: 22px;"
                   "min-height:22px}"
                   "QPushButton:hover{"
                   "background-color:rgb(220,235,250)}"
                   "QPushButton:disabled {"
                   "background-color:rgb(240,240,240)}"
                   "QPushButton:pressed{"
                   "background-color:rgb(180,213,246)}";

    return text;
}

QString
GtStyleSheets::standardLineEdit()
{
    QString text = "QLineEdit {  border: 1px solid gray;"
                   "border-radius: 4px;}";

    return text;
}

QString
GtStyleSheets::warningLabel()
{
    QString text = "QLabel { background-color : white; color : red; }";

    return text;
}

QString
GtStyleSheets::standardLabel()
{
    QString text = "QLabel { background-color : white; color : black; }";

    return text;
}
QString
GtStyleSheets::performanceTaskLineEdit0()
{
    QString text = "QLineEdit {  border: 0px solid gray;"
                   "border-radius: 4px;}"
                   "QLineEdit:hover{"
                   "background-color:rgb(220,235,250)}"
                   "QPushButton:pressed{"
                   "background-color:rgb(180,213,246)}";

    return text;
}

QString
GtStyleSheets::processRunButton(QString stdBackgroundRGB)
{
    QString text = "QPushButton { border: 1px solid gray;"
                   "border-radius: 4px;"
                   "background-color:"+ stdBackgroundRGB +";"
                   "min-width: 70px;min-height:22px}"
                   "QPushButton:hover{"
                   "background-color:rgb(220,235,250)}"
                   "QPushButton:disabled {"
                   "background-color:rgb(240,240,240)}"
                   "QPushButton:pressed{"
                   "background-color:rgb(180,213,246)}";

    return text;
}

QString
GtStyleSheets::selectionComboBox(QString minWidth, QString maxWidth)
{
    QString text = "QComboBox { border: 1px solid gray;"
                   "border-radius: 4px;"
                   "background-color: white;"
                   "min-width:" + minWidth + "px;max-width: " + maxWidth + "px;"
                   "min-height:15px}"
                   "QComboBox:hover{"
                   "background-color:rgb(220,235,250)}"
                   "QComboBox:disabled {"
                   "background-color:rgb(240,240,240)}"
                   "QComboBox::drop-down {"
                   "subcontrol-origin: padding;"
                   "subcontrol-position: top right;"
                   "width: 15px;border-left-width: 1px;"
                   "border-left-color: darkgray;"
                   "border-left-style: solid;"
                   "border-top-right-radius: 3px;"
                   "border-bottom-right-radius: 3px;}"
                   "QComboBox::down-arrow {"
                   "image: url(:/icons/arrowdownGrayIcon_7.png);"
                   "width: 7px;height: 7px;}";

    return text;
}

QString
GtStyleSheets::resultViewerTitleLabel()
{
    return "QLabel { background-color : #f2f3f5; color : black; }";
}

QString
GtStyleSheets::coloredCarpetPlotBtn(QColor col)
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
                   "min-height:22px}"
                   "QPushButton:hover{"
                   "background-color:rgb(220,235,250)}"
                   "QPushButton:disabled {"
                   "background-color:rgb(240,240,240)}"
                   "QPushButton:pressed{"
                   "background-color:rgb(180,213,246)}";

    return text;
}
