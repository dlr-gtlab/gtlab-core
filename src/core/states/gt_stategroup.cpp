/* GTlab - Gas Turbine laboratory
 * Source File: gt_stategroup.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 29.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QSettings>
#include <QDir>

#include "gt_statecontainer.h"
#include "gt_state.h"
#include "gt_project.h"

#include "gt_stategroup.h"

GtStateGroup::GtStateGroup(GtStateContainer* parent) : GtObject(parent),
    m_container(parent)
{

}

GtState*
GtStateGroup::findState(const QString& id, const QString& path,
                        const QVariant& initVal, GtObject* guardian)
{
    QList<GtState*> states = findDirectChildren<GtState*>();

    auto iter = std::find_if(std::begin(states), std::end(states),
                             [&path](const GtState* state) {
        return state->path() == path;
    });

    if (iter != std::end(states)) return *iter;

    GtState* retval = new GtState(id, path, initVal, guardian, this);

    loadState(*retval);

    connect(retval, SIGNAL(valueChanged(GtState*)),
            SLOT(onStateChanged(GtState*)));

    return retval;
}

void
GtStateGroup::loadState(GtState& state)
{
    if (!m_container->project())
    {
        loadStateGlobal(state);
    }
    else
    {
        loadStateSpecific(state);
    }
}

void
GtStateGroup::loadStateGlobal(GtState& state)
{

    if (!findDirectChildren<GtState*>().contains(&state))
    {
        return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());

    settings.beginGroup(objectName());

    loadStateFromSettings(settings, state);
}

void
GtStateGroup::loadStateSpecific(GtState& state)
{

    if (!findDirectChildren<GtState*>().contains(&state))
    {
        return;
    }

    GtProject* project = m_container->project();

    if (!project)
    {
        return;
    }

    QString iniFileName = project->path() + QDir::separator() +
                          QStringLiteral("project.ini");

    QSettings settings(iniFileName, QSettings::IniFormat);

    settings.beginGroup(objectName());

    loadStateFromSettings(settings, state);
}

void
GtStateGroup::loadStateFromSettings(const QSettings& settings, GtState& state)
{
    QVariant var = settings.value(state.path());

    if (var.isValid())
    {
        state.setValue(var, false);
    }
}

QList<GtState*>
GtStateGroup::states()
{
    return findDirectChildren<GtState*>();
}

void
GtStateGroup::saveState(GtState* state)
{
    if (!m_container)
    {
        return;
    }

    if (!m_container->project())
    {
        saveStateGlobal(state);
    }
    else
    {
        saveStateSpecific(state);
    }
}

void
GtStateGroup::saveStateGlobal(GtState* state)
{
    if (!state)
    {
        return;
    }

    if (!findDirectChildren<GtState*>().contains(state))
    {
        return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QCoreApplication::organizationName(),
                       QCoreApplication::applicationName());

    settings.beginGroup(objectName());

    saveStateToSettings(state, settings);
}

void
GtStateGroup::saveStateSpecific(GtState* state)
{
    if (!state)
    {
        return;
    }

    if (!findDirectChildren<GtState*>().contains(state))
    {
        return;
    }

    GtProject* project = m_container->project();

    if (!project)
    {
        return;
    }

    QString iniFileName = project->path() + QDir::separator() +
                          QStringLiteral("project.ini");

    QSettings settings(iniFileName, QSettings::IniFormat);

    settings.beginGroup(objectName());

    saveStateToSettings(state, settings);
}

void
GtStateGroup::saveStateToSettings(GtState* state, QSettings& settings)
{
    if (!state)
    {
        return;
    }

    settings.setValue(state->path(), state->getValue());
}

void
GtStateGroup::onStateChanged(GtState* state)
{
    saveState(state);
}
