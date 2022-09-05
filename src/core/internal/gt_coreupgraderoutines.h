#ifndef GT_COREUPGRADEROUTINES_H
#define GT_COREUPGRADEROUTINES_H

#include "gt_moduleinterface.h"

namespace gtlab
{
namespace internal
{

namespace GtDataModelConverter
{
    namespace to200
    {
        bool run(QDomElement& root, const QString& targetPath);
    }

}

inline QList<VersionUpgradeRoutine> coreUpgradeRoutines() {

    return
        {
//            VersionUpgradeRoutine {GtVersionNumber(2, 0, 0),
//                        &GtDataModelConverter::to200::run}
        };

};



} // namespace internal
} // namespace gtlab

#endif // GT_COREUPGRADEROUTINES_H
