/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_customprocesswizard.h
 *
 *  Created on: 17.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCUSTOMPROCESSWIZARD_H
#define GTCUSTOMPROCESSWIZARD_H

#include "gt_gui_exports.h"

#include <QObject>
#include <QPointer>
#include <QMap>
#include <QSize>

class GtProcessWizard;

/**
 * @brief The GtCustomProcessWizard class
 */
class GT_GUI_EXPORT GtCustomProcessWizard : public QObject
{
    Q_OBJECT

    friend class GtProcessWizard;

public:
    /**
     * @brief Constructor
     */
    GtCustomProcessWizard();

    /**
     * @brief Returns page count of custom prozess wizard.
     * @return Page count.
     */
    int pageCount();

    /**
     * @brief Returns list of all custom wizard pages.
     * @return List of all custom wizard pages.
     */
    QList<QMetaObject> pages();

    /**
     * @brief pageIds
     * @return
     */
    QList<int> pageIds() const;

    /**
     * @brief setPage
     * @param id
     * @param metaPage
     * @return
     */
    bool setPage(int id, const QMetaObject& metaPage);

    /**
     * @brief nextId
     * @return
     */
    virtual int nextId() const;

    /**
     * @brief size
     * @return
     */
    const QSize& size() const;

    /**
     * @brief setSize
     * @param size
     */
    void setSize(const QSize& size);

    /**
     * @brief setSize
     * @param w
     * @param h
     */
    void setSize(int w, int h);

protected:
    /**
     * @brief currentId
     * @return
     */
    int currentId() const;

private:
    /// Pages.
    QMap<int, QMetaObject> m_pages;

    /// Main wizard.
    QPointer<GtProcessWizard> m_wizard;

    /// Calculator size.
    QSize m_size;

};

#endif // GTCUSTOMPROCESSWIZARD_H
