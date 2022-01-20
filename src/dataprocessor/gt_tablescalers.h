#ifndef GT_TABLESCALERS_H
#define GT_TABLESCALERS_H

#include "gt_datamodel_exports.h"

class GT_DATAMODEL_EXPORT PIScalerFuncObj
{
public:
    /** Constructor */
    PIScalerFuncObj(double scalingFactor)
        : m_scaler(scalingFactor){}

    double operator()(double x)
    {
        return m_scaler * (x - 1.0) + 1.0;
    }

private:
    double m_scaler;
};

#endif // GT_TABLESCALERS_H
