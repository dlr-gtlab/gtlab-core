/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QLineEdit>
#include <QToolTip>
#include <QDebug>

#include "gt_sessionlistwidget.h"
#include "gt_application.h"
#include "gt_deleteitemmessagebox.h"

GtSessionListWidget::GtSessionListWidget(bool initialize, QWidget* parent) :
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

void
GtSessionListWidget::init()
{
    clear();

    addItems(gtApp->sessionIds());

    for (int i = 0; i < count() ; i++)
    {
        QListWidgetItem* lwi = item(i);

        bool editable = true;

        if (lwi->text() == gtApp->sessionId())
        {
            QFont font = lwi->font();
            font.setBold(true);
            lwi->setFont(font);
            setCurrentItem(lwi);
            editable = false;
        }
        if (lwi->text() == "default")
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
GtSessionListWidget::renameItem()
{
    QListWidgetItem* item = currentItem();

    if (!item)
    {
        return;
    }

    if (item->text() == "default" || item->text() == gtApp->sessionId())
    {
        return;
    }

    lastIdChanged(item->text());

    editItem(item);
}

void
GtSessionListWidget::newItem()
{
    QString id = generateId("New Session", gtApp->sessionIds());

    if (addSessionItem(id))
    {
        int newIdx = gtApp->sessionIds().indexOf(id);

        if (newIdx > 0 && newIdx < gtApp->sessionIds().size())
        {
             QListWidgetItem* newItem = item(newIdx);

             lastIdChanged(newItem->text());

             setCurrentItem(newItem);
             editItem(newItem);
        }
    }
}

void
GtSessionListWidget::duplicateItem()
{
    QListWidgetItem* curItem = currentItem();

    if (!curItem)
    {
        return;
    }

    QString id = generateId(curItem->text(), gtApp->sessionIds());

    if (duplicateSessionItem(curItem->text(), id))
    {
        int newIdx = gtApp->sessionIds().indexOf(id);

        if (newIdx > 0 && newIdx < gtApp->sessionIds().size())
        {
             QListWidgetItem* newItem = item(newIdx);

             lastIdChanged(newItem->text());

             setCurrentItem(newItem);
             editItem(newItem);
        }
    }
}

void
GtSessionListWidget::deleteItem()
{
    QListWidgetItem* item = currentItem();

    if (!item)
    {
        return;
    }

    if (item->text() == "default" || item->text() == gtApp->sessionId())
    {
        return;
    }

    GtDeleteItemMessageBox mb(tr("Delete Session"), item->text());

    int ret = mb.exec();
    switch (ret)
    {
    case QMessageBox::Yes:
    {
        if (gtApp->deleteSession(item->text()))
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

// TODO: has to be moved to utilities
QString
GtSessionListWidget::generateId(const QString& name, const QStringList& names,
                                  QString initName, int iteration)
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
GtSessionListWidget::addSessionItem(const QString& id)
{
    if (!gtApp->newSession(id))
    {
        return false;
    }

    init();

    return true;
}

bool
GtSessionListWidget::duplicateSessionItem(const QString& source,
                                          const QString& target)
{
    if (!gtApp->duplicateSession(source, target))
    {
        return false;
    }

    init();

    return true;
}

void
GtSessionListWidget::onLstItemsCommitData(QWidget* pLineEdit)
{
    QString strNewText = reinterpret_cast<QLineEdit*>(pLineEdit)->text();

//    qDebug() << "strNewText = " << strNewText;

    if (gtApp->sessionIds().contains(strNewText))
    {
        // TODO: user warning
        qDebug() << "duplicate!";
        item(currentRow())->setText(m_lastId);
//        qDebug() << "show tool tip";
//        QToolTip::showText(mapToGlobal(QPoint(0, 0)),
//                           tr("Session id already in use!"));
    }
    else
    {
        // rename session
        if (!gtApp->renameSession(m_lastId, strNewText))
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
GtSessionListWidget::lastIdChanged(const QString& val)
{
//    qDebug() << "GtSessionListWidget::lastIdChanged = " << val;
    m_lastId = val;
}

