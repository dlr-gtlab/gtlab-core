/* GTlab - Gas Turbine laboratory
 * Source File: gt_dummyobject.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 02.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTDUMMYOBJECT_H
#define GTDUMMYOBJECT_H

#include "gt_object.h"

#include <QVector>
#include <QVariant>

/**
 * @brief The GtDummyObject class
 */
class GtDummyObject : public GtObject
{
    Q_OBJECT

public:
    enum PROPERTY_TYPE
    {
        SIMPLE,
        LIST
    };

    /// Dummy property container
    struct DummyProperty
    {
        QString m_id;
        QString m_type;
        bool m_optional;
        bool m_active;
        QVariant m_val;
        PROPERTY_TYPE m_p_type;
    };

    /**
     * @brief GtDummyObject
     * @param parent
     */
    GtDummyObject(GtObject* parent = Q_NULLPTR);

    /**
     * @brief origClassName
     * @return
     */
    QString origClassName() const;

    /**
     * @brief setOrigClassName
     * @param origClassName
     */
    void setOrigClassName(const QString& origClassName);

    /**
     * @brief addDummyProperty
     * @param id
     * @param type
     * @param val
     */
    void addDummyProperty(const QString& id, const QString& type,
                          bool optional, bool active,
                          const QVariant& val);

    /**
     * @brief addDummyProperty
     * @param id
     * @param type
     * @param val
     */
    void addDummyPropertyList(const QString& id, const QString& type,
                              bool optional, bool active,
                              const QVariant& val);

    /**
     * @brief dummyProperties
     * @return
     */
    const QVector<DummyProperty>& dummyProperties() const;

private:
    /// original class name
    QString m_origClassName;

    /// List of dummy properties
    QVector<DummyProperty> m_d_props;

};

#endif // GTDUMMYOBJECT_H
