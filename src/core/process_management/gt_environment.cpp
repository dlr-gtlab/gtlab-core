/* GTlab - Gas Turbine laboratory
 * Source File: gt_environment.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 03.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QCoreApplication>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

#include "gt_logging.h"

#include "gt_environment.h"

GtEnvironment*
GtEnvironment::instance()
{
    static GtEnvironment* retval = nullptr;

    if (retval == nullptr)
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

    qSort(retval);

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

    for (auto e : m_vars.keys())
    {
        gtDebug() << e << " = " << m_vars.value(e);
    }

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
    if (m_varsUndefined.contains(str))
    {
        return true;
    }

    return false;
}

QVariant
GtEnvironment::value(const QString& var)
{
    if (m_vars.contains(var))
    {
        return m_vars.value(var);
    }

    return QVariant();
}

bool
GtEnvironment::environmentVariableExists(const QString& var)
{
    if (m_vars.contains(var) || m_varsUndefined.contains(var))
    {
        return true;
    }

    return false;
}

void
GtEnvironment::setValue(const QString& var, QVariant val)
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

    for (auto e : m_vars.keys())
    {
        settings.setValue(e, m_vars.value(e));
    }
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
