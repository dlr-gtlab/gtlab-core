/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 30.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPERSPECTIVESWITCHWIDGET_H
#define GTPERSPECTIVESWITCHWIDGET_H

#include <QDialog>

class QListWidget;

/**
 * @brief The GtPerspectiveSwitchWidget class
 */
class GtPerspectiveSwitchWidget : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief GtPerspectiveSwitchWidget
     * @param parent
     */
    explicit GtPerspectiveSwitchWidget(QWidget* parent = nullptr);

protected:
    /**
     * @brief keyPressEvent
     * @param event
     */
    void keyPressEvent(QKeyEvent * event) override;

    /**
     * @brief keyReleaseEvent
     * @param event
     */
    void keyReleaseEvent(QKeyEvent * event) override;

private:
    /// List of all perspective ids
    QListWidget* m_list;

    /**
     * @brief init
     */
    void init();

    /**
     * @brief nextPerspective
     */
    void nextPerspective();

};

#endif // GTPERSPECTIVESWITCHWIDGET_H