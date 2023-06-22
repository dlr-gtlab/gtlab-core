/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 17.03.2016
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */

#ifndef BATCHREMOTE_H
#define BATCHREMOTE_H

class QString;

namespace gt
{
namespace batch
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

} // namespace batch

} // namespace gt

#endif // BATCHREMOTE_H
