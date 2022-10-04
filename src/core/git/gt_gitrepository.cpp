/* GTlab - Gas Turbine laboratory
 * Source File: gt_gitrepository.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 18.07.2022
 *  Author: David Nelles (AT-TUR)
 *  Tel.: +495517092708
 */

#include "gt_gitrepository.h"
#include "gt_gitprocess.h"

GtGitRepository*
GtGitRepository::clone(const QString &remote, const QDir& localDirectory, GtObject *parent)
{
    GtGitProcess::ProcessOutput out = GtGitProcess::ProcessOutput::logError(GtGitProcess::runProcess("git", {"clone", remote}, localDirectory.absolutePath()));

    if (out.sucessed)
    {
        QString directoryName = out.standardOutput.split("\n").first().split("'")[1];

        if (localDirectory.exists(directoryName))
        {
            return new GtGitRepository(localDirectory.absoluteFilePath(directoryName), parent);
        }
    }

    return nullptr;
}

GtGitRepository*
GtGitRepository::init(const QDir& localDirectory, bool create, GtObject *parent)
{
    // Create local directory in case it does not exist and create is true
    if (!localDirectory.exists() && create)
    {
        localDirectory.mkpath(localDirectory.absolutePath());
    }

    /// runProcess will automatically check if localDirectory exists
    if ( GtGitProcess::ProcessOutput::logError(GtGitProcess::runProcess("git", {"init"}, localDirectory.absolutePath())).sucessed)
    {
        return new GtGitRepository(localDirectory.absolutePath(), parent);
    }

    return nullptr;
}

GtGitRepository*
GtGitRepository::open(const QDir &directory, GtObject *parent)
{
    if (isRepository(directory))
    {
        return new GtGitRepository(directory, parent);
    }

    return nullptr;
}

bool
GtGitRepository::isRepository(const QDir &directory)
{
    return GtGitProcess::runProcess("git", {"status"}, directory.absolutePath()).sucessed;
}

GtGitRepository::GtGitRepository(const QDir& directory, GtObject *parent) : GtObject(parent),
    m_directory(directory)
    //indexWatcher(new QFileSystemWatcher(this)),
    //gitDirectoryWatcher(new GitIncrementFileSystemWatcher(this)),
    //repositoryWatcher(new GitIncrementFileSystemWatcher(this))
{
    // Set FileSystemWatcher paths
    const QString absolutePath = directory.absolutePath();
    //indexWatcher->addPath(absolutePath + "/.git/index");
    //gitDirectoryWatcher->addPath(absolutePath + "/.git");
    //repositoryWatcher->addPath(absolutePath);

    //connect(indexWatcher, &QFileSystemWatcher::fileChanged, this, &GitRepository::stagingAreaChanged);

    //connect(gitDirectoryWatcher, &GitIncrementFileSystemWatcher::fileChanged, this, &GitRepository::gitDirectoryChanged);
    //connect(gitDirectoryWatcher, &GitIncrementFileSystemWatcher::directoryChanged, this, &GitRepository::gitDirectoryChanged);
    //connect(gitDirectoryWatcher, &GitIncrementFileSystemWatcher::fileChanged, this, &GitRepository::update);
    //connect(gitDirectoryWatcher, &GitIncrementFileSystemWatcher::directoryChanged, this, &GitRepository::update);

    //connect(repositoryWatcher, &GitIncrementFileSystemWatcher::fileChanged, this, &GitRepository::repositoryChanged);
    //connect(repositoryWatcher, &GitIncrementFileSystemWatcher::directoryChanged, this, &GitRepository::repositoryChanged);

    //update();
}

GtGitProcess::ProcessOutput
GtGitRepository::runProcess(const QString &program, const QStringList &args) const
{
    return GtGitProcess::runProcess(program, args, m_directory);
}
