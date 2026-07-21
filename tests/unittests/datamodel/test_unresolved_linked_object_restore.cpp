/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"

#include "gt_objectfactory.h"
#include "gt_objectmemento.h"
#include "gt_xmlutilities.h"
#include "gt_xmlexpr.h"

#include "test_gt_object.h"

#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

namespace
{

    QDomDocument parseObjectDocument(const QByteArray& xml)
    {
        QDomDocument doc;
        QString errorMsg;
        int errorLine;
        int errorColumn;

        const bool ok =
            doc.setContent(xml, true, &errorMsg, &errorLine, &errorColumn);
        EXPECT_TRUE(ok) << "XML parse error at " << errorLine << ":"
                        << errorColumn << " " << errorMsg.toStdString();
        EXPECT_FALSE(doc.documentElement().isNull());
        return doc;
    }

    bool writeTextFile(const QString& path, const QByteArray& data)
    {
        QFileInfo info(path);
        QDir().mkpath(info.dir().absolutePath());

        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return false;
        }

        return f.write(data) == data.size();
    }

    QDomDocument readDomFromFile(const QString& path)
    {
        QFile f(path);
        if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            return {};
        }

        QDomDocument doc;
        QString errorMsg;
        int errorLine = 0;
        int errorColumn = 0;
        const bool ok =
            doc.setContent(&f, true, &errorMsg, &errorLine, &errorColumn);
        EXPECT_TRUE(ok) << "XML parse error at " << errorLine << ":"
                        << errorColumn << " " << errorMsg.toStdString();
        return doc;
    }

    QDomDocument mementoToDomDocument(const GtObjectMemento& memento)
    {
        QDomDocument doc;
        QString errorMsg;
        int errorLine = 0;
        int errorColumn = 0;

        const bool ok = doc.setContent(memento.toByteArray(), true, &errorMsg,
                                       &errorLine, &errorColumn);
        EXPECT_TRUE(ok) << "Memento XML parse error at " << errorLine << ":"
                        << errorColumn << " " << errorMsg.toStdString();
        EXPECT_FALSE(doc.documentElement().isNull());
        return doc;
    }

    QDomElement importMementoElement(QDomDocument& target,
                                     const GtObjectMemento& memento)
    {
        QDomDocument sourceDoc = mementoToDomDocument(memento);
        QDomNode imported =
            target.importNode(sourceDoc.documentElement(), true);
        EXPECT_FALSE(imported.isNull());
        return imported.toElement();
    }

    class ObjectWithDefaultChild : public GtObject
    {
    public:
        ObjectWithDefaultChild() : child(new TestSpecialGtObject)
        {
            child->setDefault(true);
            appendChild(child);
        }

        TestSpecialGtObject* child;
    };

} // namespace

class TestLinkedObjectRestore : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (!gtObjectFactory->knownClass("TestSpecialGtObject"))
        {
            gtObjectFactory->registerClass(
                TestSpecialGtObject::staticMetaObject);
        }
    }
};

/** Restoring a missing linked objectref for a known class yields a dummy object. */
TEST_F(TestLinkedObjectRestore, KnownClassObjectRefMissingCreatesDummy)
{
    const auto doc = parseObjectDocument(
        R"(<objectref class="TestSpecialGtObject" name="MissingChild" uuid="{U-1}" href="missing.gtobj.xml"/>)");
    const auto refElem = doc.documentElement();

    GtObjectMemento memento = GtObjectMemento(refElem);

    ASSERT_TRUE(memento.isFlagEnabled(GtObjectMemento::IsUnresolved));

    auto obj = memento.toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(obj != nullptr);

    EXPECT_TRUE(obj->isDummy());
    EXPECT_TRUE(obj->saveAsOwnFile());
    EXPECT_EQ(obj->objectName(), QStringLiteral("MissingChild"));
    EXPECT_EQ(obj->uuid(), QStringLiteral("{U-1}"));
}

/** An unresolved dummy roundtrips back to XML with aslink="refonly". */
TEST_F(TestLinkedObjectRestore, UnresolvedDummyRoundtripSerializesAsRefOnly)
{
    const auto doc = parseObjectDocument(
        R"(<objectref class="TestSpecialGtObject" name="MissingChild" uuid="{U-2}" href="missing.gtobj.xml"/>)");
    const auto refElem = doc.documentElement();

    auto obj = GtObjectMemento(refElem).toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(obj != nullptr);
    ASSERT_TRUE(obj->isDummy());

    auto roundtrip = obj->toMemento(false);
    QDomDocument outDoc = mementoToDomDocument(roundtrip);
    QDomElement outElem = outDoc.documentElement();

    EXPECT_EQ(outElem.tagName(), QStringLiteral("object"));
    EXPECT_EQ(outElem.attribute(gt::xml::S_CLASS_TAG),
              QStringLiteral("TestSpecialGtObject"));
    EXPECT_EQ(outElem.attribute(gt::xml::S_ASLINK_TAG),
              QStringLiteral("refonly"));
}

/** Merging unresolved data into an existing default child keeps the child and marks it dummy. */
TEST_F(TestLinkedObjectRestore,
       MergeUnresolvedIntoDefaultChildKeepsChildAndMarksDummy)
{
    ObjectWithDefaultChild obj;
    obj.child->setObjectName(QStringLiteral("DefaultChild"));
    obj.child->setUuid(QStringLiteral("{CHILD-1}"));

    const auto oldChildAddr = obj.child;
    ASSERT_EQ(obj.childCount<GtObject*>(), 1);

    GtObjectMemento m = obj.toMemento(false);
    ASSERT_EQ(m.childObjects.size(), 1);

    auto& childMemento = m.childObjects[0];
    childMemento.setFlagEnabled(GtObjectMemento::IsUnresolved, true);

    ASSERT_TRUE(m.mergeTo(obj, *GtObjectFactory::instance()));

    EXPECT_EQ(obj.childCount<GtObject*>(), 1);
    EXPECT_EQ(obj.child, oldChildAddr);
    EXPECT_TRUE(obj.child->isDummy());
    EXPECT_TRUE(obj.child->saveAsOwnFile());
}

/** A missing linked child objectref is restored as a dummy child while root stays regular. */
TEST_F(TestLinkedObjectRestore, MissingLinkedChildObjectRefCreatesDummyChild)
{
    const auto doc = parseObjectDocument(R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-1}">
  <objectlist>
    <objectref class="TestSpecialGtObject" name="MissingChild" uuid="{CHILD-XML-1}" href="missing_child.gtobj.xml"/>
  </objectlist>
</object>)");

    GtObjectMemento memento = GtObjectMemento(doc.documentElement());

    auto root = memento.toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);
    ASSERT_FALSE(root->isDummy());
    ASSERT_EQ(root->childCount<GtObject*>(), 1);

    auto child =
        root->findDirectChild<GtObject*>(QStringLiteral("MissingChild"));
    ASSERT_TRUE(child != nullptr);
    EXPECT_TRUE(child->isDummy());
    EXPECT_TRUE(child->saveAsOwnFile());
    EXPECT_EQ(child->uuid(), QStringLiteral("{CHILD-XML-1}"));
}

/** A parsed missing child objectref merges into an existing default child and keeps that instance. */
TEST_F(TestLinkedObjectRestore,
       MissingLinkedChildObjectRefKeepsExistingDefaultChildOnMerge)
{
    ObjectWithDefaultChild obj;
    obj.setObjectName(QStringLiteral("RootObject"));
    obj.child->setObjectName(QStringLiteral("DefaultChild"));
    obj.child->setUuid(QStringLiteral("{CHILD-XML-2}"));
    obj.child->setDouble(42.0);

    const auto oldChildAddr = obj.child;

    const QString xml =
        QStringLiteral(
            "<object class=\"GtObject\" name=\"%1\" uuid=\"%2\">"
            "<objectlist>"
            "<objectref class=\"TestSpecialGtObject\" name=\"DefaultChild\" "
            "uuid=\"{CHILD-XML-2}\" href=\"missing_default_child.gtobj.xml\"/>"
            "</objectlist>"
            "</object>")
            .arg(obj.objectName(), obj.uuid());

    const auto doc = parseObjectDocument(xml.toUtf8());

    GtObjectMemento memento = GtObjectMemento(doc.documentElement());
    ASSERT_EQ(memento.childObjects.size(), 1);
    ASSERT_TRUE(
        memento.childObjects[0].isFlagEnabled(GtObjectMemento::IsUnresolved));

    ASSERT_TRUE(memento.mergeTo(obj, *GtObjectFactory::instance()));

    EXPECT_EQ(obj.childCount<GtObject*>(), 1);
    EXPECT_EQ(obj.child, oldChildAddr);
    EXPECT_TRUE(obj.child->isDummy());
    EXPECT_TRUE(obj.child->saveAsOwnFile());
}

/** Missing linked child with unknown class restores as dummy and preserves class metadata. */
TEST_F(TestLinkedObjectRestore,
       MissingUnknownClassChildObjectRefRestoresDummyAndKeepsClass)
{
    const auto doc = parseObjectDocument(R"(
<object class="GtObject" name="Root" uuid="{ROOT-2}">
  <objectlist>
    <objectref class="UnknownMissingClass" name="UnknownChild" uuid="{CHILD-XML-3}" href="missing_unknown.gtobj.xml"/>
  </objectlist>
</object>)");

    GtObjectMemento rootMemento = GtObjectMemento(doc.documentElement());
    ASSERT_EQ(rootMemento.childObjects.size(), 1);
    auto& childMementoInTree = rootMemento.childObjects[0];
    ASSERT_TRUE(
        childMementoInTree.isFlagEnabled(GtObjectMemento::IsUnresolved));

    auto child = childMementoInTree.toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(child != nullptr);
    EXPECT_TRUE(child->isDummy());
    EXPECT_TRUE(child->saveAsOwnFile());

    auto childMemento = child->toMemento(false);
    EXPECT_EQ(childMemento.className(), QStringLiteral("UnknownMissingClass"));
}

/** Mixed child list keeps valid children normal while unresolved linked child becomes dummy. */
TEST_F(TestLinkedObjectRestore, MixedChildrenOnlyMissingLinkedChildBecomesDummy)
{
    const auto doc = parseObjectDocument(R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-3}">
  <objectlist>
    <object class="TestSpecialGtObject" name="GoodChild" uuid="{GOOD-1}"/>
    <objectref class="TestSpecialGtObject" name="MissingChild" uuid="{MISS-1}" href="missing_mix.gtobj.xml"/>
  </objectlist>
</object>)");

    auto root = GtObjectMemento(doc.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(root->childCount<GtObject*>(), 2);

    auto good = root->findDirectChild<GtObject*>(QStringLiteral("GoodChild"));
    auto miss =
        root->findDirectChild<GtObject*>(QStringLiteral("MissingChild"));
    ASSERT_TRUE(good != nullptr);
    ASSERT_TRUE(miss != nullptr);

    EXPECT_FALSE(good->isDummy());
    EXPECT_FALSE(good->saveAsOwnFile());
    EXPECT_TRUE(miss->isDummy());
    EXPECT_TRUE(miss->saveAsOwnFile());
}

/** Unresolved linked grandchildren are also converted to dummy objects recursively. */
TEST_F(TestLinkedObjectRestore,
       MissingLinkedGrandChildObjectRefCreatesDummyGrandChild)
{
    const auto doc = parseObjectDocument(R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-4}">
  <objectlist>
    <object class="TestSpecialGtObject" name="MidChild" uuid="{MID-1}">
      <objectlist>
        <objectref class="TestSpecialGtObject" name="MissingGrandChild" uuid="{GRAND-1}" href="missing_grand.gtobj.xml"/>
      </objectlist>
    </object>
  </objectlist>
</object>)");

    auto root = GtObjectMemento(doc.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);

    auto mid = root->findDirectChild<GtObject*>(QStringLiteral("MidChild"));
    ASSERT_TRUE(mid != nullptr);
    EXPECT_FALSE(mid->isDummy());

    auto grand =
        mid->findDirectChild<GtObject*>(QStringLiteral("MissingGrandChild"));
    ASSERT_TRUE(grand != nullptr);
    EXPECT_TRUE(grand->isDummy());
    EXPECT_TRUE(grand->saveAsOwnFile());
}

/** A default child marked unresolved is persisted as refonly to avoid overwriting missing files. */
TEST_F(TestLinkedObjectRestore, UnresolvedDefaultChildSerializesAsRefOnly)
{
    ObjectWithDefaultChild obj;
    obj.child->setObjectName(QStringLiteral("DefaultChild"));
    obj.child->setUuid(QStringLiteral("{CHILD-2}"));

    GtObjectMemento m = obj.toMemento(false);
    ASSERT_EQ(m.childObjects.size(), 1);
    m.childObjects[0].setFlagEnabled(GtObjectMemento::IsUnresolved, true);
    ASSERT_TRUE(m.mergeTo(obj, *GtObjectFactory::instance()));

    QDomDocument doc;
    QDomElement rootElem = importMementoElement(doc, obj.toMemento(false));
    doc.appendChild(rootElem);

    QDomElement listElem =
        rootElem.firstChildElement(gt::xml::S_OBJECTLIST_TAG);
    ASSERT_FALSE(listElem.isNull());

    QDomElement childElem = listElem.firstChildElement(gt::xml::S_OBJECT_TAG);
    ASSERT_FALSE(childElem.isNull());

    EXPECT_EQ(childElem.attribute(gt::xml::S_CLASS_TAG),
              QStringLiteral("TestSpecialGtObject"));
    EXPECT_EQ(childElem.attribute(gt::xml::S_ASLINK_TAG),
              QStringLiteral("refonly"));
}

/** End-to-end save keeps unresolved child as objectref and does not write linked data file. */
TEST_F(TestLinkedObjectRestore,
       EndToEndMissingLinkedChildRoundtripKeepsObjectRefWithoutLinkedFile)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    const QString masterPath =
        QDir(tempDir.path()).filePath(QStringLiteral("master.xml"));
    const QByteArray masterXml = R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-E2E}">
  <objectlist>
    <objectref class="TestSpecialGtObject" name="MissingChild" uuid="{CHILD-E2E}" href="missing/child.gtobj.xml"/>
  </objectlist>
</object>)";
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    QStringList warnings;
    QDomDocument loaded =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);
    ASSERT_FALSE(loaded.isNull());
    EXPECT_FALSE(warnings.isEmpty());

    auto root = GtObjectMemento(loaded.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);

    QDomDocument saveDoc;
    saveDoc.appendChild(importMementoElement(saveDoc, root->toMemento(false)));

    const QString outPath =
        QDir(tempDir.path()).filePath(QStringLiteral("roundtrip.xml"));
    QString error;
    ASSERT_TRUE(gt::xml::saveProjectXmlWithLinkedObjects(
        QStringLiteral("TestProject"), saveDoc, QDir(tempDir.path()), outPath,
        gt::xml::LinkFileSaveType::WithLinkedFiles, &error))
        << error.toStdString();

    QDomDocument outLoaded =
        gt::xml::loadProjectXmlWithLinkedObjects(outPath, nullptr);
    ASSERT_FALSE(outLoaded.isNull());

    QDomElement outRoot = outLoaded.documentElement();
    ASSERT_FALSE(outRoot.isNull());
    QDomElement outList = outRoot.firstChildElement(gt::xml::S_OBJECTLIST_TAG);
    ASSERT_FALSE(outList.isNull());
    QDomElement outRef = outList.firstChildElement(gt::xml::S_OBJECTREF_TAG);
    ASSERT_FALSE(outRef.isNull());
    EXPECT_EQ(outRef.attribute(gt::xml::S_NAME_TAG),
              QStringLiteral("MissingChild"));

    const QString expectedLinkedFile =
        QDir(tempDir.path())
            .filePath(QStringLiteral(
                "roundtrip/Root/MissingChild_CHILD-E2E.gtobj.xml"));
    EXPECT_FALSE(QFile::exists(expectedLinkedFile));
}

/** Recovery works: once linked file exists, the same child objectref restores as non-dummy. */
TEST_F(TestLinkedObjectRestore,
       RecoveryAfterProvidingLinkedFileRestoresRegularChild)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    const QString masterPath =
        QDir(tempDir.path()).filePath(QStringLiteral("master_recover.xml"));
    const QByteArray masterXml = R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-R}">
  <objectlist>
    <objectref class="TestSpecialGtObject" name="RecoverChild" uuid="{CHILD-R}" href="linked/recover_child.gtobj.xml"/>
  </objectlist>
</object>)";
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    QStringList warningsMissing;
    QDomDocument loadedMissing =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warningsMissing);
    ASSERT_FALSE(loadedMissing.isNull());
    EXPECT_FALSE(warningsMissing.isEmpty());

    auto rootMissing = GtObjectMemento(loadedMissing.documentElement())
                           .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(rootMissing != nullptr);
    auto childMissing =
        rootMissing->findDirectChild<GtObject*>(QStringLiteral("RecoverChild"));
    ASSERT_TRUE(childMissing != nullptr);
    EXPECT_TRUE(childMissing->isDummy());

    const QString linkedPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("linked/recover_child.gtobj.xml"));
    const QByteArray linkedXml = R"(
<GTLABOBJECTFILE>
  <object class="TestSpecialGtObject" name="RecoverChild" uuid="{CHILD-R}"/>
</GTLABOBJECTFILE>)";
    ASSERT_TRUE(writeTextFile(linkedPath, linkedXml));

    QStringList warningsRecovered;
    QDomDocument loadedRecovered = gt::xml::loadProjectXmlWithLinkedObjects(
        masterPath, &warningsRecovered);
    ASSERT_FALSE(loadedRecovered.isNull());
    EXPECT_TRUE(warningsRecovered.isEmpty());

    auto rootRecovered = GtObjectMemento(loadedRecovered.documentElement())
                             .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(rootRecovered != nullptr);
    auto childRecovered = rootRecovered->findDirectChild<GtObject*>(
        QStringLiteral("RecoverChild"));
    ASSERT_TRUE(childRecovered != nullptr);
    EXPECT_FALSE(childRecovered->isDummy());
}

/** Objectrefs with missing attributes do not crash and still restore as unresolved dummy children. */
TEST_F(TestLinkedObjectRestore, MissingObjectRefAttributesFallbackToStableDummy)
{
    const auto doc = parseObjectDocument(R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-MISS-ATTR}">
  <objectlist>
    <objectref/>
  </objectlist>
</object>)");

    auto rootMemento = GtObjectMemento(doc.documentElement());
    ASSERT_EQ(rootMemento.childObjects.size(), 1);
    auto& childMemento = rootMemento.childObjects[0];
    ASSERT_TRUE(childMemento.isFlagEnabled(GtObjectMemento::IsUnresolved));

    auto child = childMemento.toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(child != nullptr);
    EXPECT_TRUE(child->isDummy());
    EXPECT_TRUE(child->saveAsOwnFile());
    EXPECT_TRUE(child->objectName().isEmpty());

    auto root = rootMemento.toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);
    ASSERT_FALSE(root->isDummy());
}

/** Two unresolved siblings with the same name but different UUIDs are both kept as separate children. */
TEST_F(TestLinkedObjectRestore,
       DuplicateChildNamesDifferentUuidsRemainDistinctWhenUnresolved)
{
    const auto doc = parseObjectDocument(R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-DUP}">
  <objectlist>
    <objectref class="TestSpecialGtObject" name="SameName" uuid="{DUPE-1}" href="missing_dupe_1.gtobj.xml"/>
    <objectref class="TestSpecialGtObject" name="SameName" uuid="{DUPE-2}" href="missing_dupe_2.gtobj.xml"/>
  </objectlist>
</object>)");

    auto root = GtObjectMemento(doc.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);
    ASSERT_EQ(root->childCount<GtObject*>(), 2);

    auto children = root->findDirectChildren<GtObject*>();
    ASSERT_EQ(children.size(), 2);
    EXPECT_TRUE(children[0]->isDummy());
    EXPECT_TRUE(children[1]->isDummy());

    QSet<QString> uuids;
    uuids.insert(children[0]->uuid());
    uuids.insert(children[1]->uuid());
    EXPECT_TRUE(uuids.contains(QStringLiteral("{DUPE-1}")));
    EXPECT_TRUE(uuids.contains(QStringLiteral("{DUPE-2}")));
}

/** A refonly object survives load-restore-save-load as objectref and still does not create linked files. */
TEST_F(TestLinkedObjectRestore,
       RefOnlyObjectPreservedAcrossFullRoundtripWithoutLinkedFileCreation)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QDomDocument inDoc = parseObjectDocument(R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-REFONLY}">
  <objectlist>
    <object class="TestSpecialGtObject" name="RefOnlyChild" uuid="{REF-1}" aslink="refonly"/>
  </objectlist>
</object>)");

    const QString firstPath =
        QDir(tempDir.path()).filePath(QStringLiteral("first.xml"));
    QString error;
    ASSERT_TRUE(gt::xml::saveProjectXmlWithLinkedObjects(
        QStringLiteral("TestProject"), inDoc, QDir(tempDir.path()), firstPath,
        gt::xml::LinkFileSaveType::WithLinkedFiles, &error))
        << error.toStdString();

    // No linked file may be created for refonly content.
    const QString linkedFile =
        QDir(tempDir.path())
            .filePath(
                QStringLiteral("first/Root/RefOnlyChild_REF-1.gtobj.xml"));
    EXPECT_FALSE(QFile::exists(linkedFile));

    QStringList warnings;
    QDomDocument loaded =
        gt::xml::loadProjectXmlWithLinkedObjects(firstPath, &warnings);
    ASSERT_FALSE(loaded.isNull());
    EXPECT_FALSE(warnings.isEmpty());

    auto root = GtObjectMemento(loaded.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);

    QDomDocument saveDoc;
    saveDoc.appendChild(importMementoElement(saveDoc, root->toMemento(false)));

    const QString secondPath =
        QDir(tempDir.path()).filePath(QStringLiteral("second.xml"));
    ASSERT_TRUE(gt::xml::saveProjectXmlWithLinkedObjects(
        QStringLiteral("TestProject"), saveDoc, QDir(tempDir.path()),
        secondPath, gt::xml::LinkFileSaveType::WithLinkedFiles, &error))
        << error.toStdString();

    EXPECT_FALSE(
        QFile::exists(QDir(tempDir.path())
                          .filePath(QStringLiteral(
                              "second/Root/RefOnlyChild_REF-1.gtobj.xml"))));

    QDomDocument secondLoaded =
        gt::xml::loadProjectXmlWithLinkedObjects(secondPath, nullptr);
    ASSERT_FALSE(secondLoaded.isNull());
    auto listElem = secondLoaded.documentElement().firstChildElement(
        gt::xml::S_OBJECTLIST_TAG);
    ASSERT_FALSE(listElem.isNull());
    auto refElem = listElem.firstChildElement(gt::xml::S_OBJECTREF_TAG);
    ASSERT_FALSE(refElem.isNull());
    EXPECT_EQ(refElem.attribute(gt::xml::S_NAME_TAG),
              QStringLiteral("RefOnlyChild"));
}

/** Type-restricting parents can reject unknown-class unresolved dummy children without destabilizing restore. */
TEST_F(TestLinkedObjectRestore,
       RestrictiveParentRejectsUnknownClassUnresolvedChild)
{
    const auto doc = parseObjectDocument(R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-STRICT}">
  <objectlist>
    <objectref class="UnknownMissingClass" name="UnknownChild" uuid="{STRICT-1}" href="missing_strict.gtobj.xml"/>
  </objectlist>
</object>)");

    auto root = GtObjectMemento(doc.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);
    ASSERT_FALSE(root->isDummy());

    // TestSpecialGtObject::childAccepted() rejects plain GtObject dummy children.
    EXPECT_EQ(root->childCount<GtObject*>(), 0);
}

/** Relative href paths that traverse to parent directories are resolved against the master file directory. */
TEST_F(TestLinkedObjectRestore, ParentDirectoryHrefIsResolved)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    const QString projectDir =
        QDir(tempDir.path()).filePath(QStringLiteral("project"));
    ASSERT_TRUE(QDir().mkpath(projectDir));

    const QString masterPath =
        QDir(projectDir).filePath(QStringLiteral("master.xml"));
    const QByteArray masterXml = R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-PARENT-HREF}">
  <objectlist>
    <objectref class="TestSpecialGtObject" name="ChildFromParent" uuid="{PARENT-CHILD}" href="../external/relinked.gtobj.xml"/>
  </objectlist>
</object>)";
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    const QString linkedPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("external/relinked.gtobj.xml"));
    const QByteArray linkedXml = R"(
<GTLABOBJECTFILE>
  <object class="TestSpecialGtObject" name="ChildFromParent" uuid="{PARENT-CHILD}"/>
</GTLABOBJECTFILE>)";
    ASSERT_TRUE(writeTextFile(linkedPath, linkedXml));

    QStringList warnings;
    QDomDocument loaded =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);
    ASSERT_FALSE(loaded.isNull());
    EXPECT_TRUE(warnings.isEmpty());

    auto root = GtObjectMemento(loaded.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);
    auto child =
        root->findDirectChild<GtObject*>(QStringLiteral("ChildFromParent"));
    ASSERT_TRUE(child != nullptr);
    EXPECT_FALSE(child->isDummy());
}

/** Switching save mode does not keep stale objectrefs in master and does not auto-delete old linked files. */
TEST_F(TestLinkedObjectRestore,
       SaveModeToggleKeepsMasterConsistentAndLeavesOldLinkedFiles)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    const QDir baseDir(tempDir.path());
    const QByteArray inputXml = R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-TOGGLE}">
  <objectlist>
    <object class="TestSpecialGtObject" name="LinkedChild" uuid="{TOGGLE-LINK}" aslink="true"/>
    <object class="TestSpecialGtObject" name="RefOnlyChild" uuid="{TOGGLE-REF}" aslink="refonly"/>
  </objectlist>
</object>)";

    QString error;
    const QDomDocument inputDoc1 = parseObjectDocument(inputXml);
    const QString oneFilePath1 =
        baseDir.filePath(QStringLiteral("toggle_one_1.xml"));
    ASSERT_TRUE(gt::xml::saveProjectXmlWithLinkedObjects(
        QStringLiteral("TestProject"), inputDoc1, baseDir, oneFilePath1,
        gt::xml::LinkFileSaveType::OneFile, &error))
        << error.toStdString();

    QDomDocument one1Doc = readDomFromFile(oneFilePath1);
    ASSERT_FALSE(one1Doc.isNull());
    EXPECT_EQ(one1Doc.elementsByTagName(gt::xml::S_OBJECTREF_TAG).count(), 0);

    const QDomDocument inputDoc2 = parseObjectDocument(inputXml);
    const QString withLinkedPath =
        baseDir.filePath(QStringLiteral("toggle_with_links.xml"));
    ASSERT_TRUE(gt::xml::saveProjectXmlWithLinkedObjects(
        QStringLiteral("TestProject"), inputDoc2, baseDir, withLinkedPath,
        gt::xml::LinkFileSaveType::WithLinkedFiles, &error))
        << error.toStdString();

    QDomDocument withLinkedDoc = readDomFromFile(withLinkedPath);
    ASSERT_FALSE(withLinkedDoc.isNull());
    EXPECT_EQ(withLinkedDoc.elementsByTagName(gt::xml::S_OBJECTREF_TAG).count(),
              2);

    const QString linkedFile = baseDir.filePath(
        QStringLiteral("toggle_with_links/LinkedChild_TOGGLE-LINK.gtobj.xml"));
    const QString refOnlyLinkedFile = baseDir.filePath(
        QStringLiteral("toggle_with_links/RefOnlyChild_TOGGLE-REF.gtobj.xml"));
    EXPECT_TRUE(QFile::exists(linkedFile));
    EXPECT_FALSE(QFile::exists(refOnlyLinkedFile));

    const QDomDocument inputDoc3 = parseObjectDocument(inputXml);
    const QString oneFilePath2 =
        baseDir.filePath(QStringLiteral("toggle_one_2.xml"));
    ASSERT_TRUE(gt::xml::saveProjectXmlWithLinkedObjects(
        QStringLiteral("TestProject"), inputDoc3, baseDir, oneFilePath2,
        gt::xml::LinkFileSaveType::OneFile, &error))
        << error.toStdString();

    QDomDocument one2Doc = readDomFromFile(oneFilePath2);
    ASSERT_FALSE(one2Doc.isNull());
    EXPECT_EQ(one2Doc.elementsByTagName(gt::xml::S_OBJECTREF_TAG).count(), 0);

    // Existing linked files are intentionally not auto-deleted when switching back to OneFile.
    EXPECT_TRUE(QFile::exists(linkedFile));
}

/** A resolved linked child objectref restores as a regular non-dummy child. */
TEST_F(TestLinkedObjectRestore, ResolvedChildObjectRefCreatesRegularChild)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    const QString masterPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("resolved_child_master.xml"));
    const QByteArray masterXml = R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-RES-CHILD}">
  <objectlist>
    <objectref class="TestSpecialGtObject" name="ResolvedChild" uuid="{RES-CHILD}" href="linked/resolved_child.gtobj.xml"/>
  </objectlist>
</object>)";
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    const QString linkedPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("linked/resolved_child.gtobj.xml"));
    const QByteArray linkedXml = R"(
<GTLABOBJECTFILE>
  <object class="TestSpecialGtObject" name="ResolvedChild" uuid="{RES-CHILD}"/>
</GTLABOBJECTFILE>)";
    ASSERT_TRUE(writeTextFile(linkedPath, linkedXml));

    QStringList warnings;
    QDomDocument loaded =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);
    ASSERT_FALSE(loaded.isNull());
    EXPECT_TRUE(warnings.isEmpty());

    auto root = GtObjectMemento(loaded.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);
    ASSERT_FALSE(root->isDummy());

    auto child =
        root->findDirectChild<GtObject*>(QStringLiteral("ResolvedChild"));
    ASSERT_TRUE(child != nullptr);
    EXPECT_FALSE(child->isDummy());
}

/** A resolved linked grandchild objectref restores recursively as a regular non-dummy object. */
TEST_F(TestLinkedObjectRestore,
       ResolvedGrandChildObjectRefCreatesRegularGrandChild)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    const QString masterPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("resolved_grand_master.xml"));
    const QByteArray masterXml = R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-RES-GRAND}">
  <objectlist>
    <object class="TestSpecialGtObject" name="MidChild" uuid="{MID-RES}">
      <objectlist>
        <objectref class="TestSpecialGtObject" name="ResolvedGrandChild" uuid="{RES-GRAND}" href="linked/grandchild.gtobj.xml"/>
      </objectlist>
    </object>
  </objectlist>
</object>)";
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    const QString linkedPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("linked/grandchild.gtobj.xml"));
    const QByteArray linkedXml = R"(
<GTLABOBJECTFILE>
  <object class="TestSpecialGtObject" name="ResolvedGrandChild" uuid="{RES-GRAND}"/>
</GTLABOBJECTFILE>)";
    ASSERT_TRUE(writeTextFile(linkedPath, linkedXml));

    QStringList warnings;
    QDomDocument loaded =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);
    ASSERT_FALSE(loaded.isNull());
    EXPECT_TRUE(warnings.isEmpty());

    auto root = GtObjectMemento(loaded.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);

    auto mid = root->findDirectChild<GtObject*>(QStringLiteral("MidChild"));
    ASSERT_TRUE(mid != nullptr);
    EXPECT_FALSE(mid->isDummy());

    auto grand =
        mid->findDirectChild<GtObject*>(QStringLiteral("ResolvedGrandChild"));
    ASSERT_TRUE(grand != nullptr);
    EXPECT_FALSE(grand->isDummy());
}

/** Existing linked file with unknown class still yields a dummy, but not via unresolved objectref fallback. */
TEST_F(TestLinkedObjectRestore,
       ResolvedUnknownClassCreatesDummyWithoutUnresolvedFlag)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    const QString masterPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("resolved_unknown_master.xml"));
    const QByteArray masterXml = R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-RES-UNK}">
  <objectlist>
    <objectref class="UnknownResolvedClass" name="UnknownResolvedChild" uuid="{RES-UNK}" href="linked/unknown_resolved.gtobj.xml"/>
  </objectlist>
</object>)";
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    const QString linkedPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("linked/unknown_resolved.gtobj.xml"));
    const QByteArray linkedXml = R"(
<GTLABOBJECTFILE>
  <object class="UnknownResolvedClass" name="UnknownResolvedChild" uuid="{RES-UNK}"/>
</GTLABOBJECTFILE>)";
    ASSERT_TRUE(writeTextFile(linkedPath, linkedXml));

    QStringList warnings;
    QDomDocument loaded =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);
    ASSERT_FALSE(loaded.isNull());
    EXPECT_TRUE(warnings.isEmpty());

    GtObjectMemento rootMemento = GtObjectMemento(loaded.documentElement());
    ASSERT_EQ(rootMemento.childObjects.size(), 1);
    auto& childMemento = rootMemento.childObjects[0];
    EXPECT_FALSE(childMemento.isFlagEnabled(GtObjectMemento::IsUnresolved));

    auto child = childMemento.toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(child != nullptr);
    EXPECT_TRUE(child->isDummy());
}

/** Resolved linked objects roundtrip with linked mode without generating refonly markers. */
TEST_F(TestLinkedObjectRestore,
       ResolvedRoundtripWithLinkedModeDoesNotWriteRefOnly)
{
    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    const QString masterPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("resolved_roundtrip_master.xml"));
    const QByteArray masterXml = R"(
<object class="TestSpecialGtObject" name="Root" uuid="{ROOT-RES-RT}">
  <objectlist>
    <objectref class="TestSpecialGtObject" name="ResolvedChild" uuid="{RES-RT}" href="linked/resolved_roundtrip.gtobj.xml"/>
  </objectlist>
</object>)";
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    const QString linkedPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("linked/resolved_roundtrip.gtobj.xml"));
    const QByteArray linkedXml = R"(
<GTLABOBJECTFILE>
  <object class="TestSpecialGtObject" name="ResolvedChild" uuid="{RES-RT}"/>
</GTLABOBJECTFILE>)";
    ASSERT_TRUE(writeTextFile(linkedPath, linkedXml));

    QStringList warnings;
    QDomDocument loaded =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);
    ASSERT_FALSE(loaded.isNull());
    EXPECT_TRUE(warnings.isEmpty());

    auto root = GtObjectMemento(loaded.documentElement())
                    .toObject(*GtObjectFactory::instance());
    ASSERT_TRUE(root != nullptr);

    QDomDocument saveDoc;
    saveDoc.appendChild(importMementoElement(saveDoc, root->toMemento(false)));

    QString error;
    const QString outPath =
        QDir(tempDir.path())
            .filePath(QStringLiteral("resolved_roundtrip_out.xml"));
    ASSERT_TRUE(gt::xml::saveProjectXmlWithLinkedObjects(
        QStringLiteral("TestProject"), saveDoc, QDir(tempDir.path()), outPath,
        gt::xml::LinkFileSaveType::WithLinkedFiles, &error))
        << error.toStdString();

    const QDomDocument outMaster = readDomFromFile(outPath);
    ASSERT_FALSE(outMaster.isNull());

    QDomNodeList objects = outMaster.elementsByTagName(gt::xml::S_OBJECT_TAG);
    for (int i = 0; i < objects.count(); ++i)
    {
        const QDomElement obj = objects.at(i).toElement();
        EXPECT_NE(obj.attribute(gt::xml::S_ASLINK_TAG),
                  QStringLiteral("refonly"));
    }
}
