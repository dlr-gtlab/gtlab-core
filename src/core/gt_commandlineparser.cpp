/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 15.09.022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_commandlineparser.h"
#include <QDebug>
#include <iostream>

bool
GtCommandLineParser::parse(const QStringList& args)
{
    if (args.isEmpty())
    {
        std::cout << "No arguments given" << std::endl;
        return false;
    }

    m_args = args;

    return true;
}

bool
GtCommandLineParser::option(const QString& id) const
{
    if (m_opts.contains(id))
    {
        return GtCommandLineParsing::hasOption(m_args, m_opts.value(id));
    }

    return false;
}

QVariant
GtCommandLineParser::optionValue(const QString& id) const
{
    if (!m_opts.contains(id))
    {
         return QVariant();
    }

    QVariant value =  GtCommandLineParsing::valueOfOption(
                m_args, m_opts.value(id));

    if (!value.isValid())
    {
        return m_opts.value(id).defaultValue;
    }

    return value;
}

bool
GtCommandLineParser::argument(QString const& arg) const
{
    return m_args.contains(arg);
}

void
GtCommandLineParser::addOption(QString const& id,
                               GtCommandLineOption const& o)
{
    m_opts.insert(id, o);
}

void
GtCommandLineParser::addOption(const QString& id,
                               const QStringList& names,
                               const QString& brief,
                               const QVariant& defaultValue)
{
    m_opts.insert(id, GtCommandLineOption(names, brief, defaultValue));
}


QString
GtCommandLineParser::firstPositionalArgument() const
{
    auto iter = std::find_if(std::begin(m_args), std::end(m_args),
                             [](const QString& e)
    {
        return !e.startsWith("-");
    });

    if (iter != std::end(m_args))
    {
        return *iter;
    }

    return {};
}

void
GtCommandLineParser::removeArg(QString const& arg)
{
    m_args.removeOne(arg);
}

void
GtCommandLineParser::debugArguments() const
{
    qWarning() << m_args;
}

void
GtCommandLineParser::addHelpOption()
{
    addOption("help",
              {"help", "h", "?"},
              "\tDisplays help on commandline options");
}

bool
GtCommandLineParser::helpOption() const
{
    return option("help");
}

bool
GtCommandLineParser::checkSize(int target) const
{
    return m_args.size() == target;
}

QStringList const&
GtCommandLineParser::arguments() const
{
    return m_args;
}

QStringList
GtCommandLineParser::positionalArguments()
{
    QStringList retVal;

    for (QString const& a : qAsConst(m_args))
    {
        if (!a.startsWith("-"))
        {
            retVal.append(a);
        }
    }

    return retVal;
}

bool
GtCommandLineParsing::hasOption(const QStringList& args,
                                const GtCommandLineOption& o)
{
    return (GtCommandLineParsing::indexOfOption(args, o) >= 0);
}

int
GtCommandLineParsing::indexOfOption(const QStringList& args,
                                    const GtCommandLineOption& o)
{
    QStringList optionStrings = o.names;

    for (QString const& s : qAsConst(optionStrings))
    {
        QString effectiveString = QString("--") + s;

        if (s.size() == 1)
        {
            effectiveString = QString("-") + s;
        }

        /// some of the options, arguments might have values defined by a
        /// "=" sign
        QStringList argsWithoutValues;
        for (const QString& s2 : args)
        {
            argsWithoutValues.append(s2.split("=").first());
        }

        if (argsWithoutValues.contains(effectiveString))
        {
            return argsWithoutValues.indexOf(effectiveString);
        }
    }

    return -1;
}

QVariant
GtCommandLineParsing::valueOfOption(const QStringList& args,
                                    const GtCommandLineOption& o)
{
    QStringList optionStrings = o.names;

    for (QString const& s : qAsConst(optionStrings))
    {
        QString effectiveString = QString("--") + s;

        if (s.size() == 1)
        {
            effectiveString = QString("-") + s;
        }

        /// some of the options, arguments might have values defined by a
        /// "=" sign
        QMap<QString, QVariant> optionsValues;

        for (const QString& s2 : args)
        {
            QStringList split = s2.split("=");

            if (split.size() > 1)
            {
                optionsValues.insert(split.constFirst(), split.at(1));
            }
            else
            {
                optionsValues.insert(split.constFirst(), QVariant());
            }

        }

        if (optionsValues.contains(effectiveString))
        {

            return optionsValues.value(effectiveString);
        }
    }

    return {};
}




