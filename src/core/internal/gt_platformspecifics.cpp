#include <QString>

#include "gt_platformspecifics.h"

std::string
gt::detail::systemUsername()
{
    QString qstr = qEnvironmentVariable("USER");

    if (qstr.isEmpty())
    {
        qstr = qEnvironmentVariable("USERNAME");
    }

    return qstr.toStdString();
}
