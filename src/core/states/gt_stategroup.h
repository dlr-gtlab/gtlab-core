/* GTlab - Gas Turbine laboratory
 * Source File: gt_stategroup.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 29.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSTATEGROUP_H
#define GTSTATEGROUP_H

#include <QPointer>

#include "gt_object.h"
#include "gt_core_exports.h"

class QSettings;
class GtStateContainer;
class GtState;

/**
 * @brief The GtStateGroup class
 */
class GT_CORE_EXPORT GtStateGroup : public GtObject
{
    Q_OBJECT

    friend class GtStateContainer;

public:
    /**
     * @brief Returns state list.
     * @return State list.
     */
    QList<GtState*> states();

private:
    /// Parent state container
    QPointer<GtStateContainer> m_container;

    /**
     * @brief Constructor.
     * @param Parent state container.
     */
    explicit GtStateGroup(GtStateContainer* parent);

    /**
     * @brief findState
     * @param path
     * @return
     */
    GtState* findState(const QString& id, const QString& path,
                       const QVariant& initVal, GtObject* guardian);

    /**
     * @brief Loads state. Location depends on state container settings.
     * @param State.
     */
    void loadState(GtState* state);

    /**
     * @brief loadStateGlobal
     * @param state
     */
    void loadStateGlobal(GtState* state);

    /**
     * @brief loadStateSpecific
     * @param state
     */
    void loadStateSpecific(GtState* state);

    /**
     * @brief loadStateFromSettings
     * @param state
     * @param settings
     */
    void loadStateFromSettings(GtState* state, QSettings& settings);

    /**
     * @brief Saves state. Location depends on state container settings.
     * @param State.
     */
    void saveState(GtState* state);

    /**
     * @brief saveStateGlobal
     * @param state
     */
    void saveStateGlobal(GtState* state);

    /**
     * @brief saveStateSpecific
     * @param state
     */
    void saveStateSpecific(GtState* state);

    /**
     * @brief saveStateToSettings
     * @param state
     * @param settings
     */
    void saveStateToSettings(GtState* state, QSettings& settings);

private slots:
    /**
     * @brief onStateChanged
     * @param state
     */
    void onStateChanged(GtState* state);

};

#endif // GTSTATEGROUP_H
