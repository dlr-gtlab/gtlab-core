/* GTlab - Gas Turbine laboratory
 * Source File: gt_stylesheets.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 30.01.2018
 *      Author: Jens Schmeink (AT-TWK)
 *		  Tel.: +49 2203 601 2191
 */
#ifndef GTSTYLESHEETS_H
#define GTSTYLESHEETS_H

#include "gt_mdi_exports.h"

#include <QString>
#include <QColor>

/**
 * @brief GtStyleSheets -
 * StyleSheets for typical use in GTlab to match the corporate design
 */
namespace GtStyleSheets
{
    /**
    * @brief buttonStyleSheet
    * @return the standard button style sheet (rounded edges,
    * white background, min-heigth 22...)
    */
    QString GT_MDI_EXPORT buttonStyleSheet();

    /**
    * @brief buttonStyleSheet
    * @return the standard button style sheet (rounded edges,
    * white background, min-heigth 20...)
    */
    QString GT_MDI_EXPORT buttonStyleSheet2();

    /**
     * @brief performanceTaskElementDelBtn
     * @return
     */
    QString GT_MDI_EXPORT performanceTaskElementDelBtn();

    /**
     * @brief processRunButton -
     *  Return the Style-Sheet for the run-process-button with a specification
     * for the color
     * @param stdBackgroundRGB - use a rgb-colorCode like "rgb(XXX,YYY,ZZZ)"
     * Example usage:
     *          GtStyleSheets::processRunButton("rgb(236,219,184)");
     * @return the stylesheet
     *
     */
    QString GT_MDI_EXPORT processRunButton(QString stdBackgroundRGB);

    /**
     * @brief performanceTaskLineEdit0
     * @return
     */
    QString GT_MDI_EXPORT performanceTaskLineEdit0();

    /**
     * @brief standardLineEdit -
     * (rounded corners, white backgorund...)
     * @return
     */
    QString GT_MDI_EXPORT standardLineEdit();

    /**
     * @brief warningLabel - returns styleSheet for QLabel
     * with white background and red letters
     * @return
     */
    QString GT_MDI_EXPORT warningLabel();

    /**
     * @brief standardLabel- returns styleSheet for QLabel
     * with white background and black letters
     * @return
     */
    QString GT_MDI_EXPORT standardLabel();

    /**
     * @brief selectionComboBox
     * @param minWidth
     * @param maxWidth
     * @return
     */
    QString GT_MDI_EXPORT selectionComboBox(QString minWidth, QString maxWidth);

    /**
     * @brief resultViewerTitleLabel - standard title line styleSheet for
     * resultviewer
     * @return
     */

    QString GT_MDI_EXPORT resultViewerTitleLabel();
    /**
     * @brief coloredCarpetPlotBtn - gives carpet plot button a color
     * @return
     */
    QString GT_MDI_EXPORT coloredCarpetPlotBtn(QColor col);

}

#endif // GTSTYLESHEETS_H
