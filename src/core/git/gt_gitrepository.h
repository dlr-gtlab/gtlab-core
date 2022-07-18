/* GTlab - Gas Turbine laboratory
 * Source File: gt_gitrepository.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 18.07.2022
 *  Author: David Nelles (AT-TUR)
 *  Tel.: +495517092708
 */

#include "gt_object.h"
#include "gt_gitprocess.h"

#include <QDir>

#ifndef GT_GITREPOSITORY_H
#define GT_GITREPOSITORY_H

class GtGitRepository : public GtObject
{
    Q_OBJECT

public:

// Methods to create a GtGitRepository

    /**
     * @brief Clone a repository.
     * @param remote repository url.
     * @param localDirectory where the local repository should be placed.
     * @return GitRepository object. On failure returns nullptr.
     */
    static GtGitRepository* clone(const QString& remote, const QDir& localDirectory, GtObject *parent = nullptr);

    /**
     * @brief Initializes the given directory as repository
     * @param localDirectory which should become a repository
     * @param create defines if the direcotry should be created in case it does not already exist
     * @return GitRepository object. On failure returns nullptr.
     */
    static GtGitRepository* init(const QDir& localDirectory, bool create = false, GtObject *parent = nullptr);

    /**
     * @brief Opens an existing repository
     * @param directory which represents the repository
     * @return GitRepository object. On failure returns nullptr.
     */
    static GtGitRepository* open(const QDir& directory, GtObject *parent = nullptr);

    /**
     * @brief Retrieves if a directory is a repository (intitalized) or not.
     * @param directory which shuld be checked.
     * @return In case the repository is initialized true otherwise false.
     */
    static bool isRepository(const QDir &directory);

private:
    /**
     * @brief GitRepository constructor
     * @param path The Path of the repository
     * @param parent
     */
    GtGitRepository(const QDir& directory, GtObject *parent);

    /**
     * @brief To run subprocesses easily.
     * @param The process wich should be executed.
     * @param Argruments of the process
     * @return StdOutput and result of the subprocess.
     */
    GtGitProcess::ProcessOutput runProcess(const QString &program, const QStringList &args) const;

    /**
     * @brief The directory which represents the repository
     */
    const QDir m_directory;
};


#endif // GT_GITREPOSITORY_H
