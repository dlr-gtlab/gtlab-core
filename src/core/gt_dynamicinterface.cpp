
#include "gt_dynamicinterface.h"
#include "internal/gt_dynamicinterfacehandler.h"

gtlab::InterfaceFunction gtlab::interface::get_function(const QString& moduleId,
                               const QString &ident)
{
    return gtlab::internal::DynamicInterfaceHandler::instance()
        .getInterfaceFunc(moduleId, ident);
}
