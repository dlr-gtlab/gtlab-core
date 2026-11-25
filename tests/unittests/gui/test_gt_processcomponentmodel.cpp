/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "core/test_gt_processtestclasses.h"

#include "gt_datamodel.h"
#include "gt_makepropertyconnection.h"
#include "gt_processcomponentmodel.h"
#include "gt_propertyconnection.h"
#include "gt_taskgroup.h"

#include <QMimeData>

#include <memory>

#include "gtest/gtest.h"

namespace
{

class ProcessDropDataModel final : public GtDataModel
{
public:
    explicit ProcessDropDataModel(
        GtTask& task,
        GtCoreDatamodel* previousModel = GtCoreDatamodel::instance()) :
        m_task(task),
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
            return 1;
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

        return parent.isValid() ? GtDataModel::index(row, column, parent)
                                : indexFromObject(&m_task, column);
    }

    bool deleteFromModel(QList<GtObject*> objects) override
    {
        // Use the real row removal without the application's undo stack.
        return GtCoreDatamodel::deleteFromModel(objects);
    }

private:
    GtTask& m_task;
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

    ProcessDropDataModel dataModel(*task);
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
