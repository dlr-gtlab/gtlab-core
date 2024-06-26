/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 *
 *  Created on: 21.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include "gt_package.h"
#include "gt_objectmemento.h"
#include "gt_objectfactory.h"
#include "gt_logging.h"

GtPackage::GtPackage()
{
    setObjectName("Package");
}

bool
GtPackage::readData(const QDomElement& root)
{
    QDomElement oe = root.firstChildElement("object");
    while (!oe.isNull())
    {
        GtObjectMemento memento(oe);

        if (!memento.isNull())
        {
            // default object
            GtObject* dobj = findDirectChild<GtObject*>(memento.ident());

            if (dobj)
            {
                // TODO: warning for unknown data element
                //gtDebug() << tr("default object found!") << " ("
                //          << dobj->objectName() << ") " << tr("restoring...");
                dobj->setFactory(gtObjectFactory);
                dobj->fromMemento(memento);
            }
            else
            {
                GtObject* cobj = memento.restore(gtObjectFactory);

                if (cobj)
                {
                    appendChild(cobj);
                }
                else
                {
                    gtWarning() << tr("unknown data element skipped!")
                                << "(" << memento.className() << ")";
                }
            }
        }

        oe = oe.nextSiblingElement("object");
    }

    return true;
}

bool GtPackage::readMiscData(const QDir &projectDir)
{
    return true;
}

bool
GtPackage::saveData(QDomElement& root, QDomDocument& /*doc*/)
{
    foreach (GtObject* obj, findDirectChildren<GtObject*>())
    {
        GtObjectMemento memento = obj->toMemento();
        root.appendChild(memento.documentElement());
    }

    return true;
}

bool GtPackage::saveMiscData(const QDir &projectDir)
{
    return true;
}
