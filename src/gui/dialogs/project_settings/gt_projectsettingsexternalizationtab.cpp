/* GTlab - Gas Turbine laboratory
 * Source File: gt_projectsettingsexternalizationtab.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 31.03.2022
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#include "gt_projectsettingsexternalizationtab.h"

#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QMessageBox>

#include "gt_logging.h"
#include "gt_statehandler.h"
#include "gt_state.h"
#include "gt_project.h"
#include "gt_externalizedobject.h"
#include "gt_colors.h"
#include "gt_palette.h"
#include "gt_icons.h"

GtProjectSettingsExternalizationTab::GtProjectSettingsExternalizationTab(
        GtProject* project,
        QWidget* parent):
    GtProjectSettingsTab{project, parent}
{
    auto* infoLayout = new QHBoxLayout();
    auto* infoLabel = new QLabel{tr("Certain data objects may be externalized "
                                    "dynamically at runtime to improve memory "
                                    "usage and overall performance for data "
                                    "intensive projects. This feature must be "
                                    "enabled on project basis.")};

    QSize size{16, 16};
    auto* infoIcon = new QLabel();
    infoIcon->setPixmap(GtGUI::Icon::infoBlue16().pixmap(size));
    infoIcon->setFixedSize(size);

    infoLayout->addWidget(infoIcon);
    infoLayout->addSpacing(4);
    infoLayout->addWidget(infoLabel);

    auto* warningLabel = new QLabel{tr("This project contains unknow data "
                                       "objects. Disabling the externalization "
                                       "may result in data loss!")};

    auto palette = GtGUI::currentTheme();
    palette.setColor(warningLabel->foregroundRole(),
                     GtGUI::Color::warningText());

    infoLabel->setWordWrap(true);
    warningLabel->setWordWrap(true);
    warningLabel->setPalette(palette);

    if (!project->hasDummyChildren())
    {
        warningLabel->hide();
    }

    m_extCheckBox = new QCheckBox{tr("Enable the externalization of "
                                     "data objects for this project")};

    // retrieve externalization setting and set checkbox
    m_extState = gtStateHandler->initializeState(project,
                                    QStringLiteral("ExternalizationSettings"),
                                    QStringLiteral("Enable Externalization"),
                                    project->objectPath(),
                                    true, project, false);

    m_extCheckBox->setChecked(m_extState->getValue().toBool());

    const auto externalObjs = project->findChildren<GtExternalizedObject*>();

    // sum of fetched objects
    auto fetchedObjs = std::accumulate(std::begin(externalObjs),
                                       std::end(externalObjs),
                                       int{0}, [](int sum, auto* obj){
        return sum + obj->isFetched();
    });

    auto* objectCountLabel = new QLabel{tr("Currently fetched objects: ") +
                                        QString::number(fetchedObjs) +
                                        tr(" out of ") +
                                        QString::number(externalObjs.size())};

    auto* line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    layout()->addLayout(infoLayout);
    layout()->addWidget(line);
    layout()->addWidget(m_extCheckBox);
    layout()->addWidget(objectCountLabel);
    layout()->addWidget(warningLabel);
    layout()->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum,
                                      QSizePolicy::Expanding));
}

void
GtProjectSettingsExternalizationTab::saveSettings()
{
    bool checked = m_extCheckBox->isChecked();
    // check if setting changed
    if (m_extState->getValue().toBool() == checked)
    {
        return;
    }

    // externalization enabled
    if (checked)
    {                
        // update state
        m_extState->setValue(true);
        project()->setInternalizeOnSave(false);
        return;
    }

    // externalization disabled -> internalize all data
    auto btn = QMessageBox::warning(nullptr,
                                    tr("Project must be saved and closed"),
                                    tr("The project must be saved and "
                                       "closed to internalize all data."
                                       "\nThis may take some time."),
                                    QMessageBox::Ok | QMessageBox::Abort,
                                    QMessageBox::Ok);

    if (btn == QMessageBox::Abort)
    {
        return;
    }

    // update state
    m_extState->setValue(false);
    project()->setInternalizeOnSave(true);

    emit projectSaveRequested();
    emit projectCloseRequested();
}

QString
GtProjectSettingsExternalizationTab::tabName() const
{
    return tr("Externalization");
}
