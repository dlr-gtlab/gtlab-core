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

#include <iostream>

GtCommandLineFunction
gt::makeCommandLineFunction(const QString& id,
                            GtCommandLineFunction::FunctionType func,
                            QString brief)
{
    return GtCommandLineFunction(id, std::move(func), std::move(brief));
}

bool
GtCommandLineFunction::useDefaultHelp() const
{
    return m_defaultHelp;
}

GtCommandLineFunction&
GtCommandLineFunction::setUseDefaultHelp(bool val)
{
    m_defaultHelp = val;
    return *this;
}

void
GtCommandLineFunction::showDefaultHelp() const
{
    std::cout << "Usage: GTlabConsole.exe " << id().toStdString();
    for (const GtCommandLineArgument& arg : arguments())
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

        for (const GtCommandLineOption& o : options())
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

        for (const GtCommandLineArgument& arg : arguments())
        {

            std::cout << "  " << arg.name.toStdString() << "\t\t"
                      << arg.description.toStdString() << std::endl;
        }
    }
}

GtCommandLineFunction&
GtCommandLineFunction::setOptions(const QList<GtCommandLineOption>& newOptions)
{
    m_options = newOptions;
    return *this;
}

GtCommandLineFunction&
GtCommandLineFunction::setArgs(const QList<GtCommandLineArgument>& newArgs)
{
    m_args = newArgs;
    return *this;
}

