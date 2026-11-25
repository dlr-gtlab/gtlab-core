/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "test_gt_processtestclasses.h"

#include "gt_datamodel.h"
#include "gt_finally.h"
#include "gt_makepropertyconnection.h"
#include "gt_mementochangecommand.h"
#include "gt_objectfactory.h"
#include "gt_processcomponentmodel.h"
#include "gt_propertyconnection.h"
#include "gt_taskgroup.h"

#include <QMimeData>
#include <QUndoStack>

#include <memory>

#include "gtest/gtest.h"

namespace
{

class ProcessDropDataModel final : public GtDataModel
{
public:
    explicit ProcessDropDataModel(
        GtTaskGroup& group,
        GtCoreDatamodel* previousModel = GtCoreDatamodel::instance()) :
        m_group(group),
        m_previousModel(previousModel)
    { }

    ~ProcessDropDataModel() override
    {
        m_self = m_previousModel;
    }

    int rowCount(const QModelIndex& parent = {}) const override
    {
        if (!parent.isValid())
        {
            return m_group.childCount<GtObject*>();
        }

        return parent.column() == 0
                   ? objectFromIndex(parent)->childCount<GtObject*>()
                   : 0;
    }

    QModelIndex index(int row, int column,
                      const QModelIndex& parent = {}) const override
    {
        if (!hasIndex(row, column, parent))
        {
            return {};
        }

        return parent.isValid()
                   ? GtDataModel::index(row, column, parent)
                   : indexFromObject(
                         m_group.findDirectChildren<GtObject*>().at(row),
                         column);
    }

    QModelIndexList appendChildren(const QList<GtObject*>& children,
                                   GtObject* parent) override
    {
        return GtCoreDatamodel::appendChildren(children, parent);
    }

    bool deleteFromModel(QList<GtObject*> objects) override
    {
        // Use the real row removal without the application's undo stack.
        return GtCoreDatamodel::deleteFromModel(objects);
    }

private:
    GtTaskGroup& m_group;
    GtCoreDatamodel* const m_previousModel;
};

} // namespace

TEST(GtProcessComponentModelTest,
     reorderingCalculatorPreservesPropertyConnection)
{
    GtTaskGroup group;
    auto* task = new TestTaskWithProperty;
    group.appendChild(task);

    auto* unconnected = new TestGtCalculator;
    auto* source = new TestGtCalculator;
    auto* target = new TestGtCalculator;
    task->appendChild(unconnected);
    task->appendChild(source);
    task->appendChild(target);

    auto* connection = gt::utils::process::makePropertyConnection(
        *source, source->prop2()->ident(), *target, target->prop3()->ident());
    ASSERT_NE(connection, nullptr);
    ASSERT_TRUE(connection->isConnected());

    const auto sourceUuid = source->uuid();
    const auto connectionUuid = connection->uuid();

    ProcessDropDataModel dataModel(group);
    GtProcessComponentModel processModel;
    processModel.setSourceModel(&dataModel);

    const auto taskIndex = processModel.index(0, 0);
    ASSERT_TRUE(taskIndex.isValid());
    const auto sourceIndex = processModel.index(1, 0, taskIndex);
    ASSERT_TRUE(sourceIndex.isValid());
    std::unique_ptr<QMimeData> mimeData(processModel.mimeData({sourceIndex}));
    ASSERT_NE(mimeData, nullptr);

    QAbstractItemModel& dropModel = processModel;
    ASSERT_TRUE(dropModel.dropMimeData(mimeData.get(), Qt::MoveAction, 0, 0,
                                       taskIndex));

    const QList<TestGtCalculator*> expectedOrder{source, unconnected, target};
    EXPECT_EQ(task->findDirectChildren<TestGtCalculator*>(), expectedOrder);
    EXPECT_EQ(source->uuid(), sourceUuid);

    const auto connections = task->findChildren<GtPropertyConnection*>();
    ASSERT_EQ(connections.size(), 1);
    auto* remainingConnection = connections.front();
    EXPECT_EQ(remainingConnection->uuid(), connectionUuid);
    EXPECT_EQ(remainingConnection->sourceUuid(), sourceUuid);
    EXPECT_EQ(remainingConnection->targetUuid(), target->uuid());
    EXPECT_EQ(remainingConnection->sourceProp(), source->prop2()->ident());
    EXPECT_EQ(remainingConnection->targetProp(), target->prop3()->ident());

    source->prop2()->setVal(42.0);
    EXPECT_DOUBLE_EQ(target->prop3()->getVal(), 42.0);
}

class GtProcessComponentCrossRootMoveTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        ASSERT_TRUE(m_group.appendChild(m_oldRoot));
        ASSERT_TRUE(m_group.appendChild(m_newRoot));
        ASSERT_TRUE(m_oldRoot->appendChild(m_subtask));
        ASSERT_TRUE(m_oldRoot->appendChild(m_oldTarget));
        ASSERT_TRUE(m_subtask->appendChild(m_source));
        ASSERT_TRUE(m_subtask->appendChild(m_target));

        auto* internal = gt::utils::process::makePropertyConnection(
            *m_source, m_source->prop2()->ident(), *m_target,
            m_target->prop3()->ident());
        auto* external = gt::utils::process::makePropertyConnection(
            *m_source, m_source->prop2()->ident(), *m_oldTarget,
            m_oldTarget->prop3()->ident());
        ASSERT_NE(internal, nullptr);
        ASSERT_NE(external, nullptr);
        ASSERT_TRUE(internal->isConnected());
        ASSERT_TRUE(external->isConnected());

        m_internalUuid = internal->uuid();
        m_externalUuid = external->uuid();
        m_processModel.setSourceModel(&m_dataModel);
    }

    void moveSubtask()
    {
        const auto oldRootIndex = m_processModel.mapFromSource(
            m_dataModel.indexFromObject(m_oldRoot));
        const auto newRootIndex = m_processModel.mapFromSource(
            m_dataModel.indexFromObject(m_newRoot));
        const auto subtaskIndex = m_processModel.index(0, 0, oldRootIndex);
        ASSERT_TRUE(oldRootIndex.isValid());
        ASSERT_TRUE(newRootIndex.isValid());
        ASSERT_TRUE(subtaskIndex.isValid());

        std::unique_ptr<QMimeData> mime(
            m_processModel.mimeData({subtaskIndex}));
        ASSERT_NE(mime, nullptr);
        QAbstractItemModel& dropModel = m_processModel;
        ASSERT_TRUE(dropModel.dropMimeData(mime.get(), Qt::MoveAction, 0, 0,
                                           newRootIndex));
    }

    GtTaskGroup m_group;
    TestTaskWithProperty* m_oldRoot = new TestTaskWithProperty;
    TestTaskWithProperty* m_newRoot = new TestTaskWithProperty;
    TestTaskWithProperty* m_subtask = new TestTaskWithProperty;
    TestGtCalculator* m_source = new TestGtCalculator;
    TestGtCalculator* m_target = new TestGtCalculator;
    TestGtCalculator* m_oldTarget = new TestGtCalculator;
    QString m_internalUuid;
    QString m_externalUuid;
    ProcessDropDataModel m_dataModel{m_group};
    GtProcessComponentModel m_processModel;
};

TEST_F(GtProcessComponentCrossRootMoveTest,
       transfersInternalConnectionAndDeletesExternalConnection)
{
    ASSERT_NO_FATAL_FAILURE(moveSubtask());

    EXPECT_EQ(m_subtask->parentObject(), m_newRoot);
    EXPECT_EQ(m_oldTarget->parentObject(), m_oldRoot);
    EXPECT_TRUE(m_oldRoot->findChildren<GtPropertyConnection*>().isEmpty());
    EXPECT_EQ(m_group.getObjectByUuid(m_externalUuid), nullptr);

    const auto connections =
        m_newRoot->findDirectChildren<GtPropertyConnection*>();
    ASSERT_EQ(connections.size(), 1);
    const auto* transferred = connections.front();
    EXPECT_EQ(transferred->uuid(), m_internalUuid);
    EXPECT_EQ(transferred->sourceUuid(), m_source->uuid());
    EXPECT_EQ(transferred->targetUuid(), m_target->uuid());
    EXPECT_EQ(transferred->sourceProp(), m_source->prop2()->ident());
    EXPECT_EQ(transferred->targetProp(), m_target->prop3()->ident());

    const double oldTargetValue = m_oldTarget->prop3()->getVal();
    m_source->prop2()->setVal(42.0);
    EXPECT_DOUBLE_EQ(m_target->prop3()->getVal(), 42.0);
    EXPECT_DOUBLE_EQ(m_oldTarget->prop3()->getVal(), oldTargetValue);
}

TEST_F(GtProcessComponentCrossRootMoveTest,
       undoRestoresSubtaskAndBothConnectionsToOldRoot)
{
    const auto restoredClasses = {TestTaskWithProperty::staticMetaObject,
                                  TestGtCalculator::staticMetaObject};
    QList<QMetaObject> registeredClasses;
    auto unregisterClasses = gt::finally([&registeredClasses] {
        for (const auto& metaObject : registeredClasses)
        {
            gtObjectFactory->unregisterClass(metaObject);
        }
    });
    for (const auto& metaObject : restoredClasses)
    {
        if (!gtObjectFactory->knownClass(
                QString::fromLatin1(metaObject.className())))
        {
            ASSERT_TRUE(gtObjectFactory->registerClass(
                metaObject, QStringLiteral("UnitTest")));
            registeredClasses.append(metaObject);
        }
    }

    const auto subtaskUuid = m_subtask->uuid();
    const auto sourceUuid = m_source->uuid();
    const auto targetUuid = m_target->uuid();
    const auto oldTargetUuid = m_oldTarget->uuid();

    // Take snapshots at the same row-move signals as the process dock.
    QUndoStack undoStack;
    GtObjectMemento beforeMove;
    QObject::connect(&m_processModel, &QAbstractItemModel::rowsAboutToBeMoved,
                     &undoStack, [&] { beforeMove = m_group.toMemento(); });
    QObject::connect(
        &m_processModel, &QAbstractItemModel::rowsMoved, &undoStack, [&] {
            const GtObjectMementoDiff diff(beforeMove, m_group.toMemento());
            undoStack.push(new GtMementoChangeCommand(
                diff, QStringLiteral("Process elements moved"), &m_group));
        });

    ASSERT_NO_FATAL_FAILURE(moveSubtask());
    ASSERT_EQ(m_subtask->parentObject(), m_newRoot);
    ASSERT_EQ(m_group.getObjectByUuid(m_externalUuid), nullptr);
    ASSERT_EQ(undoStack.count(), 1);
    ASSERT_TRUE(undoStack.canUndo());
    undoStack.undo();

    const auto* subtask = m_oldRoot->getDirectChildByUuid(subtaskUuid);
    ASSERT_NE(subtask, nullptr);
    EXPECT_EQ(subtask->childNumber(), 0);
    EXPECT_TRUE(m_newRoot->findDirectChildren<GtObject*>().isEmpty());
    ASSERT_EQ(m_oldRoot->findDirectChildren<GtPropertyConnection*>().size(), 2);

    const auto* internal = qobject_cast<const GtPropertyConnection*>(
        m_oldRoot->getDirectChildByUuid(m_internalUuid));
    const auto* external = qobject_cast<const GtPropertyConnection*>(
        m_oldRoot->getDirectChildByUuid(m_externalUuid));
    ASSERT_NE(internal, nullptr);
    ASSERT_NE(external, nullptr);
    EXPECT_EQ(internal->sourceUuid(), sourceUuid);
    EXPECT_EQ(internal->targetUuid(), targetUuid);
    EXPECT_EQ(external->sourceUuid(), sourceUuid);
    EXPECT_EQ(external->targetUuid(), oldTargetUuid);

    auto* source =
        qobject_cast<TestGtCalculator*>(m_group.getObjectByUuid(sourceUuid));
    auto* target =
        qobject_cast<TestGtCalculator*>(m_group.getObjectByUuid(targetUuid));
    auto* oldTarget =
        qobject_cast<TestGtCalculator*>(m_group.getObjectByUuid(oldTargetUuid));
    ASSERT_NE(source, nullptr);
    ASSERT_NE(target, nullptr);
    ASSERT_NE(oldTarget, nullptr);
    source->prop2()->setVal(42.0);
    EXPECT_DOUBLE_EQ(target->prop3()->getVal(), 42.0);
    EXPECT_DOUBLE_EQ(oldTarget->prop3()->getVal(), 42.0);
}
