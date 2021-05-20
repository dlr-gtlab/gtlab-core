/* GTlab - Gas Turbine laboratory
 * Source File: gt_updatechecker.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 20.06.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_UPDATECHECKER_H
#define GT_UPDATECHECKER_H

#include "gt_core_exports.h"

#include <QObject>

/**
 * @brief The GtUpdateChecker class
 */
class GT_CORE_EXPORT GtUpdateChecker : public QObject
{
    Q_OBJECT

public:
    struct PackageInfo
    {
        QString m_id;
        QString m_name;
        QString m_currentVers;
        QString m_newVers;
    };

    /**
     * @brief Constructor.
     * @param parent Parent object
     */
    GtUpdateChecker(QObject* parent = Q_NULLPTR);

    /**
     * @brief Enables or disables extended info about available package updates
     * (e.g. id, current version, new version)
     * @param val If true, extended info is created.
     */
    void enableExtendedInfo(bool val);

    /**
     * @brief Returns list of available package updats.
     * @return List of available package updates.
     */
    QList<PackageInfo> pkgList() const;

public slots:
    /**
     * @brief Checks whether update exists or not.
     */
    void checkForUpdate();

private:
    /// List of available package updates.
    QList<PackageInfo> m_pkgList;

    /// Indicator whether extended info about package updates should be created
    /// or not.
    bool m_extendedInfo;

    /// Error code of update check procedure.
    int m_errorCode;

    /// Error message of update check procedure.
    QString m_errorMessage;

    /**
     * @brief Helper method to run update check procedure.
     * @return Return code of update check procedure.
     */
    int updateHelper();

    /**
     * @brief Reads extended info about available package updates.
     * (e.g. id, current version, new version)
     * @param mTool Maintenance tool path
     * @param pkgInfo Package info to store infos into.
     * @return Returns true if extended info was successfully stored, otherwise
     * false is returned.
     */
    bool readExtendedInfo(const QString& mTool, PackageInfo& pkgInfo);

signals:
    /**
     * @brief Emitted if check update process ends up with error.
     * @param errorCode Error Code
     * 1    Maintenance Tool no found
     * 2    Timed out
     * 3    Update file not found
     * 4    Could not open update file
     * 5    Connection refused
     * 6    No updates available
     * 7    Failure
     * 8    Error
     * @param errorStr Error string
     */
    void error(int errorCode, const QString& errorStr);

    /**
     * @brief Emitted if update exists.
     */
    void updateAvailable();

};

#endif // GT_UPDATECHECKER_H
