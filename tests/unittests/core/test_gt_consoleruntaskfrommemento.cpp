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

    class MementoTestObject final : public GtObject
    {
        Q_OBJECT
    public:
        Q_INVOKABLE MementoTestObject() = default;
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
            if (!shouldSucceed)
            {
                return false;
            }
            observedProjectPath = gtApp && gtApp->currentProject()
                                      ? gtApp->currentProject()->path()
                                      : QString{};
            observedWorkingDirectory = QDir::currentPath();
            for (auto const& object : linkedObjects())
            {
                if (auto* package = qobject_cast<MementoTestPackage*>(object))
                {
                    package->value.setVal(42);
                    setWarningFlag(produceWarning);
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
        static bool produceWarning;
        static bool shouldSucceed;

    private:
        GtObjectLinkProperty packageLink;
    };

    QString MementoTestCalculator::observedProjectPath;
    QString MementoTestCalculator::observedWorkingDirectory;
    bool MementoTestCalculator::produceWarning = false;
    bool MementoTestCalculator::shouldSucceed = true;

    bool writeFile(QString const& path, QByteArray const& data)
    {
        QFile file(path);
        return file.open(QIODevice::WriteOnly) &&
               file.write(data) == data.size();
    }

    bool writeRunnableMementos(QString const& projectPath,
                              QString const& taskPath)
    {
        GtObjectGroup projectData;
        auto package = std::make_unique<MementoTestPackage>();
        package->setFactory(gtObjectFactory);
        const QString packageUuid = package->uuid();
        if (!projectData.appendChild(package.get()))
        {
            return false;
        }
        package.release();

        GtTask task;
        task.setFactory(gtObjectFactory);
        auto calculator = std::make_unique<MementoTestCalculator>();
        calculator->setFactory(gtCalculatorFactory);
        calculator->setPackageUuid(packageUuid);
        if (!task.appendChild(calculator.get()))
        {
            return false;
        }
        calculator.release();

        return writeFile(projectPath,
                         projectData.toMemento(true).toByteArray()) &&
               writeFile(taskPath, task.toMemento().toByteArray());
    }

    class TestGtConsoleRunTaskFromMemento : public ::testing::Test
    {
    protected:
        void SetUp() override
        {
            MementoTestCalculator::produceWarning = false;
            MementoTestCalculator::shouldSucceed = true;
            application = std::make_unique<TestApplication>();
            gtObjectFactory->registerClass(
                MementoTestPackage::staticMetaObject);
            gtObjectFactory->registerClass(
                MementoTestObject::staticMetaObject);
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

    TEST_F(TestGtConsoleRunTaskFromMemento,
           InvalidInputPreservesExistingOutput)
    {
        ASSERT_TRUE(writeFile(projectPath, "not xml"));
        ASSERT_TRUE(writeFile(taskPath, "not xml"));
        ASSERT_TRUE(writeFile(outputPath, "stale diff"));
        EXPECT_NE(gt::console::runTaskFromMemento(
                      {"--project-memento", projectPath, "--task-memento",
                       taskPath, "--output-diff", outputPath}),
                  0);
        QFile output(outputPath);
        ASSERT_TRUE(output.open(QIODevice::ReadOnly));
        EXPECT_EQ(output.readAll(), "stale diff");
    }

    TEST_F(TestGtConsoleRunTaskFromMemento, OutputCannotOverwriteInputMemento)
    {
        ASSERT_TRUE(writeFile(projectPath, "input"));
        EXPECT_NE(gt::console::runTaskFromMemento(
                      {"-p", projectPath, "-t", taskPath, "-o", projectPath}),
                  0);
        EXPECT_TRUE(QFileInfo::exists(projectPath));
    }

    TEST_F(TestGtConsoleRunTaskFromMemento,
           WarningFinishedTaskProducesApplicableDiff)
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

        MementoTestCalculator::produceWarning = true;
        EXPECT_EQ(gt::console::runTaskFromMemento(
                      {"-p", projectPath, "-t", taskPath, "-o", outputPath}),
                  0);

        QFile output(outputPath);
        ASSERT_TRUE(output.open(QIODevice::ReadOnly));
        GtObjectMementoDiff diff(output.readAll());
        EXPECT_FALSE(diff.isNull());
        auto restored = std::unique_ptr<GtObjectGroup>(
            original.restore<GtObjectGroup*>(gtObjectFactory));
        ASSERT_TRUE(restored);
        ASSERT_TRUE(restored->applyDiff(diff));
        auto* restoredPackage =
            restored->findDirectChild<MementoTestPackage*>();
        ASSERT_TRUE(restoredPackage);
        EXPECT_EQ(restoredPackage->value.getVal(), 42);
    }

    TEST_F(TestGtConsoleRunTaskFromMemento, RejectsInvalidCommandLineArguments)
    {
        EXPECT_EQ(gt::console::runTaskFromMemento({"--unknown"}), 2);
        EXPECT_EQ(gt::console::runTaskFromMemento({"-p", projectPath}), 2);
        EXPECT_EQ(gt::console::runTaskFromMemento(
                      {"-p", projectPath, "-t", taskPath, "-o", outputPath,
                       "unexpected"}),
                  2);
    }

    TEST_F(TestGtConsoleRunTaskFromMemento,
           RejectsMissingInputAndWorkingDirectory)
    {
        ASSERT_TRUE(writeFile(projectPath, "project"));
        EXPECT_EQ(gt::console::runTaskFromMemento(
                      {"-p", projectPath, "-t", taskPath, "-o", outputPath}),
                  3);

        ASSERT_TRUE(writeFile(taskPath, "task"));
        const QString missingDirectory =
            QDir(tempDir.path()).filePath("missing directory");
        EXPECT_EQ(gt::console::runTaskFromMemento(
                      {"-p", projectPath, "-t", taskPath, "-o", outputPath,
                       "-w", missingDirectory}),
                  3);
    }

    TEST_F(TestGtConsoleRunTaskFromMemento, RejectsUnsupportedProjectMemento)
    {
        GtTask projectTask;
        projectTask.setFactory(gtObjectFactory);
        ASSERT_TRUE(
            writeFile(projectPath, projectTask.toMemento().toByteArray()));
        ASSERT_TRUE(writeRunnableMementos(
            QDir(tempDir.path()).filePath("valid project.xml"), taskPath));

        EXPECT_EQ(gt::console::runTaskFromMemento(
                      {"-p", projectPath, "-t", taskPath, "-o", outputPath}),
                  4);
    }

    TEST_F(TestGtConsoleRunTaskFromMemento,
           RejectsUnsupportedTaskAndProjectRoot)
    {
        const QString validProjectPath =
            QDir(tempDir.path()).filePath("valid project.xml");
        ASSERT_TRUE(writeRunnableMementos(validProjectPath, taskPath));
        ASSERT_TRUE(writeFile(
            projectPath, GtObjectGroup().toMemento(true).toByteArray()));
        EXPECT_EQ(gt::console::runTaskFromMemento(
                      {"-p", validProjectPath, "-t", projectPath, "-o",
                       outputPath}),
                  4);

        GtObjectGroup projectData;
        auto object = std::make_unique<MementoTestObject>();
        object->setFactory(gtObjectFactory);
        ASSERT_TRUE(projectData.appendChild(object.get()));
        object.release();
        ASSERT_TRUE(
            writeFile(projectPath, projectData.toMemento(true).toByteArray()));
        ASSERT_TRUE(writeRunnableMementos(validProjectPath, taskPath));
        EXPECT_EQ(gt::console::runTaskFromMemento(
                      {"-p", projectPath, "-t", taskPath, "-o", outputPath}),
                  4);
    }

    TEST_F(TestGtConsoleRunTaskFromMemento, ExposesAllCommandLineOptions)
    {
        EXPECT_EQ(gt::console::runTaskFromMementoOptions().size(), 4);
    }

    TEST_F(TestGtConsoleRunTaskFromMemento,
           RejectsOutputDirectory)
    {
        ASSERT_TRUE(writeRunnableMementos(projectPath, taskPath));
        ASSERT_TRUE(QDir().mkpath(outputPath));
        EXPECT_EQ(gt::console::runTaskFromMemento(
                      {"-p", projectPath, "-t", taskPath, "-o", outputPath}),
                  6);
        ASSERT_TRUE(QDir(outputPath).removeRecursively());
    }

    TEST_F(TestGtConsoleRunTaskFromMemento,
           TaskFailureRemovesExistingOutput)
    {
        ASSERT_TRUE(writeRunnableMementos(projectPath, taskPath));
        ASSERT_TRUE(writeFile(outputPath, "stale diff"));
        MementoTestCalculator::shouldSucceed = false;
        EXPECT_EQ(gt::console::runTaskFromMemento(
                      {"-p", projectPath, "-t", taskPath, "-o", outputPath}),
                  5);
        EXPECT_FALSE(QFileInfo::exists(outputPath));
    }

} // namespace

#include "test_gt_consoleruntaskfrommemento.moc"
