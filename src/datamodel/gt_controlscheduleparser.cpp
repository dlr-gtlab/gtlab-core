/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 01.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */

#include "gt_controlscheduleparser.h"
#include "gt_objectfactory.h"
#include "gt_logging.h"
#include "gt_tableparser.h"
#include "gt_objectmemento.h"

GtControlScheduleParser::GtControlScheduleParser()
{

}

GtControlSchedule*
GtControlScheduleParser::fromDomElement(QDomElement& root,
                                        bool& success)
{
    GtControlSchedule* csParsed = new GtControlSchedule;
    csParsed->setFactory(gtObjectFactory);

    success = parseSchedule(root, csParsed);

    if (!success)
    {
        gtError() << tr("Control Schedule could not be parsed")
                  << QStringLiteral(".");

        delete csParsed;

        return Q_NULLPTR;
    }

    return csParsed;
}

bool
GtControlScheduleParser::parseSchedule(QDomElement& root, GtControlSchedule* cs)
{
    if (root.tagName() != "GTlabControlSchedule")
    {
        gtError() << "Given DomElements tag is '" << root.tagName()
                  << " Expected 'GTlabControlSchedule'";
        return false;
    }

    QDomElement tableNode = root.firstChildElement("Table");

    if (tableNode.isNull())
    {
        gtError() << "No Table node found in Schedule";
        return false;
    }
    else if (tableNode.attribute("name").isNull())
    {
        gtError() << "No attribute 'name' found in Schedule";
        return false;
    }
    else if (tableNode.attribute("name").isEmpty())
    {
        gtError() << "Attribute 'name' is empty in Schedule";
        return false;
    }

    if (!parseTable(tableNode, cs))
    {
        gtError() << "Could not parse table of Control Schedule"
                  << cs->objectName();
        return false;
    }

    // store description in parent of table class
//    cs->setDescription(cs->findDirectChild<GtTable*>()->getDescription());

    return true;
}

bool
GtControlScheduleParser::parseTable(QDomElement& root, GtControlSchedule* cs)
{
    GtTableParser tabParser;
    bool success = false;

    tabParser.fromDomElement(root, cs, success);

    QString tabname = root.attribute("name");

    if (!success)
    {
        gtError() << tr("Table ") << tabname << tr("could not be parsed") << ".";
        return false;
    }

    // mark as transient schedules if exactly one axes exist und its name
    // equals to time

    cs->isTransient();

    return true;
}
