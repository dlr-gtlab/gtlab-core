/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "test_memento_append_importer.h"
#include "test_memento_append_importer_page.h"

#include "test_dmi_package.h"

#include "gt_objectmemento.h"
#include "gt_objectfactory.h"
#include "gt_icons.h"
#include "gt_selectfiletoimportpage.h"
#include "gt_logging.h"

#include <QIcon>
#include <QFile>

TestMementoAppendImporter::TestMementoAppendImporter()
{
    setObjectName(tr("Append Memento"));
}

QStringList
TestMementoAppendImporter::targets() const
{
    return {GT_CLASSNAME(TestDmiPackage)};
}

QIcon
TestMementoAppendImporter::icon() const
{
    return gt::gui::icon::import();
}

bool
TestMementoAppendImporter::import(GtObject* data, QFile& file)
{
    if (!data)
    {
        gtError() << tr("Data corrupted!");
        return false;
    }

    if (!file.open(QIODevice::ReadOnly))
    {
        gtError() << tr("Could not open file!") << file.fileName();
        return false;
    }

    GtObjectMemento memento(file.readAll());
    file.close();

    if (memento.isNull())
    {
        gtError() << tr("No memento found!") << file.fileName();
        return false;
    }

    auto* obj = memento.restore(gtObjectFactory, true);

    if (!obj)
    {
        gtError() << tr("Could not restore object!") << memento.className();
        return false;
    }

    if (obj->isDummy())
    {
        gtError() << tr("Unknown object type!") << memento.className();
        delete obj;
        return false;
    }

    if (!m_suffix.isEmpty())
    {
        obj->setObjectName(obj->objectName() + m_suffix);
    }

    if (!data->appendChild(obj))
    {
        gtError() << tr("Could not add object to") << data->objectName();
        delete obj;
        return false;
    }

    gtInfo() << "Memento append finished";

    return true;
}

QStringList
TestMementoAppendImporter::extensions() const
{
    return {QStringLiteral("XML (*.xml)")};
}

QList<GtAbstractImporterPage*>
TestMementoAppendImporter::pages(GtObject* data)
{
    auto selectPage = new GtSelectFileToImportPage(data, this);
    selectPage->setFileExtensions(extensions());

    return {new TestMementoAppendImporterPage(data, this), selectPage};
}

void
TestMementoAppendImporter::setSuffix(const QString& suffix)
{
    m_suffix = suffix;
}

QString
TestMementoAppendImporter::suffix() const
{
    return m_suffix;
}
