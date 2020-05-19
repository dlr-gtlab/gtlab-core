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

class QDir;
class QString;

class GtTestHelper
{
public:
    /**
     * @brief instance
     * @return
     */
    static GtTestHelper* instance();

    QDir newTempDir();

    void setRemoveTempPath(bool val);

    ~GtTestHelper();

private:
    bool m_removeTempPath;

    GtTestHelper();

    QString tempPath();

};

#endif // GTTESTHELPER_H
