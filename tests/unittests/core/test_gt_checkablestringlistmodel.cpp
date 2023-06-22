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

#include <QDebug>

#include "gtest/gtest.h"
#include "gt_checkablestringlistmodel.h"

/// This is a test fixture that does a init for each test
class TestGtCheckableStringListModel : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        strList << "A" << "B" << "C";
        model = new GtCheckableStringListModel(strList);
    }

    virtual void TearDown()
    {
        delete model;
    }

    GtCheckableStringListModel* model;

    QStringList strList;
};

TEST_F(TestGtCheckableStringListModel, rowCount)
{
    // check size
    ASSERT_EQ(model->rowCount(), strList.size());
}

TEST_F(TestGtCheckableStringListModel, stringList)
{
    QStringList strings = model->stringList();

    // check size
    ASSERT_EQ(strings.size(), strList.size());

    // check string equal
    for (int i = 0; i < strList.size(); i++)
    {
        ASSERT_STREQ(strList[i].toStdString().c_str(),
                     strings[i].toStdString().c_str());
    }
}

TEST_F(TestGtCheckableStringListModel, setStringList)
{
    QStringList newStrings;

    newStrings << "D" << "E";

    model->setStringList(newStrings);
    QStringList strings = model->stringList();

    // check new size
    ASSERT_EQ(newStrings.size(), strings.size());

    // check old size
    ASSERT_NE(strList.size(), strings.size());

    // check string equal
    for (int i = 0; i < newStrings.size(); i++)
    {
        ASSERT_STREQ(newStrings[i].toStdString().c_str(),
                     strings[i].toStdString().c_str());
    }
}

TEST_F(TestGtCheckableStringListModel, selectedStringList)
{
    QStringList strings = model->selectedStringList();

    // check size
    ASSERT_EQ(strings.size(), strList.size());

    // check string equal
    for (int i = 0; i < strList.size(); i++)
    {
        ASSERT_STREQ(strList[i].toStdString().c_str(),
                     strings[i].toStdString().c_str());
    }
}

TEST_F(TestGtCheckableStringListModel, data)
{
    const int rowCount = model->rowCount();

    for (int i = 0; i < rowCount; i++)
    {
        QModelIndex index = model->index(i);

        QString str = model->data(index).toString();

        // check string equal
        ASSERT_STREQ(strList[i].toStdString().c_str(),
                     str.toStdString().c_str());

        // check selection equal

        bool success = false;
        int state = model->data(index, Qt::CheckStateRole).toInt(&success);

        ASSERT_TRUE(success);
        ASSERT_EQ(state, Qt::Checked);
    }
}

TEST_F(TestGtCheckableStringListModel, setData)
{
    QStringList entries;
    entries << "D" << "E" << "F";

    QList<Qt::CheckState> states;
    states << Qt::Unchecked << Qt::Checked << Qt::Unchecked;

    for (int i = 0; i < entries.size(); i++)
    {
        QModelIndex index = model->index(i);

        // check display/edit role
        bool success = model->setData(index, entries[i]);
        ASSERT_TRUE(success);
        ASSERT_STREQ(model->data(index).toString().toStdString().c_str(),
                     entries[i].toStdString().c_str());

        // check checkstate role
        success = model->setData(index, states[i], Qt::CheckStateRole);
        ASSERT_TRUE(success);
        int state = model->data(index, Qt::CheckStateRole).toInt(&success);
        ASSERT_TRUE(success);
        ASSERT_EQ(states[i], state);
    }
}

TEST_F(TestGtCheckableStringListModel, selectItems)
{
    QStringList selStrings;
    selStrings << "A" << "C";

    model->selectItems(selStrings);

    QStringList strings = model->selectedStringList();

    // check size
    ASSERT_EQ(strings.size(), selStrings.size());

    // check string equal
    for (int i = 0; i < selStrings.size(); i++)
    {
        ASSERT_STREQ(selStrings[i].toStdString().c_str(),
                     strings[i].toStdString().c_str());
    }
}

TEST_F(TestGtCheckableStringListModel, selectItem)
{
    QString selString("A");

    // single selection test
    model->setSelectionMode(GtCheckableStringListModel::SingleSelection);
    model->selectItem(selString);

    QStringList strings = model->selectedStringList();

    // check size
    ASSERT_EQ(1, strings.size());

    // check string equal

    ASSERT_STREQ(strings.first().toStdString().c_str(),
                 selString.toStdString().c_str());

    model->selectItem("B");

    strings = model->selectedStringList();

    ASSERT_EQ(1, strings.size());

    ASSERT_STREQ(strings.first().toStdString().c_str(), "B");

    // multiple selection test
    model->setSelectionMode(GtCheckableStringListModel::MultipleSelection);
    model->selectItem("A");
    strings = model->selectedStringList();

    // check size
    ASSERT_EQ(2, strings.size());
}

TEST_F(TestGtCheckableStringListModel, setSelectionMode)
{
    QStringList selStrings;
    selStrings << "A" << "C";

    // multiple selection
    ASSERT_EQ(model->selectionMode(),
              GtCheckableStringListModel::MultipleSelection);

    model->selectItems(selStrings);

    QStringList strings = model->selectedStringList();

    // check size
    ASSERT_EQ(strings.size(), selStrings.size());

    // single selection
    model->setSelectionMode(GtCheckableStringListModel::SingleSelection);

    ASSERT_TRUE(model->selectedStringList().isEmpty());

    model->selectItems(selStrings);

    strings = model->selectedStringList();

    // check size
    ASSERT_EQ(strings.size(), 1);

    ASSERT_STREQ(selStrings.first().toStdString().c_str(),
                 strings.first().toStdString().c_str());
}

TEST_F(TestGtCheckableStringListModel, selectionMode)
{
    ASSERT_EQ(model->selectionMode(),
              GtCheckableStringListModel::MultipleSelection);

    model->setSelectionMode(GtCheckableStringListModel::SingleSelection);

    ASSERT_EQ(model->selectionMode(),
              GtCheckableStringListModel::SingleSelection);

    model->setSelectionMode(GtCheckableStringListModel::MultipleSelection);

    ASSERT_EQ(model->selectionMode(),
              GtCheckableStringListModel::MultipleSelection);

}
