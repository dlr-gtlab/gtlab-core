#ifndef TESTMDIVIEWER_H
#define TESTMDIVIEWER_H

#include "gt_mdiitem.h"

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

    /**
     * @brief Test that only one instance of this item can be created
     * @return false
     */
    bool allowsMultipleInstances();

private:

};

#endif // TESTMDIVIEWER_H
