/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPERSPECTIVE_H
#define GTPERSPECTIVE_H

#include "gt_gui_exports.h"

#include "gt_object.h"

/**
 * @brief The GtPerspective class
 */
class GT_GUI_EXPORT GtPerspective : public GtObject
{
    friend class GtApplication;

public:


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
     * @brief saveGeometry
     * @param data
     */
    void saveGeometry(const QByteArray& data);

    /**
     * @brief saveState
     * @param data
     */
    void saveState(const QByteArray& data);

    /**
     * @brief loadGeometry
     * @return
     */
    QByteArray loadGeometry();

    /**
     * @brief loadState
     * @return
     */
    QByteArray loadState();

    /**
     * @brief emptyFile
     * @param dir
     * @param filename
     * @return
     */
    static bool emptyFile(const QDir& dir, const QString& filename);

};

#endif // GTPERSPECTIVE_H
