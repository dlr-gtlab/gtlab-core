/* GTlab - Gas Turbine laboratory
 * Source File: gt_state.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 28.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSTATE_H
#define GTSTATE_H

#include "gt_core_exports.h"

#include <QPointer>
#include <QVariant>

#include "gt_object.h"

class GtVariantProperty;
class GtStateGroup;

/**
 * @brief The GtState class
 */
class GT_CORE_EXPORT GtState : public GtObject
{
    Q_OBJECT

    friend class GtStateGroup;

public:
    /**
     * @brief Destructor
     */
    ~GtState() override;

    /**
     * @brief Sets state value.
     * @param New state value.
     */
    void setValue(const QVariant& val, bool undoCommand = true);

    /**
     * @brief Returns current state value.
     * @return Current state value.
     */
    QVariant getValue() const;

    /**
     * @brief Returns whether state is valid or not.
     * @return State validation.
     */
    bool isValid() const;

    /**
     * @brief Sets current state identification string.
     * @param New state identification string.
     */
    void setId(const QString& val);

    /**
     * @brief Returns current state identification string.
     * @return Identification string.
     */
    QString id();

    /**
     * @brief Returns state path.
     * @return State path.
     */
    const QString& path() const;

protected:
    /**
     * @brief Constructor.
     * @param State identification string.
     * @param Initial state value.
     * @param State guardian.
     */
    GtState(const QString& id, const QString& path, const QVariant& initVal,
            GtObject* guardian, GtObject* parent);

    /**
     * @brief Returns property pointer.
     * @return Property pointer.
     */
    GtVariantProperty* property();

private:
    /// Variant property to store state value.
    GtVariantProperty* m_property;

    /// State guardian.
    QPointer<GtObject> m_guard;

    /// State path.
    QString m_path;

private slots:
    /**
     * @brief onPropertyChange
     */
    void onPropertyChange();

signals:
    /**
     * @brief valueChanged
     */
    void valueChanged(GtState*);
    void valueChanged(QVariant);

    /**
     * @brief renamed
     * @param oldId
     * @param newId
     */
    void idChanged(QString oldId, QString newId);

};

#endif // GTSTATE_H
