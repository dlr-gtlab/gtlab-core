/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

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
    enum Category {Default = 0,
                   /// physical units
                   Length,
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
                   MassMomentInertia,
                   VGVGradients,
                   VGVGradientsPow2,
                   kinematicViscosity,
                   Voltage,
                   Current,
                   Resistance,
                   Impedance,
                   /// non physical units
                   NonDimensionalPercentage = 64,
                   NonDimensional,
                   DataSize,
                   /// no unit
                   None = 128
                  };

    GtUnit() = delete;
    ~GtUnit() = delete;

    static QString siUnit(Category);

    static QString categoryToString(Category cat);
};

#endif // GTLAB_UNIT_H
