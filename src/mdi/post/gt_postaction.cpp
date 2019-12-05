#include "gt_postaction.h"

GtPostAction::GtPostAction() :
    QAction(QString(), Q_NULLPTR)
{

}

GtPostAction::GtPostAction(QString str, QObject* o) :
    QAction(str, o)
{

}
