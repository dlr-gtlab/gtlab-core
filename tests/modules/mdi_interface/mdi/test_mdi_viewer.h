#ifndef TESTMDIVIEWER_H
#define TESTMDIVIEWER_H

#include "gt_mdiitem.h"


class GtGraphicsView;
/**
 * @brief The TestMdiViewer class
 */
class TestMdiViewer : public GtMdiItem
{
    Q_OBJECT

public:

    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestMdiViewer();
    ~TestMdiViewer() override;

    /**
     * @brief Test that only one instance of this item can be created
     * @return false
     */
    bool allowsMultipleInstances() const override;

private:

    /// Graphics view to test basic functionality
    GtGraphicsView* m_view;
};

#endif // TESTMDIVIEWER_H
