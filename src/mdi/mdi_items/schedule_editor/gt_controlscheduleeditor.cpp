/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2016 by DLR
 *
 *  Created on: 02.08.2016
 *  Author: Maximilian Vieweg (AT-TWK)
 *  Tel.: +49 2203 601 4799
 */

#include "gt_controlscheduleeditor.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QKeyEvent>
#include <QTableWidget>
#include <QSizePolicy>
#include <QHeaderView>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QTableView>
#include <QModelIndex>
#include <QInputDialog>

#include "gt_application.h"
#include "gt_controlschedule.h"
#include "gt_controlscheduleplot.h"
#include "gt_controlschedulecanvas.h"
#include "gt_controlschedulemodel.h"
#include "gt_objectmementodiff.h"
#include "gt_tableaxis.h"
#include "gt_command.h"
#include "gt_stylesheets.h"
#include "gt_project.h"

GtControlScheduleEditor::GtControlScheduleEditor() :
    m_plot(Q_NULLPTR), m_title(Q_NULLPTR),
    m_addPointButton(Q_NULLPTR), m_probeButton(Q_NULLPTR),
    m_editButton(Q_NULLPTR), m_saveButton(Q_NULLPTR),
    m_revertButton(Q_NULLPTR), m_modAxisButton(Q_NULLPTR),
    m_tableClearButton(Q_NULLPTR), m_interpBox(Q_NULLPTR),
    m_loExtrapBox(Q_NULLPTR), m_hiExtrapBox(Q_NULLPTR),
    m_schedule(Q_NULLPTR), m_probeTable(Q_NULLPTR),
    m_tableView(Q_NULLPTR), m_model(Q_NULLPTR),
    m_tabWidget(Q_NULLPTR), m_tableAddButton(Q_NULLPTR),
    m_tableDeleteButton(Q_NULLPTR)
{
    setObjectName(tr("Control Schedule Editor"));

    widget()->installEventFilter(this);

    QHBoxLayout* hLay = new QHBoxLayout;
    hLay->setContentsMargins(0, 0, 0, 0);
    hLay->setSpacing(0);
    widget()->setLayout(hLay);

    QVBoxLayout* vLay = new QVBoxLayout;
    vLay->setContentsMargins(0, 0, 0, 0);
    vLay->setSpacing(0);
    hLay->addLayout(vLay);

    m_tabWidget = new QTabWidget();
    m_tabWidget->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Expanding);
    hLay->addWidget(m_tabWidget);

    m_tableView = new QTableView;
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
//    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    m_probeTable = new QTableWidget;
    m_probeTable->setColumnCount(2);
    m_probeTable->setHorizontalHeaderLabels(QStringList() << "x" << "y");
    m_probeTable->setAlternatingRowColors(true);
    m_probeTable->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    m_probeTable->horizontalHeader()->setSectionResizeMode(
                QHeaderView::Stretch);

    QWidget* widget1 = new QWidget;
    QVBoxLayout* newLay1 = new QVBoxLayout;
    QHBoxLayout* newHlay = new QHBoxLayout;

    widget1->setLayout(newLay1);
    newLay1->addWidget(m_tableView);

    m_tableAddButton = new QPushButton(tr("Add point"));
    m_tableAddButton->setIcon(gtApp->icon("addIcon.png"));
    m_tableAddButton->setToolTip(tr("Add point to schedule"));
    m_tableAddButton->setStyleSheet(GtStyleSheets::buttonStyleSheet());
    m_tableAddButton->setDisabled(true);

    m_tableDeleteButton = new QPushButton(tr("Delete point(s)"));
    m_tableDeleteButton->setIcon(gtApp->icon("crossIcon.png"));
    m_tableDeleteButton->setToolTip(tr("Delete selected point(s) "
                                       "from schedule"));
    m_tableDeleteButton->setStyleSheet(GtStyleSheets::buttonStyleSheet());
    m_tableDeleteButton->setDisabled(true);

    newHlay->addWidget(m_tableAddButton);
    newHlay->addWidget(m_tableDeleteButton);
    newLay1->addLayout(newHlay);

    QWidget* widget2 = new QWidget;
    QVBoxLayout* newLay2 = new QVBoxLayout;
    widget2->setLayout(newLay2);
    newLay2->addWidget(m_probeTable);

    m_tableClearButton = new QPushButton(tr("Clear"));
    m_tableClearButton->setIcon(gtApp->icon("clearIcon.png"));
    m_tableClearButton->setToolTip(tr("Clear data probes"));
    m_tableClearButton->setStyleSheet(GtStyleSheets::buttonStyleSheet());
    m_tableClearButton->setDisabled(true);

    m_probeAtButton = new QPushButton(tr("Data probe at"));
    m_probeAtButton->setIcon(gtApp->icon("cursorIcon.png"));
    m_probeAtButton->setToolTip(tr("Take data probe at certain position"));
    m_probeAtButton->setStyleSheet(GtStyleSheets::buttonStyleSheet());
    m_probeAtButton->setDisabled(false);

    QHBoxLayout* newLay3 = new QHBoxLayout;
    newLay3->addWidget(m_probeAtButton);
    newLay3->addWidget(m_tableClearButton);
    newLay2->addLayout(newLay3);

    m_tabWidget->addTab(widget1, "Data Points");
    m_tabWidget->addTab(widget2, "Data Probes");

    QFrame* frame = new QFrame;
    QPalette palette = frame->palette();
    palette.setColor(frame->backgroundRole(), QColor(255, 255, 255));
    frame->setPalette(palette);
    frame->setAutoFillBackground(true);

    m_title = new QLabel;
    QFont font;
    font.setBold(true);
    font.setPointSize(10);
    m_title->setFont(font);
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setLayoutDirection(Qt::LeftToRight);

    QHBoxLayout* layTitle = new QHBoxLayout();
    layTitle->setContentsMargins(3, 3, 3, 3);
    layTitle->addWidget(m_title);

    layTitle->setSizeConstraint(QLayout::SetMinimumSize);
    frame->setLayout(layTitle);

    vLay->addWidget(frame);

    m_plot = new GtControlSchedulePlot();
    m_model = new GtControlScheduleModel(this);

    vLay->addWidget(m_plot);

    m_interpBox = new QComboBox;
    m_interpBox->addItems(QStringList() << "Linear" << "Lagrange 2"
                          << "Lagrange 3" << "Catmull" << "Logarithmic");
    m_interpBox->setDisabled(true);

    m_loExtrapBox = new QComboBox;
    m_loExtrapBox->addItems(QStringList() << "None" << "Constant"
                            << "Linear" << "Langrange 2");
    m_loExtrapBox->setDisabled(true);

    m_hiExtrapBox = new QComboBox;
    m_hiExtrapBox->addItems(QStringList() << "None" << "Constant"
                            << "Linear" << "Langrange 2");
    m_hiExtrapBox->setDisabled(true);

    QHBoxLayout* buttonsLay = new QHBoxLayout;
    buttonsLay->setContentsMargins(0, 0, 0, 0);
    buttonsLay->setSpacing(0);
    buttonsLay->addStretch();
    vLay->addLayout(buttonsLay);

    m_probeButton = new QPushButton;
    m_probeButton->setIcon(gtApp->icon("cursorIcon.png"));
    m_probeButton->setMaximumSize(QSize(20, 20));
    m_probeButton->setFlat(true);
    m_probeButton->setCheckable(true);
    m_probeButton->setChecked(false);
    m_probeButton->setToolTip(tr("Data probe"));

    m_editButton = new QPushButton;
    m_editButton->setIcon(gtApp->icon("monitoringIcon.png"));
    m_editButton->setMaximumSize(QSize(20, 20));
    m_editButton->setFlat(true);
    m_editButton->setCheckable(true);
    m_editButton->setChecked(false);
    m_editButton->setToolTip(tr("Enable schedule editing"));

    m_addPointButton = new QPushButton;
    m_addPointButton->setIcon(gtApp->icon("addIcon.png"));
    m_addPointButton->setMaximumSize(QSize(20, 20));
    m_addPointButton->setFlat(true);
    m_addPointButton->setCheckable(true);
    m_addPointButton->setToolTip(tr("Add Point"));
    m_addPointButton->setDisabled(true);

    m_modAxisButton = new QPushButton;
    m_modAxisButton->setIcon(gtApp->icon("configIcon.png"));
    m_modAxisButton->setMaximumSize(QSize(20, 20));
    m_modAxisButton->setFlat(true);
    m_modAxisButton->setCheckable(false);
    m_modAxisButton->setToolTip(tr("Schedule configuration"));
    m_modAxisButton->setDisabled(true);

    m_saveButton = new QPushButton;
    m_saveButton->setIcon(gtApp->icon("checkIcon.png"));
    m_saveButton->setMaximumSize(QSize(20, 20));
    m_saveButton->setFlat(true);
    m_saveButton->setCheckable(false);
    m_saveButton->setToolTip(tr("Accept changes"));

    m_revertButton = new QPushButton;
    m_revertButton->setIcon(gtApp->icon("undoIcon.png"));
    m_revertButton->setMaximumSize(QSize(20, 20));
    m_revertButton->setFlat(true);
    m_revertButton->setCheckable(false);
    m_revertButton->setToolTip(tr("Revert and discard changes"));

    buttonsLay->addWidget(m_probeButton, 0, Qt::AlignRight);
    buttonsLay->addWidget(m_editButton, 0, Qt::AlignRight);
    buttonsLay->addWidget(m_addPointButton, 0, Qt::AlignRight);
    buttonsLay->addWidget(m_modAxisButton, 0, Qt::AlignRight);
    buttonsLay->addItem(new QSpacerItem(3, 0, QSizePolicy::Fixed,
                                        QSizePolicy::Fixed));
    buttonsLay->addWidget(new QLabel("Interpolation: "));
    buttonsLay->addWidget(m_interpBox);
    buttonsLay->addItem(new QSpacerItem(3, 0, QSizePolicy::Fixed,
                                        QSizePolicy::Fixed));
    buttonsLay->addWidget(new QLabel("Low Extrapolation: "));
    buttonsLay->addWidget(m_loExtrapBox);
    buttonsLay->addItem(new QSpacerItem(3, 0, QSizePolicy::Fixed,
                                        QSizePolicy::Fixed));
    buttonsLay->addWidget(new QLabel("High Extrapolation: "));
    buttonsLay->addWidget(m_hiExtrapBox);
    buttonsLay->addItem(new QSpacerItem(3, 0, QSizePolicy::Fixed,
                                        QSizePolicy::Fixed));
    buttonsLay->addWidget(m_revertButton, 0, Qt::AlignRight);
    buttonsLay->addWidget(m_saveButton, 0, Qt::AlignRight);

    connect(m_probeButton, SIGNAL(clicked(bool)),
            this, SLOT(onDataProbePress()));

    connect(m_editButton, SIGNAL(clicked(bool)),
            this, SLOT(onEditButtonPress()));

    connect(m_addPointButton, SIGNAL(clicked(bool)),
            this, SLOT(onAddButtonPress()));

    connect(m_modAxisButton, SIGNAL(clicked(bool)),
            this, SLOT(onModAxisButtonPress()));

    connect(m_saveButton, SIGNAL(clicked(bool)),
            this, SLOT(onSaveButtonpress()));

    connect(m_revertButton, SIGNAL(clicked(bool)),
            this, SLOT(onRevertButtonPress()));

    connect(m_plot->plotCanvas(), SIGNAL(changeEditMode()),
            this, SLOT(onChangeEditMode()));

    connect(m_plot->plotCanvas(), SIGNAL(changeAddPointMode()),
            this, SLOT(onChangeAddPointMode()));

    connect(m_plot->plotCanvas(), SIGNAL(changeAxisData()),
            this, SLOT(onChangeAxisInformation()));

    connect(m_plot->plotCanvas(), SIGNAL(changeDataProbeMode()),
            this, SLOT(onChangeDataProbeMode()));

    connect(m_plot, SIGNAL(markerSelected(GtControlScheduleMarker*)),
            this, SLOT(onMarkerSelected(GtControlScheduleMarker*)));

    connect(m_plot, SIGNAL(markersDeselected()),
            this, SLOT(onMarkersDeselected()));

    connect(m_interpBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onInterpolationChanged()));

    connect(m_loExtrapBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onExtrapolationChanged()));

    connect(m_hiExtrapBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onExtrapolationChanged()));

    connect(m_plot->plotCanvas(), SIGNAL(revertAndDiscard()),
            this, SLOT(onRevertRequest()));

    connect(m_plot->plotCanvas(), SIGNAL(save()),
            this, SLOT(onSaveRequest()));

    connect(m_plot->plotCanvas(), SIGNAL(dataProbeTaken(QPointF)),
            this, SLOT(onDataProbeTaken(QPointF)));

    connect(m_tableClearButton, SIGNAL(pressed()),
            this, SLOT(clearDataProbes()));

    connect(m_probeAtButton, SIGNAL(pressed()),
            this, SLOT(onDataProbeAtButtonPress()));

    connect(m_tableAddButton, SIGNAL(pressed()),
            this, SLOT(onTableAddButtonPressed()));

    connect(m_tableDeleteButton, SIGNAL(clicked()),
            this, SLOT(onTableDeleteButtonPressed()));

    connect(gtApp, SIGNAL(currentProjectChanged(GtProject*)),
            this, SLOT(deleteLater()));
}

bool
GtControlScheduleEditor::eventFilter(QObject* obj, QEvent* event)
{
    if (obj != widget())
    {
        return false;
    }

    switch (event->type())
    {
        case QEvent::KeyPress:
        {
            const QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
            onKeyPressEvent(keyEvent);
            return true;
        }

        default:
            break;
    }

    return QObject::eventFilter(obj, event);
}

void
GtControlScheduleEditor::setData(GtObject* obj)
{
    if (m_schedule != Q_NULLPTR)
    {
        disconnect(m_schedule);
    }

    GtControlSchedule* cs = qobject_cast<GtControlSchedule*>(obj);

    if (cs == Q_NULLPTR)
    {
        gtWarning() << tr("Given object '")
                    << obj->objectName()
                    << tr("' is no control schedule");
        return;
    }

    connect(cs, SIGNAL(destroyed(QObject*)), this, SLOT(deleteLater()));

    m_title->setText(cs->objectName());
    m_title->setToolTip(cs->getDescription());

    if (m_plot != Q_NULLPTR)
    {
        m_plot->initPlotFromSchedule(cs);
    }

    m_schedule = cs;

    QList<GtTableAxis*> axList = m_schedule->getAxesList();
    if (axList.size() != 1)
    {
        return;
    }

    m_oldInterp = axList.first()->interMethod();
    m_oldLoExtrap = axList.first()->loExtMethod();
    m_oldHiExtrap = axList.first()->hiExtMethod();

    initExtrpComboBox(m_loExtrapBox, m_oldLoExtrap);
    initExtrpComboBox(m_hiExtrapBox, m_oldHiExtrap);
    initInterpComboBox(m_interpBox, m_oldInterp);

    m_probeTable->setHorizontalHeaderLabels(QStringList() << m_plot->xTitle()
                                            << m_plot->yTitle());

    m_model->setPlot(m_plot);
    m_tableView->setModel(m_model);

    connect(m_tableView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(onTableViewSelectionChanged()));
}

void
GtControlScheduleEditor::onKeyPressEvent(const QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        if (m_plot->plotCanvas()->addPointActive())
        {
            onChangeAddPointMode();
        }
        else if (m_plot->plotCanvas()->dataProbeActive())
        {
            onChangeDataProbeMode();
        }
        else if (m_plot->editModeActive())
        {
            onChangeEditMode();
        }
    }
    else if (event->modifiers() == Qt::ControlModifier)
    {
        if (event->key() == Qt::Key_A)
        {
            if (m_plot->editModeActive())
            {
                onChangeAddPointMode();
            }
        }
        else if (event->key() == Qt::Key_D)
        {
            if (!m_plot->editModeActive())
            {

                onChangeDataProbeMode();
            }
        }
        else if (event->key() == Qt::Key_E)
        {
            onChangeEditMode();
        }
    }
}

void
GtControlScheduleEditor::onEditButtonPress()
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    if (m_schedule == Q_NULLPTR)
    {
        return;
    }

    if (m_editButton->isChecked())
    {
        m_addPointButton->setEnabled(true);
        m_modAxisButton->setEnabled(true);
        m_loExtrapBox->setEnabled(true);
        m_hiExtrapBox->setEnabled(true);
        m_interpBox->setEnabled(true);
        m_saveButton->setEnabled(true);
        m_revertButton->setEnabled(true);
        m_tableAddButton->setEnabled(true);
        m_tableDeleteButton->setEnabled(false);
        m_probeButton->setDisabled(true);
        m_plot->activateEditMode();
    }
    else
    {
        if (m_addPointButton->isChecked())
        {
            m_addPointButton->click();
        }

        if (!isEqualToDatamodel())
        {
            QMessageBox msgBox;
            msgBox.setText(tr("The schedule has been modified."));
            msgBox.setInformativeText(tr("Do you want to save your changes?"));
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                                      QMessageBox::Cancel);

            msgBox.setDefaultButton(QMessageBox::Save);
            int ret = msgBox.exec();

            switch (ret)
            {
                case QMessageBox::Save:
                    onSaveButtonpress();
                    break;

                case QMessageBox::Discard:
                    onRevertButtonPress();
                    break;

                case QMessageBox::Cancel:
                    m_editButton->click();
                    return;
                    break;

                default:
                    break;
            }
        }

        m_addPointButton->setDisabled(true);
        m_modAxisButton->setEnabled(false);
        m_loExtrapBox->setDisabled(true);
        m_hiExtrapBox->setDisabled(true);
        m_tableAddButton->setDisabled(true);
        m_tableDeleteButton->setDisabled(true);
        m_interpBox->setDisabled(true);
        m_plot->activateEditMode(false);
        m_saveButton->setDisabled(true);
        m_revertButton->setDisabled(true);
        m_probeButton->setEnabled(true);
        m_plot->deselectMarkers();
    }
}

void
GtControlScheduleEditor::onDataProbePress()
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    if (m_probeButton->isChecked())
    {
        m_plot->plotCanvas()->activateDataProbeState();
        m_tabWidget->setCurrentIndex(1);
    }
    else
    {
        m_plot->plotCanvas()->activateDataProbeState(false);
    }
}

void
GtControlScheduleEditor::onAddButtonPress()
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    if (m_addPointButton->isChecked())
    {
        m_plot->plotCanvas()->activateAddPointState();
        m_tabWidget->setCurrentIndex(0);
    }
    else
    {
        m_plot->plotCanvas()->activateAddPointState(false);
    }
}

void
GtControlScheduleEditor::onModAxisButtonPress()
{
    if (m_schedule == Q_NULLPTR)
    {
        return;
    }

    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    QDialog dialog;
    dialog.setWindowTitle("Edit schedule information...");
    QFormLayout formLay(&dialog);

    dialog.setWindowIcon(gtApp->icon("schedules3Icon.png"));

    QLineEdit* schedName = new QLineEdit(&dialog);
    schedName->setText(m_title->text());

    QLineEdit* schedDescr = new QLineEdit(&dialog);
    schedDescr->setText(m_title->toolTip());

    QLineEdit* xName = new QLineEdit(&dialog);
    xName->setText(m_plot->xTitle());

    QLineEdit* xUnit = new QLineEdit(&dialog);
    xUnit->setText(m_plot->xUnit());

    QLineEdit* xDescr = new QLineEdit(&dialog);
    xDescr->setText(m_plot->xDescription());

    QLineEdit* yName = new QLineEdit(&dialog);
    yName->setText(m_plot->yTitle());

    QLineEdit* yUnit = new QLineEdit(&dialog);
    yUnit->setText(m_plot->yUnit());

    QLineEdit* yDescr = new QLineEdit(&dialog);
    yDescr->setText(m_plot->yDescription());

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
        m_title->setText(schedName->text());
        m_title->setToolTip(schedDescr->text());
        m_plot->setXunit(xUnit->text());
        m_plot->setYunit(yUnit->text());
        m_plot->setXtitle(xName->text());
        m_plot->setYtitle(yName->text());
        m_plot->setXdescription(xDescr->text());
        m_plot->setYdescription(yDescr->text());
    }
}

void
GtControlScheduleEditor::onSaveButtonpress()
{
    if (m_schedule == Q_NULLPTR)
    {
        return;
    }

    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    GtObjectMemento orig = m_schedule->toMemento();
    GtObjectMemento fromPlot = constructTableFromPlot()->toMemento();

    GtObjectMementoDiff diff(orig, fromPlot);

    if (!diff.isNull())
    {
        clearDataProbes();

        GtCommand cmmd = gtApp->startCommand(
                    gtApp->currentProject(), "schedule data change");

        m_schedule->applyDiff(diff);

        gtApp->endCommand(cmmd);
    }

    m_plot->initPlotFromSchedule(m_schedule);
}

void
GtControlScheduleEditor::onRevertButtonPress()
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    if (m_schedule == Q_NULLPTR)
    {
        return;
    }

    setData(m_schedule);
}

void
GtControlScheduleEditor::onChangeEditMode()
{
    m_editButton->click();
}

void
GtControlScheduleEditor::onChangeAddPointMode()
{
    m_addPointButton->click();
}

void
GtControlScheduleEditor::onChangeDataProbeMode()
{
    m_probeButton->click();
}

void
GtControlScheduleEditor::onExtrapolationChanged()
{
    QComboBox* cBox = qobject_cast<QComboBox*>(sender());

    if (cBox == Q_NULLPTR)
    {
        return;
    }

    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    int nPoints = m_plot->nPoints();

    GtTableAxis::ExtrapMethod extrap = findExtrpMethod(cBox);

    if (extrap == GtTableAxis::E_LINEAR && nPoints < 2)
    {
        gtError() << "For linear extrapolation at least two points are "
                  "required!";
        gtError() << "Extrapolation method is set to its original value!";

        revertComboBox(cBox);

        return;
    }
    else if (extrap == GtTableAxis::E_LAGRANGE2 && nPoints < 3)
    {
        gtError() << "For Lagrange (2) extrapolation at least three points are "
                  "required!";
        gtError() << "Extrapolation method is set to its original value!";

        revertComboBox(cBox);
        return;
    }

    if (cBox == m_loExtrapBox)
    {
        m_oldLoExtrap = findExtrpMethod(cBox);
    }
    else if (cBox == m_hiExtrapBox)
    {
        m_oldHiExtrap = findExtrpMethod(cBox);
    }
}

void
GtControlScheduleEditor::onInterpolationChanged()
{
    QComboBox* cBox = qobject_cast<QComboBox*>(sender());

    if (cBox == Q_NULLPTR)
    {
        return;
    }

    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    int nPoints = m_plot->nPoints();

    GtTableAxis::InterpMethod interp = findInterpMethod(cBox);

    if (interp == GtTableAxis::I_LINEAR && nPoints < 2)
    {
        gtError() << "For linear interpolation at least two points are "
                  "required!";
        gtError() << "Interpolation method is set to its original value!";

        revertComboBox(cBox);

        return;
    }
    else if (interp == GtTableAxis::I_LAGRANGE2 && nPoints < 3)
    {
        gtError() << "For Lagrange (2) interpolation at least three points are "
                  "required!";
        gtError() << "Interpolation method is set to its original value!";

        revertComboBox(cBox);
        return;
    }
    else if (interp == GtTableAxis::I_LAGRANGE3 && nPoints < 4)
    {
        gtError() << "For Lagrange (3) interpolation at least four points are "
                  "required!";
        gtError() << "Interpolation method is set to its original value!";

        revertComboBox(cBox);
        return;
    }
    else if (interp == GtTableAxis::I_CATMULL && nPoints < 4)
    {
        gtError() << "For Catmull interpolation at least four points are "
                  "required!";
        gtError() << "Interpolation method is set to its original value!";

        revertComboBox(cBox);
        return;
    }
    else if (interp == GtTableAxis::I_LOGARITHMIC && nPoints < 2)
    {
        gtError() << "For Logarithmic interpolation at least two points are "
                  "required!";
        gtError() << "Interpolation method is set to its original value!";

        revertComboBox(cBox);
        return;
    }

    if (cBox == m_interpBox)
    {
        m_oldInterp = findInterpMethod(cBox);
    }
}

void
GtControlScheduleEditor::onRevertRequest()
{
    m_revertButton->click();
}

void
GtControlScheduleEditor::onSaveRequest()
{
    m_saveButton->click();
}

void
GtControlScheduleEditor::onDataProbeTaken(const QPointF point)
{
    if (m_probeTable == Q_NULLPTR)
    {
        return;
    }

    m_probeTable->setRowCount(m_probeTable->rowCount() + 1);

    QTableWidgetItem* item1 = new QTableWidgetItem(QString::number(point.x()));
    QTableWidgetItem* item2 = new QTableWidgetItem(QString::number(point.y()));

    item1->setFlags(item1->flags() & ~Qt::ItemIsEditable
                    & ~Qt::ItemIsSelectable);
    item2->setFlags(item2->flags() & ~Qt::ItemIsEditable
                    & ~Qt::ItemIsSelectable);

    m_probeTable->setItem(m_probeTable->rowCount() - 1, 0, item1);
    m_probeTable->setItem(m_probeTable->rowCount() - 1, 1, item2);

    m_probeTable->sortByColumn(0, Qt::AscendingOrder);

    if (!m_tableClearButton->isEnabled())
    {
        m_tableClearButton->setEnabled(true);
    }
}

GtTableAxis::ExtrapMethod
GtControlScheduleEditor::findExtrpMethod(const QComboBox* cBox)
{
    if (cBox == Q_NULLPTR)
    {
        return GtTableAxis::E_NONE;
    }

    switch (cBox->currentIndex())
    {
        case 0:
        {
            return GtTableAxis::E_NONE;
        }

        case 1:
        {
            return GtTableAxis::E_CONST;
        }

        case 2:
        {
            return GtTableAxis::E_LINEAR;
        }

        case 3:
        {
            return  GtTableAxis::E_LAGRANGE2;
        }

        default:
            break;
    }

    return GtTableAxis::E_LINEAR;
}

GtTableAxis::InterpMethod
GtControlScheduleEditor::findInterpMethod(const QComboBox* cBox)
{
    if (cBox == Q_NULLPTR)
    {
        return GtTableAxis::I_LINEAR;
    }

    switch (cBox->currentIndex())
    {
        case 0:
        {
            return GtTableAxis::I_LINEAR;
        }

        case 1:
        {
            return GtTableAxis::I_LAGRANGE2;
        }

        case 2:
        {
            return GtTableAxis::I_LAGRANGE3;
        }

        case 3:
        {
            return GtTableAxis::I_CATMULL;
        }

        case 4:
        {
            return GtTableAxis::I_LOGARITHMIC;
        }

        default:
            break;
    }

    return GtTableAxis::I_LINEAR;
}

void
GtControlScheduleEditor::revertComboBox(QComboBox* cBox)
{
    if (cBox == Q_NULLPTR)
    {
        return;
    }

    if (cBox == m_loExtrapBox)
    {
        initExtrpComboBox(m_loExtrapBox, m_oldLoExtrap);
    }
    else if (cBox == m_hiExtrapBox)
    {
        initExtrpComboBox(m_hiExtrapBox, m_oldHiExtrap);
    }
    else if (cBox == m_interpBox)
    {
        initInterpComboBox(m_interpBox, m_oldInterp);
    }
}

bool
GtControlScheduleEditor::isEqualToDatamodel()
{
    if (m_plot == Q_NULLPTR)
    {
        return false;
    }

    GtObjectMemento orig = m_schedule->toMemento();
    GtObjectMemento fromPlot = constructTableFromPlot()->toMemento();

    GtObjectMementoDiff diff(orig, fromPlot);

    if (diff.isNull())
    {
        return true;
    }

    return false;
}

GtControlSchedule*
GtControlScheduleEditor::constructTableFromPlot()
{
    if (m_schedule == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    if (m_plot == Q_NULLPTR)
    {
        return Q_NULLPTR;
    }

    GtControlSchedule* clone = new GtControlSchedule;
    clone->setUuid(m_schedule->uuid());

    clone->setObjectName(m_title->text());
    clone->setDescription(m_title->toolTip());

    GtTableAxis::ExtrapMethod loEM = findExtrpMethod(m_loExtrapBox);
    GtTableAxis::ExtrapMethod hiEM = findExtrpMethod(m_hiExtrapBox);
    GtTableAxis::InterpMethod inM = findInterpMethod(m_interpBox);

    QString axUnit = m_plot->xUnit();
    QString axDescr = m_plot->xDescription();

    QString valUnit = m_plot->yUnit();
    QString valDes = m_plot->yDescription();

    GtTableAxis* axis = new GtTableAxis;
    axis->setUuid(m_schedule->getAxesList().first()->uuid());
    axis->setObjectName(m_plot->xTitle());
    axis->setDescription(axDescr);
    axis->setUnit(axUnit);
    axis->setLoExtMethod(loEM);
    axis->setHiExtMethod(hiEM);
    axis->setInterMethod(inM);
    axis->setTicks(m_plot->xValues());

    GtTableValues* vals = new GtTableValues;

    vals->setUuid(m_schedule->getVals(
                      m_schedule->valueNames().first())->uuid());

    vals->setObjectName(m_plot->yTitle());
    vals->setDescription(valDes);
    vals->setUnit(valUnit);
    vals->setValues(m_plot->yValues());

    if (!clone->addAxis(axis))
    {
        return Q_NULLPTR;
    }

    if (!clone->addValues(vals))
    {
        return Q_NULLPTR;
    }

    return clone;
}

void
GtControlScheduleEditor::clearDataProbes()
{
    if (m_probeTable == Q_NULLPTR)
    {
        return;
    }

    m_probeTable->clearContents();
    m_probeTable->setRowCount(0);

    m_tableClearButton->setDisabled(true);
}

void
GtControlScheduleEditor::onTableAddButtonPressed()
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    QDialog dialog(m_plot->canvas());

    QFormLayout formLay(&dialog);

    dialog.setWindowIcon(gtApp->icon("operatingPoint.png"));

    formLay.addRow(new QLabel("Please enter the desired point corrdinates:"));

    QDoubleSpinBox* xCoord = new QDoubleSpinBox(&dialog);
    xCoord->setSingleStep(0.1);
    xCoord->setMaximum(1e12);
    xCoord->setMinimum(-1e12);
    xCoord->setValue(0.0);

    QDoubleSpinBox* yCoord = new QDoubleSpinBox(&dialog);
    yCoord->setSingleStep(0.1);
    yCoord->setMaximum(1e12);
    yCoord->setMinimum(-1e12);
    yCoord->setValue(0.0);

    formLay.addRow(m_plot->xTitle() + ":", xCoord);
    formLay.addRow(m_plot->yTitle() + ":", yCoord);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                               Qt::Horizontal, &dialog);
    formLay.addRow(&buttonBox);

    connect(&buttonBox, SIGNAL(accepted()), &dialog, SLOT(accept()));
    connect(&buttonBox, SIGNAL(rejected()), &dialog, SLOT(reject()));

    dialog.layout()->setSizeConstraint(QLayout::SetFixedSize);

    if (dialog.exec() == QDialog::Accepted)
    {
        m_plot->addPoint(xCoord->value(), yCoord->value());
    }
}

void
GtControlScheduleEditor::onTableDeleteButtonPressed()
{
    QItemSelectionModel* sModel = m_tableView->selectionModel();

    QModelIndexList selection = sModel->selectedRows();

    if (selection.size() == 0)
    {
        return;
    }
    else if (selection.size() == 1)
    {
        int rowIndex = selection.first().row();

        GtControlScheduleMarker* toDelete = m_plot->tmpMarkerList().at(rowIndex);

        if (toDelete == Q_NULLPTR)
        {
            return;
        }

        m_plot->deleteMarker(toDelete);

        if (rowIndex == 0)
        {
            if (m_plot->nPoints() != 0)
            {
                m_tableView->setCurrentIndex(m_model->index(0, 0));
            }
        }
        else if (m_plot->nPoints()-1 >= rowIndex)
        {
            m_tableView->setCurrentIndex(m_model->index(rowIndex, 0));
        }
        else
        {
            m_tableView->setCurrentIndex(m_model->index(m_plot->nPoints()-1, 0));
        }
    }
    else
    {
        QList<GtControlScheduleMarker*> markers = m_plot->tmpMarkerList();
        QList<GtControlScheduleMarker*> toDelete;
        int highestRow = 0;

        foreach(QModelIndex index, selection)
        {
            if (index.row() >= 0 && index.row() <= markers.size() - 1)
            {
                toDelete.append(markers.at(index.row()));

                if (index.row() > highestRow)
                {
                    highestRow = index.row();
                }
            }
        }

        foreach(GtControlScheduleMarker* marker, toDelete)
        {
            m_plot->deleteMarker(marker);
        }


        int markersRemain = m_plot->nPoints();
        if (highestRow >= markersRemain - 1)
        {
            m_tableView->setCurrentIndex(
                        m_model->index(m_plot->nPoints() - 1, 0));
        }
        else
        {
            m_tableView->setCurrentIndex(
                        m_model->index(highestRow, 0));
        }
    }
}

void
GtControlScheduleEditor::onTableViewSelectionChanged()
{
    QItemSelectionModel* sModel = m_tableView->selectionModel();

    if (sModel == Q_NULLPTR)
    {
        return;
    }

    QModelIndexList selection = sModel->selectedRows();

    if (selection.size() == 0 || selection.size() > 1)
    {
        if (m_plot->hasSelectedMarkers())
        {
            m_plot->deselectMarkers();
        }

        if (selection.size() == 0)
        {
            m_tableDeleteButton->setDisabled(true);
        }
        else
        {
            m_tableDeleteButton->setEnabled(true);
        }

        return;
    }

    int rowIndex = selection.first().row();

    GtControlScheduleMarker* toSelect = m_plot->tmpMarkerList().at(rowIndex);

    if (toSelect == Q_NULLPTR || toSelect->isSelected())
    {
        return;
    }

    if (m_plot->hasSelectedMarkers())
    {
        m_plot->deselectMarkers();
    }

    m_plot->selectMarker(toSelect);

    m_tableDeleteButton->setEnabled(true);
}

void
GtControlScheduleEditor::onMarkerSelected(GtControlScheduleMarker* marker)
{
    if (marker == Q_NULLPTR)
    {
        return;
    }

    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    if (!m_plot->tmpMarkerList().contains(marker))
    {
        return;
    }

    int rowIndex = m_plot->tmpMarkerList().indexOf(marker);

    if (m_tableView != Q_NULLPTR)
    {
        if (m_tableView->currentIndex().row() != rowIndex)
        {
            m_tableView->setCurrentIndex(m_model->index(rowIndex, 0));
            m_tableDeleteButton->setEnabled(true);
        }
    }
}

void
GtControlScheduleEditor::onMarkersDeselected()
{
    QItemSelectionModel* sModel = m_tableView->selectionModel();
    if (sModel != Q_NULLPTR)
    {
        QModelIndexList selection = sModel->selectedRows();
        if (!selection.isEmpty())
        {
            sModel->clearCurrentIndex();
            sModel->clearSelection();
        }
    }
}

void
GtControlScheduleEditor::onChangeAxisInformation()
{
    m_modAxisButton->click();
}

void
GtControlScheduleEditor::onLineEditCheckRequired()
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

void
GtControlScheduleEditor::onDataProbeAtButtonPress()
{
    if (m_plot == Q_NULLPTR)
    {
        return;
    }

    if (m_schedule == Q_NULLPTR)
    {
        return;
    }

    double xPosition = QInputDialog::getDouble(m_plot, tr("Data probe at:"),
                                               tr("Position"), 0.0,
                                               -1e08, 1e08, 4);

    double val = m_schedule->getValue1D(
                m_schedule->valueNames().first(), xPosition);

    onDataProbeTaken(QPointF(xPosition, val));
}

void
GtControlScheduleEditor::initExtrpComboBox(QComboBox* cBox,
        const GtTableAxis::ExtrapMethod em)
{
    if (cBox == Q_NULLPTR)
    {
        return;
    }

    switch (em)
    {
        case GtTableAxis::E_NONE:
        {
            cBox->setCurrentIndex(0);
            break;
        }

        case GtTableAxis::E_CONST:
        {
            cBox->setCurrentIndex(1);
            break;
        }

        case GtTableAxis::E_LINEAR:
        {
            cBox->setCurrentIndex(2);
            break;
        }

        case GtTableAxis::E_LAGRANGE2:
        {
            cBox->setCurrentIndex(3);
            break;
        }

        default:
            break;
    }
}

void
GtControlScheduleEditor::initInterpComboBox(QComboBox* cBox,
        const GtTableAxis::InterpMethod im)
{
    if (cBox == Q_NULLPTR)
    {
        return;
    }

    switch (im)
    {
        case GtTableAxis::I_LINEAR:
        {
            cBox->setCurrentIndex(0);
            break;
        }

        case GtTableAxis::I_LAGRANGE2:
        {
            cBox->setCurrentIndex(1);
            break;
        }

        case GtTableAxis::I_LAGRANGE3:
        {
            cBox->setCurrentIndex(2);
            break;
        }

        case GtTableAxis::I_CATMULL:
        {
            cBox->setCurrentIndex(3);
            break;
        }

        case GtTableAxis::I_LOGARITHMIC:
        {
            cBox->setCurrentIndex(4);
            break;
        }

        default:
            break;
    }
}
