#ifndef GTPOSTACTION_H
#define GTPOSTACTION_H

#include "QAction"

class GtPostAction : public QAction
{
    Q_OBJECT

public:
    GtPostAction();

    GtPostAction(QString str, QObject* o);
};

#endif // GTPOSTACTION_H
