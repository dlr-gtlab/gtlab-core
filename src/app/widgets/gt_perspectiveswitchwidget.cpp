/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 30.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QVBoxLayout>
#include <QListWidget>
#include <QKeyEvent>

#include "gt_perspectiveswitchwidget.h"
#include "gt_application.h"
#include "gt_icons.h"

GtPerspectiveSwitchWidget::GtPerspectiveSwitchWidget(QWidget* parent) :
    GtDialog(parent)
{
    QVBoxLayout* lay = new QVBoxLayout;
    m_list = new QListWidget;
    m_list->setFrameStyle(QListWidget::NoFrame);

    lay->addWidget(m_list);
    setLayout(lay);

    init();
}

void
GtPerspectiveSwitchWidget::keyPressEvent(QKeyEvent* event)
{
    if (gtApp->compareKeyEvent(event, "switchPerspective"))
    {
        nextPerspective();
    }

    GtDialog::keyPressEvent(event);
}

void
GtPerspectiveSwitchWidget::keyReleaseEvent(QKeyEvent* event)
{
    /// Currently there is no way to check only the modifier of a short cut
    /// The short cut for switch perspective is set as readOnly
    if (event->key() == Qt::Key_Alt)
    {
        accept();
    }

    GtDialog::keyReleaseEvent(event);
}

void
GtPerspectiveSwitchWidget::init()
{
    // populate list
//    m_list->addItems(gtApp->perspectiveIds());
    QStringList ids = gtApp->perspectiveIds();

    for (int i = 0; i < ids.size(); i++)
    {
        QListWidgetItem* item = new QListWidgetItem(ids[i], m_list);
        QFont font = item->font();
        font.setPointSize(12);
        item->setFont(font);
        item->setIcon(gt::gui::icon::perspectives16());
        m_list->addItem(item);
        item->setSizeHint(QSize(m_list->sizeHint().width(), 30));
        if (ids[i] == gtApp->perspectiveId())
        {
            m_list->setCurrentItem(item);
        }
    }
}

void
GtPerspectiveSwitchWidget::nextPerspective()
{
    if (m_list->count() <= 1)
    {
        return;
    }

    QStringList ids = gtApp->perspectiveIds();

    const int current = m_list->currentRow();
    int next = 0;

    if (current < (m_list->count() - 1))
    {
        next = current + 1;
    }

    m_list->setCurrentRow(next);
    gtApp->switchPerspective(ids[next]);
}

