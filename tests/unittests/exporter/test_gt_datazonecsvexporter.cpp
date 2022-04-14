/* GTlab - Gas Turbine laboratory
 * Source File: test_gt_datazonecsvexporter.cpp
 * copyright 2009-2021 by DLR
 *
 *  Created on: 05.04.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gtest/gtest.h"

#include <QDir>
#include <QFile>
#include <QDateTime>
#include <QDebug>

#include "gt_datazonecsvexporter.h"
#include "gt_datazone.h"
#include "gt_datazone0d.h"
#include "gt_testhelper.h"

/// This is a test fixture that does a init for each test
class TestGtDataZoneCsvExporter : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        exporter = new GtDataZoneCsvExporter;

        dz1D = new GtDataZone;
        dz2D = new GtDataZone;
        dz0D = new GtDataZone0D;

        {
            auto data = dz1D->fetchData();
            data.setData1D({ "A", "B", "C", "D"},
                           {{1.0, { 0.1, 1.2, 2.3, 3.4 }},
                            {2.0, { 1.1, 2.2, 3.3, 4.4 }},
                            {3.0, { 2.1, 3.2, 4.3, 5.4 }},
                            {4.0, { 3.1, 4.2, 5.3, 6.4 }}},
                           "axis_1",
                           { "[-]", "[K]", "[s]", "[m]"});
            ASSERT_TRUE(data.isValid());
            // same as sample
            dz1D->setUuid("{d2b1e6e4-d1d2-477e-a99d-3be6cac32c7d}");
            dz1D->setObjectName("DZ1D");
        }

        {
            auto data = dz2D->fetchData();
            data.setData2D({ "A", "B", "C", "D"},
                           { 1.0, 2.0 },
                           { 2.0, 3.0, 4.0 },
                           "axis_1",
                           "axis_2",
                           {{ "A", { 0.1, 1.2, 2.3, 3.4, 4.5, 5.6 }},
                            { "B", { 1.1, 2.2, 3.3, 4.4, 5.5, 6.6 }},
                            { "C", { 2.1, 3.2, 4.3, 5.4, 6.5, 7.6 }},
                            { "D", { 3.1, 4.2, 5.3, 6.4, 7.5, 8.6 }}},
                           { "[-]", "[K]", "[s]", "[m]"});
            ASSERT_TRUE(data.isValid());
            // same as sample
            dz2D->setUuid("{52eaed7e-e2bc-45d2-bfdf-544c98fb2ea8}");
            dz2D->setObjectName("DZ2D");
        }

        {
            auto data = dz0D->fetchData();
            data.setData({ "A", "B", "C", "D"},
                         { 0.1, 1.2, 2.3, 3.4 },
                         { "[-]", "[K]", "[s]", "[m]"});
            ASSERT_TRUE(data.isValid());
            // same as sample
            dz0D->setUuid("{3ca51532-90ec-4b3d-9e81-48799c720525}");
            dz0D->setObjectName("DZ0D");
        }
    }

    virtual void TearDown()
    {
        delete exporter;
        delete dz0D;
        delete dz1D;
        delete dz2D;
    }

    /// export contains current time. this must be extracted for comparing
    /// results
    QByteArray getExportTime(const QByteArray& fileContent)
    {
        QByteArray line = fileContent.split('\n')[3];
        QByteArray string = "\"Export Time\";\"";

        if (!line.startsWith(string))
        {
            qCritical() << "Export Time not found!";
            return {};
        }

        auto timeFormat = QDateTime::currentDateTime().
                toString("yyyyMMdd-hh:mm:ss");

        return line.mid(string.length(), timeFormat.length());
    }

    GtDataZoneCsvExporter* exporter{};
    GtDataZone* dz1D{};
    GtDataZone* dz2D{};
    GtDataZone0D* dz0D{};

    QDir m_dir{gtTestHelper->newTempDir()};
    QString m_fileName{"data_"};
    QString m_suffix{".csv"};
};

TEST_F(TestGtDataZoneCsvExporter, doExport0D)
{
    QString filePath{m_fileName + "0D" + m_suffix};
    QFile file{m_dir.absoluteFilePath(filePath)};

    qDebug() << "exporting csv to:" << file.fileName();


    // export
    EXPECT_TRUE(exporter->doExport(dz0D, file));
    EXPECT_TRUE(file.exists());

    file.close(); // file is still opened here
    EXPECT_TRUE(file.open(QFile::ReadOnly | QFile::Text));

    /// compare to sample
    QFile rsc{":/unittests/" + filePath};
    EXPECT_TRUE(rsc.open(QFile::ReadOnly | QFile::Text));

    auto fileContent = file.readAll();
    auto rscContent = rsc.readAll();
    rscContent.replace("$$TIME$$", getExportTime(fileContent));
    EXPECT_EQ(fileContent, rscContent);
}

TEST_F(TestGtDataZoneCsvExporter, doExport1D)
{
    QString filePath{m_fileName + "1D" + m_suffix};
    QFile file{m_dir.absoluteFilePath(filePath)};

    qDebug() << "exporting csv to:" << file.fileName();

    // export
    EXPECT_TRUE(exporter->doExport(dz1D, file));
    EXPECT_TRUE(file.exists());

    file.close(); // file is still opened here
    EXPECT_TRUE(file.open(QFile::ReadOnly | QFile::Text));

    /// compare to sample
    QFile rsc{":/unittests/" + filePath};
    EXPECT_TRUE(rsc.open(QFile::ReadOnly | QFile::Text));

    auto fileContent = file.readAll();
    auto rscContent = rsc.readAll();
    rscContent.replace("$$TIME$$", getExportTime(fileContent));
    EXPECT_EQ(fileContent, rscContent);
}

TEST_F(TestGtDataZoneCsvExporter, doExport2D)
{
    QString filePath{m_fileName + "2D" + m_suffix};
    QFile file{m_dir.absoluteFilePath(filePath)};

    qDebug() << "exporting csv to:" << file.fileName();

    // export
    EXPECT_TRUE(exporter->doExport(dz2D, file));
    EXPECT_TRUE(file.exists());

    file.close(); // file is still opened here
    EXPECT_TRUE(file.open(QFile::ReadOnly | QFile::Text));

    /// compare to sample
    QFile rsc{":/unittests/" + filePath};
    EXPECT_TRUE(rsc.open(QFile::ReadOnly | QFile::Text));

    auto fileContent = file.readAll();
    auto rscContent = rsc.readAll();
    rscContent.replace("$$TIME$$", getExportTime(fileContent));
    EXPECT_EQ(fileContent, rscContent);
}
