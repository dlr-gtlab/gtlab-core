/* GTlab - Gas Turbine laboratory
 * Source File: gt_aboutdialog.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.07.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTABOUTDIALOG_H
#define GTABOUTDIALOG_H

#include <QDialog>

/**
 * @brief The GtAboutLogo class
 */
class GtAboutLogo : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtAboutLogo
     * @param parent
     */
    explicit GtAboutLogo(QWidget* parent = Q_NULLPTR);

protected:
    /**
     * @brief paintEvent
     * @param e
     */
    void paintEvent(QPaintEvent* e);

    /**
     * @brief mouseDoubleClickEvent
     * @param event
     */
    void mouseDoubleClickEvent(QMouseEvent* event);

private:
    /**
     * @brief m_clicks
     */
    int m_clicks;

    /**
     * @brief m_pixmap
     */
    QPixmap m_pixmap;

};

/**
 * @brief The GtAboutDialog class
 */
class GtAboutDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent widget
     */
    explicit GtAboutDialog(QWidget* parent = Q_NULLPTR);

private slots:
    /**
     * @brief Exports footprint of current framework configuration.
     */
    void exportFootprint();

};

#endif // GTABOUTDIALOG_H
