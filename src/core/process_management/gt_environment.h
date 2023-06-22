/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_environment.h
 *
 *  Created on: 03.07.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTENVIRONMENT_H
#define GTENVIRONMENT_H

#include "gt_core_exports.h"

#include <QObject>

#include <QVariant>

#define gtEnvironment (GtEnvironment::instance())

/**
 * @brief The GtEnvironment class
 */
class GT_CORE_EXPORT GtEnvironment : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Returns instance of singleton.
     * @return Simgleton object
     */
    static GtEnvironment* instance();

    /**
     * @brief Returns identification strings of all environment variables.
     * @return Environment variable identification strings.
     */
    QStringList varIds();

    /**
     * @brief Saves environment settings to roaming directory.
     */
    void saveEnvironment();

    /**
     * @brief Sets roaming directory.
     * @param New roaming directory path
     */
    void setRoamingDir(const QString& roamingDir);

    /**
     * @brief Loads environment settings from roaming directory.
     */
    void loadEnvironment();

    /**
     * @brief Debugs environment variables.
     */
    void debugEnvironmentVariables();

    /**
     * @brief Adds a new environment variable.
     * @param var Environment variable identification string
     * @return Returns true if new environment variable was successfully
     * created
     */
    bool addEnvironmentVariable(const QString& var);

    /**
     * @brief Adds a list of new environment variables.
     * @param vars List of environment variables.
     */
    void addEnvironmentVariables(const QStringList& vars);

    /**
     * @brief Checks whether given environment variable is undefined.
     * @param str Environment variable identification string
     * @return Returns true if environment variable is undefined. Otherwise
     * false is returned.
     */
    bool isUndefined(const QString& str);

    /**
     * @brief Returns value of given environment variable.
     * @param str Environment variable identification string
     * @return Value of given environment variable
     */
    QVariant value(const QString& var);

    /**
     * @brief Checks whether environment variable already exists.
     * @param var Environment variable identification string
     * @return Returns true if environment variable already exists. Otherwise
     * false is returned.
     */
    bool environmentVariableExists(const QString& var);

    /**
     * @brief Sets value of given environment variable.
     * @param var Environment variable identification string
     * @param val New environment variable value
     */
    void setValue(const QString& var, const QVariant& val);

private:
    /// Hash of environment variables
    QVariantHash m_vars;

    /// Undefined environment variables
    QStringList m_varsUndefined;

    /// Roaming directory
    QString m_roamingDir;

    /**
     * @brief Constructor.
     * @param Parent object
     */
    explicit GtEnvironment(QObject* parent = nullptr);

    /**
     * @brief Returns current roaming directory path.
     * @return Current roaming directory path
     */
    QString roamingPath();

};

#endif // GTENVIRONMENT_H
