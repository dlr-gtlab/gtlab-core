// SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
//
// SPDX-License-Identifier: MPL-2.0+

/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.11.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTEXAMPLEGRAPHICALITEM_H
#define GTEXAMPLEGRAPHICALITEM_H

#include "gt_examplesentry.h"

#include <QWidget>

class QLabel;
class QFrame;
class QPushButton;

class GtExamplesEntry;

/**
 * @brief The GtExampleGraphicalItem class
 * Class for graphical representation of Example Item
 */
class GtExampleGraphicalItem: public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtExampleGraphicalItem - Constructor
     * @param data
     * @param parent
     */
    explicit GtExampleGraphicalItem(GtExamplesEntry data, QWidget* parent = 0);

    GtExamplesEntry m_data;
protected:
    /**
     * @brief mousePressEvent
     * @param event
     */
    void mousePressEvent(QMouseEvent* event) override;

    /**
     * @brief enterEvent
     * @param event
     */
    void enterEvent(QEvent* event) override;

    /**
     * @brief leaveEvent
     * @param event
     */
    void leaveEvent(QEvent* event) override;

private:
    QPixmap m_pixmap;

    QLabel* m_picLabel;

    QFrame* m_picFrame;

    QPushButton* m_zoomButton;

    bool m_selected;

    QString m_description;

    /**
     * @brief init
     */
    void init();

private slots:
    /**
     * @brief showZoom
     */
    void showZoom();

signals:
    /**
     * @brief openProject
     */
    void openProject(QString);
};

#endif // GTEXAMPLEGRAPHICALITEM_H
