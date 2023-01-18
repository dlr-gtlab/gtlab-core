#ifndef GTLAB_UNIT_H
#define GTLAB_UNIT_H

#include "gt_datamodel_exports.h"

#include <QString>

class GT_DATAMODEL_EXPORT GtUnit
{
public:
    /// Sytem of measurement
    enum System {SI, Imperial, US};

    /// Unit category
    enum Category {Length,
                   Area,
                   Volume,
                   VolumeFlow,
                   Temperature,
                   Time,
                   Angle,
                   Velocity,
                   Mass,
                   Massflow,
                   Density,
                   Force,
                   Frequency,
                   Torque,
                   Pressure,
                   Energy,
                   SpecEnergy,
                   Power,
                   Inertia,
                   SpecificEnergyPerTemperature,
                   EnergyDensity,
                   PowerDensity,
                   PowerTempArea,
                   Custom,
                   NonDimensionalPercentage,
                   NonDimensional,
                   MassMomentInertia,
                   VGVGradients,
                   VGVGradientsPow2,
                   kinematicViscosity,
                   DataSize,
                   Default,
                   None
                  };

    GtUnit() = delete;
    ~GtUnit() = delete;

    static QString siUnit(Category);

    static QString categoryToString(Category cat);
};

#endif // GTLAB_UNIT_H
