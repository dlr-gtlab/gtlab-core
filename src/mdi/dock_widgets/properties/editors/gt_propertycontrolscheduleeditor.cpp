/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */

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

#include "gt_propertycontrolscheduleeditor.h"
#include "gt_controlscheduleproperty.h"
#include "gt_controlscheduleparser.h"

#include "gt_objectmemento.h"

GtPropertyControlScheduleEditor::GtPropertyControlScheduleEditor(
        QWidget* parent) :
    QWidget(parent)
{
    QHBoxLayout* lay = new QHBoxLayout;
    m_scheduleId = new QLineEdit;
    lay->addWidget(m_scheduleId);

    QPushButton* restoreButton = new QPushButton(QStringLiteral(""));
    restoreButton->setIcon(gtApp->icon(QStringLiteral("restoreIcon_16.png")));
    restoreButton->setFlat(true);
    restoreButton->setMaximumWidth(15);
    restoreButton->setAutoDefault(false);
    restoreButton->setToolTip(tr("Delete Schedule"));

    QPushButton* selectObjectButton = new QPushButton(QStringLiteral("..."));
    selectObjectButton->setMaximumWidth(30);
    selectObjectButton->setAutoDefault(false);
    selectObjectButton->setToolTip(tr("Choose Schedule"));

    lay->addWidget(restoreButton);
    lay->addWidget(selectObjectButton);
    setLayout(lay);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    m_scheduleId->setFrame(false);
    m_scheduleId->setReadOnly(true);

    connect(selectObjectButton, SIGNAL(clicked(bool)),
            SLOT(selectMap()));
    connect(restoreButton, SIGNAL(clicked(bool)), SLOT(deleteMap()));
}

void
GtPropertyControlScheduleEditor::setControlScheduleProperty(
        GtControlScheduleProperty* prop)
{
    m_prop = prop;

    if (m_prop == Q_NULLPTR)
    {
        gtWarning() << "m_prop == Q_NULLPTR";
    }

    updateText();
}

void GtPropertyControlScheduleEditor::updateText()
{
    if (m_prop)
    {
        GtControlSchedule* cs = m_prop->controlSchedule();

        if (cs == Q_NULLPTR)
        {
            m_scheduleId->setText(QStringLiteral("-"));
        }
        else
        {
            m_scheduleId->setText(cs->getDescription());
        }
    }
}

void
GtPropertyControlScheduleEditor::selectMap()
{
    QString filename = GtFileDialog::getOpenFileName(
                       this,
                       tr("Open Schedule File"),
                       QString(),
                       tr("SDL files (*.sdl)"));

    QFile file(filename);

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

    bool success;
    QDomElement root = document.documentElement();

    if (root.isNull())
    {
        gtError() << tr("Root not found") << QStringLiteral("!");
        return;
    }

    // parse map to map object
    QDomElement tableNode = root.firstChildElement("Table");

    if (tableNode.isNull())
    {
        gtError() << tr("No Table node found in Schedule");
        return;
    }
    else if (tableNode.attribute("name").isNull())
    {
        gtError() << tr("No attribute 'name' found in Schedule");
        return;
    }
    else if (tableNode.attribute("name").isEmpty())
    {
        gtError() << tr("Attribute 'name' is empty in Schedule");
        return;
    }

    GtControlSchedule* cs = qobject_cast<GtControlSchedule*>(m_prop->object());

    if (cs == Q_NULLPTR)
    {
        gtError() << tr("Control Schedule is a Q_NULLPTR");
        return;
    }

    GtObject* parent = cs->findParent<GtObject*>();

    if (parent == Q_NULLPTR)
    {
        gtError() << tr("parent is a Q_NULLPTR");
        return;
    }

    QString tabName = tableNode.attribute("name");

    foreach (GtControlSchedule* s,
             parent->findDirectChildren<GtControlSchedule*>())
    {
        if (s == Q_NULLPTR)
        {
            continue;
        }

        if (tabName == cs->objectName())
        {
            gtWarning() << tr("Overriding current Control Schedule '")
                        << tabName
                        << tr("' .");
            break;
        }

        if (tabName == s->objectName())
        {
            gtWarning() << tr("Schedule with name '") << tabName
                      << tr("' already exists.");
            return;
        }
    }

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

    GtControlScheduleParser parser;

    GtControlSchedule* newSchedule = parser.fromDomElement(root, success);

    if (newSchedule == Q_NULLPTR)
    {
        qDebug() << "newSchedule is a Q_NULLPTR";
        return;
    }

    const int row = cs->childNumber();
    newSchedule->setUuid(cs->uuid());

    gtDataModel->deleteFromModel(cs);

    gtDataModel->insertChild(newSchedule, parent, row);

    updateText();
}

void GtPropertyControlScheduleEditor::deleteMap()
{
//    if (m_prop == Q_NULLPTR)
//    {
//        gtError() << "Property error!";
//        return;
//    }

//    if (m_prop->object() == Q_NULLPTR)
//    {
//        gtError() << "Target object not found!";
//        return;
//    }

//    GtControlSchedule* schedule = m_prop->controlSchedule();

//    if (schedule == Q_NULLPTR)
//    {
//        return;
//    }

//    gtDataModel->deleteFromModel(schedule);

//    updateText();
}
