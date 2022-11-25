/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 06.10.022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#include "gt_consoleparser.h"

#include <iostream>

void
GtConsoleParser::addPositionalArgument(GtCommandLineFunction const& f)
{
    m_funcs.append(f);
}

void
GtConsoleParser::showHelp() const
{
    std::cout << std::endl;
    std::cout << "This is the help function of GTlab Batch" << std::endl;

    std::cout << "Usage: GTlabConsole.exe [options] <command>" << std::endl;
    std::cout << std::endl;

    std::cout << "There are few general options to modify the"
                 " application execution" << std::endl;

    if (!m_opts.isEmpty())
    {
        std::cout << "Options:" << std::endl;

        for (const GtCommandLineOption& o : qAsConst(m_opts))
        {
            QString nameString;

            for (QString const& s : o.names)
            {
                if (s.size() == 1)
                {
                    nameString += QString(" -") + s;
                }
                else
                {
                    nameString += QString(" --") + s;
                }
            }

            std::cout << "  " << nameString.toStdString() << "\t";
            if (nameString.size() < 10)
            {
                std::cout << "\t";
            }

            std::cout << o.description.toStdString() << std::endl;
        }

        std::cout << std::endl;
    }
    if (!m_funcs.isEmpty())
    {
        std::cout << "GTlab and the included modules offer several commands:"
                  << std::endl;

        for (const GtCommandLineFunction& f : qAsConst(m_funcs))
        {

            std::cout << "  " << f.id().toStdString() << "\t\t"
                      << f.brief().toStdString() << std::endl;
        }

        std::cout << "  " << "load_module" << "\t\t"
                  << "Executes a test to load modules of the given arguments. "
                     "\n\t\t\tUsage: load_modules <module_file> "
                  << std::endl;
    }
}
