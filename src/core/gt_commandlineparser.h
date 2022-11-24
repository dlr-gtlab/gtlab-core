/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 15.09.022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTCOMMANDLINEPARSER_H
#define GTCOMMANDLINEPARSER_H

#include "gt_core_exports.h"

#include <QStringList>
#include <QVariant>

namespace gt
{

/**
 * @brief The gt::CommandLineFunctionOption struct
 * Struct to collect information for options to register connected to a
 * Option which can be used for a command line function
 *
 * This struct is only used for the default help function
 */
struct CommandLineOption
{
    CommandLineOption(){};

    CommandLineOption(QStringList const& names,
                      QString const& brief,
                      QVariant defaultValue = true) :
        names(names),
        description(brief),
        defaultValue(defaultValue)
    {

    }

    QStringList names;

    QString description;

    QVariant defaultValue;
};

/**
 * @brief The gt::CommandLineFunctionArgument class
 * Struct to collect information for arguments to register connected to a
 * Option which can be used for a command line function
 *
 * This struct is only used for the default help function
 */
struct CommandLineFunctionArgument
{
    QString name;

    QString description;
};

} // namespace gt

/**
 * @brief The GtCommandLineParser class
 */
class GT_CORE_EXPORT GtCommandLineParser
{
public:
    /**
     * @brief GtCommandLineParser - default
     */
    GtCommandLineParser() = default;

    /**
     * @brief parse
     * parse the command line arguments to check if the list is valid.
     * @param args - arguments of the command line
     * @return false for empty argumentslist
     */
    bool parse(const QStringList& args);

    /**
     * @brief option
     * checks if the option with the given id is active in the parsed arguments
     * @param id to check
     * @return true if the option is used in the argument list;
     */
    bool option(const QString& id) const;

    /**
     * @brief optionValue
     * reads the option value for a given id
     * @param id
     * @return the string of the value of the option
     */
    QVariant optionValue(const QString& id) const;

    /**
     * @brief argument
     * @param arg
     * @return
     */
    bool argument(const QString& arg) const;

    /**
     * @brief addOption - add a command line option to the parser
     * @param o - command line option
     */
    void addOption(const QString& id,
                   const gt::CommandLineOption& o);

    /**
     * @brief addOption
     * @param id - id to find the option in the parser
     * @param names - names to be used for the option in the command line
     * @param brief - breif description for the option
     */
    void addOption(const QString& id, const QStringList& names,
                   const QString& brief,
                   const QVariant& defaultValue = true);

    /**
     * @brief mainArgument
     * @return the main argument as the first argument which is not an option
     */
    QString firstPositionalArgument() const;

    /**
     * @brief removeArg
     * @param argument to remove
     */
    void removeArg(QString const& arg);

    /**
     * @brief debugArguments - print the current list of arguments of the parser
     */
    void debugArguments() const;

    /**
     * @brief addHelpOption
     * Adds the default help option
     */
    void addHelpOption();

    /**
     * @brief helpOption
     * @return true if the default help option is used
     * in the command line arguments
     */
    bool helpOption() const;

    /**
     * @brief checkSize
     * @param target
     * @return true if the size of arguments fits the target size
     */
    bool checkSize(int target) const;

    /**
     * @brief arguments
     * @return a const list of the arguments used by the parser
     * including the options
     */
    const QStringList& arguments() const;

    /**
     * @brief positionalArguments
     * @return filtered list of only positional arguments without options
     */
    QStringList positionalArguments();

    /**
     * @brief printHelp
     * prints a basic help based on the given options and arguments
     */
    void printHelp(const QString& commandName);
protected:
    QMap<QString, gt::CommandLineOption> m_opts;

private:
    QStringList m_args;
};

namespace gt
{
namespace commandline
{
namespace detail
{

/**
 * @brief indexOfOption
 * @param args - application arguments
 * @param o - option to check the index of
 * @return the index of the current option in the argument list args
 */
GT_CORE_EXPORT int indexOfOption(QStringList const& args,
                                 const gt::CommandLineOption& o);

/**
 * @brief valueOfOption
 * @param args - application arguments
 * @param o - option to check the value of
 * @return the value of the current option in the argument list args
 * The value is defined in the arguments by a "=" sign connected to the option
 */
GT_CORE_EXPORT QVariant valueOfOption(
        QStringList const& args,
        const gt::CommandLineOption& o);

/**
 * @brief hasOption
 * @param args - application arguments
 * @param o - option to check the index of
 * @return check if a option is part of the argument list args
 */
GT_CORE_EXPORT bool hasOption(QStringList const& args,
                              const gt::CommandLineOption& o);

} // namespace detail

} // namespace commandline

} // namespace gt

#endif // GTCOMMANDLINEPARSER_H
