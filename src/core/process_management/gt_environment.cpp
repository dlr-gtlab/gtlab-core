/* GTlab - Gas Turbine laboratory
 * Source File: gt_environment.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 03.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#include "gt_environment.h"

#include "gt_logging.h"
#include "gt_algorithms.h"

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

#include <algorithm>

GtEnvironment*
GtEnvironment::instance()
{
    static GtEnvironment* retval = nullptr;

    if (!retval)
    {
        retval = new GtEnvironment(qApp);
    }

    return retval;
}

QStringList
GtEnvironment::varIds()
{
    QStringList retval = m_vars.keys();
    retval.append(m_varsUndefined);

    retval.removeDuplicates();

    std::sort(std::begin(retval), std::end(retval));

    return retval;
}

GtEnvironment::GtEnvironment(QObject* parent) : QObject(parent)
{
    // test environment variable
    //    m_vars.insert(QStringLiteral("My Variable"), QStringLiteral("D:?"));
}

void
GtEnvironment::loadEnvironment()
{
    if (roamingPath().isEmpty())
    {
        return;
    }

    m_vars.clear();

    QString iniFileName = roamingPath() + QDir::separator() +
                          QStringLiteral("env.ini");

//    gtDebug() << "env ini = " << iniFileName;

    QSettings settings(iniFileName, QSettings::IniFormat);

    foreach (const QString& key, settings.allKeys())
    {
        if (m_varsUndefined.contains(key))
        {
            m_varsUndefined.removeOne(key);
        }

        m_vars.insert(key, settings.value(key));
    }

    //    QFile file(path);

    //    if (!file.exists())
    //    {
    //        gtDebug() << "environment settings not found!";
    //        return;
    //    }
}

void
GtEnvironment::debugEnvironmentVariables()
{
    gtDebug() << "ENVIRONMENT VARIABLES:";
    gtDebug() << "-DEFINED--------------";

    for_each_key (m_vars, [&](const QString& e)
    {
        gtDebug() << e << " = " << m_vars.value(e);
    });

    if (!m_varsUndefined.isEmpty())
    {
        gtDebug() << "-UNDEFINED------------";

        foreach (const QString& str, m_varsUndefined)
        {
            gtDebug() << str;
        }
    }

    gtDebug() << "----------------------";
}

bool
GtEnvironment::addEnvironmentVariable(const QString& var)
{
    if (m_vars.contains(var) || m_varsUndefined.contains(var))
    {
        return false;
    }

    m_varsUndefined << var;

    return true;
}

void
GtEnvironment::addEnvironmentVariables(const QStringList& vars)
{
    foreach (const QString& var, vars)
    {
        addEnvironmentVariable(var);
    }
}

bool
GtEnvironment::isUndefined(const QString& str)
{
    return m_varsUndefined.contains(str);
}

QVariant
GtEnvironment::value(const QString& var)
{
    if (m_vars.contains(var))
    {
        return m_vars.value(var);
    }

    return {};
}

bool
GtEnvironment::environmentVariableExists(const QString& var)
{
    return (m_vars.contains(var) || m_varsUndefined.contains(var));
}

void
GtEnvironment::setValue(const QString& var, const QVariant& val)
{
    if (!environmentVariableExists(var))
    {
        gtWarning() << tr("Could not save environment variable!") <<
                       QStringLiteral(" - ") << tr("Variable not found!");
        return;
    }

    if (val.isNull())
    {
        if (!isUndefined(var))
        {
            m_vars.remove(var);
            m_varsUndefined << var;
        }
    }
    else
    {
        if (!isUndefined(var))
        {
            m_vars[var] = val;
        }
        else
        {
            m_varsUndefined.removeOne(var);
            m_vars.insert(var, val);
        }
    }
}

void
GtEnvironment::saveEnvironment()
{
    if (roamingPath().isEmpty())
    {
        return;
    }

    QString iniFileName = roamingPath() + QDir::separator() +
                          QStringLiteral("env.ini");

//    gtDebug() << "env ini = " << iniFileName;

    QSettings settings(iniFileName, QSettings::IniFormat);

    settings.clear();

    for_each_key (m_vars, [&](const QString& e)
    {
        settings.setValue(e, m_vars.value(e));
    });
}

void
GtEnvironment::setRoamingDir(const QString& roamingDir)
{
    m_roamingDir = roamingDir;
}

QString
GtEnvironment::roamingPath()
{
    //    QString retval;

    //#ifdef _WIN32
    //    retval = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    //#endif
    //    retval = QStandardPaths::writableLocation(
    //               QStandardPaths::GenericConfigLocation);

    //    return retval;

    return m_roamingDir;
}
