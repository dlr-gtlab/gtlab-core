#ifndef GTP_GtUnitCONVERTER_H
#define GTP_GtUnitCONVERTER_H

#include <QMap>
#include <qmath.h>
#include <QString>
#include <QVector>

#include "gt_unit.h"
#include "gt_utilities.h"

#include "gt_logging.h"
#include "gt_logging/qt_bindings.h"

template<class T>
class GtUnitConverter
{
public:
    static GtUnitConverter& instance();

    T from(GtUnit::Category category, const QString &GtUnit,
           T value, bool* success = nullptr);

    T To(GtUnit::Category category, const QString &GtUnit,
         T value, bool* success = nullptr);

    /**
     * @brief Returns a list of all unit names for a given category.
     * @return
     */
    QStringList units(GtUnit::Category) const;

private:
    /** Constructor */
    GtUnitConverter(){initialize();}

    /** Forbid the copy constructor */
    GtUnitConverter(GtUnitConverter&) = delete;

    /** Forbid the assigment operator */
    void operator=(GtUnitConverter&) = delete;

    /** Performs the initialization of the factor map */
    void initialize();

    /// Factor map
    QMap<GtUnit::Category, QMap<QString, double> > m_factorMap;

    ///Summand Map
    QMap<GtUnit::Category, QMap<QString, double> > m_summandMap;

};

template<class T>
GtUnitConverter<T>& GtUnitConverter<T>::instance()
{
    static GtUnitConverter<T> inst;
    return inst;
}

template<class T>
QStringList
GtUnitConverter<T>::units(GtUnit::Category category) const
{
    QMap<QString, double> const factors = m_factorMap[category];
    QMap<QString, double> const summands = m_summandMap[category];

    QStringList retval;

    retval.append(factors.keys());
    retval.append(summands.keys());

    retval.removeDuplicates();

    return retval;
}

template<class T>
T GtUnitConverter<T>::from(GtUnit::Category category, const QString& GtUnit,
                           T value, bool* success)
{
    QMap<QString, double> const factors = m_factorMap[category];
    QMap<QString, double> const summands = m_summandMap[category];

    bool const hasFac = factors.contains(GtUnit);
    bool const hasSum = summands.contains(GtUnit);

    if (!hasFac && !hasSum)
    {
        gtWarning().nospace()
                << QObject::tr("No such unit") << " (" << GtUnit << ") "
                << QObject::tr("in category") << " ("
                << GtUnit::categoryToString(category)
                << ") - "
                << QObject::tr("No conversion done!");
        return gt::valueError(value, success);
    }

    if (hasSum)
    {
        value = value - summands[GtUnit];
    }
    if (hasFac)
    {
        assert(factors[GtUnit] != 0);
        value = value / factors[GtUnit];
    }
    return gt::valueSuccess(value, success);
}

template<class T>
T GtUnitConverter<T>::To(GtUnit::Category category,
                         const QString &GtUnit, T value, bool *success)
{
    QMap<QString, double> const factors = m_factorMap[category];
    QMap<QString, double> const summands = m_summandMap[category];

    bool const hasFac = factors.contains(GtUnit);
    bool const hasSum = summands.contains(GtUnit);

    if (!hasFac && !hasSum)
    {
        gtWarning().nospace()
                << QObject::tr("No such unit") << " (" << GtUnit << ") "
                << QObject::tr("in category") << " ("
                << GtUnit::categoryToString(category)
                << ") - "
                << QObject::tr("No conversion done!");
        return gt::valueError(value, success);
    }

    if (hasFac)
    {
        value = value * factors[GtUnit];
    }
    if (hasSum)
    {
        value = value + summands[GtUnit];
    }

    return gt::valueSuccess(value, success);
}

template<class T>
void GtUnitConverter<T>::initialize()
{
    /** LENGTH **/

    QMap<QString, double> lengthFac;

    lengthFac["km"] = 0.001;
    lengthFac["hm"] = 0.01;
    lengthFac["dam"] = 0.1;
    lengthFac["m"] = 1.0;
    lengthFac["dm"] = 10.0;
    lengthFac["cm"] = 100.0;
    lengthFac["mm"] = 1000.0;
    lengthFac["µm"] = 1000000.0;
    lengthFac["nm"] = 1000000000.0;
    lengthFac["mi"] = 1. / 1609.344;    /// mile
    lengthFac["nmi"] = 1. / 1852.;      /// nautic mile
    lengthFac["ft"] = 1. / 0.3048;
    lengthFac["inch"] = 1. / 0.0254;

    m_factorMap.insert(GtUnit::Length, lengthFac);

    /** AREA **/

    QMap<QString, double> areaFac;

    areaFac["km^2"] = lengthFac["km"] * lengthFac["km"];
    areaFac["m^2"] = lengthFac["m"] * lengthFac["m"];
    areaFac["dm^2"] = lengthFac["dm"] * lengthFac["dm"];
    areaFac["cm^2"] = lengthFac["cm"] * lengthFac["cm"];
    areaFac["mm^2"] = lengthFac["mm"] * lengthFac["mm"];
    areaFac["ft^2"] = lengthFac["ft"] * lengthFac["ft"];
    areaFac["inch^2"] = lengthFac["inch"] * lengthFac["inch"];

    m_factorMap.insert(GtUnit::Area, areaFac);

    /** VOLUME **/

    QMap<QString, double> volumeFac;

    volumeFac["km^3"] = lengthFac["km"] * lengthFac["km"] * lengthFac["km"];
    volumeFac["m^3"] = lengthFac["m"] * lengthFac["m"] * lengthFac["m"];
    volumeFac["dm^3"] = lengthFac["dm"] * lengthFac["dm"] * lengthFac["dm"];
    volumeFac["cm^3"] = lengthFac["cm"] * lengthFac["cm"] * lengthFac["cm"];
    volumeFac["mm^3"] = lengthFac["mm"] * lengthFac["mm"] * lengthFac["mm"];
    volumeFac["l"] = 1000.0;
    volumeFac["ml"] = 1000000.0;
    volumeFac["ft^3"] = lengthFac["ft"] * lengthFac["ft"] * lengthFac["ft"];
    volumeFac["inch^3"] = lengthFac["inch"] * lengthFac["inch"]
            * lengthFac["inch"];
    volumeFac["gal"] =  volumeFac["inch^3"] / 231.0;
    volumeFac["floz"] = 128. * volumeFac["gal"];

    m_factorMap.insert(GtUnit::Volume, volumeFac);

    /** TIME **/

    QMap<QString, double> timeFac;

    timeFac["µs"] = 1000000.0;
    timeFac["ms"] = 1000.0;
    timeFac["s"] = 1.0;
    timeFac["min"] =  1.0 / 60.0;
    timeFac["h"] = 1.0 / 3600.0;
    timeFac["d"] = 1.0 / 86400.0;

    m_factorMap.insert(GtUnit::Time, timeFac);

    /** Volume Flow Rate ***/

    QMap<QString, double> volumeFlow;
    volumeFlow["km^3/s"] = volumeFac["km^3"] / timeFac["s"];
    volumeFlow["km^3/min"] = volumeFac["km^3"] / timeFac["min"];
    volumeFlow["km^3/h"] = volumeFac["km^3"] / timeFac["h"];
    volumeFlow["m^3/s"] = volumeFac["m^3"] / timeFac["s"];
    volumeFlow["m^3/min"] = volumeFac["m^3"] / timeFac["min"];
    volumeFlow["m^3/h"] = volumeFac["m^3"] / timeFac["h"];
    volumeFlow["dm^3/s"] = volumeFac["dm^3"] / timeFac["s"];
    volumeFlow["dm^3/min"] = volumeFac["dm^3"] / timeFac["min"];
    volumeFlow["dm^3/h"] = volumeFac["dm^3"] / timeFac["h"];
    volumeFlow["l/s"] = volumeFac["l"] / timeFac["s"];
    volumeFlow["l/min"] = volumeFac["l"] / timeFac["min"];
    volumeFlow["l/h"] = volumeFac["l"] / timeFac["h"];
    volumeFlow["ml/s"] = volumeFac["ml"] / timeFac["s"];
    volumeFlow["ml/min"] = volumeFac["ml"] / timeFac["min"];
    volumeFlow["ml/h"] = volumeFac["ml"] / timeFac["h"];
    volumeFlow["cm^3/s"] = volumeFac["cm^3"] / timeFac["s"];
    volumeFlow["cm^3/min"] = volumeFac["cm^3"] / timeFac["min"];
    volumeFlow["cm^3/h"] = volumeFac["cm^3"] / timeFac["h"];
    volumeFlow["mm^3/s"] = volumeFac["mm^3"] / timeFac["s"];
    volumeFlow["mm^3/min"] = volumeFac["mm^3"] / timeFac["min"];
    volumeFlow["mm^3/h"] = volumeFac["mm^3"] / timeFac["h"];
    volumeFlow["gal/s"] = volumeFac["gal"] / timeFac["s"];
    volumeFlow["gal/min"] = volumeFac["gal"] / timeFac["min"];
    volumeFlow["gal/h"] = volumeFac["gal"] / timeFac["h"];
    volumeFlow["floz/s"] = volumeFac["floz"] / timeFac["s"];
    volumeFlow["floz/min"] = volumeFac["floz"] / timeFac["min"];
    volumeFlow["floz/h"] = volumeFac["floz"] / timeFac["h"];
    volumeFlow["ft^3/s"] = volumeFac["ft^3"] / timeFac["s"];
    volumeFlow["ft^3/min"] = volumeFac["ft^3"] / timeFac["min"];
    volumeFlow["ft^3/h"] = volumeFac["ft^3"] / timeFac["h"];
    volumeFlow["inch^3/s"] = volumeFac["inch^3"] / timeFac["s"];
    volumeFlow["inch^3/min"] = volumeFac["inch^3"] / timeFac["min"];
    volumeFlow["inch^3/h"] = volumeFac["inch^3"] / timeFac["h"];

    m_factorMap.insert(GtUnit::VolumeFlow, volumeFlow);

    /** TEMPERATURE **/

    QMap<QString, double> temperatureFac;

    temperatureFac["K"] = 1.0;
    temperatureFac["°Ra"] = 1.8;
    temperatureFac["°F"] = 1.8;

    m_factorMap.insert(GtUnit::Temperature, temperatureFac);

    QMap<QString, double> temperatureSum;

    temperatureSum["°C"] = -273.15;
    temperatureSum["°F"] = -459.67;

    m_summandMap.insert(GtUnit::Temperature, temperatureSum);

    /** ANGLE **/

    QMap<QString, double> angleFac;

    angleFac["rad"] = 1.0;
    angleFac["deg"] = 1. / M_PI * 180.;
    angleFac["min"] = 1. / M_PI * 180. * 60.;
    angleFac["sec"] = 1. / M_PI * 180. * 3600.;

    m_factorMap.insert(GtUnit::Angle, angleFac);

    /** VELOCITY **/

    QMap<QString, double> velocityFac;

    velocityFac["mm/s"] = lengthFac["mm"] / timeFac["s"];
    velocityFac["cm/s"] = lengthFac["cm"] / timeFac["s"];
    velocityFac["dm/s"] = lengthFac["dm"] / timeFac["s"];
    velocityFac["m/s"] = lengthFac["m"] / timeFac["s"];
    velocityFac["km/s"] = lengthFac["km"] / timeFac["s"];
    velocityFac["m/min"] = lengthFac["m"] / timeFac["min"];
    velocityFac["km/min"] = lengthFac["km"] / timeFac["min"];
    velocityFac["dm/min"] = lengthFac["dm"] / timeFac["min"];
    velocityFac["cm/min"] = lengthFac["cm"] / timeFac["min"];
    velocityFac["mm/min"] = lengthFac["mm"] / timeFac["min"];
    velocityFac["m/h"] = lengthFac["m"] / timeFac["h"];
    velocityFac["km/h"] = lengthFac["km"] / timeFac["h"];
    velocityFac["dm/h"] = lengthFac["dm"] / timeFac["h"];
    velocityFac["cm/h"] = lengthFac["cm"] / timeFac["h"];
    velocityFac["mm/h"] = lengthFac["mm"] / timeFac["h"];
    velocityFac["mph"] = lengthFac["mi"] / timeFac["h"];
    velocityFac["kn"] = lengthFac["nmi"] / timeFac["h"];
    velocityFac["in/s"] = lengthFac["inch"] / timeFac["s"];
    velocityFac["ft/s"] = lengthFac["ft"] / timeFac["s"];

    m_factorMap.insert(GtUnit::Velocity, velocityFac);

    /** MASS **/

    QMap<QString, double> massFac;

    massFac["µg"] = 1000000000.0;
    massFac["mg"] = 1000000.0;
    massFac["g"] = 1000.0;
    massFac["dag"] = 100.0;
    massFac["kg"] = 1.0;
    massFac["dt"] = 0.01;
    massFac["t"] = 0.001;
    massFac["lbm"] = 1. / 0.45359237;
    massFac["oz"] = 16. * massFac["lbm"] ;
    massFac["tn. sh."] = 0.0005 * massFac["lbm"] ;

    m_factorMap.insert(GtUnit::Mass, massFac);

    /** MASS MOMENT OF INERTIA **/

    QMap<QString, double> massMomentInertiaFac;

    massMomentInertiaFac["g*mm^2"] = 1000000000.;
    massMomentInertiaFac["g*cm^2"] = 10000000.;
    massMomentInertiaFac["kg*mm^2"] = 1000000.;
    massMomentInertiaFac["kg*cm^2"] = 10000.;
    massMomentInertiaFac["lb*ft^2"] = massFac["lbm"] * areaFac["ft^2"];
    massMomentInertiaFac["kg*m^2"] = 1.0;
    massMomentInertiaFac["slug*ft^2"] = 0.737562142;

    m_factorMap.insert(GtUnit::MassMomentInertia, massMomentInertiaFac);


    /** Unit for VGVs **/
    QMap<QString, double> vgvGradientsFac;
    vgvGradientsFac["1/deg"] = 1.0;
    vgvGradientsFac["%/deg"] = 100.0;
    m_factorMap.insert(GtUnit::VGVGradients, vgvGradientsFac);

    QMap<QString, double> vgvGradients2Fac;
    vgvGradients2Fac["1/deg^2"] = 1.0;
    vgvGradients2Fac["%/deg^2"] = 100.0;
    m_factorMap.insert(GtUnit::VGVGradientsPow2, vgvGradients2Fac);



    /** MASSFLOW **/

    QMap<QString, double> massFlowFac;

    massFlowFac["mg/s"] = 1000000.0;
    massFlowFac["g/s"] = 1000.0;
    massFlowFac["dag/s"] = 100.0;
    massFlowFac["kg/s"] = 1.0;
    massFlowFac["dt/s"] = 0.01;
    massFlowFac["t/s"] = 0.001;
    massFlowFac["oz/s"] = massFac["oz"] / timeFac["s"];
    massFlowFac["lbm/s"] = massFac["lbm"] / timeFac["s"];;
    massFlowFac["(tn. sh.)/s"] = massFac["tn. sh."] / timeFac["s"];
    massFlowFac["mg/min"] = 1000000.0 * 60.0;
    massFlowFac["g/min"] = 1000.0 * 60.0;
    massFlowFac["dag/min"] = 100.0 * 60.0;
    massFlowFac["kg/min"] = 1.0 * 60.0;
    massFlowFac["dt/min"] = 0.01 * 60.;
    massFlowFac["t/min"] = 0.001 * 60.0;
    massFlowFac["oz/min"] = massFac["oz"] / timeFac["min"];
    massFlowFac["lbm/min"] = massFac["lbm"] / timeFac["min"];
    massFlowFac["(tn. sh.)/min"] = massFac["tn. sh."] / timeFac["min"];
    massFlowFac["mg/h"] = 1000000.0 * 3600.0;
    massFlowFac["g/h"] = 1000.0 * 3600.0;
    massFlowFac["dag/h"] = 100.0 * 3600.0;
    massFlowFac["kg/h"] = 1.0 * 3600.0;
    massFlowFac["dt/h"] = 0.01 * 3600.;
    massFlowFac["t/h"] = 0.001 * 3600.0;
    massFlowFac["oz/h"] = massFac["oz"] / timeFac["h"];
    massFlowFac["lbm/h"] = massFac["lbm"] / timeFac["h"];
    massFlowFac["(tn. sh.)/h"] = massFac["tn. sh."] / timeFac["h"];

    m_factorMap.insert(GtUnit::Massflow, massFlowFac);

    /** DENSITY **/

    QMap<QString, double> densityFac;

    densityFac["mg/m^3"] = 1000000.0;
    densityFac["g/m^3"] = 1000.0;
    densityFac["kg/m^3"] = 1.0;
    densityFac["t/m^3"] = 0.001;
    densityFac["mg/dm^3"] = 1000.0;
    densityFac["mg/l"] = 1000.0;
    densityFac["g/dm^3"] = 1.0;
    densityFac["g/l"] = 1.0;
    densityFac["kg/dm^3"] = 1. / 1000.;
    densityFac["kg/l"] = 1. / 1000.;
    densityFac["t/dm^3"] = 1. / 1000000.;
    densityFac["t/l"] = 1. / 1000000.;
    densityFac["mg/cm^3"] = massFac["mg"] / volumeFac["cm^3"];
    densityFac["g/cm^3"] = massFac["g"] / volumeFac["cm^3"];
    densityFac["kg/cm^3"] = massFac["kg"] / volumeFac["cm^3"];
    densityFac["t/cm^3"] = massFac["t"] / volumeFac["cm^3"];
    densityFac["lbm/in^3"] = massFac["lbm"] / volumeFac["inch^3"];
    densityFac["lbm/ft^3"] = massFac["lbm"] / volumeFac["ft^3"];
    densityFac["lbm/gal"] = massFac["lbm"] / volumeFac["gal"];

    m_factorMap.insert(GtUnit::Density, densityFac);

    /** FORCE **/

    QMap<QString, double> forceFac;

    forceFac["mN"] = 1000.0;
    forceFac["N"] = 1.0;
    forceFac["kN"] = 0.001;
    forceFac["MN"] = 0.000001;
    forceFac["lbf"] = massFac["lbm"] / 9.80665;
    forceFac["kp"] = 1. / 9.80665;
    forceFac["Mp"] = 0.001 / 9.80665;
    forceFac["dyn"] = 1e5;
    forceFac["pdl"] = massFac["lbm"] / 0.3048;

    m_factorMap.insert(GtUnit::Force, forceFac);

    /** FREQUENCY **/

    QMap<QString, double> frequencyFac;

    frequencyFac["1/s"] = 1.0 / timeFac["s"];
    frequencyFac["Hz"] = 1.0 / timeFac["s"];
    frequencyFac["1/min"] = 1.0 / timeFac["min"];
    frequencyFac["1/h"] = 1.0 / timeFac["h"];
    frequencyFac["rpm"] = 60.0;

    m_factorMap.insert(GtUnit::Frequency, frequencyFac);

    /** TORQUE **/

    QMap<QString, double> torqueFac;

    torqueFac["N*m"] = 1.0;
    torqueFac["kN*m"] = 0.001;
    torqueFac["kgf*m"] = 1. / 9.80665;
    torqueFac["kp*m"] = forceFac["kp"];
    torqueFac["lbf*in"] = forceFac["lbf"] * lengthFac["inch"];
    torqueFac["lbf*ft"] = forceFac["lbf"] * lengthFac["ft"];

    m_factorMap.insert(GtUnit::Torque, torqueFac);

    /** PRESSURE **/

    QMap<QString, double> pressureFac;

    pressureFac[GtUnit::siUnit(GtUnit::Pressure)] = 1.0;
    pressureFac["N/m^2"] = 1.0;
    pressureFac["Pa"] = 1.;
    pressureFac["kPa"] = 0.001;
    pressureFac["MPa"] = 0.000001;
    pressureFac["mbar"] = 0.01;
    pressureFac["bar"] = 0.00001;
    pressureFac["at"] = 0.0001 / 9.80665;
    pressureFac["atm"] = 1. / 101325.;
    pressureFac["Torr"] = 760. / 101325.;
    pressureFac["mmHg"] = 1. / 133.322387415;
    pressureFac["psi"] = forceFac["lbf"] / areaFac["inch^2"];

    m_factorMap.insert(GtUnit::Pressure, pressureFac);

    /** ENERGY **/

    QMap<QString, double> energyFac;

    energyFac["J"] = 1.0;
    energyFac["kJ"] = 0.001;
    energyFac["MJ"] = 1.0e-6;
    energyFac["W*h"] = 1. / 3600. ;
    energyFac["kW*h"] = 0.001 / 3600.;
    energyFac["cal"] = 1. / 4.184;
    energyFac["kcal"] = 0.001 * energyFac["cal"];
    // TODO: BTU ist hier nach ISO definiert.
    //       Es gibt andere gebraeuchlichere Umrechnungen. Welche verwenden?
    energyFac["BTU"] = 1. / 1055.056; // ISO Definition.
    energyFac["ft*lbf"] = lengthFac["ft"] * forceFac["lbf"];

    m_factorMap.insert(GtUnit::Energy, energyFac);

    /** POWER **/

    QMap<QString, double> powerFac;

    powerFac["W"] = 1.0;
    powerFac["kW"] = 0.001;
    powerFac["MW"] = 0.000001;
    powerFac["GW"] = 0.000000001;
    powerFac["PS"] = 1. / 75. / 9.80665;
    powerFac["hp"] = 1. / 550. / 0.45359237 / 9.80665 / 0.3048;
    // TODO: BTU ist hier nach ISO definiert.
    //       Es gibt andere gebraeuchlichere (!) Umrechnungen. Welche verwenden?
    powerFac["BTU/s"] = energyFac["BTU"] / timeFac["s"];
    powerFac["BTU/min"] = energyFac["BTU"] / timeFac["min"];
    powerFac["BTU/h"] = energyFac["BTU"] / timeFac["h"];
    powerFac["ft*lbf/s"] = lengthFac["ft"] * forceFac["lbf"] / timeFac["s"];
    powerFac["ft*lbf/min"] = lengthFac["ft"] * forceFac["lbf"] / timeFac["min"];
    powerFac["ft*lbf/h"] = lengthFac["ft"] * forceFac["lbf"] / timeFac["h"];

    m_factorMap.insert(GtUnit::Power, powerFac);

    /** INERTIA **/

    QMap<QString, double> inertiaFac;

    inertiaFac["g*mm^2"] = 1000000000.0;
    inertiaFac["g*cm^2"] = 10000000.0;
    inertiaFac["g*m^2"] = 1000.0;
    inertiaFac["kg*mm^2"] = 1000000.0;
    inertiaFac["kg*cm^2"] = 10000.0;
    inertiaFac["kg*m^2"] = 1.0;
    inertiaFac["lbm*in^2"] = massFac["lbm"] * areaFac["inch^2"];
    inertiaFac["lbm*ft^2"] = massFac["lbm"] * areaFac["ft^2"];
    inertiaFac["(tn. sh.)*in^2"] = massFac["tn. sh."] * areaFac["inch^2"];
    inertiaFac["(tn. sh.)*ft^2"] = massFac["tn. sh."] * areaFac["ft^2"];

    m_factorMap.insert(GtUnit::Inertia, inertiaFac);


    /** Energy Density **/

    QMap<QString, double> energyDensityFac;

    energyDensityFac["J/kg"] = energyFac["J"] / massFac["kg"];
    energyDensityFac["Ws/kg"] = powerFac["W"] * timeFac["s"] / massFac["kg"];
    energyDensityFac["Wh/kg"] = powerFac["W"] * timeFac["h"] / massFac["kg"];
    energyDensityFac["kWh/kg"] = powerFac["kW"] * timeFac["h"] / massFac["kg"];
    energyDensityFac["kJ/kg"] = energyFac["kJ"] / massFac["kg"];
    energyDensityFac["kWs/kg"] = powerFac["kW"] * timeFac["s"] / massFac["kg"];
    energyDensityFac["MJ/kg"] = energyFac["MJ"] / massFac["kg"];
    energyDensityFac["MWs/kg"] = powerFac["MW"] * timeFac["s"] / massFac["kg"];
    energyDensityFac["MWh/kg"] = 0.000001 * energyDensityFac["Wh/kg"];

    m_factorMap.insert(GtUnit::EnergyDensity, energyDensityFac);

    /** Energy Density **/

    QMap<QString, double> powerDensityFac;

    powerDensityFac["W/kg"] = 1.0;
    powerDensityFac["kW/kg"] = 0.001;
    powerDensityFac["MW/kg"] = 0.000001;
    powerDensityFac["kJ/(s*kg)"] = 0.001;
    powerDensityFac["MJ/(s*kg)"] = 0.000001;

    m_factorMap.insert(GtUnit::PowerDensity, powerDensityFac);


    /** kinmatic Viscosity **/

    QMap<QString, double> kinematicViscosityFac;

    kinematicViscosityFac["mm^2/s"] = lengthFac["mm"] * lengthFac["mm"];
    kinematicViscosityFac["m^2/s"] = 1.0;

    m_factorMap.insert(GtUnit::kinematicViscosity, kinematicViscosityFac);

    /** Data size **/

    QMap<QString, double> dataSizeFac;

    dataSizeFac["KB"] = 1.0;
    dataSizeFac["MB"] = 1.E-3;
    dataSizeFac["Bytes"] = 1.0E3;
    dataSizeFac["GB"] = 1.0E-6;

    m_factorMap.insert(GtUnit::DataSize, dataSizeFac);


    /** Non Dimensional Percentage **/

    QMap<QString, double> nonDimensionalPercentage;

    nonDimensionalPercentage["-"] = 1.0;
    nonDimensionalPercentage["%"] = 100.0;

    m_factorMap.insert(GtUnit::NonDimensionalPercentage,
                       nonDimensionalPercentage);


    /** Non Dimensional **/

    QMap<QString, double> nonDimensional;

    nonDimensional["-"] = 1.0;

    m_factorMap.insert(GtUnit::NonDimensional,
                       nonDimensional);



    /** CUSTOM **/

    QMap<QString, double> customFac;

    customFac["1/rad"] = 1.0;
    customFac["1/deg"] = 1.0 / angleFac["deg"];
    customFac["1/min"] = 1.0 / angleFac["min"];
    customFac["1/sec"] = 1.0 / angleFac["sec"];

    customFac["mg/(s*rad)"] = massFlowFac["mg/s"];
    customFac["g/(s*rad)"] = massFlowFac["g/s"];
    customFac["dag/(s*rad)"] = massFlowFac["dag/s"];
    customFac["kg/(s*rad)"] = 1.0;
    customFac["t/(s*rad)"] = massFlowFac["t/s"];
    customFac["mg/(min*rad)"] = massFlowFac["mg/min"];
    customFac["g/(min*rad)"] = massFlowFac["g/min"];
    customFac["dag/(min*rad)"] = massFlowFac["dag/min"];
    customFac["kg/(min*rad)"] = massFlowFac["kg/min"];
    customFac["t/(min*rad)"] = massFlowFac["t/min"];
    customFac["mg/(h*rad)"] = massFlowFac["mg/h"];
    customFac["g/(h*rad)"] = massFlowFac["g/h"];
    customFac["dag/(h*rad)"] = massFlowFac["dag/h"];
    customFac["kg/(h*rad)"] = massFlowFac["kg/h"];
    customFac["t/(h*rad)"] = massFlowFac["t/h"];

    customFac["mg/(s*deg)"] = massFlowFac["mg/s"] * customFac["1/deg"];
    customFac["g/(s*deg)"] = massFlowFac["g/s"] * customFac["1/deg"];
    customFac["dag/(s*deg)"] = massFlowFac["dag/s"] * customFac["1/deg"];
    customFac["kg/(s*deg)"] = customFac["1/deg"];
    customFac["t/(s*deg)"] = massFlowFac["t/s"] * customFac["1/deg"];
    customFac["mg/(min*deg)"] = massFlowFac["mg/min"] * customFac["1/deg"];
    customFac["g/(min*deg)"] = massFlowFac["g/min"] * customFac["1/deg"];
    customFac["dag/(min*deg)"] = massFlowFac["dag/min"] * customFac["1/deg"];
    customFac["kg/(min*deg)"] = massFlowFac["kg/min"] * customFac["1/deg"];
    customFac["t/(min*deg)"] = massFlowFac["t/min"] * customFac["1/deg"];
    customFac["mg/(h*deg)"] = massFlowFac["mg/h"] * customFac["1/deg"];
    customFac["g/(h*deg)"] = massFlowFac["g/h"] * customFac["1/deg"];
    customFac["dag/(h*deg)"] = massFlowFac["dag/h"] * customFac["1/deg"];
    customFac["kg/(h*deg)"] = massFlowFac["kg/h"] * customFac["1/deg"];
    customFac["t/(h*deg)"] = massFlowFac["t/h"] * customFac["1/deg"];

    customFac["mg/(s*min)"] = massFlowFac["mg/s"] * customFac["1/min"];
    customFac["g/(s*min)"] = massFlowFac["g/s"] * customFac["1/min"];
    customFac["dag/(s*min)"] = massFlowFac["dag/s"] * customFac["1/min"];
    customFac["kg/(s*min)"] = customFac["1/min"];
    customFac["t/(s*min)"] = massFlowFac["t/s"] * customFac["1/min"];
    customFac["mg/(min*min)"] = massFlowFac["mg/min"] * customFac["1/min"];
    customFac["g/(min*min)"] = massFlowFac["g/min"] * customFac["1/min"];
    customFac["dag/(min*min)"] = massFlowFac["dag/min"] * customFac["1/min"];
    customFac["kg/(min*min)"] = massFlowFac["kg/min"] * customFac["1/min"];
    customFac["t/(min*min)"] = massFlowFac["t/min"] * customFac["1/min"];
    customFac["mg/(h*min)"] = massFlowFac["mg/h"] * customFac["1/min"];
    customFac["g/(h*min)"] = massFlowFac["g/h"] * customFac["1/min"];
    customFac["dag/(h*min)"] = massFlowFac["dag/h"] * customFac["1/min"];
    customFac["kg/(h*min)"] = massFlowFac["kg/h"] * customFac["1/min"];
    customFac["t/(h*min)"] = massFlowFac["t/h"] * customFac["1/min"];

    customFac["mg/(s*sec)"] = massFlowFac["mg/s"] * customFac["1/sec"];
    customFac["g/(s*sec)"] = massFlowFac["g/s"] * customFac["1/sec"];
    customFac["dag/(s*sec)"] = massFlowFac["dag/s"] * customFac["1/sec"];
    customFac["kg/(s*sec)"] = customFac["1/sec"];
    customFac["t/(s*sec)"] = massFlowFac["t/s"] * customFac["1/sec"];
    customFac["mg/(min*sec)"] = massFlowFac["mg/min"] * customFac["1/sec"];
    customFac["g/(min*sec)"] = massFlowFac["g/min"] * customFac["1/sec"];
    customFac["dag/(min*sec)"] = massFlowFac["dag/min"] * customFac["1/sec"];
    customFac["kg/(min*sec)"] = massFlowFac["kg/min"] * customFac["1/sec"];
    customFac["t/(min*sec)"] = massFlowFac["t/min"] * customFac["1/sec"];
    customFac["mg/(h*sec)"] = massFlowFac["mg/h"] * customFac["1/sec"];
    customFac["g/(h*sec)"] = massFlowFac["g/h"] * customFac["1/sec"];
    customFac["dag/(h*sec)"] = massFlowFac["dag/h"] * customFac["1/sec"];
    customFac["kg/(h*sec)"] = massFlowFac["kg/h"] * customFac["1/sec"];
    customFac["t/(h*sec)"] = massFlowFac["t/h"] * customFac["1/sec"];

    customFac["J/mg"] = 1.0 / massFac["mg"];
    customFac["J/g"] = 1.0 / massFac["g"];
    customFac["J/dag"] = 1.0 / massFac["dag"];
    customFac["J/kg"] = 1.0;
    customFac["J/t"] = 1.0 / massFac["t"];
    customFac["kJ/mg"] = energyFac["kJ"] / massFac["mg"];
    customFac["kJ/g"] = energyFac["kJ"] / massFac["g"];
    customFac["kJ/dag"] = energyFac["kJ"] / massFac["dag"];
    customFac["kJ/kg"] = energyFac["kJ"] / massFac["kg"];
    customFac["kJ/t"] = energyFac["kJ"] / massFac["t"];
    customFac["MJ/mg"] = energyFac["MJ"]/ massFac["mg"];
    customFac["MJ/g"] = energyFac["MJ"] / massFac["g"];
    customFac["MJ/dag"] = energyFac["MJ"]/ massFac["dag"];
    customFac["MJ/kg"] = energyFac["MJ"] / massFac["kg"];
    customFac["MJ/t"] = energyFac["MJ"] / massFac["t"];



    m_factorMap.insert(GtUnit::Custom, customFac);
}


#endif // GTP_GtUnitCONVERTER_H
