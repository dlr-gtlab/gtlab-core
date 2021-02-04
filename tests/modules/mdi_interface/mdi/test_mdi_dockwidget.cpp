#include "test_mdi_dockwidget.h"

#include <QLabel>

TestMdiDockWidget::TestMdiDockWidget()
{
    setObjectName(tr("Test Dock Widget"));

    m_testLabel = new QLabel(QStringLiteral("Test Label"));

    setWidget(m_testLabel);
}

Qt::DockWidgetArea
TestMdiDockWidget::getDockWidgetArea()
{
    return Qt::RightDockWidgetArea;
}
