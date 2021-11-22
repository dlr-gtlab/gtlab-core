/* GTlab - Gas Turbine laboratory
 * Source File: gt_testhelper.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 08.09.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTTESTHELPER_H
#define GTTESTHELPER_H

#define gtTestHelper (GtTestHelper::instance())

#include <QVector>

class QDir;
class QString;
class QStringList;

class GtTestHelper
{
public:
    /**
     * @brief instance
     * @return
     */
    static GtTestHelper* instance();

    QDir newTempDir();

    /**
     * @brief randomStringList generates a random string list (uuids)
     * @param length length of the list
     * @return list
     */
    QStringList randomStringList(int length);

    /**
     * @brief randomStringList generates a random doubles
     * @param length length of the list
     * @return list
     */
    QVector<double> randomDataVector(int length);

    ~GtTestHelper();

private:

    GtTestHelper();

    QString tempPath();

};

#endif // GTTESTHELPER_H
