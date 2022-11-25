/* GTlab - Gas Turbine laboratory
 * Source File: gt_commandlinefunction.cpp
 * copyright 2009-2022 by DLR
 *
 *  Created on: 14.09.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
//#include <QCommandLineParser>
#include "gt_commandlinefunction.h"

#include <QDebug>
#include <iostream>

gt::CommandLineFunction
gt::makeCommandLineFunction(const QString& id,
                            CommandLineFunction::FunctionType func,
                            QString brief)
{
    return gt::CommandLineFunction(id, std::move(func), std::move(brief));
}

bool
gt::CommandLineFunction::useDefaultHelp() const
{
    return m_defaultHelp;
}

gt::CommandLineFunction&
gt::CommandLineFunction::setUseDefaultHelp(bool val)
{
    m_defaultHelp = val;
    return *this;
}

void
gt::CommandLineFunction::showDefaultHelp() const
{
    std::cout << "Usage: GTlabConsole.exe " << id().toStdString();
    for (const gt::commandline::Argument& arg : arguments())
    {
        std::cout << " " << arg.name.toStdString();
    }
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << brief().toStdString() << std::endl;

    std::cout << std::endl;

    if (!options().isEmpty())
    {
        std::cout << "Options:" << std::endl;

        for (const gt::commandline::Option& o : options())
        {
            QString optionString;

            for (const QString& name : o.names)
            {
                if (name.size() > 1)
                {
                    optionString.append("--" + name + " ");
                }
                else
                {
                    optionString.append("-" + name + " ");
                }
            }

            std::cout << "  " << optionString.toStdString() << "\t\t"
                      << o.description.toStdString() << std::endl;
        }

        std::cout << std::endl;
    }
    if (!arguments().isEmpty())
    {
        std::cout << "Arguments:" << std::endl;

        for (const gt::commandline::Argument& arg : arguments())
        {

            std::cout << "  " << arg.name.toStdString() << "\t\t"
                      << arg.description.toStdString() << std::endl;
        }
    }
}

gt::CommandLineFunction&
gt::CommandLineFunction::setOptions(const QList<commandline::Option>& newOptions)
{
    m_options = newOptions;
    return *this;
}

gt::CommandLineFunction&
gt::CommandLineFunction::setArgs(const QList<commandline::Argument>& newArgs)
{
    m_args = newArgs;
    return *this;
}

