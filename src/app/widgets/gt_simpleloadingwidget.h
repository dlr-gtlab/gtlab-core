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

#include "gt_dialog.h"

#include "gt_abstractloadinghelper.h"

/**
 * @brief The GtSimpleLoadingWidget class
 */
class GtSimpleLoadingWidget : public GtDialog
{
    Q_OBJECT

public:
    /**
     * @brief GtSimpleLoadingWidget
     */
    explicit GtSimpleLoadingWidget(GtAbstractLoadingHelper* helper,
                                   QWidget* parent = nullptr);
protected:
    /**
     * @brief showEvent
     * @param event
     */
    void showEvent(QShowEvent* event) override;

    /**
     * @brief closeEvent
     * @param event
     */
    void closeEvent(QCloseEvent* event) override;

    /**
     * @brief keyPressEvent
     * @param event
     */
    void keyPressEvent(QKeyEvent* event) override;

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
