/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPERSPECTIVE_H
#define GTPERSPECTIVE_H

#include "gt_gui_exports.h"

#include "gt_object.h"

#include <QDir>

/**
 * @brief The GtPerspective class
 */
class GT_GUI_EXPORT GtPerspective : public GtObject
{
    friend class GtApplication;

public:

    /**
     * @brief saveGeometry
     * @param data
     */
    void saveGeometry(const QByteArray& data);

    /**
     * @brief saveState
     * @param data
     */
    void saveDockState(const QByteArray& data);

    /**
     * @brief saveState
     * @param data
     */
    void saveState(const QByteArray& data);

    /**
     * @brief loadGeometry
     * @return
     */
    QByteArray loadGeometry() const;

    /**
     * @brief saveState
     * @param data
     */
    QByteArray loadDockState() const;

    /**
     * @brief loadState
     * @return
     */
    QByteArray loadState() const;

protected:
    /**
     * @brief GtPerspective
     * @param id
     */
    explicit GtPerspective(const QString& id = QString());

    /**
     * @brief createDefault
     * @return
     */
    static bool createDefault();

    /**
     * @brief createEmptyPerspective
     * @param id
     * @return
     */
    static bool createEmptyPerspective(const QString& id);

    /**
     * @brief duplicatePerspective
     * @param source
     * @param target
     * @return
     */
    static bool duplicatePerspective(const QString& source,
                                     const QString& target);

    /**
     * @brief emptyFile
     * @param dir
     * @param filename
     * @return
     */
    static bool emptyFile(const QDir& dir, const QString& filename);

};

#endif // GTPERSPECTIVE_H
