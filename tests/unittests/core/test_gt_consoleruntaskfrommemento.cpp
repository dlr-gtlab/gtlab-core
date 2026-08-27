/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_calculator.h"
#include "gt_calculatorfactory.h"
#include "gt_consoleruntaskfrommemento.h"
#include "gt_coreapplication.h"
#include "gt_intproperty.h"
#include "gt_objectfactory.h"
#include "gt_objectgroup.h"
#include "gt_objectlinkproperty.h"
#include "gt_objectmementodiff.h"
#include "gt_package.h"
#include "gt_project.h"
#include "gt_task.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>

#include <memory>

namespace
{

    class TestApplication : public GtCoreApplication
    {
    public:
        TestApplication() : GtCoreApplication(qApp, AppMode::Batch)
        {
            init();
            initDatamodel();
            initCalculators();
        }

    protected:
        bool initFirstRun() override
        {
            return true;
        }
    };

    class MementoTestPackage final : public GtPackage
    {
        Q_OBJECT
    public:
        Q_INVOKABLE MementoTestPackage() : value("value", "Value", "Value")
        {
            registerProperty(value);
        }
        GtIntProperty value;
    };

    class MementoTestCalculator final : public GtCalculator
    {
        Q_OBJECT
    public:
        Q_INVOKABLE MementoTestCalculator() :
            packageLink("package", "Package", "Package", this,
                        {GT_CLASSNAME(MementoTestPackage)})
        {
            registerProperty(packageLink);
        }

        bool run() override
        {
            observedProjectPath = gtApp && gtApp->currentProject()
                                      ? gtApp->currentProject()->path()
                                      : QString{};
            observedWorkingDirectory = QDir::currentPath();
            for (auto const& object : linkedObjects())
            {
                if (auto* package = qobject_cast<MementoTestPackage*>(object))
                {
                    package->value.setVal(42);
                    return true;
                }
            }
            return false;
        }

        void setPackageUuid(QString const& uuid)
        {
            packageLink.setVal(uuid);
        }

        static QString observedProjectPath;
        static QString observedWorkingDirectory;

    private:
        GtObjectLinkProperty packageLink;
    };

    QString MementoTestCalculator::observedProjectPath;
    QString MementoTestCalculator::observedWorkingDirectory;

    bool writeFile(QString const& path, QByteArray const& data)
    {
        QFile file(path);
        return file.open(QIODevice::WriteOnly) &&
               file.write(data) == data.size();
    }

    class TestGtConsoleRunTaskFromMemento : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            application = std::make_unique<TestApplication>();
            gtObjectFactory->registerClass(
                MementoTestPackage::staticMetaObject);
            gtCalculatorFactory->registerClass(
                MementoTestCalculator::staticMetaObject);
            ASSERT_TRUE(tempDir.isValid());
            workingDirectory =
                QDir(tempDir.path()).filePath("paths with spaces");
            ASSERT_TRUE(QDir().mkpath(workingDirectory));
            projectPath =
                QDir(workingDirectory).filePath("project memento.xml");
            taskPath = QDir(workingDirectory).filePath("task memento.xml");
            outputPath = QDir(workingDirectory).filePath("result diff.xml");
        }

        std::unique_ptr<TestApplication> application;
        QTemporaryDir tempDir;
        QString workingDirectory;
        QString projectPath;
        QString taskPath;
        QString outputPath;
    };

    TEST_F(TestGtConsoleRunTaskFromMemento,
           ExecutesWithProjectContextAndProducesApplicableDiff)
    {
        GtObjectGroup projectData;
        projectData.setUuid("-");
        projectData.setObjectName("Memento Root");
        auto package = std::make_unique<MementoTestPackage>();
        package->setFactory(gtObjectFactory);
        package->setObjectName("Package");
        const QString packageUuid = package->uuid();
        ASSERT_TRUE(projectData.appendChild(package.get()));
        package.release();

        GtObjectMemento original = projectData.toMemento(true);
        ASSERT_TRUE(writeFile(projectPath, original.toByteArray()));

        GtTask task;
        task.setFactory(gtObjectFactory);
        auto calculator = std::make_unique<MementoTestCalculator>();
        calculator->setFactory(gtCalculatorFactory);
        calculator->setPackageUuid(packageUuid);
        ASSERT_TRUE(task.appendChild(calculator.get()));
        calculator.release();
        ASSERT_TRUE(writeFile(taskPath, task.toMemento().toByteArray()));

        EXPECT_EQ(gt::console::runTaskFromMemento({"-p", projectPath, "-t",
                                                   taskPath, "-o", outputPath,
                                                   "-w", workingDirectory}),
                  0);
        EXPECT_EQ(MementoTestCalculator::observedProjectPath, workingDirectory);
        EXPECT_EQ(MementoTestCalculator::observedWorkingDirectory,
                  workingDirectory);

        QFile output(outputPath);
        ASSERT_TRUE(output.open(QIODevice::ReadOnly));
        GtObjectMementoDiff diff(output.readAll());
        EXPECT_FALSE(diff.isNull());
        auto restored = std::unique_ptr<GtObjectGroup>(
            original.restore<GtObjectGroup*>(gtObjectFactory));
        ASSERT_TRUE(restored);
        ASSERT_TRUE(restored->applyDiff(diff));
        EXPECT_EQ(restored->objectName(), "Memento Root");
        auto* restoredPackage =
            restored->findDirectChild<MementoTestPackage*>();
        ASSERT_TRUE(restoredPackage);
        EXPECT_EQ(restoredPackage->value.getVal(), 42);
    }

    TEST_F(TestGtConsoleRunTaskFromMemento, InvalidInputDoesNotPublishOutput)
    {
        ASSERT_TRUE(writeFile(projectPath, "not xml"));
        ASSERT_TRUE(writeFile(taskPath, "not xml"));
        EXPECT_NE(gt::console::runTaskFromMemento(
                      {"--project-memento", projectPath, "--task-memento",
                       taskPath, "--output-diff", outputPath}),
                  0);
        EXPECT_FALSE(QFileInfo::exists(outputPath));
    }

} // namespace

#include "test_gt_consoleruntaskfrommemento.moc"
