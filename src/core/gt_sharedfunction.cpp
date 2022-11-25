
#include "gt_sharedfunction.h"
#include "internal/gt_sharedfunctionhandler.h"

gt::SharedFunction
gt::interface::getSharedFunction(const QString& moduleId,
                            const QString& ident)
{
    return gt::detail::SharedFunctionHandler::instance()
        .getSharedFunction(moduleId, ident);
}
