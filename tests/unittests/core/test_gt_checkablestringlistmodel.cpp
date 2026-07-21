/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: test_gt_checkablestringlistmodel.cpp
 *
 *  Created on: 16.02.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gtest/gtest.h"
#include "gt_checkablestringlistmodel.h"

/// This is a test fixture that does a init for each test
class TestGtCheckableStringListModel : public ::testing::Test
{
protected:
    TestGtCheckableStringListModel() : model(strList)
    {
        strList << "A" << "B" << "C";
        model.setStringList(strList);
    }

    QStringList strList;
    GtCheckableStringListModel model;
};

TEST_F(TestGtCheckableStringListModel, rowCount)
{
    EXPECT_EQ(model.rowCount(), strList.size());
    EXPECT_EQ(model.rowCount(model.index(0)), 0);
}

TEST_F(TestGtCheckableStringListModel, stringList)
{
    QStringList strings = model.stringList();

    ASSERT_EQ(strings.size(), strList.size());

    for (int i = 0; i < strList.size(); i++)
    {
        EXPECT_STREQ(strList[i].toStdString().c_str(),
                     strings[i].toStdString().c_str());
    }
}

TEST_F(TestGtCheckableStringListModel, setStringList)
{
    QStringList newStrings;
    newStrings << "D" << "E";

    model.setStringList(newStrings, Qt::Unchecked);
    QStringList strings = model.stringList();

    ASSERT_EQ(newStrings.size(), strings.size());
    EXPECT_NE(strList.size(), strings.size());

    for (int i = 0; i < newStrings.size(); i++)
    {
        EXPECT_STREQ(newStrings[i].toStdString().c_str(),
                     strings[i].toStdString().c_str());
        EXPECT_EQ(model.data(model.index(i), Qt::CheckStateRole).toInt(),
                  Qt::Unchecked);
    }
}

TEST_F(TestGtCheckableStringListModel, selectedStringList)
{
    QStringList strings = model.selectedStringList();

    ASSERT_EQ(strings.size(), strList.size());

    for (int i = 0; i < strList.size(); i++)
    {
        EXPECT_STREQ(strList[i].toStdString().c_str(),
                     strings[i].toStdString().c_str());
    }
}

TEST_F(TestGtCheckableStringListModel, data)
{
    const int rowCount = model.rowCount();

    for (int i = 0; i < rowCount; i++)
    {
        QModelIndex index = model.index(i);

        QString str = model.data(index).toString();
        EXPECT_STREQ(strList[i].toStdString().c_str(),
                     str.toStdString().c_str());

        bool success = false;
        int state = model.data(index, Qt::CheckStateRole).toInt(&success);

        EXPECT_TRUE(success);
        EXPECT_EQ(state, Qt::Checked);
    }

    EXPECT_FALSE(model.data(model.index(-1)).isValid());
    EXPECT_FALSE(model.data(model.index(model.rowCount())).isValid());
}

TEST_F(TestGtCheckableStringListModel, setData)
{
    QStringList entries;
    entries << "D" << "E" << "F";

    QList<Qt::CheckState> states;
    states << Qt::Unchecked << Qt::Checked << Qt::Unchecked;

    for (int i = 0; i < entries.size(); i++)
    {
        QModelIndex index = model.index(i);

        bool success = model.setData(index, entries[i]);
        ASSERT_TRUE(success);
        EXPECT_STREQ(model.data(index).toString().toStdString().c_str(),
                     entries[i].toStdString().c_str());

        success = model.setData(index, states[i], Qt::CheckStateRole);
        ASSERT_TRUE(success);
        int state = model.data(index, Qt::CheckStateRole).toInt(&success);
        EXPECT_TRUE(success);
        EXPECT_EQ(states[i], state);
    }

    EXPECT_FALSE(model.setData(model.index(-1), "X"));
    EXPECT_FALSE(model.setData(model.index(model.rowCount()), "X"));
    EXPECT_FALSE(model.setData(model.index(0), QVariant(1), Qt::UserRole));
}

TEST_F(TestGtCheckableStringListModel, selectItems)
{
    QStringList selStrings;
    selStrings << "A" << "C";

    int selectionChanged = 0;
    QObject::connect(&model, &GtCheckableStringListModel::selectionChanged,
                     [&selectionChanged]() { ++selectionChanged; });

    model.selectItems(selStrings);

    QStringList strings = model.selectedStringList();

    ASSERT_EQ(strings.size(), selStrings.size());
    for (int i = 0; i < selStrings.size(); i++)
    {
        EXPECT_STREQ(selStrings[i].toStdString().c_str(),
                     strings[i].toStdString().c_str());
    }
    EXPECT_EQ(selectionChanged, 1);
}

TEST_F(TestGtCheckableStringListModel, selectItem)
{
    model.setSelectionMode(GtCheckableStringListModel::SingleSelection);
    model.selectItem("A");

    QStringList strings = model.selectedStringList();
    ASSERT_EQ(1, strings.size());
    EXPECT_STREQ(strings.first().toStdString().c_str(), "A");

    model.selectItem("B");
    strings = model.selectedStringList();
    ASSERT_EQ(1, strings.size());
    EXPECT_STREQ(strings.first().toStdString().c_str(), "B");

    model.setSelectionMode(GtCheckableStringListModel::MultipleSelection);
    model.selectItem("A");
    strings = model.selectedStringList();
    EXPECT_EQ(2, strings.size());

    model.selectItem(QString());
    EXPECT_EQ(model.selectedStringList().size(), 2);
}

TEST_F(TestGtCheckableStringListModel, setSelectionMode)
{
    QStringList selStrings;
    selStrings << "A" << "C";

    EXPECT_EQ(model.selectionMode(),
              GtCheckableStringListModel::MultipleSelection);

    model.selectItems(selStrings);
    QStringList strings = model.selectedStringList();
    EXPECT_EQ(strings.size(), selStrings.size());

    model.setSelectionMode(GtCheckableStringListModel::SingleSelection);
    EXPECT_TRUE(model.selectedStringList().isEmpty());

    model.selectItems(selStrings);
    strings = model.selectedStringList();
    ASSERT_EQ(strings.size(), 1);
    EXPECT_STREQ(selStrings.first().toStdString().c_str(),
                 strings.first().toStdString().c_str());

    model.setSelectionMode(GtCheckableStringListModel::SingleSelection);
    EXPECT_EQ(model.selectedStringList().size(), 1);
}

TEST_F(TestGtCheckableStringListModel, selectionMode)
{
    EXPECT_EQ(model.selectionMode(),
              GtCheckableStringListModel::MultipleSelection);

    model.setSelectionMode(GtCheckableStringListModel::SingleSelection);
    EXPECT_EQ(model.selectionMode(),
              GtCheckableStringListModel::SingleSelection);

    model.setSelectionMode(GtCheckableStringListModel::MultipleSelection);
    EXPECT_EQ(model.selectionMode(),
              GtCheckableStringListModel::MultipleSelection);
}

TEST_F(TestGtCheckableStringListModel, flags)
{
    EXPECT_TRUE(model.flags(model.index(0)) & Qt::ItemIsUserCheckable);
    EXPECT_TRUE(model.flags(QModelIndex()) & Qt::ItemIsDropEnabled);
    EXPECT_FALSE(model.flags(model.index(0)) & Qt::ItemIsDropEnabled);
}

TEST_F(TestGtCheckableStringListModel, clearSelectionAndSelectAllUpdateSelectionState)
{
    bool noItemSelected = false;
    int emissions = 0;
    QObject::connect(&model, &GtCheckableStringListModel::noItemSelected,
                     [&noItemSelected, &emissions](bool value)
                     {
                         noItemSelected = value;
                         ++emissions;
                     });

    model.clearSelection();
    EXPECT_TRUE(model.selectedStringList().isEmpty());

    model.setData(model.index(1), Qt::Checked, Qt::CheckStateRole);
    EXPECT_EQ(model.selectedStringList(), QStringList({"B"}));

    model.selectAll();
    EXPECT_EQ(model.selectedStringList(), strList);
    EXPECT_EQ(emissions, 1);
    EXPECT_FALSE(noItemSelected);
}

TEST_F(TestGtCheckableStringListModel, singleSelectionCheckStateUnchecksOthers)
{
    model.setSelectionMode(GtCheckableStringListModel::SingleSelection);

    EXPECT_FALSE(model.setData(model.index(0), Qt::Checked, Qt::UserRole));
    EXPECT_FALSE(model.setData(model.index(-1), Qt::Checked, Qt::CheckStateRole));

    EXPECT_FALSE(model.setData(model.index(1), Qt::Checked, Qt::CheckStateRole));
    EXPECT_EQ(model.selectedStringList(), QStringList({"B"}));
}

TEST_F(TestGtCheckableStringListModel, insertAndRemoveRows)
{
    EXPECT_FALSE(model.insertRows(-1, 1));
    EXPECT_FALSE(model.insertRows(0, 0));

    EXPECT_TRUE(model.insertRows(1, 2));
    EXPECT_EQ(model.rowCount(), 5);
    EXPECT_EQ(model.data(model.index(1)).toString(), QString());
    EXPECT_EQ(model.data(model.index(1), Qt::CheckStateRole).toInt(), Qt::Checked);

    EXPECT_FALSE(model.removeRows(-1, 1));
    EXPECT_FALSE(model.removeRows(4, 2));
    EXPECT_FALSE(model.removeRows(0, 0));

    EXPECT_TRUE(model.removeRows(1, 2));
    EXPECT_EQ(model.rowCount(), 3);
    EXPECT_EQ(model.stringList(), strList);
}

TEST_F(TestGtCheckableStringListModel, supportedDropActions)
{
    EXPECT_EQ(model.supportedDropActions(), Qt::MoveAction);
}
