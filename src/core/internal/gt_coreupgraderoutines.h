#ifndef GT_COREUPGRADEROUTINES_H
#define GT_COREUPGRADEROUTINES_H

#include "gt_moduleinterface.h"

namespace gt
{
namespace detail
{

namespace GtDataModelConverter
{
    // Upgrade logic to convert old process tree from project file to new
    // file based task handling.
    namespace to_2_0_0_alpha1
    {
        bool run(QDomElement& root, const QString& targetPath);
    } // namespace to200alpha1

} // namespace GtDataModelConverter

inline QList<VersionUpgradeRoutine> coreUpgradeRoutines()
{
	
    return
    {
        VersionUpgradeRoutine {GtVersionNumber(2, 0, 0, "alpha.1"),
                    &GtDataModelConverter::to_2_0_0_alpha1::run}
    };

};


} // namespace detail

} // namespace gt

#endif // GT_COREUPGRADEROUTINES_H
