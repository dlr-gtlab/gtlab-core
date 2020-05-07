#include "gt_propertynozzlemapeditor.h"

#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QApplication>

#include "gt_filedialog.h"
#include "gt_datamodel.h"
#include "gt_application.h"

#include "gt_propertynozzlemapeditor.h"
#include "gt_nozzlemapproperty.h"
#include "gt_nozzlemap.h"

#include "gt_objectmemento.h"

GtPropertyNozzleMapEditor::GtPropertyNozzleMapEditor(
        QWidget* parent) :
    QWidget(parent)
{
    QHBoxLayout* lay = new QHBoxLayout;
    m_mapId = new QLineEdit;
    lay->addWidget(m_mapId);

    QPushButton* restoreButton = new QPushButton(QStringLiteral(""));
    restoreButton->setIcon(gtApp->icon(QStringLiteral("restoreIcon_16.png")));
    restoreButton->setFlat(true);
    restoreButton->setMaximumWidth(15);
    restoreButton->setAutoDefault(false);
    restoreButton->setToolTip(tr("Delete Map"));

    QPushButton* selectObjectButton = new QPushButton(QStringLiteral("..."));
    selectObjectButton->setMaximumWidth(30);
    selectObjectButton->setAutoDefault(false);
    selectObjectButton->setToolTip(tr("Choose Map"));

    lay->addWidget(restoreButton);
    lay->addWidget(selectObjectButton);
    setLayout(lay);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    m_mapId->setFrame(false);
    m_mapId->setReadOnly(true);

    connect(selectObjectButton, SIGNAL(clicked(bool)),
            SLOT(selectMap()));
    connect(restoreButton, SIGNAL(clicked(bool)), SLOT(deleteMap()));
}

void
GtPropertyNozzleMapEditor::setNozzleMapProperty(GtNozzleMapProperty *prop)
{
    m_prop = prop;

    if (m_prop == Q_NULLPTR)
    {
        gtWarning() << "m_prop == Q_NULLPTR";
    }

    updateText();
}

void
GtPropertyNozzleMapEditor::updateText()
{
    if (m_prop)
    {
        GtNozzleMap* m = m_prop->nozzleMap();

        if (m == Q_NULLPTR)
        {
            m_mapId->setText(QStringLiteral("-"));
        }
        else
        {
            m_mapId->setText(m->id());
        }
    }
}

void
GtPropertyNozzleMapEditor::selectMap()
{
    m_filename = GtFileDialog::getOpenFileName(
                       this,
                       tr("Open Map File"),
                       QString(),
                       tr("XML files (*.xml)"));

    QFile file(m_filename);

    if (!file.exists())
    {
        return;
    }

    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        gtError() << tr("Could not open file") << QStringLiteral("!");
        return;
    }

    QDomDocument document;
    QString errorStr;
    int errorLine;
    int errorColumn;

    if (!document.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
    {
        gtError() << QObject::tr("XML ERROR!") << " "
                  << QObject::tr("line") << ": "
                  << errorLine << " " << QObject::tr("column") << ": "
                  << errorColumn << " -> " << errorStr;
        return;
    }

    QDomElement root = document.documentElement();

    if (m_prop == Q_NULLPTR)
    {
        gtError() << tr("Property error!");
        return;
    }

    if (m_prop->object() == Q_NULLPTR)
    {
        gtError() << tr("Target object not found") << QStringLiteral("!");
        return;
    }

    GtNozzleMap* m = new GtNozzleMap;

    //GtNozzleMap* m = qobject_cast<GtNozzleMap*>(m_prop->nozzleMap());

    QFileInfo fileinfo(m_filename);
    m->setId(fileinfo.fileName());

    if (!m->readXML(root))
    {
        gtError() << "Nozzle Map not readable!";
        return;
    }

    deleteMap();
    m->setObjectName(m_prop->getVal());
    gtDataModel->appendChild(m, m_prop->object());

    updateText();
}

void
GtPropertyNozzleMapEditor::deleteMap()
{
    if (m_prop == Q_NULLPTR)
    {
        gtError() << "Property error!";
        return;
    }

    if (m_prop->object() == Q_NULLPTR)
    {
        gtError() << "Target object not found!";
        return;
    }

    GtNozzleMap* m = m_prop->nozzleMap();

    if (m == Q_NULLPTR)
    {
        return;
    }

    gtDataModel->deleteFromModel(m);

    updateText();
}
