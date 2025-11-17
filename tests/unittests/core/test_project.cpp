/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2025 German Aerospace Center (DLR)
 *
 *  Author: Martin Siggel (AT-TWK)
 */

#include <gtest/gtest.h>

#include "gt_project.h"

#include <QTemporaryDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDomDocument>
#include <QDir>

namespace
{

static QDomDocument
readXmlToDom(const QByteArray& data)
{
    QDomDocument doc;
    QString errorMsg;
    int errorLine = 0, errorCol = 0;

    if (!doc.setContent(data, &errorMsg, &errorLine, &errorCol))
        return QDomDocument();  // or std::move(doc), but this is fine

    return doc;
}

static QDomDocument
readFileToDom(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text))
        return QDomDocument();

    const QByteArray data = f.readAll();
    return readXmlToDom(data);
}

} // namespace

class SaveXmlWithLinkedObjectsTest : public ::testing::Test
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
// 1) Simple doc: no aslink attributes -> only master file is written,
//    no linked files.
// --------------------------------------------------------
TEST_F(SaveXmlWithLinkedObjectsTest, MasterOnly_NoAslink)
{
    const char* xml = R"(
<Root>
  <object class="Foo" name="A" uuid="{111-222}"/>
</Root>
)";

    QDomDocument doc = readXmlToDom(xml);

    const QString masterPath = makePath("master.xml");
    QString error;

    const bool ok = gt::project::saveXmlWithLinkedObjects(
        QStringLiteral("TestProject"), doc, baseDir(), masterPath, &error);

    ASSERT_TRUE(ok) << error.toStdString();

    // Master file exists
    EXPECT_TRUE(QFile::exists(masterPath));
    // No obvious linked file should exist (we don't know all names; just check none with _A.xml)
    const QString candidate = baseDir().filePath("A_111-222.gtobj.xml");
    EXPECT_FALSE(QFile::exists(candidate));

    // Master file should still contain an <object>, not <objectref>
    QDomDocument masterDoc = readFileToDom(masterPath);
    ASSERT_FALSE(masterDoc.isNull());
    QDomElement masterRoot = masterDoc.documentElement();
    ASSERT_FALSE(masterRoot.isNull());

    QDomElement child = masterRoot.firstChildElement();
    ASSERT_FALSE(child.isNull());
    EXPECT_EQ(child.tagName(), QStringLiteral("object"));
    EXPECT_EQ(child.attribute("class"), QStringLiteral("Foo"));
    EXPECT_EQ(child.attribute("name"), QStringLiteral("A"));
    EXPECT_EQ(child.attribute("uuid"), QStringLiteral("{111-222}"));
}

// --------------------------------------------------------
// 2) Single object with aslink="true" -> linked file + objectref.
// --------------------------------------------------------
TEST_F(SaveXmlWithLinkedObjectsTest, SingleLinkedObject_AslinkTrue)
{
    const char* xml = R"(
<Root>
  <object class="Foo" name="A" uuid="{111-222}" aslink="true">
    <property name="x">42</property>
  </object>
</Root>
)";
    QDomDocument doc = readXmlToDom(xml);

    const QString masterPath = makePath("master.xml");
    QString error;

    const bool ok = gt::project::saveXmlWithLinkedObjects(
        QStringLiteral("TestProject"), doc, baseDir(), masterPath, &error);

    ASSERT_TRUE(ok) << error.toStdString();

    // Master file must exist
    EXPECT_TRUE(QFile::exists(masterPath));

    // Linked file name according to our convention:
    // cleanUuid: "111-222", sanitizedName: "A" -> "111-222_A.xml"
    const QString linkedPath = baseDir().filePath("master/A_111-222.gtobj.xml");
    EXPECT_TRUE(QFile::exists(linkedPath));

    // ---- Check master content: objectref instead of object ----
    QDomDocument masterDoc = readFileToDom(masterPath);
    ASSERT_FALSE(masterDoc.isNull());
    QDomElement masterRoot = masterDoc.documentElement();
    ASSERT_FALSE(masterRoot.isNull());

    QDomElement child = masterRoot.firstChildElement();
    ASSERT_FALSE(child.isNull());
    EXPECT_EQ(child.tagName(), QStringLiteral("objectref"));
    EXPECT_EQ(child.attribute("class"), QStringLiteral("Foo"));
    EXPECT_EQ(child.attribute("name"), QStringLiteral("A"));
    EXPECT_EQ(child.attribute("uuid"), QStringLiteral("{111-222}"));

    // href should be relative, here just "111-222_A.xml"
    EXPECT_EQ(child.attribute("href"), QStringLiteral("master/A_111-222.gtobj.xml"));

    // ---- Check linked content ----
    QDomDocument extDoc = readFileToDom(linkedPath);
    ASSERT_FALSE(extDoc.isNull());

    QDomElement extRoot = extDoc.documentElement();
    ASSERT_FALSE(extRoot.isNull());
    EXPECT_EQ(extRoot.tagName(), QStringLiteral("GTLABOBJECTFILE"));

    QDomElement extObj = extRoot.firstChildElement("object");
    ASSERT_FALSE(extObj.isNull());
    EXPECT_EQ(extObj.attribute("class"), QStringLiteral("Foo"));
    EXPECT_EQ(extObj.attribute("name"), QStringLiteral("A"));
    EXPECT_EQ(extObj.attribute("uuid"), QStringLiteral("{111-222}"));
    // The linked copy should not keep aslink
    EXPECT_FALSE(extObj.hasAttribute("aslink"));

    // And property is preserved
    QDomElement extProp = extObj.firstChildElement("property");
    ASSERT_FALSE(extProp.isNull());
    EXPECT_EQ(extProp.attribute("name"), QStringLiteral("x"));
    EXPECT_EQ(extProp.text(), QStringLiteral("42"));
}

// --------------------------------------------------------
// 3) Multiple linked objects: both linked files exist,
//    master contains multiple objectref nodes.
// --------------------------------------------------------
TEST_F(SaveXmlWithLinkedObjectsTest, MultipleLinkedObjects)
{
    const QByteArray xml = R"(
<Root>
  <object class="Foo" name="A" uuid="{UUID-A}" aslink="true"/>
  <object class="Bar" name="B" uuid="{UUID-B}" aslink="true"/>
</Root>
)";

    QDomDocument doc = readXmlToDom(xml);

    const QString masterPath = makePath("master.xml");
    QString error;

    const bool ok = gt::project::saveXmlWithLinkedObjects(
        QStringLiteral("TestProject"), doc, baseDir(), masterPath, &error);

    ASSERT_TRUE(ok) << error.toStdString();

    // Check master exists
    EXPECT_TRUE(QFile::exists(masterPath));

    // Expected linked filenames (uuid braces stripped)
    const QString extA = baseDir().filePath("master/A_UUID-A.gtobj.xml");
    const QString extB = baseDir().filePath("master/B_UUID-B.gtobj.xml");

    EXPECT_TRUE(QFile::exists(extA));
    EXPECT_TRUE(QFile::exists(extB));

    // Check master has two objectref children
    QDomDocument masterDoc = readFileToDom(masterPath);
    ASSERT_FALSE(masterDoc.isNull());
    QDomElement masterRoot = masterDoc.documentElement();
    ASSERT_FALSE(masterRoot.isNull());

    QDomElement ref1 = masterRoot.firstChildElement("objectref");
    ASSERT_FALSE(ref1.isNull());
    EXPECT_EQ(ref1.attribute("name"), QStringLiteral("A"));

    QDomElement ref2 = ref1.nextSiblingElement("objectref");
    ASSERT_FALSE(ref2.isNull());
    EXPECT_EQ(ref2.attribute("name"), QStringLiteral("B"));
}


// --------------------------------------------------------
// 4) Objects without aslink attribute are treated as inline
//    (same behavior as explicit "false").
// --------------------------------------------------------
TEST_F(SaveXmlWithLinkedObjectsTest, MissingAslinkTreatedAsFalse)
{
    const QByteArray xml = R"(
<Root>
  <object class="Foo" name="A" uuid="{111-222}">
    <!-- no aslink attribute -->
  </object>
</Root>
)";

    QDomDocument doc = readXmlToDom(xml);

    const QString masterPath = makePath("master.xml");
    QString error;

    const bool ok = gt::project::saveXmlWithLinkedObjects(
        QStringLiteral("TestProject"), doc, baseDir(), masterPath, &error);

    ASSERT_TRUE(ok) << error.toStdString();

    // Master exists, no linked file
    EXPECT_TRUE(QFile::exists(masterPath));
    const QString extPath = baseDir().filePath("A_111-222.gtobj.xml");
    EXPECT_FALSE(QFile::exists(extPath));

    // Master has an <object>, not an <objectref>
    QDomDocument masterDoc = readFileToDom(masterPath);
    ASSERT_FALSE(masterDoc.isNull());
    QDomElement masterRoot = masterDoc.documentElement();
    ASSERT_FALSE(masterRoot.isNull());

    QDomElement child = masterRoot.firstChildElement();
    ASSERT_FALSE(child.isNull());
    EXPECT_EQ(child.tagName(), QStringLiteral("object"));
}

// --------------------------------------------------------
// 5) Hierarchical object path: nested objects with aslink="true"
//    should produce directory structure derived from their path.
// --------------------------------------------------------
TEST_F(SaveXmlWithLinkedObjectsTest, HierarchicalObjectPathCreation)
{
    const QByteArray xml = R"(
<Root>
  <object class="Group" name="Parameterization" uuid="{UUID-P}">
    <object class="Pkg" name="HPT_curvePackage" uuid="{UUID-C}">
      <object class="Leaf" name="Mean Line" uuid="{ABC-123}" aslink="true">
        <property name="x">1.0</property>
      </object>
    </object>
  </object>
</Root>
)";

    QDomDocument doc = readXmlToDom(xml);

    const QString masterPath = makePath("master.xml");
    QString error;

    const bool ok = gt::project::saveXmlWithLinkedObjects(
        QStringLiteral("TestProject"), doc, baseDir(), masterPath, &error);

    ASSERT_TRUE(ok) << error.toStdString();

    // Expected relative path:
    // objectPath at leaf = ["Parameterization", "HPT_curvePackage", "Mean_Line"]
    // -> relDir = "master/Parameterization/HPT_curvePackage"
    // cleanUuid = "ABC-123", sanitized name = "Mean_Line"
    // -> file = "ABC-123_Mean_Line.xml"
    const QString relDir = QStringLiteral("master/Parameterization/HPT_curvePackage");
    const QString relFile = QStringLiteral("Mean_Line_ABC-123.gtobj.xml");
    const QString relHref = relDir + QLatin1Char('/') + relFile;
    const QString absPath = baseDir().filePath(relHref);

    // Directory and file should exist
    EXPECT_TRUE(QDir(baseDir().filePath(relDir)).exists())
        << "Expected directory: " << relDir.toStdString();
    EXPECT_TRUE(QFile::exists(absPath))
        << "Expected linked file: " << absPath.toStdString();

    // ---- Check master: objectref with correct href ----
    QDomDocument masterDoc = readFileToDom(masterPath);
    ASSERT_FALSE(masterDoc.isNull());
    QDomElement masterRoot = masterDoc.documentElement();
    ASSERT_FALSE(masterRoot.isNull());

    // Walk down the hierarchy again:
    QDomElement mParam = masterRoot.firstChildElement("object");
    ASSERT_FALSE(mParam.isNull());
    EXPECT_EQ(mParam.attribute("name"), QStringLiteral("Parameterization"));

    QDomElement mPkg = mParam.firstChildElement("object");
    ASSERT_FALSE(mPkg.isNull());
    EXPECT_EQ(mPkg.attribute("name"), QStringLiteral("HPT_curvePackage"));

    QDomElement mRef = mPkg.firstChildElement("objectref");
    ASSERT_FALSE(mRef.isNull());
    EXPECT_EQ(mRef.attribute("class"), QStringLiteral("Leaf"));
    EXPECT_EQ(mRef.attribute("name"), QStringLiteral("Mean Line"));
    EXPECT_EQ(mRef.attribute("uuid"), QStringLiteral("{ABC-123}"));
    EXPECT_EQ(mRef.attribute("href"), relHref);

    // ---- Check linked document content ----
    QDomDocument extDoc = readFileToDom(absPath);
    ASSERT_FALSE(extDoc.isNull());
    QDomElement extRoot = extDoc.documentElement();
    ASSERT_FALSE(extRoot.isNull());
    EXPECT_EQ(extRoot.tagName(), QStringLiteral("GTLABOBJECTFILE"));

    QDomElement extObj = extRoot.firstChildElement("object");
    ASSERT_FALSE(extObj.isNull());
    EXPECT_EQ(extObj.attribute("class"), QStringLiteral("Leaf"));
    EXPECT_EQ(extObj.attribute("name"), QStringLiteral("Mean Line"));
    EXPECT_EQ(extObj.attribute("uuid"), QStringLiteral("{ABC-123}"));
    EXPECT_FALSE(extObj.hasAttribute("aslink"));

    QDomElement extProp = extObj.firstChildElement("property");
    ASSERT_FALSE(extProp.isNull());
    EXPECT_EQ(extProp.attribute("name"), QStringLiteral("x"));
    EXPECT_EQ(extProp.text(), QStringLiteral("1.0"));
}
