#ifndef TESTMDIEXTVIEWER_H
#define TESTMDIEXTVIEWER_H

#include "gt_mdiitem.h"

/**
 * @brief The TestMdiExtViewer class
 */
class TestMdiExtViewer : public GtMdiItem
{
    Q_OBJECT

public:

    /**
     * @brief Constructor.
     */
    Q_INVOKABLE TestMdiExtViewer();

    /**
     * @brief Test that multiple one instance of this item can be created
     * @return true
     */
    bool allowsMultipleInstances();

private:

};

#endif // TESTMDIEXTVIEWER_H
