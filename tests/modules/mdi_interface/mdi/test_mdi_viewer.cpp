#include "test_mdi_viewer.h"

#include "gt_graphicsview.h"
#include "gt_graphicsscene.h"
#include "gt_grid.h"
#include "gt_logging.h"

#include <QVBoxLayout>

TestMdiViewer::TestMdiViewer()
{
    setObjectName("Test Mdi Viewer");

    GtGraphicsScene* scene = new GtGraphicsScene();
    m_view = new GtGraphicsView(scene);

    GtGrid* grid = new GtGrid(*m_view);
    grid->setShowAxis(false);
    grid->setGridWidth(10);
    grid->setGridHeight(10);

    m_view->setGrid(grid);

    QVBoxLayout* lay = new QVBoxLayout();
    lay->addWidget(m_view);

    widget()->setLayout(lay);
}

TestMdiViewer::~TestMdiViewer()
{
    gtDebug() << __FUNCTION__;
}

bool
TestMdiViewer::allowsMultipleInstances() const
{
    return false;
}
