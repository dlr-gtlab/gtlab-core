/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Created on: 20.11.2026
 *  Author: Schmalfuess (AT-TWK)
 */

#include "gtest/gtest.h"

#include <QTextCursor>
#include <QTextDocument>
#include <QVector>

#include "gt_codefoldmanager.h"

namespace
{
    /// Test document
    /// 0: "def main():"      region 0: 0..2
    /// 1: "    x = 1"
    /// 2: "    y = 2"
    /// 3: ""
    /// 4: "def other():"     region 1: 4..5
    /// 5: "    z = 3"
    const QString kText =
        "def main():\n"
        "    x = 1\n"
        "    y = 2\n"
        "\n"
        "def other():\n"
        "    z = 3";

    /// kText with the body of region 0 removed from the document
    const QString kFoldedText =
        "def main():\n"
        "\n"
        "def other():\n"
        "    z = 3";

    /// Smaller test document, header not on the first line
    /// 0: "a = 0"
    /// 1: "def f():"         region 0: 1..2
    /// 2: "    b = 1"
    /// 3: "c = 2"
    const QString kSmallText = "a = 0\ndef f():\n    b = 1\nc = 2";

    const QString kSmallFoldedText = "a = 0\ndef f():\nc = 2";

    QVector<GtFoldManager::Region> makeRegions()
    {
        return {GtFoldManager::Region{0, 2}, GtFoldManager::Region{4, 5}};
    }
}

/// This is a test fixture that does a init for each test
struct TestGtCodeFoldManager : public ::testing::Test
{
    QTextDocument document;

    GtFoldManager manager{&document};
};

TEST_F(TestGtCodeFoldManager, Fold_removesBody_keepsFullText)
{
    document.setPlainText(kText);

    manager.updateLogicalRegions(makeRegions());

    EXPECT_TRUE(manager.fold(0));

    EXPECT_TRUE(manager.hasFolds());
    EXPECT_TRUE(manager.isFolded(0));
    EXPECT_EQ(document.toPlainText(), kFoldedText);
    EXPECT_EQ(manager.fullText(), kText);
}

TEST_F(TestGtCodeFoldManager, Unfold_restoresText)
{
    document.setPlainText(kText);

    manager.updateLogicalRegions(makeRegions());
    manager.fold(0);

    EXPECT_TRUE(manager.unfold(0));

    EXPECT_FALSE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), kText);
    EXPECT_EQ(manager.fullText(), kText);
}

TEST_F(TestGtCodeFoldManager, Fold_preservedWhenRegionsUnchanged)
{
    document.setPlainText(kText);

    const QVector<GtFoldManager::Region> regions = makeRegions();

    manager.updateLogicalRegions(regions);
    manager.fold(0);

    manager.updateLogicalRegions(regions);

    EXPECT_TRUE(manager.hasFolds());
    EXPECT_TRUE(manager.isFolded(0));
    EXPECT_EQ(manager.fullText(), kText);
}

TEST_F(TestGtCodeFoldManager, Fold_unfoldedWhenRegionRemoved)
{
    document.setPlainText(kText);

    manager.updateLogicalRegions(makeRegions());
    manager.fold(0);

    manager.updateLogicalRegions({});

    EXPECT_FALSE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), kText);
    EXPECT_EQ(manager.fullText(), kText);
}

TEST_F(TestGtCodeFoldManager, FoldAll_unfoldAll)
{
    document.setPlainText(kText);

    manager.updateLogicalRegions(makeRegions());
    manager.foldAll();

    EXPECT_TRUE(manager.hasFolds());
    EXPECT_TRUE(manager.isFolded(0));
    EXPECT_TRUE(manager.isFolded(2));
    EXPECT_EQ(manager.fullText(), kText);

    manager.unfoldAll();

    EXPECT_FALSE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), kText);
    EXPECT_EQ(manager.fullText(), kText);
}

TEST_F(TestGtCodeFoldManager, Edit_beforeFold_rebasesOffsets)
{
    document.setPlainText(kSmallText);

    const QVector<GtFoldManager::Region> regions = {{1, 2}};

    manager.updateLogicalRegions(regions);
    manager.fold(1);

    EXPECT_EQ(document.toPlainText(), kSmallFoldedText);

    QTextCursor cursor(&document);
    cursor.setPosition(0);
    cursor.insertText("x");

    EXPECT_TRUE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), "x" + kSmallFoldedText);
    EXPECT_EQ(manager.fullText(), "x" + kSmallText);
}

TEST_F(TestGtCodeFoldManager, Edit_inHeader_updatesHeaderText)
{
    document.setPlainText(kSmallText);

    const QVector<GtFoldManager::Region> regions = {{1, 2}};

    manager.updateLogicalRegions(regions);
    manager.fold(1);

    QTextCursor cursor(&document);
    cursor.setPosition(7);
    cursor.insertText("X");

    EXPECT_TRUE(manager.hasFolds());
    EXPECT_EQ(manager.fullText(), "a = 0\ndXef f():\n    b = 1\nc = 2");
}

TEST_F(TestGtCodeFoldManager, HeaderMerge_restoresHiddenText)
{
    document.setPlainText(kSmallText);

    const QVector<GtFoldManager::Region> regions = {{1, 2}};

    manager.updateLogicalRegions(regions);
    manager.fold(1);

    EXPECT_EQ(document.toPlainText(), kSmallFoldedText);

    // Merge the line above into the header line
    QTextCursor cursor(&document);
    cursor.setPosition(5);
    cursor.deleteChar();

    EXPECT_FALSE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), "a = 0def f():\n    b = 1\nc = 2");
}

TEST_F(TestGtCodeFoldManager, UndoOfFold_dropsSpan)
{
    document.setPlainText(kText);

    manager.updateLogicalRegions(makeRegions());
    manager.fold(0);

    document.undo();

    EXPECT_FALSE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), kText);
    EXPECT_EQ(manager.fullText(), kText);
}

TEST_F(TestGtCodeFoldManager, RedoOfFold_recreatesSpan)
{
    document.setPlainText(kText);

    manager.updateLogicalRegions(makeRegions());
    manager.fold(0);

    document.undo();
    document.redo();

    EXPECT_TRUE(manager.hasFolds());
    EXPECT_TRUE(manager.isFolded(0));
    EXPECT_EQ(document.toPlainText(), kFoldedText);
    EXPECT_EQ(manager.fullText(), kText);
}

TEST_F(TestGtCodeFoldManager, ExternalEdit_afterUndo_ofFold_doesNotRecreateSpan)
{
    document.setPlainText(kSmallText);

    const QVector<GtFoldManager::Region> regions = {{1, 2}};

    manager.updateLogicalRegions(regions);
    manager.fold(1);

    // Undo the fold externally
    document.undo();

    EXPECT_FALSE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), kSmallText);

    // An unrelated edit invalidates the unfold record
    QTextCursor cursor(&document);
    cursor.movePosition(QTextCursor::End);
    cursor.insertText("x");

    document.redo();

    EXPECT_FALSE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), "a = 0\ndef f():\nc = 2x");
    EXPECT_EQ(manager.fullText(), document.toPlainText());
}

TEST_F(TestGtCodeFoldManager, FullReplacement_clearsFolds)
{
    document.setPlainText(kText);

    manager.updateLogicalRegions(makeRegions());
    manager.fold(0);

    document.setPlainText("new text");

    EXPECT_FALSE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), "new text");
    EXPECT_EQ(manager.fullText(), "new text");
}

TEST_F(TestGtCodeFoldManager, NestedFold_absorbedByOuterFold)
{
    const QString text =
        "def outer():\n"
        "    if x:\n"
        "        y = 1\n"
        "    z = 2";

    document.setPlainText(text);

    const QVector<GtFoldManager::Region> regions = {{0, 3}, {1, 2}};

    manager.updateLogicalRegions(regions);

    // Fold the inner region first
    EXPECT_TRUE(manager.fold(1));

    // The outer fold absorbs the nested span
    EXPECT_TRUE(manager.fold(0));

    EXPECT_EQ(document.toPlainText(), "def outer():");
    EXPECT_EQ(manager.fullText(), text);

    manager.unfold(0);

    // The nested fold state is lost (documented limitation)
    EXPECT_FALSE(manager.hasFolds());
    EXPECT_EQ(document.toPlainText(), "def outer():\n    if x:\n    z = 2");
}
