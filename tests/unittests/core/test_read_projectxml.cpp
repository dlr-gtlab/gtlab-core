/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TWK)
 */

#include <gtest/gtest.h>

#include "gt_xmlutilities.h"

#include <gt_logging.h>

#include <QTemporaryDir>

// --------------------------------------------------------
// Helper for writing files in tests
// --------------------------------------------------------
namespace
{
    static bool writeTextFile(const QString& path, const QByteArray& data)
    {
        QFile f(path);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;
        const qint64 written = f.write(data);
        return written == data.size();
    }
} // namespace

// A second fixture for the loader tests; it could share the same fixture as well.
class LoadXmlWithLinkedObjectsTest : public ::testing::Test
{
protected:
    QTemporaryDir tempDir;

    QString makePath(const QString& name) const
    {
        return tempDir.path() + QLatin1Char('/') + name;
    }

    QDir baseDir() const
    {
        return QDir(tempDir.path());
    }

    void SetUp() override
    {
        ASSERT_TRUE(tempDir.isValid());
    }
};

// --------------------------------------------------------
// 1) Master only, no <objectref> at all -> load should just
//    return the master document as-is (no warnings).
// --------------------------------------------------------
TEST_F(LoadXmlWithLinkedObjectsTest, MasterOnly_NoObjectRef)
{
    const QByteArray masterXml = R"(
<Root>
  <object class="Foo" name="A" uuid="{111-222}">
    <property name="x">42</property>
  </object>
</Root>
)";

    const QString masterPath = makePath("master.xml");
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    QStringList warnings;
    QDomDocument doc =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);

    ASSERT_FALSE(doc.isNull());
    EXPECT_TRUE(warnings.isEmpty());

    QDomElement root = doc.documentElement();
    ASSERT_FALSE(root.isNull());
    EXPECT_EQ(root.tagName(), QStringLiteral("Root"));

    QDomElement obj = root.firstChildElement("object");
    ASSERT_FALSE(obj.isNull());
    EXPECT_EQ(obj.attribute("class"), QStringLiteral("Foo"));
    EXPECT_EQ(obj.attribute("name"),  QStringLiteral("A"));
    EXPECT_EQ(obj.attribute("uuid"),  QStringLiteral("{111-222}"));

    QDomElement prop = obj.firstChildElement("property");
    ASSERT_FALSE(prop.isNull());
    EXPECT_EQ(prop.attribute("name"), QStringLiteral("x"));
    EXPECT_EQ(prop.text(), QStringLiteral("42"));

    // No <objectref> should be present
    EXPECT_TRUE(root.firstChildElement("objectref").isNull());
}

// --------------------------------------------------------
// 2) Single linked object:
//    master has <objectref>, external file has GTLABOBJECTFILE/object.
//    loadProjectXmlWithLinkedObjects should inline the object and
//    remove the objectref (no warnings).
// --------------------------------------------------------
TEST_F(LoadXmlWithLinkedObjectsTest, SingleLinkedObject_Expansion)
{
    // Master contains only an objectref
    const QByteArray masterXml = R"(
<Root>
  <objectref class="Foo" name="A"
             uuid="{111-222}"
             href="master/A_111-222.gtobj.xml"/>
</Root>
)";

    // Linked file content, as produced by the writer tests
    const QByteArray extXml = R"(
<GTLABOBJECTFILE>
  <object class="Foo" name="A" uuid="{111-222}">
    <property name="x">42</property>
  </object>
</GTLABOBJECTFILE>
)";

    const QString masterPath = makePath("master.xml");
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    const QString linkedPath = baseDir().filePath("master/A_111-222.gtobj.xml");
    QDir().mkpath(QFileInfo(linkedPath).dir().absolutePath());
    ASSERT_TRUE(writeTextFile(linkedPath, extXml));

    QStringList warnings;
    QDomDocument doc =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);

    ASSERT_FALSE(doc.isNull());
    EXPECT_TRUE(warnings.isEmpty());

    QDomElement root = doc.documentElement();
    ASSERT_FALSE(root.isNull());
    EXPECT_EQ(root.tagName(), QStringLiteral("Root"));

    // objectref should have been replaced by an object
    QDomElement obj = root.firstChildElement("object");
    ASSERT_FALSE(obj.isNull());
    EXPECT_TRUE(root.firstChildElement("objectref").isNull());

    EXPECT_EQ(obj.attribute("class"), QStringLiteral("Foo"));
    EXPECT_EQ(obj.attribute("name"),  QStringLiteral("A"));
    EXPECT_EQ(obj.attribute("uuid"),  QStringLiteral("{111-222}"));

    QDomElement prop = obj.firstChildElement("property");
    ASSERT_FALSE(prop.isNull());
    EXPECT_EQ(prop.attribute("name"), QStringLiteral("x"));
    EXPECT_EQ(prop.text(), QStringLiteral("42"));
}

// --------------------------------------------------------
// 3) Single linked object, but the linked file is missing:
//    - objectref must be kept in the final document.
//    - a warning should be produced.
// --------------------------------------------------------
TEST_F(LoadXmlWithLinkedObjectsTest, SingleLinkedObject_MissingFile)
{
    const QByteArray masterXml = R"(
<Root>
  <objectref class="Foo" name="A"
             uuid="{111-222}"
             href="master/A_111-222.gtobj.xml"/>
</Root>
)";

    const QString masterPath = makePath("master.xml");
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    // NOTE: we do NOT create the linked file

    QStringList warnings;
    QDomDocument doc =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);

    ASSERT_FALSE(doc.isNull());
    EXPECT_FALSE(warnings.isEmpty()) << "Expected at least one warning for missing linked file";

    QDomElement root = doc.documentElement();
    ASSERT_FALSE(root.isNull());

    // The objectref must still be there
    QDomElement ref = root.firstChildElement("objectref");
    ASSERT_FALSE(ref.isNull());
    EXPECT_EQ(ref.attribute("class"), QStringLiteral("Foo"));
    EXPECT_EQ(ref.attribute("name"),  QStringLiteral("A"));
    EXPECT_EQ(ref.attribute("uuid"),  QStringLiteral("{111-222}"));

    // No inlined object expected
    EXPECT_TRUE(root.firstChildElement("object").isNull());
}

// --------------------------------------------------------
// 4) Hierarchical expansion:
//    master:
//      Group/Parameterization
//        Pkg/HPT_curvePackage
//          objectref Leaf/Mean Line -> linked file
//    After loading, the objectref under Pkg should be expanded,
//    giving Group -> Pkg -> Leaf object hierarchy.
// --------------------------------------------------------
TEST_F(LoadXmlWithLinkedObjectsTest, HierarchicalObjectPathExpansion)
{
    const QByteArray masterXml = R"(
<Root>
  <object class="Group" name="Parameterization" uuid="{UUID-P}">
    <object class="Pkg" name="HPT_curvePackage" uuid="{UUID-C}">
      <objectref class="Leaf" name="Mean Line"
                 uuid="{ABC-123}"
                 href="master/Parameterization/HPT_curvePackage/Mean_Line_ABC-123.gtobj.xml"/>
    </object>
  </object>
</Root>
)";

    const QByteArray extLeafXml = R"(
<GTLABOBJECTFILE>
  <object class="Leaf" name="Mean Line" uuid="{ABC-123}">
    <property name="x">1.0</property>
  </object>
</GTLABOBJECTFILE>
)";

    const QString masterPath = makePath("master.xml");
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    const QString relDirLeaf  = QStringLiteral("master/Parameterization/HPT_curvePackage");
    const QString relFileLeaf = QStringLiteral("Mean_Line_ABC-123.gtobj.xml");
    const QString absPathLeaf = baseDir().filePath(relDirLeaf + QLatin1Char('/') + relFileLeaf);

    QDir().mkpath(QFileInfo(absPathLeaf).dir().absolutePath());
    ASSERT_TRUE(writeTextFile(absPathLeaf, extLeafXml));

    QStringList warnings;
    QDomDocument doc =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);

    ASSERT_FALSE(doc.isNull());
    EXPECT_TRUE(warnings.isEmpty());

    QDomElement root = doc.documentElement();
    ASSERT_FALSE(root.isNull());
    EXPECT_EQ(root.tagName(), QStringLiteral("Root"));

    QDomElement mParam = root.firstChildElement("object");
    ASSERT_FALSE(mParam.isNull());
    EXPECT_EQ(mParam.attribute("class"), QStringLiteral("Group"));
    EXPECT_EQ(mParam.attribute("name"),  QStringLiteral("Parameterization"));

    QDomElement mPkg = mParam.firstChildElement("object");
    ASSERT_FALSE(mPkg.isNull());
    EXPECT_EQ(mPkg.attribute("class"), QStringLiteral("Pkg"));
    EXPECT_EQ(mPkg.attribute("name"),  QStringLiteral("HPT_curvePackage"));

    // Under Pkg we expect the Leaf object (expanded), not objectref
    QDomElement mLeaf = mPkg.firstChildElement("object");
    ASSERT_FALSE(mLeaf.isNull());
    EXPECT_TRUE(mPkg.firstChildElement("objectref").isNull());

    EXPECT_EQ(mLeaf.attribute("class"), QStringLiteral("Leaf"));
    EXPECT_EQ(mLeaf.attribute("name"),  QStringLiteral("Mean Line"));
    EXPECT_EQ(mLeaf.attribute("uuid"),  QStringLiteral("{ABC-123}"));

    QDomElement mProp = mLeaf.firstChildElement("property");
    ASSERT_FALSE(mProp.isNull());
    EXPECT_EQ(mProp.attribute("name"), QStringLiteral("x"));
    EXPECT_EQ(mProp.text(), QStringLiteral("1.0"));
}

// --------------------------------------------------------
// 5) Recursive expansion:
//    master:
//      Group/Parameterization
//        objectref Pkg/HPT_curvePackage -> pkg file
//    pkg file:
//      GTLABOBJECTFILE
//        object Pkg/HPT_curvePackage
//          objectref Leaf/Mean Line -> leaf file
//    leaf file:
//      GTLABOBJECTFILE
//        object Leaf/Mean Line ...
//
//    After loading, we expect:
//      Group -> Pkg -> Leaf hierarchy, all as <object>, with no <objectref>
//    and no warnings.
// --------------------------------------------------------
TEST_F(LoadXmlWithLinkedObjectsTest, RecursiveHierarchicalObjectPathExpansion)
{
    // Master has objectref to Pkg
    const QByteArray masterXml = R"(
<Root>
  <object class="Group" name="Parameterization" uuid="{UUID-P}">
    <objectref class="Pkg" name="HPT_curvePackage"
               uuid="{UUID-C}"
               href="master_recursive/Parameterization/HPT_curvePackage_UUID-C.gtobj.xml"/>
  </object>
</Root>
)";

    // Pkg external file has object Pkg with objectref to Leaf
    const QByteArray pkgXml = R"(
<GTLABOBJECTFILE>
  <object class="Pkg" name="HPT_curvePackage" uuid="{UUID-C}">
    <objectref class="Leaf" name="Mean Line"
               uuid="{ABC-123}"
               href="master_recursive/Parameterization/HPT_curvePackage/Mean_Line_ABC-123.gtobj.xml"/>
  </object>
</GTLABOBJECTFILE>
)";

    // Leaf external file has the actual object
    const QByteArray leafXml = R"(
<GTLABOBJECTFILE>
  <object class="Leaf" name="Mean Line" uuid="{ABC-123}">
    <property name="x">1.0</property>
  </object>
</GTLABOBJECTFILE>
)";

    const QString masterPath = makePath("master_recursive.xml");
    ASSERT_TRUE(writeTextFile(masterPath, masterXml));

    const QString relDirPkg  = QStringLiteral("master_recursive/Parameterization");
    const QString relFilePkg = QStringLiteral("HPT_curvePackage_UUID-C.gtobj.xml");
    const QString absPathPkg = baseDir().filePath(relDirPkg + QLatin1Char('/') + relFilePkg);

    const QString relDirLeaf  = QStringLiteral("master_recursive/Parameterization/HPT_curvePackage");
    const QString relFileLeaf = QStringLiteral("Mean_Line_ABC-123.gtobj.xml");
    const QString absPathLeaf = baseDir().filePath(relDirLeaf + QLatin1Char('/') + relFileLeaf);

    QDir().mkpath(QFileInfo(absPathPkg).dir().absolutePath());
    ASSERT_TRUE(writeTextFile(absPathPkg, pkgXml));

    QDir().mkpath(QFileInfo(absPathLeaf).dir().absolutePath());
    ASSERT_TRUE(writeTextFile(absPathLeaf, leafXml));

    QStringList warnings;
    QDomDocument doc =
        gt::xml::loadProjectXmlWithLinkedObjects(masterPath, &warnings);

    ASSERT_FALSE(doc.isNull());
    EXPECT_TRUE(warnings.isEmpty());

    QDomElement root = doc.documentElement();
    ASSERT_FALSE(root.isNull());
    EXPECT_EQ(root.tagName(), QStringLiteral("Root"));

    QDomElement mParam = root.firstChildElement("object");
    ASSERT_FALSE(mParam.isNull());
    EXPECT_EQ(mParam.attribute("class"), QStringLiteral("Group"));
    EXPECT_EQ(mParam.attribute("name"),  QStringLiteral("Parameterization"));

    // Pkg should now be a fully inlined object, not an objectref
    QDomElement mPkg = mParam.firstChildElement("object");
    ASSERT_FALSE(mPkg.isNull());
    EXPECT_TRUE(mParam.firstChildElement("objectref").isNull());

    EXPECT_EQ(mPkg.attribute("class"), QStringLiteral("Pkg"));
    EXPECT_EQ(mPkg.attribute("name"),  QStringLiteral("HPT_curvePackage"));
    EXPECT_EQ(mPkg.attribute("uuid"),  QStringLiteral("{UUID-C}"));

    // Inside Pkg, Leaf should also be inlined
    QDomElement mLeaf = mPkg.firstChildElement("object");
    ASSERT_FALSE(mLeaf.isNull());
    EXPECT_TRUE(mPkg.firstChildElement("objectref").isNull());

    EXPECT_EQ(mLeaf.attribute("class"), QStringLiteral("Leaf"));
    EXPECT_EQ(mLeaf.attribute("name"),  QStringLiteral("Mean Line"));
    EXPECT_EQ(mLeaf.attribute("uuid"),  QStringLiteral("{ABC-123}"));

    QDomElement mProp = mLeaf.firstChildElement("property");
    ASSERT_FALSE(mProp.isNull());
    EXPECT_EQ(mProp.attribute("name"), QStringLiteral("x"));
    EXPECT_EQ(mProp.text(), QStringLiteral("1.0"));
}
