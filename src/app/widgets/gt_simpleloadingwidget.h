/* GTlab - Gas Turbine laboratory
 * Source File: gt_simpleloadingwidget.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 29.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_SIMPLELOADINGWIDGET_H
#define GT_SIMPLELOADINGWIDGET_H

#include <QDialog>

#include "gt_abstractloadinghelper.h"

/**
 * @brief The GtSimpleLoadingWidget class
 */
class GtSimpleLoadingWidget : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief GtSimpleLoadingWidget
     */
    explicit GtSimpleLoadingWidget(GtAbstractLoadingHelper* helper,
                                   QWidget* parent = nullptr);

    /**
     * @brief ~GtSimpleLoadingWidget
     */
    virtual ~GtSimpleLoadingWidget();

protected:
    /**
     * @brief showEvent
     * @param event
     */
    virtual void showEvent(QShowEvent* event);

    /**
     * @brief closeEvent
     * @param event
     */
    virtual void closeEvent(QCloseEvent* event);

    /**
     * @brief keyPressEvent
     * @param event
     */
    virtual void keyPressEvent(QKeyEvent* event);

private:
    /// Loading animation
    QMovie* m_animation;

    /// Force close indicator
    bool m_forceClose;

    /// Loading helper
    GtAbstractLoadingHelper* m_helper;

private slots:
    /**
     * @brief onTimeOut
     */
    void onFinished();

};

#endif // GT_SIMPLELOADINGWIDGET_H
