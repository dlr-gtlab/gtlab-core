#include "gt_settings.h"
#include "gtest/gtest.h"

#include <gt_abstractsettings.h>

#include <QCoreApplication>
#include <QSettings>

class TestSettings : public testing::Test
{
public:
    void SetUp() override
    {
        QCoreApplication::setOrganizationDomain("www.dlr.de");
        QCoreApplication::setOrganizationName("DLR");
        QCoreApplication::setApplicationName("SettingsUnitTests");
        QSettings().clear();

        settings.registerSetting("test/adouble", 123.);
    }

    void TearDown() override
    {
        QSettings().clear();
    }

    GtAbstractSettings settings;
};

TEST_F(TestSettings, setGet)
{
    settings.setSetting("test/adouble", 321.);

    auto value = settings.getSetting("test/adouble");
    ASSERT_TRUE(value.isValid());

    EXPECT_EQ(321., value.value<double>());
}


TEST_F(TestSettings, getDefault)
{
    auto value = settings.getSetting("test/adouble");
    ASSERT_TRUE(value.isValid());

    EXPECT_EQ(123., value.value<double>());
}

TEST_F(TestSettings, getUnknown)
{
    auto value = settings.getSetting("test/_unknown_");
    ASSERT_TRUE(value.isNull());
}

/// check if module settings path actaully contains the correct name
TEST_F(TestSettings, moduleSettingsPath)
{
    QString value = gt::makeModuleSettingsPath("bla");
    ASSERT_TRUE(value.contains("Unittests"));
}

/// check if module settings path is correct (using a module id)
TEST_F(TestSettings, moduleSettingsPathWithModuleID)
{
    QString value = gt::makeModuleSettingsPath("mymodule", "bla");
    ASSERT_TRUE(value.contains("mymodule/bla"));
}

