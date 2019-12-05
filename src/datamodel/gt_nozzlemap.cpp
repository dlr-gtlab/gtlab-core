#include "gt_nozzlemap.h"

#include "gt_table.h"
#include <QDomElement>
#include "gt_tableparser.h"
#include "gt_objectmemento.h"

GtNozzleMap::GtNozzleMap()
{
    GtTable* table = new GtTable;
    table->setObjectName(tr("CD"));
    table->setDefault(true);
    appendChild(table);
}

GtTable*
GtNozzleMap::map()
{
    return findDirectChild<GtTable*>();
}

bool
GtNozzleMap::readXML(QDomElement &root)
{
    if (root.isNull())
    {
        gtError() << tr("Root not found") << QStringLiteral("!");
        return false;
    }

    if (root.tagName() != "GTlabMap")
    {
        gtError() << "Given DomElements tag is '" << root.tagName()
                  << " Expected 'GTlabMap'";
        return false;
    }

    // parse map to map object
    QDomElement type = root.firstChildElement("Type");

    if (type.isNull())
    {
        gtError() << "No type node found for nozzle map.";
        return false;
    }
    else if (type.text().isEmpty())
    {
        gtError() << "Empty type node given for nozzle map.";
        return false;
    }

    m_type = type.text().trimmed();

    QDomElement description = root.firstChildElement("Description");

    if (description.isNull())
    {
        //gtWarning() << "No description node found for nozzle map";
    }
    else if (description.text().isEmpty())
    {
        //gtWarning() << "No description given for nozzle map";
    }

    m_brief = description.text().trimmed();

    // fetch the source

    QDomElement source = root.firstChildElement("Source");

    if (source.isNull())
    {
        //gtWarning() << "No source node found for nozzle map";
    }
    else if (source.text().isEmpty())
    {
        //gtWarning() << "No source given for nozzle map";
    }

    m_source = source.text().trimmed();


    QDomElement tables = root.firstChildElement("Tables");

    if (tables.isNull())
    {
        gtError() << "No tables node found for nozzle map.";
        return false;
    }

    if (!parseTable(tables))
    {
        gtError() << "Could not parse table of nozzle map";
        return false;
    }

    return true;
}

void
GtNozzleMap::setId(QString str)
{
    m_id = str;
}

QString
GtNozzleMap::id() const
{
    return m_id;
}

QString
GtNozzleMap::source() const
{
    return m_source;
}

void
GtNozzleMap::setSource(const QString &source)
{
    m_source = source;
}

QString
GtNozzleMap::description() const
{
    return m_brief;
}

void
GtNozzleMap::setDescription(const QString &brief)
{
    m_brief = brief;
}

QString
GtNozzleMap::type() const
{
    return m_type;
}

void
GtNozzleMap::setType(const QString &type)
{
    m_type = type;
}

double
GtNozzleMap::CD(double npr, double petalAngle)
{
    GtTable* m = map();

    if (m == Q_NULLPTR)
    {
        gtError() << "CD map not found for nozzle, returning CD = 1.0";
        return 1.0;
    }

    return m->getValue2D(m->objectName(), npr, petalAngle);
}

bool
GtNozzleMap::parseTable(QDomElement &root)
{
    GtTableParser tabParser;
    GtTable* tab = NULL;
    bool success = false;

    // *** WredStd ***

    QDomElement CD;

    if (!findTable(root, "CD", CD))
    {
        gtError() << "No 'CD' node found for nozzle map.";
        return false;
    }

    tab = tabParser.fromDomElement(CD, success);

    if (!success)
    {
        gtError() << "Table 'CD' could not be parsed.";
        return false;
    }

    GtTable* actual = map();

//    actual = tab;
    actual->fromMemento(tab->toMemento());

    delete tab;
    tab = Q_NULLPTR;

    return true;
}

bool
GtNozzleMap::findTable(const QDomElement &tables, const QString &name,
                       QDomElement &table)
{
    QDomElement tab = tables.firstChildElement("Table");

    while (!tab.isNull())
    {
        if (tab.attributes().contains("name"))
        {
            if (tab.attribute("name") == name)
            {
                table = tab;
                return true;
            }
        }

        tab = tab.nextSiblingElement("Table");
    }

    gtError() << "No table named" << name << "found.";
    return false;
}
