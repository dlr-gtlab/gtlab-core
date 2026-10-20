/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gtest/gtest.h"
#include "gt_objectgroup.h"
#include "gt_objectmemento.h"
#include "gt_stringproperty.h"

namespace
{
    QDomDocument dynamicPropertyDocument()
    {
        GtObjectGroup object;
        const bool added =
            object.addDynamicProperty(std::make_unique<GtStringProperty>(
                QStringLiteral("runtime"), QStringLiteral("Runtime")));
        EXPECT_TRUE(added);
        QDomDocument document;
        EXPECT_TRUE(document.setContent(object.toMemento().toByteArray()));
        return document;
    }

    QDomElement dynamicPropertyElement(const QDomDocument& document)
    {
        return document.documentElement()
            .firstChildElement("dynamicProperties")
            .firstChildElement("property");
    }
} // namespace

TEST(DynamicPropertyXml, readsLegacyFlagsAndCustomCategory)
{
    auto document = dynamicPropertyDocument();
    auto property = dynamicPropertyElement(document);
    property.removeAttribute("flags");
    property.setAttribute("readOnly", "true");
    property.setAttribute("optional", "true");
    property.setAttribute("hidden", "false");
    property.setAttribute("monitoring", "true");
    property.setAttribute("category", "custom");
    property.setAttribute("customCategory", "Legacy category");
    GtObjectMemento parsed(document.documentElement());
    ASSERT_FALSE(parsed.isNull());
    ASSERT_EQ(parsed.dynamicProperties.size(), 1);
    const auto& definition = parsed.dynamicProperties.front();
    using Data = GtObjectMemento::DynamicPropertyData;
    EXPECT_EQ(definition.flags,
              Data::ReadOnly | Data::Optional | Data::Monitoring);
    EXPECT_EQ(definition.category, QStringLiteral("Legacy category"));

    for (const auto* flag : {"readOnly", "optional", "hidden", "monitoring"})
        property.removeAttribute(flag);
    property.setAttribute("category", "main");
    parsed = GtObjectMemento(document.documentElement());
    ASSERT_FALSE(parsed.isNull());
    EXPECT_EQ(parsed.dynamicProperties.front().flags, 0U);
    EXPECT_TRUE(parsed.dynamicProperties.front().category.isEmpty());
}

TEST(DynamicPropertyXml, rejectsInvalidFlagsAndPropertyData)
{
    for (const auto* flag :
         {"readOnly", "optional", "hidden", "monitoring", "flags"})
    {
        SCOPED_TRACE(flag);
        auto document = dynamicPropertyDocument();
        auto property = dynamicPropertyElement(document);
        property.removeAttribute("flags");
        property.setAttribute(flag, "invalid");
        EXPECT_TRUE(GtObjectMemento(document.documentElement()).isNull());
    }
    auto document = dynamicPropertyDocument();
    auto property = dynamicPropertyElement(document);
    property.setAttribute("flags", 1U << 15);
    EXPECT_TRUE(GtObjectMemento(document.documentElement()).isNull());

    document = dynamicPropertyDocument();
    property = dynamicPropertyElement(document);
    auto data = property.firstChildElement("propertyData");
    auto value = data.firstChildElement("property");
    ASSERT_FALSE(value.isNull());
    value.setAttribute("name", "otherProperty");
    EXPECT_TRUE(GtObjectMemento(document.documentElement()).isNull());
    value.setAttribute("name", "runtime");
    data.appendChild(value.cloneNode(true));
    EXPECT_TRUE(GtObjectMemento(document.documentElement()).isNull());
}

TEST(DynamicPropertyXml, rejectsMalformedConfigurationWithoutPartialMemento)
{
    QStringList configurations{
        "<config type='array'/>",
        "<config type='object'><entry key='x' "
        "type='bool'>maybe</entry></config>",
        "<config type='object'><entry key='x' type='unknown'/></config>",
        "<config type='object'><item key='x' type='string'/></config>",
        "<config type='object'><entry type='string'/></config>",
        "<config type='object'><entry key='x' type='string'/><entry key='x' "
        "type='string'/></config>",
        "<config type='object'><entry key='x' type='array'><entry "
        "type='null'/></entry></config>",
        "<config type='object'><entry key='x' type='uint'>-1</entry></config>",
        "<config type='object'><entry key='x' "
        "type='int64'>9223372036854775808</entry></config>",
        "<config type='object'><entry key='x' "
        "type='uint64'>18446744073709551616</entry></config>",
        "<config type='object'><entry key='x' "
        "type='double'>nan</entry></config>"};
    QString nested = "<item type='null'/>";
    for (int i = 0; i < gt::PropertyConfigurationMaxDepth; ++i)
        nested = "<item type='array'>" + nested + "</item>";
    configurations.push_back(
        "<config type='object'><entry key='x' type='array'>" + nested +
        "</entry></config>");

    for (const auto& configuration : configurations)
    {
        SCOPED_TRACE(configuration.toStdString());
        auto document = dynamicPropertyDocument();
        auto property = dynamicPropertyElement(document);
        QDomDocument configDocument;
        ASSERT_TRUE(configDocument.setContent(configuration));
        property.replaceChild(
            document.importNode(configDocument.documentElement(), true),
            property.firstChildElement("config"));
        EXPECT_TRUE(GtObjectMemento(document.documentElement()).isNull());
    }
}
