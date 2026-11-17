#ifndef GT_COLLECTIONSPATH_H
#define GT_COLLECTIONSPATH_H

#include <QString>


namespace gt
{
namespace detail
{

/**
 * @brief Retrieve the current collectionsInstallPath
 * @param mkpath If true, create the required directory path
 * @return
 */
const QString collectionsInstallPath(bool mkpath = false);


}
}

#endif // GT_COLLECTIONSPATH_H
