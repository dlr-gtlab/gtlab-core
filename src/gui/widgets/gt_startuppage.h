/* GTlab - Gas Turbine laboratory
 * Source File: gt_startuppage.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 19.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_STARTUPPAGE_H
#define GT_STARTUPPAGE_H

#include <QFrame>

/**
 * @brief The GtStartupPage class
 */
class GtStartupPage : public QFrame
{
    Q_OBJECT

public:
    /**
     * @brief GtStartupPage
     */
    GtStartupPage();

    /**
     * @brief ~GtStartupPage
     */
    virtual ~GtStartupPage();

    /**
     * @brief Virtual function to specify item specific icon.
     * @return Object specific icon.
     */
    virtual QIcon icon() const;

private slots:
    /**
     * @brief showIndicatorToggled
     * @param val
     */
    void showIndicatorToggled(bool val);

signals:
    /**
     * @brief newProject
     */
    void newProject();

    /**
     * @brief importProject
     */
    void importProject();

    /**
     * @brief helpContents
     */
    void helpContents();

    /**
     * @brief showInfo
     */
    void showInfo();

    /**
     * @brief openExamplesWidget
     */
    void openExamplesWidget();

};

#endif // GT_STARTUPPAGE_H
