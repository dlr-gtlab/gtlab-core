/* GTlab - Gas Turbine laboratory
 * Source File: gt_controlschedulegroupui.cpp
 * copyright 2009-2017 by DLR
 *
 *  Created on: 08.05.2017
 *      Author: Carsten Klein (AT-TWK)
 *		  Tel.: +49 2203 601 2859
 */
#include "gt_controlschedulegroupui.h"

#include <QIcon>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QDialog>
#include <QFile>
#include <QLabel>
#include <QPushButton>

#include "gt_application.h"
#include "gt_datamodel.h"
#include "gt_controlschedulegroup.h"
#include "gt_controlschedule.h"
#include "gt_filedialog.h"
#include "gt_controlscheduleparser.h"
#include "gt_objectfactory.h"
#include "gt_tableparser.h"
#include "gt_objectmemento.h"

GtControlScheduleGroupUI::GtControlScheduleGroupUI()
{
    addSingleAction(tr("Add from file"),
              QStringLiteral("fileinIcon_16.png"),
              QStringLiteral("addControlSchedule"));

    addSingleAction(tr("Create own"),
              QStringLiteral("own_editPen.png"),
              QStringLiteral("createControlSchedule"));
}

QIcon
GtControlScheduleGroupUI::icon(GtObject* /*obj*/) const
{
    return GtApplication::icon("schedules4Icon_16.png");
}

void
GtControlScheduleGroupUI::addControlSchedule(GtObject* obj)
{
    GtControlScheduleGroup* group =
            qobject_cast<GtControlScheduleGroup*>(obj);

    if (group == Q_NULLPTR)
    {
        gtError() << tr("Can not find parent Object GtControlScheduleGroup");
        return;
    }

    QString filename = GtFileDialog::getOpenFileName(
                       Q_NULLPTR,
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

    bool success = false;
    QDomElement root = document.documentElement();

    if (root.isNull())
    {
        gtError() << tr("Root not found") << QStringLiteral("!");
        return;
    }

    /// Direct import in case that .sdl file is a memento
    if (root.tagName() != "GTlabControlSchedule")
    {
        GtObjectMemento mem = GtObjectMemento(root);
        GtControlSchedule* sched =
                mem.restore<GtControlSchedule*>(gtObjectFactory);

        if (sched != Q_NULLPTR)
        {
            QStringList existingScheduleNames = group->existingSchedules();

            if (existingScheduleNames.contains(sched->objectName()))
            {
                gtWarning() << tr("Schedule with name '") << sched->objectName()
                          << tr("' already exists.");
                return;
            }

            gtDataModel->appendChild(sched, group);
            return;
        }
    }

    // parse map to map object
    QList<QDomElement> tableElements;
    QDomElement child = root.firstChildElement("Table");

    while (!child.isNull())
    {
        tableElements.append( child );
        child = child.nextSiblingElement("Table");
    }

    if (tableElements.size() == 0)
    {
        gtError() << tr("No Table node found in Schedule");
        return;
    }

    foreach (QDomElement tableNode, tableElements)
    {
        if (tableNode.attribute("name").isNull())
        {
            gtError() << tr("No attribute 'name' found in Schedule");
            continue;
        }
        else if (tableNode.attribute("name").isEmpty())
        {
            gtError() << tr("Attribute 'name' is empty in Schedule");
            continue;
        }

        QString tabName = tableNode.attribute("name");

        QStringList existingScheduleNames = group->existingSchedules();

        if (existingScheduleNames.contains(tabName))
        {
            gtWarning() << tr("Schedule with name '") << tabName
                      << tr("' already exists.");
            continue;
        }

        GtControlScheduleParser parser;

        GtControlSchedule* csParsed = new GtControlSchedule();
        csParsed->setFactory(gtObjectFactory);

        success = parser.parseTable(tableNode, csParsed);

        if (!success)
        {
            gtError() << tr("Control Schedule could not be parsed")
                      << QStringLiteral(".");

            delete csParsed;

            continue;
        }

        if (csParsed == Q_NULLPTR)
        {
            qDebug() << "newSchedule is a Q_NULLPTR";
            continue;
        }

        gtDataModel->appendChild(csParsed, group);
    }
}

void
GtControlScheduleGroupUI::createControlSchedule(GtObject* obj)
{
    QDialog dialog;
    dialog.setWindowTitle("Create new Schedule...");
    QFormLayout formLay(&dialog);

    dialog.setWindowIcon(gtApp->icon("schedules3Icon.png"));

    QLineEdit* schedName = new QLineEdit(&dialog);
    QLineEdit* schedDescr = new QLineEdit(&dialog);

    QLineEdit* xName = new QLineEdit(&dialog);
    QLineEdit* xUnit = new QLineEdit(&dialog);
    QLineEdit* xDescr = new QLineEdit(&dialog);

    QLineEdit* yName = new QLineEdit(&dialog);
    QLineEdit* yUnit = new QLineEdit(&dialog);
    QLineEdit* yDescr = new QLineEdit(&dialog);

    formLay.addRow(new QLabel("Schedule information:"));
    formLay.addRow("Name:", schedName);
    formLay.addRow("Description:", schedDescr);

    formLay.addItem(new QSpacerItem(0, 8, QSizePolicy::Expanding,
                                    QSizePolicy::Fixed));


    formLay.addRow(new QLabel("X-Axis definition:"));
    formLay.addRow("Name:", xName);
    formLay.addRow("Unit:", xUnit);
    formLay.addRow("Description:", xDescr);

    formLay.addItem(new QSpacerItem(0, 8, QSizePolicy::Expanding,
                                    QSizePolicy::Fixed));

    formLay.addRow(new QLabel("Y-Axis definition:"));
    formLay.addRow("Name:", yName);
    formLay.addRow("Unit:", yUnit);
    formLay.addRow("Description:", yDescr);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    formLay.addRow(&buttonBox);

    buttonBox.button(QDialogButtonBox::Ok)->setDisabled(true);

    connect(schedName, SIGNAL(textChanged(QString)),
            this, SLOT(onLineEditCheckRequired()));

    connect(xName, SIGNAL(textChanged(QString)),
            this, SLOT(onLineEditCheckRequired()));

    connect(yName, SIGNAL(textChanged(QString)),
            this, SLOT(onLineEditCheckRequired()));

    connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    dialog.layout()->setSizeConstraint(QLayout::SetFixedSize);

    if (dialog.exec() == QDialog::Accepted)
    {
        GtControlSchedule* sched = new GtControlSchedule;
        sched->setObjectName(schedName->text());
        sched->setDescription(schedDescr->text());

        GtTableAxis* xAxis = new GtTableAxis;
        xAxis->setObjectName(xName->text());
        xAxis->setDescription(xDescr->text());
        xAxis->setUnit(xUnit->text());

        xAxis->setTicks(QVector<double>() << 0 << 1);
        xAxis->setLoExtMethod(GtTableAxis::E_LINEAR);
        xAxis->setHiExtMethod(GtTableAxis::E_LINEAR);
        xAxis->setInterMethod(GtTableAxis::I_LINEAR);

        GtTableValues* vals = new GtTableValues;
        vals->setObjectName(yName->text());
        vals->setDescription(yDescr->text());
        vals->setUnit(yUnit->text());
        vals->setValues(QVector<double>() << 1 << 1);

        sched->addAxis(xAxis);
        sched->addValues(vals);

        GtControlScheduleGroup* group =
                qobject_cast<GtControlScheduleGroup*>(obj);

        if (group == Q_NULLPTR)
        {
            return;
        }

        gtDataModel->appendChild(sched, group);

        GtObjectUI* schedUI = gtApp->objectUI(sched);
        if (schedUI != Q_NULLPTR)
        {
            schedUI->doubleClicked(sched);
        }
    }
}

void
GtControlScheduleGroupUI::onLineEditCheckRequired()
{
    QLineEdit* le = qobject_cast<QLineEdit*>(sender());

    if (le == Q_NULLPTR)
    {
        return;
    }

    QDialog* dialog = qobject_cast<QDialog*>(le->parent());

    QList<QLineEdit*> lineEdits = dialog->findChildren<QLineEdit*>();
    QDialogButtonBox* bb = dialog->findChild<QDialogButtonBox*>();

    QFormLayout* fLay = qobject_cast<QFormLayout*>(dialog->layout());

    if (bb == Q_NULLPTR || fLay == Q_NULLPTR)
    {
        return;
    }

    foreach(QLineEdit* lineEdit, lineEdits)
    {
        QLabel* lab = qobject_cast<QLabel*>(fLay->labelForField(lineEdit));
        if (lab != Q_NULLPTR)
        {
            if (lab->text() == "Name:")
            {
                if (lineEdit->text().isEmpty())
                {
                    bb->button(QDialogButtonBox::Ok)->setDisabled(true);
                    return;
                }
            }
        }
    }

    bb->button(QDialogButtonBox::Ok)->setEnabled(true);
}

