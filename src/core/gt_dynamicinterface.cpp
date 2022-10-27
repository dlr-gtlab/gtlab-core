
#include "gt_dynamicinterface.h"
#include "internal/gt_dynamicinterfacehandler.h"

gt::InterfaceFunction
gt::interface::getFunction(const QString& moduleId,
                            const QString& ident)
{
    return gt::detail::DynamicInterfaceHandler::instance()
        .getInterfaceFunc(moduleId, ident);
}
