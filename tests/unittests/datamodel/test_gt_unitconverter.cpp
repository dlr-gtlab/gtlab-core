/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_unitconverter.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 27.01.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"
#include "gt_unitconverter.h"

/// This is a test fixture that does a init for each test
class TestGtUnitConverter : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        m_conv = &GtUnitConverter<double>::instance();
    }

    virtual void TearDown()
    {
    }

    GtUnitConverter<double>* m_conv;

};

/// Length
TEST_F(TestGtUnitConverter, convertLength)
{
    GtUnit::Category cat = GtUnit::Length;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "m");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Length");
}

TEST_F(TestGtUnitConverter, convertLengthTo)
{
    GtUnit::Category cat = GtUnit::Length;

    // input
    double val = 1.4;
    bool check = false;

    // ### kilometer

    // convert
    double valKm = m_conv->To(cat, "km", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKm, 0.0014);

    // ### hectometer

    // convert
    double valHm = m_conv->To(cat, "hm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valHm, 0.014);

    // ### decameter

    // convert
    double valDam = m_conv->To(cat, "dam", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDam, 0.14);

    // ### decimeter

    // convert
    double valDm = m_conv->To(cat, "dm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDm, 14.);


    // ### centimeter

    // convert
    double valCm = m_conv->To(cat, "cm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCm, 140.);

    // ### millimeter

    // convert
    double valMm = m_conv->To(cat, "mm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMm, 1400.);

    // ### micrometer

    // convert
    double valMum = m_conv->To(cat, "µm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMum, 1400000.);

    // ### nanometer

    // convert
    double valNm = m_conv->To(cat, "nm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valNm, 1400000000.);

    // ### international mile

    // convert
    double valMi = m_conv->To(cat, "mi", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMi, 0.0008699196691322675);

    // ### nautical mile

    // convert
    double valNmi = m_conv->To(cat, "nmi", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valNmi, 0.000755939524838013);

    // ### foot

    // convert
    double valFt = m_conv->To(cat, "ft", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFt, 4.5931758530183725);

    // ### inch

    // convert
    double valIn = m_conv->To(cat, "inch", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valIn, 55.118110236220475);
}

TEST_F(TestGtUnitConverter, convertLengthFrom)
{
    GtUnit::Category cat = GtUnit::Length;

    // input
    double val = 1.4;
    bool check = false;

    // ### kilometer

    // convert
    double valKm = m_conv->from(cat, "km", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKm, 1400);

    // ### hectometer

    // convert
    double valHm = m_conv->from(cat, "hm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valHm, 140);

    // ### decameter

    // convert
    double valDam = m_conv->from(cat, "dam", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDam, 14);

    // ### decimeter

    // convert
    double valDm = m_conv->from(cat, "dm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDm, 0.14);

    // ### centimeter

    // convert
    double valCm = m_conv->from(cat, "cm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCm, 0.014);

    // ### millimeter

    // convert
    double valMm = m_conv->from(cat, "mm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMm, 0.0014);

    // ### micrometer

    // convert
    double valMum = m_conv->from(cat, "µm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMum, 0.0000014);

    // ### nanometer

    // convert
    double valNm = m_conv->from(cat, "nm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valNm, 0.0000000014);

    // ### international mile

    // convert
    double valMi = m_conv->from(cat, "mi", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMi, 2253.0816);

    // ### nautical mile

    // convert
    double valNmi = m_conv->from(cat, "nmi", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valNmi, 2592.8);

    // ### foot

    // convert
    double valFt = m_conv->from(cat, "ft", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFt, 0.42672);

    // ### inch

    // convert
    double valInch = m_conv->from(cat, "inch", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valInch, 0.03556);
}


/// Area
TEST_F(TestGtUnitConverter, convertArea)
{
    GtUnit::Category cat = GtUnit::Area;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "m^2");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Area");
}

TEST_F(TestGtUnitConverter, convertAreaTo)
{
    GtUnit::Category cat = GtUnit::Area;

    // input
    double val = 1.4;
    bool check = false;

    // ### square kilometer

    // convert
    double valSqkm = m_conv->To(cat, "km^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqkm, 0.0000014);

    // ### square decimeter

    // convert
    double valSqdm = m_conv->To(cat, "dm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqdm, 140);

    // ### square centimeter

    // convert
    double valSqcm = m_conv->To(cat, "cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqcm, 14000);

    // ### square millimeter

    // convert
    double valSqmm = m_conv->To(cat, "mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqmm, 1400000);

    // ### square foot

    // convert
    double valSqft = m_conv->To(cat, "ft^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqft, 15.06947458339361);

    // ### square inch

    // convert
    double valSqin = m_conv->To(cat, "inch^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqin, 2170.00434000868);
}

TEST_F(TestGtUnitConverter, convertAreaFrom)
{
    GtUnit::Category cat = GtUnit::Area;

    // input
    double val = 1.4;
    bool check = false;

    // ### square kilometer

    // convert
    double valSqkm = m_conv->from(cat, "km^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqkm, 1400000);

    // ### square decimeter

    // convert
    double valSqdm = m_conv->from(cat, "dm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqdm, 0.014);

    // ### square centimeter

    // convert
    double valSqcm = m_conv->from(cat, "cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqcm, 0.00014);

    // ### square millimeter

    // convert
    double valSqmm = m_conv->from(cat, "mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqmm, 0.0000014);

    // ### square foot

    // convert
    double valSqft = m_conv->from(cat, "ft^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqft, 0.130064256);

    // ### square inch

    // convert
    double valSqin = m_conv->from(cat, "inch^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSqin, 0.000903224);
}


/// Volume
TEST_F(TestGtUnitConverter, convertVolume)
{
    GtUnit::Category cat = GtUnit::Volume;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "m^3");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Volume");
}

TEST_F(TestGtUnitConverter, convertVolumeTo)
{
    GtUnit::Category cat = GtUnit::Volume;

    // input
    double val = 1.4;
    bool check = false;

    // ### cubic kilometer

    // convert
    double valCukm = m_conv->To(cat, "km^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCukm, 1.4e-9);

    // ### cubic decimeter

    // convert
    double valCudm = m_conv->To(cat, "dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCudm, 1400);

    // ### cubic centimeter

    // convert
    double valCucm = m_conv->To(cat, "cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCucm, 1400000);

    // ### cubic millimeter

    // convert
    double valCumm = m_conv->To(cat, "mm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCumm, 1400000000);

    // ### liter

    // convert
    double valLiter = m_conv->To(cat, "l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLiter, 1400);

    // ### milliliter

    // convert
    double valMl = m_conv->To(cat, "ml", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMl, 1400000);

    // ### us liquid gallon

    // convert
    double valUsgal = m_conv->To(cat, "gal", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUsgal, 369.84087330140774);

    // ### us fluid ounce

    // convert
    double valFloz = m_conv->To(cat, "floz", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFloz, 47339.63178258019);

    // ### cubic foot

    // convert
    double valCuft = m_conv->To(cat, "ft^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCuft, 49.44053341008403);

    // ### cubic inch

    // convert
    double valCuin = m_conv->To(cat, "inch^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCuin, 85433.24173262521);
}

TEST_F(TestGtUnitConverter, convertVolumeFrom)
{
    GtUnit::Category cat = GtUnit::Volume;

    // input
    double val = 1.4;
    bool check = false;

    // ### cubic kilometer

    // convert
    double valCukm = m_conv->from(cat, "km^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCukm, 1.4e9);

    // ### cubic decimeter

    // convert
    double valCudm = m_conv->from(cat, "dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCudm, 0.0014);

    // ### cubic centimeter

    // convert
    double valCucm = m_conv->from(cat, "cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCucm, 1.4e-6);

    // ### cubic millimeter

    // convert
    double valCumm = m_conv->from(cat, "mm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCumm, 1.4e-9);

    // ### liter

    // convert
    double valLiter = m_conv->from(cat, "l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLiter, 0.0014);

    // ### milliliter

    // convert
    double valMl = m_conv->from(cat, "ml", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMl, 0.0000014);

    // ### us liquid gallon

    // convert
    double valUsgal = m_conv->from(cat, "gal", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUsgal, 0.0052995764976);

    // ### us fluid ounce

    // convert
    double valFloz = m_conv->from(cat, "floz", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFloz, 0.0000414029413875);

    // ### cubic foot

    // convert
    double valCuft = m_conv->from(cat, "ft^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCuft, 0.0396435852288);

    // ### cubic inch

    // convert
    double valCuin = m_conv->from(cat, "inch^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCuin, 0.0000229418896);
}


/// Temperature
TEST_F(TestGtUnitConverter, convertTemperature)
{
    GtUnit::Category cat = GtUnit::Temperature;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "K");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Temperature");
}

TEST_F(TestGtUnitConverter, convertTemperatureTo)
{
    GtUnit::Category cat = GtUnit::Temperature;

    // input
    double val = 214.;
    bool check = false;

    // ### °Rankine

    // convert
    double valRa = m_conv->To(cat, "°Ra", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valRa, 385.2);

    // ### °Celsius

    // convert
    double valC = m_conv->To(cat, "°C", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valC, -59.15);

    // ### °Fahrenheit

    // convert
    double valF = m_conv->To(cat, "°F", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valF, -74.47);
}

TEST_F(TestGtUnitConverter, convertTemperatureFrom)
{
    GtUnit::Category cat = GtUnit::Temperature;

    // input
    double val = 214.;
    bool check = false;

    // ### °Rankine

    // convert
    double valRa = m_conv->from(cat, "°Ra", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valRa, 118.8888888888889);

    // ### °Celsius

    // convert
    double valC = m_conv->from(cat, "°C", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valC, 487.15);

    // ### °Fahrenheit

    // convert
    double valF = m_conv->from(cat, "°F", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valF, 374.26111111111111);
}


/// Time
TEST_F(TestGtUnitConverter, convertTime)
{
    GtUnit::Category cat = GtUnit::Time;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "s");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Time");
}

TEST_F(TestGtUnitConverter, convertTimeTo)
{
    GtUnit::Category cat = GtUnit::Time;

    // input
    double val = 14.;
    bool check = false;

    // ### microsecond

    // convert
    double valMus = m_conv->To(cat, "µs", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMus, 1.4E7);

    // ### millisecond

    // convert
    double valMs = m_conv->To(cat, "ms", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMs, 14000.);

    // ### minute

    // convert
    double valMin = m_conv->To(cat, "min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMin, 0.2333333333333333);

    // ### hour

    // convert
    double valH = m_conv->To(cat, "h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valH, 0.00388888888888889);

    // ### day

    // convert
    double valD = m_conv->To(cat, "d", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valD, 0.000162037037037037);
}

TEST_F(TestGtUnitConverter, convertTimeFrom)
{
    GtUnit::Category cat = GtUnit::Time;

    // input
    double val = 14.;
    bool check = false;

    // ### microsecond

    // convert
    double valMus = m_conv->from(cat, "µs", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMus, 1.4e-5);

    // ### millisecond

    // convert
    double valMs = m_conv->from(cat, "ms", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMs, 0.014);

    // ### minute

    // convert
    double valMin = m_conv->from(cat, "min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMin, 840.);

    // ### hour

    // convert
    double valH = m_conv->from(cat, "h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valH, 50400.);

    // ### day

    // convert
    double valD = m_conv->from(cat, "d", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valD, 1209600.);
}


/// Angle
TEST_F(TestGtUnitConverter, convertAngle)
{
    GtUnit::Category cat = GtUnit::Angle;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "rad");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Angle");
}

TEST_F(TestGtUnitConverter, convertAngleTo)
{
    GtUnit::Category cat = GtUnit::Angle;

    // input
    double val = 0.14;
    bool check = false;

    // ### degrees

    // convert
    double valDeg = m_conv->To(cat, "deg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDeg, 8.021409131831524923);

    // ### minutes

    // convert
    double valMin = m_conv->To(cat, "min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMin, 481.28454790989149537);

    // ### seconds

    // convert
    double valSec = m_conv->To(cat, "sec", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSec, 28877.07287459348972);
}

TEST_F(TestGtUnitConverter, convertAngleFrom)
{
    GtUnit::Category cat = GtUnit::Angle;

    // input
    double val = 140.;
    bool check = false;

    // ### degrees

    // convert
    double valDeg = m_conv->from(cat, "deg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDeg, 2.443460952792061408);

    // ### minutes

    // convert
    double valMin = m_conv->from(cat, "min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMin, 0.040724349213201023);

    // ### seconds

    // convert
    double valSec = m_conv->from(cat, "sec", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valSec, 0.00067873915355335);
}

/// Mass Moment Inertia
TEST_F(TestGtUnitConverter, convertMassMomentInertia)
{
    GtUnit::Category cat = GtUnit::MassMomentInertia;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "kg*m^2");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Mass Moment of Inertia");
}

TEST_F(TestGtUnitConverter, convertMassMomentInertiaTo)
{
    GtUnit::Category cat = GtUnit::MassMomentInertia;

    // input
    double val = 8.;
    bool check = false;

    // ### gramm millimeter

    // convert
    double retval = m_conv->To(cat, "g*mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 8000000000.);

    // ### gramm centimeter

    // convert
    retval = m_conv->To(cat, "g*cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 80000000.);

    // ### kilogramm millimeter

    // convert
    retval = m_conv->To(cat, "kg*mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 8000000.);

    // ### kilogramm centimeter

    // convert
    retval = m_conv->To(cat, "kg*cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 80000.);

    // ### kilogramm meter

    // convert
    retval = m_conv->To(cat, "kg*m^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 8.);

    // ### pounts feet

    // convert
    retval = m_conv->To(cat, "lb*ft^2", val, &check);


    // check
    ASSERT_TRUE(check);
    ASSERT_NEAR(retval, 189.8428832, 0.000001);

    // ### slug meter

    // convert
    retval = m_conv->To(cat, "slug*ft^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_NEAR(retval, 5.900497135, 0.000001);
}

TEST_F(TestGtUnitConverter, convertMassMomentInertiaFrom)
{
    GtUnit::Category cat = GtUnit::MassMomentInertia;

    // input
    double val = 8000000000.;
    bool check = false;

    // ### gramm millimeter

    // convert
    double retval = m_conv->from(cat, "g*mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 8.);

    // ### gramm centimeter

    // convert
    retval = m_conv->from(cat, "g*cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 800.);

    // ### kilogramm millimeter

    // convert
    retval = m_conv->from(cat, "kg*mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 8000.);

    // ### kilogramm centimeter

    // convert
    retval = m_conv->from(cat, "kg*cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 800000.);

    // ### kilogramm meter

    // convert
    retval = m_conv->from(cat, "kg*m^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 8000000000.);

    // ### pounts feet

    // convert
    retval = m_conv->from(cat, "lb*ft^2", val, &check);


    // check
    ASSERT_TRUE(check);
    ASSERT_NEAR(retval, 337120880.753733, 0.001);

    // ### slug meter

    // convert
    retval = m_conv->from(cat, "slug*ft^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_NEAR(retval, 10846543693.6702, 0.001);

}

/// VGVGradients
TEST_F(TestGtUnitConverter, VGVGradientsUnit)
{
    GtUnit::Category cat = GtUnit::VGVGradients;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "1/deg");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "VGV Gradients");
}


TEST_F(TestGtUnitConverter, convertVGVGradientsUnitTo)
{
    GtUnit::Category cat = GtUnit::VGVGradients;

    // input
    double val = 8.;
    bool check = false;

    // convert
    double retval = 0.0;

    retval = m_conv->To(cat, "%/deg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(retval, 800);
}

TEST_F(TestGtUnitConverter, convertVGVGradientsUnitaFrom)
{
    GtUnit::Category cat = GtUnit::VGVGradients;

    // input
    double val = 8.;
    bool check = false;

    // convert
    double retval = 0.0;

    retval = m_conv->from(cat, "%/deg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_NEAR(retval, 0.08, 0.00000000001);
}


/// Velocity
TEST_F(TestGtUnitConverter, convertVelocity)
{
    GtUnit::Category cat = GtUnit::Velocity;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "m/s");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Velocity");
}

TEST_F(TestGtUnitConverter, convertVelocityTo)
{
    GtUnit::Category cat = GtUnit::Velocity;

    // input
    double val = 14.;
    bool check = false;

    // ### millimeters per second

    // convert
    double valMmps = m_conv->To(cat, "mm/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMmps, 14000.);

    // ### centimeters per second

    // convert
    double valCmps = m_conv->To(cat, "cm/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCmps, 1400.);

    // ### decimeters per second

    // convert
    double valDmps = m_conv->To(cat, "dm/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDmps, 140.);

    // ### kilometers per second

    // convert
    double valKmps = m_conv->To(cat, "km/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKmps, 0.014);

    // ### millimeters per minute

    // convert
    double valMmpmin = m_conv->To(cat, "mm/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMmpmin, 840000.);

    // ### centimeters per minute

    // convert
    double valCmpmin = m_conv->To(cat, "cm/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCmpmin, 84000.);

    // ### decimeters per minute

    // convert
    double valDmpmin = m_conv->To(cat, "dm/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDmpmin, 8400.);

    // ### meters per minute

    // convert
    double valMtrpmin = m_conv->To(cat, "m/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMtrpmin, 840.);

    // ### kilometers per minute

    // convert
    double valKmpmin = m_conv->To(cat, "km/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKmpmin, 0.84);

    // ### millimeters per hour

    // convert
    double valMmph = m_conv->To(cat, "mm/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMmph, 50400000.);

    // ### centimeters per hour

    // convert
    double valCmph = m_conv->To(cat, "cm/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCmph, 5040000.);

    // ### decimeters per hour

    // convert
    double valDmph = m_conv->To(cat, "dm/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDmph, 504000.);

    // ### meters per hour

    // convert
    double valMtrph = m_conv->To(cat, "m/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMtrph, 50400.);

    // ### kilometers per hour

    // convert
    double valKmph = m_conv->To(cat, "km/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKmph, 50.4);

    // ### miles per hour

    // convert
    double valMph = m_conv->To(cat, "mph", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMph, 31.317108088761630);

    // ### knots

    // convert
    double valKn = m_conv->To(cat, "kn", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKn, 27.2138228941684665);
}

TEST_F(TestGtUnitConverter, convertVelocityFrom)
{
    GtUnit::Category cat = GtUnit::Velocity;

    // input
    double val = 14.;
    bool check = false;

    // ### millimeters per second

    // convert
    double valMmps = m_conv->from(cat, "mm/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMmps, 0.014);

    // ### centimeters per second

    // convert
    double valCmps = m_conv->from(cat, "cm/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCmps, 0.14);

    // ### decimeters per second

    // convert
    double valDmps = m_conv->from(cat, "dm/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDmps, 1.4);

    // ### kilometers per second

    // convert
    double valKmps = m_conv->from(cat, "km/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKmps, 14000.);

    // ### millimeters per minute

    // convert
    double valMmpmin = m_conv->from(cat, "mm/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMmpmin, 0.0002333333333333333);

    // ### centimeters per minute

    // convert
    double valCmpmin = m_conv->from(cat, "cm/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCmpmin, 0.002333333333333333);

    // ### decimeters per minute

    // convert
    double valDmpmin = m_conv->from(cat, "dm/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDmpmin, 0.02333333333333333);

    // ### meters per minute

    // convert
    double valMtrpmin = m_conv->from(cat, "m/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMtrpmin, 0.2333333333333333);

    // ### kilometers per minute

    // convert
    double valKmpmin = m_conv->from(cat, "km/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKmpmin, 233.3333333333333);

    // ### millimeters per hour

    // convert
    double valMmph = m_conv->from(cat, "mm/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMmph, 0.00000388888888888889);

    // ### centimeters per hour

    // convert
    double valCmph = m_conv->from(cat, "cm/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCmph, 0.0000388888888888889);

    // ### decimeters per hour

    // convert
    double valDmph = m_conv->from(cat, "dm/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDmph, 0.000388888888888889);

    // ### meters per hour

    // convert
    double valMtrph = m_conv->from(cat, "m/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMtrph, 0.00388888888888889);

    // ### kilometers per hour

    // convert
    double valKmph = m_conv->from(cat, "km/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKmph, 3.888888888888889);

    // ### miles per hour

    // convert
    double valMph = m_conv->from(cat, "mph", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMph, 6.258560);

    // ### knots

    // convert
    double valKn = m_conv->from(cat, "kn", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKn, 7.2022222222222222);
}


/// Mass
TEST_F(TestGtUnitConverter, convertMass)
{
    GtUnit::Category cat = GtUnit::Mass;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "kg");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Mass");
}

TEST_F(TestGtUnitConverter, convertMassTo)
{
    GtUnit::Category cat = GtUnit::Mass;

    // input
    double val = 1.4;
    bool check = false;

    // ### microgram

    // convert
    double valMug = m_conv->To(cat, "µg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMug, 1.40e+9);

    // ### milligram

    // convert
    double valMg = m_conv->To(cat, "mg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMg, 1.40e+6);

    // ### gram

    // convert
    double valG = m_conv->To(cat, "g", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valG, 1400.);

    // ### decagram

    // convert
    double valDag = m_conv->To(cat, "dag", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDag, 140.);

    // ### deciton

    // convert
    double valDt = m_conv->To(cat, "dt", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDt, 0.014);

    // ### tonne

    // convert
    double valT = m_conv->To(cat, "t", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valT, 0.0014);

    // ### us ounce

    // convert
    double valOz = m_conv->To(cat, "oz", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valOz, 49.3835467294125750);

    // ### us pound-mass

    // convert
    double valLbm = m_conv->To(cat, "lbm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbm, 3.086471670588286);

    // ### us (short) ton

    // convert
    double valUst = m_conv->To(cat, "tn. sh.", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUst, 0.0015432358352941430);
}

TEST_F(TestGtUnitConverter, convertMassFrom)
{
    GtUnit::Category cat = GtUnit::Mass;

    // input
    double val = 1.4;
    bool check = false;

    // ### microgram

    // convert
    double valMug = m_conv->from(cat, "µg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMug, 1.40e-9);

    // ### milligram

    // convert
    double valMg = m_conv->from(cat, "mg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMg, 1.40e-6);

    // ### gram

    // convert
    double valG = m_conv->from(cat, "g", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valG, 0.0014);

    // ### decagram

    // convert
    double valDag = m_conv->from(cat, "dag", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDag, 0.014);

    // ### deciton

    // convert
    double valDt = m_conv->from(cat, "dt", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDt, 140.);

    // ### tonne

    // convert
    double valT = m_conv->from(cat, "t", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valT, 1400.);

    // ### us ounce

    // convert
    double valOz = m_conv->from(cat, "oz", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valOz, 0.0396893323750);

    // ### us pound-mass

    // convert
    double valLbm = m_conv->from(cat, "lbm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbm, 0.6350293180);

    // ### us (short) ton

    // convert
    double valUst = m_conv->from(cat, "tn. sh.", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUst, 1270.0586360);
}


/// Massflow
TEST_F(TestGtUnitConverter, convertMassflow)
{
    GtUnit::Category cat = GtUnit::Massflow;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "kg/s");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Massflow");
}

TEST_F(TestGtUnitConverter, convertMassflowTo)
{
    GtUnit::Category cat = GtUnit::Massflow;

    // input
    double val = 1.4;
    bool check = false;

    // ### milligram per second

    // convert
    double valMgps = m_conv->To(cat, "mg/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgps, 1.40e+6);

    // ### gram per second

    // convert
    double valGps = m_conv->To(cat, "g/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGps, 1400.);

    // ### decagram per second

    // convert
    double valDagps = m_conv->To(cat, "dag/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDagps, 140.);

    // ### deciton per second

    // convert
    double valDtps = m_conv->To(cat, "dt/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDtps, 0.014);

    // ### tonne per second

    // convert
    double valTps = m_conv->To(cat, "t/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTps, 0.0014);

    // ### us ounce per second

    // convert
    double valOzps = m_conv->To(cat, "oz/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valOzps, 49.3835467294125750);

    // ### us pound-mass per second

    // convert
    double valLbmps = m_conv->To(cat, "lbm/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmps, 3.086471670588286);

    // ### us (short) ton per second

    // convert
    double valUstps = m_conv->To(cat, "(tn. sh.)/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstps, 0.0015432358352941430);

    // ### milligram per minute

    // convert
    double valMgpmin = m_conv->To(cat, "mg/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpmin, 8.40e+7);

    // ### gram per minute

    // convert
    double valGpmin = m_conv->To(cat, "g/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpmin, 8.40e+4);

    // ### decagram per minute

    // convert
    double valDagpmin = m_conv->To(cat, "dag/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDagpmin, 8400.);

    // ### kilogram per minute

    // convert
    double valKgpmin = m_conv->To(cat, "kg/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgpmin, 84.);

    // ### deciton per minute

    // convert
    double valDtpmin = m_conv->To(cat, "dt/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDtpmin, 0.84);

    // ### tonne per minute

    // convert
    double valTpmin = m_conv->To(cat, "t/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpmin, 0.084);

    // ### us ounce per minute

    // convert
    double valOzpmin = m_conv->To(cat, "oz/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valOzpmin, 2963.0128037647545);

    // ### us pound-mass per minute

    // convert
    double valLbmpmin = m_conv->To(cat, "lbm/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmpmin, 185.18830023529716);

    // ### us (short) ton per minute

    // convert
    double valUstpmin = m_conv->To(cat, "(tn. sh.)/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstpmin, 0.09259415011764858);

    // ### milligram per hour

    // convert
    double valMgph = m_conv->To(cat, "mg/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgph, 50.40e+8);

    // ### gram per hour

    // convert
    double valGph = m_conv->To(cat, "g/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGph, 50.40e+5);

    // ### decagram per hour

    // convert
    double valDagph = m_conv->To(cat, "dag/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDagph, 50.40e+4);

    // ### kilogram per hour

    // convert
    double valKgph = m_conv->To(cat, "kg/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgph, 5040.);

    // ### deciton per hour

    // convert
    double valDtph = m_conv->To(cat, "dt/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDtph, 50.4);

    // ### tonne per hour

    // convert
    double valTph = m_conv->To(cat, "t/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTph, 5.04);

    // ### us ounce per hour

    // convert
    double valOzph = m_conv->To(cat, "oz/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valOzph, 177780.76822588527);

    // ### us pound-mass per hour

    // convert
    double valLbmph = m_conv->To(cat, "lbm/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmph, 11111.2980141178296);

    // ### us (short) ton per hour

    // convert
    double valUstph = m_conv->To(cat, "(tn. sh.)/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstph, 5.5556490070589148);
}

TEST_F(TestGtUnitConverter, convertMassflowFrom)
{
    GtUnit::Category cat = GtUnit::Massflow;

    // input
    double val = 1.4;
    bool check = false;

    // ### milligram per second

    // convert
    double valMgps = m_conv->from(cat, "mg/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgps, 1.40e-6);

    // ### gram per second

    // convert
    double valGps = m_conv->from(cat, "g/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGps, 0.0014);

    // ### decagram per second

    // convert
    double valDagps = m_conv->from(cat, "dag/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDagps, 0.014);

    // ### deciton per second

    // convert
    double valDtps = m_conv->from(cat, "dt/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDtps, 140.);

    // ### tonne per second

    // convert
    double valTps = m_conv->from(cat, "t/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTps, 1400.);

    // ### us ounce per second

    // convert
    double valOzps = m_conv->from(cat, "oz/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valOzps, 0.0396893323750);

    // ### us pound-mass per second

    // convert
    double valLbmps = m_conv->from(cat, "lbm/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmps, 0.6350293180);

    // ### us (short) ton per second

    // convert
    double valUstps = m_conv->from(cat, "(tn. sh.)/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstps, 1270.0586360);

    // ### milligram per minute

    // convert
    double valMgpmin = m_conv->from(cat, "mg/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpmin, 2.3333333333333333333333333333333e-8);

    // ### gram per minute

    // convert
    double valGpmin = m_conv->from(cat, "g/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpmin, 2.3333333333333333333333333333333e-5);

    // ### decagram per minute

    // convert
    double valDagpmin = m_conv->from(cat, "dag/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDagpmin, 2.3333333333333333333333333333333e-4);

    // ### kilogram per minute

    // convert
    double valKgpmin = m_conv->from(cat, "kg/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgpmin, 0.02333333333333333333333333333333);

    // ### deciton per minute

    // convert
    double valDtpmin = m_conv->from(cat, "dt/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDtpmin, 2.3333333333333333333333333333333);

    // ### tonne per minute

    // convert
    double valTpmin = m_conv->from(cat, "t/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpmin, 23.333333333333333333333333333333);

    // ### us ounce per minute

    // convert
    double valOzpmin = m_conv->from(cat, "oz/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valOzpmin, 6.6148887291666666666666666666667e-4);

    // ### us pound-mass per minute

    // convert
    double valLbmpmin = m_conv->from(cat, "lbm/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmpmin, 0.01058382196666666666666666666667);

    // ### us (short) ton per minute

    // convert
    double valUstpmin = m_conv->from(cat, "(tn. sh.)/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstpmin, 21.167643933333333333333333333333);

    // ### milligram per hour

    // convert
    double valMgph = m_conv->from(cat, "mg/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgph, 3.888888888888888888888888333333e-10);

    // ### gram per hour

    // convert
    double valGph = m_conv->from(cat, "g/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGph, 3.8888888888888888888888888883333e-7);

    // ### decagram per hour

    // convert
    double valDagph = m_conv->from(cat, "dag/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDagph, 3.8888888888888888888888888888333e-6);

    // ### kilogram per hour

    // convert
    double valKgph = m_conv->from(cat, "kg/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgph, 3.8888888888888888888888888888883e-4);

    // ### deciton per hour

    // convert
    double valDtph = m_conv->from(cat, "dt/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDtph, 0.03888888888888888888888888888889);

    // ### tonne per hour

    // convert
    double valTph = m_conv->from(cat, "t/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTph, 0.38888888888888888888888888888888);

    // ### us ounce per hour

    // convert
    double valOzph = m_conv->from(cat, "oz/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valOzph, 1.1024814548611111111111111111e-5);

    // ### us pound-mass per hour

    // convert
    double valLbmph = m_conv->from(cat, "lbm/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmph, 1.7639703277777777777777777777783e-4);

    // ### us (short) ton per hour

    // convert
    double valUstph = m_conv->from(cat, "(tn. sh.)/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstph, 0.35279406555555555555555555555555);
}


/// Density
TEST_F(TestGtUnitConverter, convertDensity)
{
    GtUnit::Category cat = GtUnit::Density;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "kg/m^3");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Density");
}

TEST_F(TestGtUnitConverter, convertDensityTo)
{
    GtUnit::Category cat = GtUnit::Density;

    // input
    double val = 1.4;
    bool check = false;

    // ### milligram per cubic meter

    // convert
    double valMgpcum = m_conv->To(cat, "mg/m^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpcum, 1.40e+6);

    // ### gram per cubic meter

    // convert
    double valGpcum = m_conv->To(cat, "g/m^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpcum, 1400.);

    // ### tonne per cubic meter

    // convert
    double valTpcum = m_conv->To(cat, "t/m^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpcum, 1.40e-3);

    // ### milligram per cubic decimeter

    // convert
    double valMgpcdm = m_conv->To(cat, "mg/dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpcdm, 1400.);

    // ### milligram per liter

    // convert
    double valMgpl = m_conv->To(cat, "mg/l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpl, 1400.);

    // ### gram per cubic decimeter

    // convert
    double valGpcdm = m_conv->To(cat, "g/dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpcdm, 1.4);

    // ### gram per liter

    // convert
    double valGpl = m_conv->To(cat, "g/l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpl, 1.4);

    // ### kilogram per cubic decimeter

    // convert
    double valKgpcdm = m_conv->To(cat, "kg/dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgpcdm, 0.0014);

    // ### kilogram per liter

    // convert
    double valKgpl = m_conv->To(cat, "kg/l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgpl, 0.0014);

    // ### tonne per cubic decimeter

    // convert
    double valTpcdm = m_conv->To(cat, "t/dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpcdm, 1.40e-6);

    // ### tonne per liter

    // convert
    double valTpl = m_conv->To(cat, "t/l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpl, 1.40e-6);

    // ### milligram per cubic centimeter

    // convert
    double valMgpccm = m_conv->To(cat, "mg/cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpccm, 1.4);

    // ### gram per cubic centimeter

    // convert
    double valGpccm = m_conv->To(cat, "g/cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpccm, 1.4e-3);

    // ### kilogram per cubic centimeter

    // convert
    double valKgpccm = m_conv->To(cat, "kg/cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgpccm, 1.40e-6);

    // ### tonne per cubic centimeter

    // convert
    double valTpccm = m_conv->To(cat, "t/cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpccm, 1.40e-9);

    // ### pound-mass per cubic inch

    // convert
    double valLbmpcin = m_conv->To(cat, "lbm/in^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmpcin, 5.057820880011716246461551314145e-5);

    // ### pound-mass per cubic foot

    // convert
    double valLbmpcft = m_conv->To(cat, "lbm/ft^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmpcft, 0.087399144806602456738855638158111);

    // ### pound-mass per us gallon

    // convert
    double valLbmpusgal = m_conv->To(cat, "lbm/gal", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmpusgal, 0.01168356623282706452932618773989);
}

TEST_F(TestGtUnitConverter, convertDensityFrom)
{
    GtUnit::Category cat = GtUnit::Density;

    // input
    double val = 1.4;
    bool check = false;

    // ### milligram per cubic meter

    // convert
    double valMgpcum = m_conv->from(cat, "mg/m^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpcum, 1.40e-6);

    // ### gram per cubic meter

    // convert
    double valGpcum = m_conv->from(cat, "g/m^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpcum, 1.40e-3);

    // ### tonne per cubic meter

    // convert
    double valTpcum = m_conv->from(cat, "t/m^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpcum, 1400.);

    // ### milligram per cubic decimeter

    // convert
    double valMgpcdm = m_conv->from(cat, "mg/dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpcdm, 0.0014);

    // ### milligram per liter

    // convert
    double valMgpl = m_conv->from(cat, "mg/l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpl, 0.0014);

    // ### gram per cubic decimeter

    // convert
    double valGpcdm = m_conv->from(cat, "g/dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpcdm, 1.4);

    // ### gram per liter

    // convert
    double valGpl = m_conv->from(cat, "g/l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpl, 1.4);

    // ### kilogram per cubic decimeter

    // convert
    double valKgpcdm = m_conv->from(cat, "kg/dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgpcdm, 1400.);

    // ### kilogram per liter

    // convert
    double valKgpl = m_conv->from(cat, "kg/l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgpl, 1400.);

    // ### tonne per cubic decimeter

    // convert
    double valTpcdm = m_conv->from(cat, "t/dm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpcdm, 1.40e+6);

    // ### tonne per liter

    // convert
    double valTpl = m_conv->from(cat, "t/l", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpl, 1.40e+6);

    // ### milligram per cubic centimeter

    // convert
    double valMgpccm = m_conv->from(cat, "mg/cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgpccm, 1.4);

    // ### gram per cubic centimeter

    // convert
    double valGpccm = m_conv->from(cat, "g/cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGpccm, 1400.);

    // ### kilogram per cubic centimeter

    // convert
    double valKgpccm = m_conv->from(cat, "kg/cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgpccm, 1.40e+6);

    // ### tonne per cubic centimeter

    // convert
    double valTpccm = m_conv->from(cat, "t/cm^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTpccm, 1.40e+9);

    // ### pound-mass per cubic inch

    // convert
    double valLbmpcin = m_conv->from(cat, "lbm/in^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmpcin, 38751.866594284369671101546927503);

    // ### pound-mass per cubic foot

    // convert
    double valLbmpcft = m_conv->from(cat, "lbm/ft^3", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmpcft, 22.425848723544195411517098916379);

    // ### pound-mass per us gallon

    // convert
    double valLbmpusgal = m_conv->from(cat, "lbm/gal", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmpusgal, 167.75699824365527996147855812772);
}


/// Force
TEST_F(TestGtUnitConverter, convertForce)
{
    GtUnit::Category cat = GtUnit::Force;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "N");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Force");
}

TEST_F(TestGtUnitConverter, convertForceTo)
{
    GtUnit::Category cat = GtUnit::Force;

    // input
    double val = 1400.;
    bool check = false;

    // ### millinewton

    // convert
    double valMN = m_conv->To(cat, "mN", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMN, 1400000.);

    // ### kilonewton

    // convert
    double valKN = m_conv->To(cat, "kN", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKN, 1.4);

    // ### meganewton

    // convert
    double valMgN = m_conv->To(cat, "MN", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgN, 0.0014);

    // ### pound-force

    // convert
    double valLbf = m_conv->To(cat, "lbf", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbf, 314.73252033959467607405517876445);

    // ### kilopond

    // convert
    double valKp = m_conv->To(cat, "kp", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKp, 142.76026981690995395981298404654);

    // ### megapond

    // convert
    double valMp = m_conv->To(cat, "Mp", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMp, 0.14276026981690995395981298404654);

    // ### dyne

    // convert
    double valDyn = m_conv->To(cat, "dyn", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDyn, 1.4e+8);

    // ### poundal

    // convert
    double valPdl = m_conv->To(cat, "pdl", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPdl, 10126.219391693852132944990875428);
}

TEST_F(TestGtUnitConverter, convertForceFrom)
{
    GtUnit::Category cat = GtUnit::Force;

    // input
    double val = 1.4;
    bool check = false;

    // ### millinewton

    // convert
    double valMN = m_conv->from(cat, "mN", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMN, 0.0014);

    // ### kilonewton

    // convert
    double valKN = m_conv->from(cat, "kN", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKN, 1400.);

    // ### meganewton

    // convert
    double valMgN = m_conv->from(cat, "MN", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMgN, 1400000.);

    // ### pound-force

    // convert
    double valLbf = m_conv->from(cat, "lbf", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbf, 6.2275102613647);

    // ### kilopond

    // convert
    double valKp = m_conv->from(cat, "kp", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKp, 13.72931);

    // ### kilopond

    // convert
    double valMp = m_conv->from(cat, "Mp", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMp, 13729.31);

    // ### dyne

    // convert
    double valDyn = m_conv->from(cat, "dyn", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valDyn, 1.4e-5);

    // ### poundal

    // convert
    double valPdl = m_conv->from(cat, "pdl", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPdl, 0.1935569361264);
}


/// Frequency
TEST_F(TestGtUnitConverter, convertFrequency)
{
    GtUnit::Category cat = GtUnit::Frequency;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "1/s");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Frequency");
}

TEST_F(TestGtUnitConverter, convertFrequencyTo)
{
    GtUnit::Category cat = GtUnit::Frequency;

    // input
    double val = 14.;
    bool check = false;

    // ### hertz

    // convert
    double valHz = m_conv->To(cat, "Hz", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valHz, 14.);

    // ### 1/min

    // convert
    double valPermin = m_conv->To(cat, "1/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPermin, 840.);

    // ### 1/hour

    // convert
    double valPh = m_conv->To(cat, "1/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPh, 50400.);

    // ### rounds per minute

    // convert
    double valRpm = m_conv->To(cat, "rpm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valRpm, 840.);
}

TEST_F(TestGtUnitConverter, convertFrequencyFrom)
{
    GtUnit::Category cat = GtUnit::Frequency;

    // input
    double val = 1400.;
    bool check = false;

    // ### hertz

    // convert
    double valHz = m_conv->from(cat, "Hz", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valHz, 1400.);

    // ### 1/min

    // convert
    double valPermin = m_conv->from(cat, "1/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPermin, 23.333333333333333333);

    // ### 1/hour

    // convert
    double valPh = m_conv->from(cat, "1/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPh, 0.388888888888888888889);

    // ### rounds per minute

    // convert
    double valRpm = m_conv->from(cat, "rpm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valRpm, 23.333333333333333333);
}


/// Torque
TEST_F(TestGtUnitConverter, convertTorque)
{
    GtUnit::Category cat = GtUnit::Torque;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "N*m");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Torque");
}

TEST_F(TestGtUnitConverter, convertTorqueTo)
{
    GtUnit::Category cat = GtUnit::Torque;

    // input
    double val = 14.;
    bool check = false;

    // ### kilonewton * meter

    // convert
    double valKnm = m_conv->To(cat, "kN*m", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKnm, 0.014);

    // ### kilogram-force * meter

    // convert
    double valKgfm = m_conv->To(cat, "kgf*m", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgfm, 1.4276026981690995395981298404654);

    // ### kilopond * meter

    // convert
    double valKpm = m_conv->To(cat, "kp*m", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKpm, 1.4276026981690995395981298404654);

    // ### pound-force * inch

    // convert
    double valLbfin = m_conv->To(cat, "lbf*in", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbfin, 123.91044107858058113151778691514);   //1.5817876063866338

    // ### pound-force * foot

    // convert
    double valLbfft = m_conv->To(cat, "lbf*ft", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbfft, 10.325870089881715094293148909595);
}

TEST_F(TestGtUnitConverter, convertTorqueFrom)
{
    GtUnit::Category cat = GtUnit::Torque;

    // input
    double val = 14.;
    bool check = false;

    // ### kilonewton * meter

    // convert
    double valKnm = m_conv->from(cat, "kN*m", val, &check);

    // check1
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKnm, 14000.);

    // ### kilogram-force * meter

    // convert
    double valKgfm = m_conv->from(cat, "kgf*m", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgfm, 137.2931);

    // ### kilopond * meter

    // convert
    double valKpm = m_conv->from(cat, "kp*m", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKpm, 137.2931);

    // ### pound-force * inch

    // convert
    double valLbfin = m_conv->from(cat, "lbf*in", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbfin, 1.5817876063866338);

    // ### pound-force * foot

    // convert
    double valLbfft = m_conv->from(cat, "lbf*ft", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbfft, 18.9814512766396056);
}


/// Pressure
TEST_F(TestGtUnitConverter, convertPressure)
{
    GtUnit::Category cat = GtUnit::Pressure;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "Pa");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Pressure");
}

TEST_F(TestGtUnitConverter, convertPressureTo)
{
    GtUnit::Category cat = GtUnit::Pressure;

    // input
    double val = 1.4e6;
    bool check = false;

    // ### newton per square meter

    // convert
    double valNpsqm = m_conv->To(cat, "N/m^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valNpsqm, 1.4e6);

    // ### kilopascal

    // convert
    double valKpa = m_conv->To(cat, "kPa", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKpa, 1.4e3);

    // ### megapascal

    // convert
    double valMpa = m_conv->To(cat, "MPa", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMpa, 1.4);

    // ### millibar

    // convert
    double valMbar = m_conv->To(cat, "mbar", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMbar, 1.4e4);

    // ### bar

    // convert
    double valBar = m_conv->To(cat, "bar", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBar, 14.);

    // ### technical atmosphere

    // convert
    double valAt = m_conv->To(cat, "at", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valAt, 14.276026981690995395981298404654);

    // ### standard atmosphere

    // convert
    double valAtm = m_conv->To(cat, "atm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valAtm, 13.816925734024179620034542314335);

    // ### Torr

    // convert
    double valTorr = m_conv->To(cat, "Torr", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTorr, 10500.863557858376511226252158895);

    // ### millimeter of mercury

    // convert
    double valMmHg = m_conv->To(cat, "mmHg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMmHg, 10500.862061839188675317797150925);

    // ### pound-force per square inch

    // convert
    double valPsi = m_conv->To(cat, "psi", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPsi, 203.05283282229290121593743913167);
}

TEST_F(TestGtUnitConverter, convertPressureFrom)
{
    GtUnit::Category cat = GtUnit::Pressure;

    // input
    double val = 14.;
    bool check = false;

    // ### newton per square meter

    // convert
    double valNpsqm = m_conv->from(cat, "N/m^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valNpsqm, 14.);

    // ### kilopascal

    // convert
    double valKpa = m_conv->from(cat, "kPa", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKpa, 1.4e4);

    // ### megapascal

    // convert
    double valMpa = m_conv->from(cat, "MPa", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMpa, 1.4e7);

    // ### millibar

    // convert
    double valMbar = m_conv->from(cat, "mbar", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMbar, 1.4e3);

    // ### bar

    // convert
    double valBar = m_conv->from(cat, "bar", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBar, 1.4e6);

    // ### technical atmosphere

    // convert
    double valAt = m_conv->from(cat, "at", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valAt, 1372931.);

    // ### standard atmosphere

    // convert
    double valAtm = m_conv->from(cat, "atm", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valAtm, 1418550.);

    // ### Torr

    // convert
    double valTorr = m_conv->from(cat, "Torr", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valTorr, 1866.5131578947368421052631578947);

    // ### millimeter of mercury

    // convert
    double valMmhg = m_conv->from(cat, "mmHg", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMmhg, 1866.51342381);

    // ### pound-force per square inch

    // convert
    double valPsi = m_conv->from(cat, "psi", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPsi, 96526.602104357058714117428234856);
}


/// Energy
TEST_F(TestGtUnitConverter, convertEnergy)
{
    GtUnit::Category cat = GtUnit::Energy;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "J");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Energy");
}

TEST_F(TestGtUnitConverter, convertEnergyTo)
{

    GtUnit::Category cat = GtUnit::Energy;

    // input
    double val = 1.4e4;
    bool check = false;

    // ### kilojoule

    // convert
    double valKj = m_conv->To(cat, "kJ", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKj, 1.4e1);

    // ### watt * hour

    // convert
    double valWh = m_conv->To(cat, "W*h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valWh, 3.88888888888888888888888888888889);

    // ### kilowatt * hour

    // convert
    double valKwh = m_conv->To(cat, "kW*h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKwh, 0.00388888888888888888888888888889);

    // ### calorie

    // convert
    double valCal = m_conv->To(cat, "cal", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCal, 3346.080305927342);

    // ### kilocalorie

    // convert
    double valKcal = m_conv->To(cat, "kcal", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKcal, 3.346080305927342);

    // ### british thermal unit (ISO)

    // convert
    double valBtu = m_conv->To(cat, "BTU", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBtu, 13.269437830788128781789781774617);

    // ### foot * pound(-force)

    // convert
    double valFtlbf = m_conv->To(cat, "ft*lbf", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFtlbf, 10325.870089881715094293148909595);
}

TEST_F(TestGtUnitConverter, convertEnergyFrom)
{
    GtUnit::Category cat = GtUnit::Energy;

    // input
    double val = 14.;
    bool check = false;

    // ### kilojoule

    // convert
    double valKj = m_conv->from(cat, "kJ", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKj, 14000.);

    // ### watt * hour

    // convert
    double valWh = m_conv->from(cat, "W*h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valWh, 50400.);

    // ### kilowatt * hour

    // convert
    double valKwh = m_conv->from(cat, "kW*h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKwh, 50400000.);

    // ### calorie

    // convert
    double valCal = m_conv->from(cat, "cal", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valCal, 58.576);

    // ### kilocalorie

    // convert
    double valKcal = m_conv->from(cat, "kcal", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKcal, 58576.);

    // ### british thermal unit (ISO)

    // convert
    double valBtu = m_conv->from(cat, "BTU", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBtu, 14770.784);

    // ### foot * pound(-force)

    // convert
    double valFtlbf = m_conv->from(cat, "ft*lbf", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFtlbf, 18.9814512766396056);
}


/// Power
/// //#######################################################################################
TEST_F(TestGtUnitConverter, convertPower)
{
    GtUnit::Category cat = GtUnit::Power;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "W");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Power");
}

TEST_F(TestGtUnitConverter, convertPowerTo)
{
    GtUnit::Category cat = GtUnit::Power;

    // input
    double val = 1.4e4;
    bool check = false;

    // ### kilowatt

    // convert
    double valKw = m_conv->To(cat, "kW", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKw, 14.);

    // ### megawatt

    // convert
    double valMw = m_conv->To(cat, "MW", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMw, 0.014);

    // ### gigawatt

    // convert
    double valGw = m_conv->To(cat, "GW", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGw, 0.000014);

    // ### Pferdestaerke (metric horse power)

    // convert
    double valPs = m_conv->To(cat, "PS", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPs, 19.034702642254660527975064539539);

    // ### mechanical horse power

    // convert
    double valHp = m_conv->To(cat, "hp", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valHp, 18.774309254330391080532998017445);

    // ### british thermal unit (ISO) per second

    // convert
    double valBtups= m_conv->To(cat, "BTU/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBtups, 13.269437830788128781789781774617);

    // ### british thermal unit (ISO) per minute

    // convert
    double valBtupmin = m_conv->To(cat, "BTU/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBtupmin, 796.16626984728772690738690647702);//


    // ### british thermal unit (ISO) per hour

    // convert
    double valBtuph = m_conv->To(cat, "BTU/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBtuph, 47769.976190837263614443214388621);

    // ### foot * pound(-force) per second

    // convert
    double valFtlbfps = m_conv->To(cat, "ft*lbf/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFtlbfps, 10325.870089881715094293148909595);

    // ### foot * pound(-force) per minute

    // convert
    double valFtlbfpmin = m_conv->To(cat, "ft*lbf/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFtlbfpmin, 619552.2053929029056575889345757);

    // ### foot * pound(-force) per hour

    // convert
    double valFtlbfph = m_conv->To(cat, "ft*lbf/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFtlbfph, 37173132.323574174339455336074542);
}

TEST_F(TestGtUnitConverter, convertPowerFrom)
{
    GtUnit::Category cat = GtUnit::Power;

    // input
    double val = 14.;
    bool check = false;

    // ### kilowatt

    // convert
    double valKw = m_conv->from(cat, "kW", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKw, 14000.);

    // ### megawatt

    // convert
    double valMw = m_conv->from(cat, "MW", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valMw, 14.e+6);

    // ### gigawatt

    // convert
    double valGw = m_conv->from(cat, "GW", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGw, 14.e+9);

    // ### Pferdestaerke (metric horse power)

    // convert
    double valPs = m_conv->from(cat, "PS", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valPs, 10296.9825);

    // ### horse power

    // convert
    double valHp = m_conv->from(cat, "hp", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valHp, 10439.79820215178308);

    // ### british thermal unit (ISO) per second

    // convert
    double valBtups = m_conv->from(cat, "BTU/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBtups, 14770.784);

    // ### british thermal unit (ISO) per minute

    // convert
    double valBtupmin = m_conv->from(cat, "BTU/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBtupmin, 246.17973333333333333333333333333);

    // ### british thermal unit (ISO) per hour

    // convert
    double valBtuph = m_conv->from(cat, "BTU/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valBtuph, 4.1029955555555555555555555555556);

    // ### foot * pound(-force) per second

    // convert
    double valFtlbfps = m_conv->from(cat, "ft*lbf/s", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFtlbfps, 18.9814512766396056);

    // ### foot * pound(-force) per minute

    // convert
    double valFtlbfpmin = m_conv->from(cat, "ft*lbf/min", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFtlbfpmin, 0.31635752127732676);

    // ### foot * pound(-force) per hour

    // convert
    double valFtlbfph = m_conv->from(cat, "ft*lbf/h", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valFtlbfph, 0.00527262535462211266666666666667);
}


/// Inertia
TEST_F(TestGtUnitConverter, convertInertia)
{
    GtUnit::Category cat = GtUnit::Inertia;

    QString siUnit = GtUnit::siUnit(cat);
    QString catString = GtUnit::categoryToString(cat);

    // check default si unit
    ASSERT_STREQ(siUnit.toStdString().c_str(), "kg*m^2");

    // check default string
    ASSERT_STREQ(catString.toStdString().c_str(), "Inertia");
}

TEST_F(TestGtUnitConverter, convertInertiaTo)
{
    GtUnit::Category cat = GtUnit::Inertia;

    // input
    double val = 14.;
    bool check = false;

    // ### gram * square millimeter

    // convert
    double valGsqmm = m_conv->To(cat, "g*mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGsqmm, 14.e+9);

    // ### gram * square centimeter

    // convert
    double valGsqcm = m_conv->To(cat, "g*cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGsqcm, 14.e+7);

    // ### gram * square meter

    // convert
    double valGsqm = m_conv->To(cat, "g*m^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGsqm, 14000.);

    // ### kilogram * square millimeter

    // convert
    double valKgsqmm = m_conv->To(cat, "kg*mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgsqmm, 14.e+6);

    // ### kilogram * square centimeter

    // convert
    double valKgsqcm = m_conv->To(cat, "kg*cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgsqcm, 14.e+4);

    // ### pound-mass * square inch

    // convert
    double valLbmsqin = m_conv->To(cat, "lbm*in^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmsqin, 47840.406574931584880055075002021);

    // ### pound-mass * square foot

    // convert
    double valLbmsqft = m_conv->To(cat, "lbm*ft^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmsqft, 332.22504565924711722260468751403);

    // ### us (short) ton * square inch

    // convert
    double valUstsqin = m_conv->To(cat, "(tn. sh.)*in^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstsqin, 23.920203287465792440027537501011);

    // ### us (short) ton * square foot

    // convert
    double valUstsqft = m_conv->To(cat, "(tn. sh.)*ft^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstsqft, 0.16611252282962355861130234375702);
}

TEST_F(TestGtUnitConverter, convertInertiaFrom)
{
    GtUnit::Category cat = GtUnit::Inertia;

    // input
    double val = 14.;
    bool check = false;

    // ### gram * square millimeter

    // convert
    double valGsqmm = m_conv->from(cat, "g*mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGsqmm, 14.e-9);

    // ### gram * square centimeter

    // convert
    double valGsqcm = m_conv->from(cat, "g*cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGsqcm, 14.e-7);

    // ### gram * square meter

    // convert
    double valGsqm = m_conv->from(cat, "g*m^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valGsqm, 0.014);

    // ### kilogram * square millimeter

    // convert
    double valKgsqmm = m_conv->from(cat, "kg*mm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgsqmm, 14.e-6);

    // ### kilogram * square centimeter

    // convert
    double valKgsqcm = m_conv->from(cat, "kg*cm^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valKgsqcm, 0.0014);

    // ### pound-mass * square inch

    // convert
    double valLbmsqin = m_conv->from(cat, "lbm*in^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmsqin, 0.0040969551480088);

    // ### pound-mass * square foot

    // convert
    double valLbmsqft = m_conv->from(cat, "lbm*ft^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valLbmsqft, 0.5899615413132672);

    // ### us (short) ton * square inch

    // convert
    double valUstsqin = m_conv->from(cat, "(tn. sh.)*in^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstsqin, 8.1939102960176);

    // ### us (short) ton * square foot

    // convert
    double valUstsqft = m_conv->from(cat, "(tn. sh.)*ft^2", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valUstsqft, 1179.9230826265344);
}

/// Non Dimensional
TEST_F(TestGtUnitConverter, convertNonDimensional)
{
    GtUnit::Category cat = GtUnit::NonDimensionalPercentage;

    // input
    double val = 0.8;
    bool check = false;

    // ### percentage

    // convert
    double valsPercent = m_conv->To(cat, "%", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valsPercent, 80.0);

    // ### simple "-"

    // convert
    double valsBasic = m_conv->To(cat, "-", val, &check);

    // check
    ASSERT_TRUE(check);
    ASSERT_DOUBLE_EQ(valsBasic, 0.8);

    // input
    double val2 = 0.8;
    bool check2 = false;

    // ### simple "-"

    // convert
    double valsBasic2 = m_conv->To(cat, "-", val2, &check2);

    // check
    ASSERT_TRUE(check2);
    ASSERT_DOUBLE_EQ(valsBasic2, 0.8);
}



