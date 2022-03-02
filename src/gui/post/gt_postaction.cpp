#include "gt_postaction.h"

GtPostAction::GtPostAction() :
    QAction(QString(), nullptr)
{

}

GtPostAction::GtPostAction(const QString& str, QObject* o) :
    QAction(str, o)
{

}
