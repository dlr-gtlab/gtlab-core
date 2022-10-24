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

#include <memory>

class GtObjectMemento;

/**
 * @brief The GtDummyObject class
 */
class GtDummyObject : public GtObject
{
    Q_OBJECT

public:
    ~GtDummyObject() override;

    /**
     * @brief GtDummyObject
     * @param parent
     */
    explicit GtDummyObject(GtObject* parent = nullptr);

    /**
     * @brief origClassName
     * @return
     */
    QString origClassName() const;


    void importMemento(const GtObjectMemento& memento);
    void exportToMemento(GtObjectMemento& memento) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;
};

#endif // GTDUMMYOBJECT_H
