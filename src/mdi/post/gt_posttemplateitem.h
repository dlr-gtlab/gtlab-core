#ifndef GTPOSTTEMPLATEITEM_H
#define GTPOSTTEMPLATEITEM_H

#include "gt_mdi_exports.h"

#include "gt_object.h"
#include "gt_abstractpostwidget.h"
#include "gt_abstractwizard.h"
#include "gt_datazonetablelist.h"


class GT_MDI_EXPORT GtPostTemplateItem : public GtObject
{
    Q_OBJECT

public:

    virtual GtAbstractPostWidget* createUI(GtPostTemplateItem* dm,
                                           QWidget* parent) = 0;


protected:
    GtPostTemplateItem();
};

#endif // GTPOSTTEMPLATEITEM_H
