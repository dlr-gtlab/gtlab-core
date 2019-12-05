#include "gt_tableparser.h"
#include "gt_extrapolators.h"
#include "gt_logging.h"

#include <QDebug>
#include <QString>
#include <QStringList>

GtTable*
GtTableParser::fromDomElement(QDomElement &root, bool &success)
{
    GtTable* table = new GtTable();

    success = parseTable(root, table);

    if (!success)
    {
        gtError() << "Table could not be parsed.";
        delete table;
        table = NULL;
    }

    return table;
}

void
GtTableParser::fromDomElement(QDomElement& root, GtTable* table, bool& success)
{
    success = parseTable(root, table);

    if (!success)
    {
        gtError() << "Table could not be parsed.";
    }
}

bool
GtTableParser::parseTable(QDomElement &root, GtTable *table)
{
    // check whether root element is a 'Table'

    if (root.tagName() != "Table")
    {
        gtError() << "Given DomElements tag is '" << root.tagName()
                 << " Expected 'Table'";
        return false;
    }

    // fetch the table's name

    QString name = root.attribute("name");

    if (name.isEmpty())
    {
        gtError() << "Could not read table name.";
        return false;
    }

    table->setObjectName(name);

    // fetch the tables description

    QDomElement description = root.firstChildElement("Description");

    if (description.isNull())
    {
        gtWarning() << "No description node found for table " << name << ".";
    }
    else if (description.text().isEmpty())
    {
        gtWarning() << "No description given for table " << name << ".";
    }

    table->setDescription(description.text().trimmed());

    // fetch the tables unit

    QDomElement unit = root.firstChildElement("Unit");

    if (unit.isNull())
    {
        gtError() << "No unit given for table '" << name << "'.";
        return false;
    }

    table->setUnit(unit.text().trimmed());

    QDomElement gridless = root.firstChildElement("Gridless");

    if (!gridless.isNull())
    {
        QString grid = gridless.text();
        if (grid == "TRUE" || "True" || "true")
        {
            table->setType(GtTable::GRIDLESS);
        }
    }

    if (!parseAxes(root, table))
    {
        gtError() << "Could not parse table axis.";
        return false;
    }

    QDomElement values = root.firstChildElement("Values");

    if (!parseValues(values, table))
    {
        gtError() << "Could not parse table values.";
        return false;
    }

    return true;
}


bool
GtTableParser::parseAxes(QDomElement &root, GtTable *table)
{
    QDomNodeList nodes = root.elementsByTagName("Axis");

    if (nodes.count() == 0)
    {
        gtError() << "Table '" << table->objectName()
                  << "must provide at least one axis - none given.";
        return false;
    }

    bool success = false;

    for (int i = 0; i < nodes.count(); ++i)
    {
        if (nodes.at(i).isElement())
        {
            QDomElement e = nodes.at(i).toElement();
            success = parseAxis(e, table);
        }
    }

    return success;
}


bool
GtTableParser::parseAxis(QDomElement &root, GtTable *table)
{
    // check whether root element is an 'Axis'

    if (root.tagName() != "Axis")
    {
        gtError() << "Given element is tagged '" << root.tagName()
                  << "' - expected 'Axis'.";
        return false;
    }

    // fetch the axis id

    QString id = root.attribute("id");

    if (id.isEmpty())
    {
        gtError() << "Empty axis id encountered in table '"
                  << table->objectName()
                  << "'.";
        return false;
    }

    // fetch the name

    QDomElement nameElement = root.firstChildElement("Name");

    if (nameElement.isNull())
    {
        gtError() << "Empty axis name encountered in table '"
                  << table->objectName() << "'.";
        return false;
    }

    // fetch the unit

    QDomElement unitElement = root.firstChildElement("Unit");

    if (unitElement.isNull())
    {
        gtError() << "Empty axis unit encountered in table '"
                  << table->objectName() << "'.";
        return false;
    }

    // fetch the description

    QDomElement descElement = root.firstChildElement("Description");

    if (descElement.isNull())
    {
        gtWarning() << "No description node found in table '"
                    << table->objectName() << "'.";
    }
    else if (descElement.text().isEmpty())
    {
        gtWarning() << "Empty description encountered in table '"
                    << table->objectName() << "'.";
    }

    // fetch the interpolation method

    QDomElement intElement = root.firstChildElement("Interpolation");

    if (intElement.isNull())
    {
        gtError() << "No interpolation method given for axis '"
                  << nameElement.text() << "'' of table '"
                  << table->objectName() << "'.";
        return false;
    }

    // fetch the lo extrapolation method

    QDomElement loElement = root.firstChildElement("LowExtrapolation");

    if (loElement.isNull())
    {
        gtError() << "No low extrapolation method given for axis '"
                  << nameElement.text() << "'' of table '"
                  << table->objectName() << "'.";
        return false;
    }

    // fetch the hi extrapolation method

    QDomElement hiElement = root.firstChildElement("HighExtrapolation");

    if (hiElement.isNull())
    {
        gtError() << "No high extrapolation method given for axis '"
                  << nameElement.text() << "'' of table '"
                  << table->objectName() << "'.";
        return false;
    }

    // fetch the ticks

    QDomElement ticksElement = root.firstChildElement("Ticks");

    if (ticksElement.isNull())
    {
        gtError() << "No ticks given for axis '"
                  << nameElement.text() << "'' of table '"
                  << table->objectName() << "'.";
        return false;
    }

    bool ticksOk = false;
    QVector<double> ticks = parseDoubleVector(ticksElement, ticksOk);

    if (!ticksOk)
    {
        gtError() << "Ticks could not be parsed for axis '"
                  << nameElement.text() << "'' of table '"
                  << table->objectName() << "'.";

        return false;
    }

    GtTableAxis::ExtrapMethod emLo = GtTableAxis::E_LINEAR;
    GtTableAxis::InterpMethod im = GtTableAxis::I_LINEAR;
    GtTableAxis::ExtrapMethod emHi = GtTableAxis::E_LINEAR;

    QString emLoStr = loElement.text().trimmed();
    QString imStr   = intElement.text().trimmed();
    QString emHiStr = hiElement.text().trimmed();

    // interpolation method

    if (imStr == "linear")
    {
        im = GtTableAxis::I_LINEAR;
    }
    else if (imStr == "lagrange2")
    {
        im = GtTableAxis::I_LAGRANGE2;
    }
    else if (imStr == "lagrange3")
    {
        im = GtTableAxis::I_LAGRANGE3;
    }
    else if (imStr == "catmull")
    {
        im = GtTableAxis::I_CATMULL;
    }
    else if ( imStr  == "logarithmic" )
    {
        im = GtTableAxis::I_LOGARITHMIC;
    }
    else
    {
        gtError() << "No such interpolation method '"
                  << imStr << "' given for axis '"
                  << nameElement.text() << "'' of table '"
                  << table->objectName() << "'.";
        return false;
    }

    // low extrapolation method

    if (emLoStr == "none")
    {
        emLo = GtTableAxis::E_NONE;
    }
    else if (emLoStr  == "const")
    {
        emLo = GtTableAxis::E_CONST;
    }
    else if (emLoStr  == "linear")
    {
        emLo = GtTableAxis::E_LINEAR;
    }
    else if (emLoStr  == "lagrange2")
    {
        emLo = GtTableAxis::E_LAGRANGE2;
    }
    else
    {
        gtError() << "No such extrapolation method '"
                  << imStr << "' given for axis '"
                  << nameElement.text() << "'' of table '"
                  << table->objectName() << "'.";
        return false;
    }

    // high extrapolation method

    if (emHiStr == "none")
    {
        emHi = GtTableAxis::E_NONE;
    }
    else if (emHiStr  == "const")
    {
        emHi = GtTableAxis::E_CONST;
    }
    else if (emHiStr  == "linear")
    {
        emHi = GtTableAxis::E_LINEAR;
    }
    else if (emHiStr  == "lagrange2")
    {
        emHi = GtTableAxis::E_LAGRANGE2;
    }
    else
    {
        gtError() << "No such extrapolation method '"
                  << imStr << "' given for axis '"
                  << nameElement.text() << "'' of table '"
                  << table->objectName() << "'.";
        return false;
    }

    if (!table->addAxis( nameElement.text().trimmed(),
                             descElement.text().trimmed(),
                             unitElement.text().trimmed(),
                             emLo, im, emHi, ticks))
    {
        gtError() << "GtTable: "
                  << "Parsing Axis not successfull.";

        return false;
    }


    return true;
}


bool GtTableParser::parseValues(QDomElement &root, GtTable *table)
{
    bool valsOk = false;
    QVector<double> vals = parseDoubleVector(root, valsOk);

    if (!valsOk)
    {
        gtError() << "Values could not be parsed for axis table '"
                  << table->objectName() << "'.";
        return false;
    }

    table->addValues(table->objectName(), table->getDescription(),
                        table->getUnit(), vals);

    return true;
}


QVector<double>
GtTableParser::parseDoubleVector(QDomElement &root, bool &success)
{
    QStringList valStrList = root.text().simplified().split(",");
    QVector<double> vals;

    if (valStrList.empty())
    {
        gtError() << "Empty list encountered during parse of double vector.";
        success = false;
    }
    else
    {
        int counter = 0;

        foreach (QString valStr, valStrList)
        {
            double val = valStr.toDouble(&success);
            counter++;

            if (!success)
            {
                gtError() << "Floating point number conversion of value '"
                          << valStr << "' at index "
                          << counter << "did not succeed.";
                break;
            }

            vals.append(val);
        }
    }

    return vals;
}
