#ifndef GT_COREUPGRADEROUTINES_H
#define GT_COREUPGRADEROUTINES_H

#include "gt_moduleinterface.h"

namespace gt
{
namespace detail
{

namespace GtDataModelConverter
{
namespace to_2_0_0
{

bool run(QDomElement& root, const QString& targetPath);

} // namespace to_2_0_0

} // GtDataModelConverter

inline QList<VersionUpgradeRoutine> coreUpgradeRoutines()
{
    return { };
};


} // namespace detail

} // namespace gt

#endif // GT_COREUPGRADEROUTINES_H
