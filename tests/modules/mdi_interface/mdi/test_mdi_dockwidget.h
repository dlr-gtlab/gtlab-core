#ifndef TESTMDIDOCKWIDGET_H
#define TESTMDIDOCKWIDGET_H

#include "gt_dockwidget.h"

class QLabel;
class TestMdiDockWidget : public GtDockWidget
{
    Q_OBJECT

public:

    Q_INVOKABLE TestMdiDockWidget();

    /** Returns the recommended dock widget area in main windows.
        @return dock widget area */
    Qt::DockWidgetArea getDockWidgetArea() override;

private:

    QLabel* m_testLabel;
};

#endif // TESTMDIDOCKWIDGET_H
