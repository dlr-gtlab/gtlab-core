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
#include <QRandomGenerator>

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
     * @brief generates a random string list (uuids)
     * @param length length of the list
     * @return list
     */
    QStringList randomStringList(int length);

    /**
     * @brief generates a random data vector
     * @tparam T type
     * @param length length of the list
     * @return list
     */
    template<typename T = double>
    QVector<T> randomDataVector(int length)
    {
        QVector<T> retVal;
        retVal.reserve(length);

        for (int i = 0; i < length; ++i)
        {
            retVal.append(static_cast<T>(
                              QRandomGenerator::global()->generateDouble()));
        }

        return retVal;
    }

    /**
     * @brief generates a linear data vector
     * @tparam T type
     * @param length length of the list
     * @return list
     */
    template<typename T = double>
    QVector<T> linearDataVector(int length, T start, T step)
    {
        QVector<T> retVal;
        retVal.reserve(length);

        T value = start;
        for (int i = 0; i < length; ++i)
        {
            retVal.append(value);
            value += step;
        }

        return retVal;
    }


    ~GtTestHelper();

private:

    GtTestHelper();

    QString tempPath();

};

#endif // GTTESTHELPER_H
