/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QLineEdit>
#include <QDebug>

#include "gt_perspectiveslistwidget.h"
#include "gt_application.h"
#include "gt_deleteitemmessagebox.h"

GtPerspectivesListWidget::GtPerspectivesListWidget(bool initialize,
                                                   QWidget* parent) :
    QListWidget(parent)
{
    if (initialize)
    {
        init();
    }

    connect(this, SIGNAL(currentTextChanged(QString)),
            SLOT(lastIdChanged(QString)));
    connect(itemDelegate(), SIGNAL(commitData(QWidget*)),
            SLOT(onLstItemsCommitData(QWidget*)));
}

void GtPerspectivesListWidget::init()
{
    clear();
    addItems(gtApp->perspectiveIds());

    for (int i = 0; i < count() ; i++)
    {
        QListWidgetItem* lwi = item(i);

        bool editable = true;

        if (lwi->text() == gtApp->perspectiveId())
        {
            QFont font = lwi->font();
            font.setBold(true);
            lwi->setFont(font);
            setCurrentItem(lwi);
            editable = false;
        }
        if (lwi->text() == QLatin1String("default"))
        {
            QFont font = lwi->font();
            font.setItalic(true);
            lwi->setFont(font);
            editable = false;
        }
        if (editable)
        {
            lwi->setFlags (lwi->flags () | Qt::ItemIsEditable);
        }
    }

    emit itemSelectionChanged();
}

void
GtPerspectivesListWidget::renameItem()
{
    QListWidgetItem* item = currentItem();

    if (item == NULL)
    {
        return;
    }

    if (item->text() == QLatin1String("default") ||
            item->text() == gtApp->perspectiveId())
    {
        return;
    }

    lastIdChanged(item->text());

    editItem(item);
}

void
GtPerspectivesListWidget::newItem()
{
    QString id = generateId(QStringLiteral("New Perspective"),
                            gtApp->perspectiveIds());

    addDataItem(id);
}

void
GtPerspectivesListWidget::duplicateItem()
{
    QListWidgetItem* item = currentItem();

    if (item == NULL)
    {
        return;
    }

    QString id = generateId(item->text(), gtApp->perspectiveIds());

    duplicateDataItem(item->text(), id);
}

void
GtPerspectivesListWidget::deleteItem()
{
    QListWidgetItem* item = currentItem();

    if (item == NULL)
    {
        return;
    }

    if (item->text() == QLatin1String("default") ||
            item->text() == gtApp->perspectiveId())
    {
        return;
    }

    GtDeleteItemMessageBox mb(tr("Delete Perspective"), item->text());

    int ret = mb.exec();
    switch (ret)
    {
    case QMessageBox::Yes:
    {
        if (gtApp->deletePerspective(item->text()))
        {
            delete takeItem(currentRow());
        }

        break;
    }
    case QMessageBox::Cancel:
    {
        break;
    }
    default:
        break;
    }
}

QString
GtPerspectivesListWidget::generateId(const QString& name,
                                     const QStringList& names,
                                     QString initName,
                                     int iteration)
{
    if (!names.contains(name))
    {
        return name;
    }

    if (initName.isEmpty())
    {
        initName = name;
    }

    iteration++;
    QString new_name = initName + QStringLiteral("[")
                       + QString::number(iteration + 1) +
                       QStringLiteral("]");

    return generateId(new_name, names, initName, iteration);
}

bool
GtPerspectivesListWidget::addDataItem(const QString id)
{
    if (!gtApp->newPerspective(id))
    {
        return false;
    }

    init();

    return true;
}

bool
GtPerspectivesListWidget::duplicateDataItem(const QString source,
                                            const QString target)
{
    if (!gtApp->duplicatePerspective(source, target))
    {
        return false;
    }

    init();

    return true;
}

void
GtPerspectivesListWidget::onLstItemsCommitData(QWidget* pLineEdit)
{
    QString strNewText = reinterpret_cast<QLineEdit*>(pLineEdit)->text();

    if (gtApp->perspectiveIds().contains(strNewText))
    {
        // TODO: user warning
        qDebug() << "duplicate!";
        item(currentRow())->setText(m_lastId);
    }
    else
    {
        // rename perspective
        if (!gtApp->renamePerspective(m_lastId, strNewText))
        {
            // TODO: user warning
        }
        else
        {
            lastIdChanged(strNewText);
        }
    }
}

void
GtPerspectivesListWidget::lastIdChanged(const QString& val)
{
    m_lastId = val;
}

