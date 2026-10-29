/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 *
 *  Author: Marius Bröcker <marius.broecker@dlr.de>
 */

#include "test_memento_restore_importer.h"
#include "test_memento_restore_importer_page.h"

#include "test_dmi_externalobject.h"

#include "gt_objectmemento.h"
#include "gt_objectfactory.h"
#include "gt_icons.h"
#include "gt_selectfiletoimportpage.h"
#include "gt_logging.h"

#include <QIcon>
#include <QFile>

TestMementoRestoreImporter::TestMementoRestoreImporter()
{
    setObjectName(tr("Memento Restore Importer"));
}

QStringList
TestMementoRestoreImporter::targets() const
{
    return {QStringLiteral("TestDmiData"), GT_CLASSNAME(TestDmiExternalObject)};
}

QIcon
TestMementoRestoreImporter::icon() const
{
    return gt::gui::icon::import();
}

bool
TestMementoRestoreImporter::import(GtObject* data, QFile& file)
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

    if (memento.className() != data->metaObject()->className())
    {
        gtError() << tr("Memento type") << memento.className()
                  << tr("does not match target type")
                  << data->metaObject()->className();
        return false;
    }

    const QString uuid = data->uuid();
    const QString name = data->objectName();

    if (!memento.mergeTo(*data, *gtObjectFactory))
    {
        gtError() << tr("Could not merge memento into object!")
                  << data->objectName();
        return false;
    }

    // exported mementos contain new uuids, always keep the object identity
    data->setUuid(uuid);

    if (m_keepObjectName)
    {
        data->setObjectName(name);
    }

    gtInfo() << "Memento restore finished";

    return true;
}

QStringList
TestMementoRestoreImporter::extensions() const
{
    return {QStringLiteral("XML (*.xml)")};
}

QList<GtAbstractImporterPage*>
TestMementoRestoreImporter::pages(GtObject* data)
{
    auto selectPage = new GtSelectFileToImportPage(data, this);
    selectPage->setFileExtensions(extensions());

    return {new TestMementoRestoreImporterPage(data, this), selectPage};
}

void
TestMementoRestoreImporter::setKeepObjectName(bool keep)
{
    m_keepObjectName = keep;
}

bool
TestMementoRestoreImporter::keepObjectName() const
{
    return m_keepObjectName;
}
