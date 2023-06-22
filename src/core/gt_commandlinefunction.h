/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_commandlinefunction.h
 *
 *  Created on: 14.09.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTCOMMANDLINEFUNCTION_H
#define GTCOMMANDLINEFUNCTION_H

#include "gt_core_exports.h"

#include <QString>
#include <QVariant>

#include <functional>

#include "gt_commandlineparser.h"


class GT_CORE_EXPORT GtCommandLineFunction
{
public:
    using FunctionType = std::function<int(const QStringList&)>;

    /**
     * @brief Constructor for the GtCommandLineFunction class
     * The first three arguments are always needed the others are optional.
     *
     * They can also be set by chains like in this example:
     *
     *      auto fun = gt::makeCommandLineFunction(
     *           "test_function", test_command::mainFun,
     *           "brief description");
     *
     *      fun.setUseDefaultHelp(false).setArgs({args}).setOptions({opts});
     *
     * @param commandId - id of the command to call.
     *  It is recommended to avoid spaces
     * @param func - std::function which takes a QStringList as arguments
     * and returns an integer
     *      The function is meant to use the arguments of the application
     *      (without the commandId) and return a main-like return value as
     *      typically 0 for success and -1 for failure.
     *      An example is shown below
     *
     * @param brief description which is shown in the help system
     * @param options [optional]
     *      list of options to be displayed in the default help
     * @param args [optional]
     *      list of arguments to be dispplayed in the default help
     * @param useDefaultHelp [optional, default is true]
     *      if the parameter is set to true the command uses the default help
     *      which is generated by means of the brief description
     *      and the lists of options and arguments.
     *
     *      If it is set to false all arguments (including the help requesting
     *      arguments --help, -h and -? are passed the registered function).
     *      In this case the function func needs to handle
     *      a help request by its own.
     *
     *      It is recommended to use a structure like presented in this example:
     *      int
     *      mainFun(QStringList const& args)
     *      {
     *          if (args.isEmpty())
     *          {
     *              gtError() << "Empty argumentList";
     *              return -1;
     *          }
     *
     *          if (GtCommandLineParser::helpActive(args))
     *          {
     *              showHelp();
     *              return 0;
     *          }
     *
     *          gtInfo() << ("Hello World");
     *          return 0;
     *      }
     *
     */
    GtCommandLineFunction(QString commandId,
                        FunctionType func,
                        QString brief,
                        QList<GtCommandLineOption> options = {},
                        QList<GtCommandLineArgument> args = {},
                        bool useDefaultHelp = true) :
        m_f(std::move(func)),
        m_id(std::move(commandId)),
        m_commandBrief(std::move(brief)),
        m_options(std::move(options)),
        m_args(std::move(args)),
        m_defaultHelp(useDefaultHelp)
    {}

    GtCommandLineFunction() = default;

    /**
     * @brief Calls the function
     */
    int operator()(const QStringList& l) const
    {
        return m_f(l);
    }

    /**
     * @brief The name of the function
     */
    const QString& id() const
    {
        return m_id;
    }

    /**
     * @brief Returns the docstring of the commandline function
     */
    const QString& brief() const
    {
        return m_commandBrief;
    }

    /**
     * @brief Returns the options of the commandline function
     */
    const QList<GtCommandLineOption>& options() const
    {
        return m_options;
    }

    /**
     * @brief Returns the arguments of the commandline function
     */
    const QList<GtCommandLineArgument>& arguments() const
    {
        return m_args;
    }

    /**
     * @brief checks, whether the function is a nullptr
     */
    bool isNull() const
    {
        return !m_f;
    }

    // We want to have implicit bool checks aka if (myFunction)
    // NOLINTNEXTLINE
    operator bool() const
    {
        return !isNull();
    }

    bool operator==(nullptr_t) const
    {
        return isNull();
    }

    bool operator!=(nullptr_t) const
    {
        return !isNull();
    }

    /**
     * @brief useDefaultHelp
     * @return check if the registered command line function should use the
     * default help or not (and will parse the help option by its own
     * registered function)
     */
    bool useDefaultHelp() const;

    /**
     * @brief setUseDefaultHelp
     * Setter for the useDefaultHelp option
     * @param val
     */
    GtCommandLineFunction& setUseDefaultHelp(bool val);

    /**
     * @brief showDefaultHelp
     * Show the default help based on the description and
     * the list of GtCommandLineFunctionOption and GtCommandLineFunctionArgument
     * of the command line function
     */
    void showDefaultHelp() const;

    /**
     * @brief setOptions
     * @param newOptions - list of command line options for the help system
     */
    GtCommandLineFunction& setOptions(const QList<GtCommandLineOption>& newOptions);

    /**
     * @brief setArgs
     * @param newArgs - list of command line arguments for the help system
     */
    GtCommandLineFunction& setArgs(const QList<GtCommandLineArgument>& newArgs);

private:
    FunctionType m_f;
    QString m_id;
    QString m_commandBrief;
    QList<GtCommandLineOption> m_options;
    QList<GtCommandLineArgument> m_args;
    bool m_defaultHelp;
};

namespace gt
{

/**
 * @brief makeCommandLineFunction
 *
 * For details see the description of the GtCommandLineFunction class
 *
 * Returns a command line object with the parameters as given by the fucntion
 * arguments:
 * @param id is the name and the command line argument to
 * use in the batch application
 * @param func is a std::function with a stringlist as argument
 * and int as return value
 * @param brief description of the function
 * @param command line function object
 */
GT_CORE_EXPORT GtCommandLineFunction
makeCommandLineFunction(const QString& id,
                        GtCommandLineFunction::FunctionType func,
                        QString brief);

} // namespace gt

#endif // GTCOMMANDLINEFUNCTION_H
