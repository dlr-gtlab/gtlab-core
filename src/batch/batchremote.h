/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 17.03.2016
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#ifndef BATCHREMOTE_H
#define BATCHREMOTE_H

class QString;

/**
 * @brief The BatchRemote namespace
 */
namespace BatchRemote
{

    /**
     * @brief checks input-file if data can be restored and versions are correct
     * @param inputName - complete path (including the name) of input-file
     * @param version - Version of GTlab used to check if input-file is ok
     * @return
     */
    bool checkInput(const QString &inputName, const QString &version,
                    bool silent = false,
                    bool checkVersion = true);

    /**
     * @brief starts execution of calculators in the input-file (checks before)
     * @param inputName - complete path (including the name) of input-file
     * @param outputName - complete path (including the name) of output-file
     * @param version - Version of GTlab used to check if input-file is ok
     * @return
     */
    bool run(const QString& inputName, const QString& outputName,
             const QString& version);

}

#endif // BATCHREMOTE_H
