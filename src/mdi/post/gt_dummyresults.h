#ifndef GTDUMMYRESULTS_H
#define GTDUMMYRESULTS_H

#include "gt_mdi_exports.h"

class GtDataZoneTable;

/**
 * @brief The GtDummyResults class
 */
class GT_MDI_EXPORT GtDummyResults
{
public:
    GtDummyResults();

    GtDataZoneTable* createDzt(int count_drops, int dim = 0);
};

#endif // GTDUMMYRESULTS_H
