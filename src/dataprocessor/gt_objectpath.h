/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectpath.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 18.12.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_OBJECTPATH_H
#define GT_OBJECTPATH_H

#include "gt_datamodel_exports.h"

#include <QStringList>

class GtObject;

/**
 * @brief The GtObjectPath class
 */
class GT_DATAMODEL_EXPORT GtObjectPath
{
public:
    /**
     * @brief Constructor.
     */
    GtObjectPath();

    /**
     * @brief Constructor.
     * @param str Initialization string.
     */
    explicit GtObjectPath(const QString& str);

    /**
     * @brief Constructor. Generates objec tpath based on given object.
     * @param obj Initialization object.
     */
    explicit GtObjectPath(GtObject* obj);

    /**
     * @brief Returns object path separator.
     * @return Object path separator.
     */
    static QChar separator();

    /**
     * @brief Searches object based on object path inside given source object.
     * Returns null pointer if corresponding object was not found.
     * @param src Source object.
     * @return Object based on object path.
     */
    GtObject* getObject(GtObject* src) const;

    /**
     * @brief Returns object path converted to string.
     * @return Object path in form of a string.
     */
    QString toString() const;

    /**
     * @brief Returns object path converted to string list.
     * @return Object path in form of a string list.
     */
    QStringList toStringList() const;

    /**
     * @brief Returns true if objec tpath is empty.
     * @return empty state.
     */
    bool isEmpty() const;

private:
    /// Internal path data
    QStringList m_data;

    /**
     * @brief Generates object path data based ob given string.
     * @param str String to generate object path data.
     */
    void makePath(const QString& str);

};

#endif // GT_OBJECTPATH_H
