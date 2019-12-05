#include "gt_posttemplate.h"

GtPostTemplate::GtPostTemplate()
{
    setObjectName("PostTemplate");
}

QList<GtPostTemplateItem*>
GtPostTemplate::items()
{
    return findDirectChildren<GtPostTemplateItem*>();
}

