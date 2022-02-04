#include "gt_postaction.h"

GtPostAction::GtPostAction() :
    QAction(QString(), Q_NULLPTR)
{

}

GtPostAction::GtPostAction(const QString& str, QObject* o) :
    QAction(str, o)
{

}
