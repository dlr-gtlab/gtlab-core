/* GTlab - Gas Turbine laboratory
 * Source File: gtd_datamodelconverter.cpp
 * copyright 2009-2022 by DLR
 *
 *  Author: Stanislaus Reitenbach (AT-TWK)
 */

#include <QFileInfo>
#include <QDomElement>

#include "gt_logging.h"
#include "gt_project.h"
#include "gt_taskgroup.h"
#include "gt_xmlexpr.h"

#include "gt_coreupgraderoutines.h"

bool
gt::detail::GtDataModelConverter::to_2_0_0_alpha1::run(QDomElement& domElement,
                                                       const QString& context)
{
    // check whether context is a project file. if not than nothing to do here
    QFileInfo info(context);

    if (info.suffix() != GtProject::mainFileExtension())
    {
        return true;
    }

    // context is project file. let us move the process information from project
    // file to separate task files.
    // why? because its fancy shit

    if (domElement.isNull() ||
            (domElement.tagName() != QLatin1String(gt::xml::S_GTLAB_TAG)))
    {
        gtError() << "Invalid GTlab project file!";
        return false;
    }

    /* process informations */
    QDomElement pdata =
            domElement.firstChildElement(QStringLiteral("PROCESSES"));

    // if no process information is available, we are already happy now
    if (pdata.isNull())
    {
        // very happy
        return true;
    }

    // not good not bad. but we need to convert the process information now
    // lets save all existing task in the new generated user group
    QDomElement pe = pdata.firstChildElement(gt::xml::S_OBJECT_TAG);
    while (!pe.isNull())
    {
        // we found a task. lets move him to his final destination in
        // a separate file. rip task

        if (!GtTaskGroup::saveTaskElementToFile(
                    info.absolutePath(),
                    GtTaskGroup::USER,
                    GtTaskGroup::defaultUserGroupId(),
                    pe))
        {
            gtError().nospace() << "could not export task! (" <<
                                   pe.attribute(gt::xml::S_NAME_TAG) << ")";
            return false;
        }

        pe = pe.nextSiblingElement(gt::xml::S_OBJECT_TAG);
    }

    // it looks like everything has gone according the masterplan up to this
    // point. now only the process data xml tree has to be removed :)

    domElement.removeChild(pdata);

    // phew... the last step was hard
    return true;
}
