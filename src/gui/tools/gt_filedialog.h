/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 14.10.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTFILEDIALOG_H
#define GTFILEDIALOG_H

#include "gt_gui_exports.h"

#include <QWidget>

/**
 * @brief The GtFileDialog class
 */
class GT_GUI_EXPORT GtFileDialog
{
public:
    /**
     * @brief GtFileDialog
     */
    GtFileDialog();

    /**
     * @brief getOpenFileName
     * @param parent
     * @param caption
     * @param dir
     * @param filter
     * @param selectedFilter
     * @return
     */
    static QString getOpenFileName(QWidget* parent = nullptr,
                                   const QString& caption = QString(),
                                   const QString& dir = QString(),
                                   const QString& filter = QString(),
                                   QString* selectedFilter = nullptr);

    /**
     * @brief getSaveFileName
     * @param parent
     * @param caption
     * @param dir
     * @param filter
     * @param selectedFilter
     * @return
     */
    static QString getSaveFileName(QWidget* parent = nullptr,
                                   const QString& caption = QString(),
                                   const QString& dir = QString(),
                                   const QString& filter = QString(),
                                   const QString& initFilename = QString(),
                                   QString* selectedFilter = nullptr);

    /**
     * @brief getExistingDirectory
     * @param parent
     * @param caption
     * @param dir
     * @return
     */
    static QString getExistingDirectory(QWidget* parent = nullptr,
                                        const QString& caption = QString(),
                                        const QString& dir = QString());

private:
    /**
     * @brief readLastPath
     * @return
     */
    static QString readLastPath();

    /**
     * @brief writeLastPath
     * @param val
     */
    static void writeLastPath(const QString& val);

};

#endif // GTFILEDIALOG_H
