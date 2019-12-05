/* GTlab - Gas Turbine laboratory
 * Source File: gt_stategroup.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 29.04.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QApplication>
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

    foreach (GtState* state, states)
    {
        if (state->path() == path)
        {
            return state;
        }
    }

    GtState* retval = new GtState(id, path, initVal, guardian, this);

    loadState(retval);

    connect(retval, SIGNAL(valueChanged(GtState*)),
            SLOT(onStateChanged(GtState*)));

    return retval;
}

void
GtStateGroup::loadState(GtState* state)
{
    if (m_container == Q_NULLPTR)
    {
        return;
    }

    if (m_container->project() == Q_NULLPTR)
    {
        loadStateGlobal(state);
    }
    else
    {
        loadStateSpecific(state);
    }
}

void
GtStateGroup::loadStateGlobal(GtState* state)
{
    if (state == Q_NULLPTR)
    {
        return;
    }

    if (!findDirectChildren<GtState*>().contains(state))
    {
        return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.beginGroup(objectName());

    loadStateFromSettings(state, settings);
}

void
GtStateGroup::loadStateSpecific(GtState* state)
{
    if (state == Q_NULLPTR)
    {
        return;
    }

    if (!findDirectChildren<GtState*>().contains(state))
    {
        return;
    }

    GtProject* project = m_container->project();

    if (project == Q_NULLPTR)
    {
        return;
    }

    QString iniFileName = project->path() + QDir::separator() +
                          QStringLiteral("project.ini");

    QSettings settings(iniFileName, QSettings::IniFormat);

    settings.beginGroup(objectName());

    loadStateFromSettings(state, settings);
}

void
GtStateGroup::loadStateFromSettings(GtState* state, QSettings& settings)
{
    QVariant var = settings.value(state->path());

    if (var.isValid())
    {
        state->setValue(var, false);
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
    if (m_container == Q_NULLPTR)
    {
        return;
    }

    if (m_container->project() == Q_NULLPTR)
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
    if (state == Q_NULLPTR)
    {
        return;
    }

    if (!findDirectChildren<GtState*>().contains(state))
    {
        return;
    }

    QSettings settings(QSettings::IniFormat, QSettings::UserScope,
                       QApplication::organizationName(),
                       QApplication::applicationName());

    settings.beginGroup(objectName());

    saveStateToSettings(state, settings);
}

void
GtStateGroup::saveStateSpecific(GtState* state)
{
    if (state == Q_NULLPTR)
    {
        return;
    }

    if (!findDirectChildren<GtState*>().contains(state))
    {
        return;
    }

    GtProject* project = m_container->project();

    if (project == Q_NULLPTR)
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
    if (state == Q_NULLPTR)
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
