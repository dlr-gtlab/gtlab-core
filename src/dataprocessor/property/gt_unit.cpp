#include "gt_unit.h"
#include <QObject>

QString
GtUnit::siUnit(Category c)
{
    switch (c)
    {
    case Category::Area:
        return QStringLiteral("m^2");
    case Category::Custom:
        return QStringLiteral("-");
    case Category::Default:
        return QStringLiteral("-");
    case Category::Density:
        return QStringLiteral("kg/m^3");
    case Category::Energy:
        return QStringLiteral("J");
    case Category::SpecEnergy:
        return QStringLiteral("J/kg");
    case Category::Force:
        return QStringLiteral("N");
    case Category::Inertia:
        return QStringLiteral("kg*m^2");
    case Category::Length:
        return QStringLiteral("m");
    case Category::Mass:
        return QStringLiteral("kg");
    case Category::Massflow:
        return QStringLiteral("kg/s");
    case Category::Power:
        return QStringLiteral("W");
    case Category::Temperature:
        return QStringLiteral("K");
    case Category::Time:
        return QStringLiteral("s");
    case Category::Torque:
        return QStringLiteral("N*m");
    case Category::Pressure:
        return QStringLiteral("Pa");
    case Category::Angle:
        return QStringLiteral("rad");
    case Category::Volume:
        return QStringLiteral("m^3");
    case Category::VolumeFlow:
        return QStringLiteral("m^3/s");
    case Category::Frequency:
        return QStringLiteral("1/s");
    case Category::Velocity:
        return QStringLiteral("m/s");
    case Category::SpecificEnergyPerTemperature:
        return QStringLiteral("J/(kg*K)");
    case Category::MassMomentInertia:
        return QStringLiteral("kg*m^2");
    case Category::VGVGradients:
        return QStringLiteral("1/deg");
    case Category::VGVGradientsPow2:
        return QStringLiteral("1/deg^2");
    case Category::EnergyDensity:
        return QStringLiteral("J/kg");
    case Category::PowerDensity:
        return QStringLiteral("J/(s*kg)");
    case Category::PowerTempArea:
        return QString("W/(m^2*K)");
    case Category::kinematicViscosity:
        return QStringLiteral("m^2/s");
    case Category::DataSize:
        return QStringLiteral("KB");
    case Category::NonDimensionalPercentage:
        return QStringLiteral("-");
    case Category::NonDimensional:
        return QStringLiteral("-");
    default:
        return QStringLiteral("");
    }
}

QString
GtUnit::categoryToString(Category c)
{
    switch (c)
    {
    case Category::Area:
        return QObject::tr("Area");
    case Category::Custom:
        return QObject::tr("Custom");
    case Category::Default:
        return QObject::tr("Default");
    case Category::Density:
        return QObject::tr("Density");
    case Category::Energy:
        return QObject::tr("Energy");
    case Category::SpecEnergy:
        return QObject::tr("Specific Energy");
    case Category::Force:
        return QObject::tr("Force");
    case Category::Inertia:
        return QObject::tr("Inertia");
    case Category::Length:
        return QObject::tr("Length");
    case Category::Mass:
        return QObject::tr("Mass");
    case Category::Massflow:
        return QObject::tr("Massflow");
    case Category::Power:
        return QObject::tr("Power");
    case Category::Temperature:
        return QObject::tr("Temperature");
    case Category::Time:
        return QObject::tr("Time");
    case Category::Torque:
        return QObject::tr("Torque");
    case Category::Pressure:
        return QObject::tr("Pressure");
    case Category::Angle:
        return QObject::tr("Angle");
    case Category::Volume:
        return QObject::tr("Volume");
    case Category::VolumeFlow:
        return QObject::tr("VolumeFlow");
    case Category::Frequency:
        return QObject::tr("Frequency");
    case Category::Velocity:
        return QObject::tr("Velocity");
    case Category::SpecificEnergyPerTemperature:
        return QObject::tr("SpecificEnergyPerTemperature");
    case Category::VGVGradients:
        return QObject::tr("VGV Gradients");
    case Category::VGVGradientsPow2:
        return QObject::tr("VGV Gradients Pow2");
    case Category::MassMomentInertia:
        return QObject::tr("Mass Moment of Inertia");
    case Category::EnergyDensity:
        return QObject::tr("EnergyDensity");
    case Category::PowerDensity:
        return QObject::tr("PowerDensity");
    case Category::PowerTempArea:
        return QObject::tr("PowerTempArea");
    case Category::kinematicViscosity:
        return QObject::tr("Kinematic Viscosity");
    case Category::DataSize:
        return QObject::tr("Data size");
    case Category::NonDimensionalPercentage:
        return QObject::tr("NonDimensionalPercentage");
    case Category::NonDimensional:
        return QObject::tr("Non Dimensional");
    default:
        return QObject::tr("No Category");
    }
}

