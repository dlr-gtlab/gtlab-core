/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 06.10.022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTCONSOLEPARSER_H
#define GTCONSOLEPARSER_H

#include "gt_commandlineparser.h"
#include "gt_commandlinefunction.h"

/**
 * @brief The GtConsoleParser class
 * Specific parser for the main function of the GTlab batch application
 */
class GtConsoleParser : public GtCommandLineParser
{
public:
    GtConsoleParser() = default;

    /**
     * @brief addPositionalArgument
     * @param f - gt commandline function
     */
    void addPositionalArgument(const GtCommandLineFunction& f);

    /**
     * @brief showHelp - show the help of the GTlab batch application
     */
    void showHelp() const;

private:
     QList<GtCommandLineFunction> m_funcs;
};

#endif // GTCONSOLEPARSER_H
