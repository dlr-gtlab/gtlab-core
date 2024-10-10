#ifndef GT_CONSOLEUPGRADEPROJECT_H
#define GT_CONSOLEUPGRADEPROJECT_H

#include <QStringList>

namespace gt
{
namespace console
{

/**
 * @brief upgradeRoutine
 * @param projectFile
 * @param projectFilePath
 * @param newProjectName
 * @return
 */
    int upgradeRoutine(const QString& projectFile,
                       const QString& projectFilePath = "");

/**
 * @brief upgrade_Project
 * @param args
 * @return
 */
int upgrade_Project(const QStringList& args);

}
}

#endif // GT_CONSOLEUPGRADEPROJECT_H
