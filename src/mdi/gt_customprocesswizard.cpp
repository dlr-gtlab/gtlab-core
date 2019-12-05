/* GTlab - Gas Turbine laboratory
 * Source File: gt_customprocesswizard.cpp
 * copyright 2009-2016 by DLR
 *
 *  Created on: 17.05.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#include <QDebug>

#include "gt_processwizardpage.h"
#include "gt_processwizard.h"

#include "gt_customprocesswizard.h"

GtCustomProcessWizard::GtCustomProcessWizard() :
    m_wizard(Q_NULLPTR)
{

}

bool
GtCustomProcessWizard::setPage(int id, const QMetaObject& metaPage)
{
    if (id < 1)
    {
        return false;
    }

    if (m_pages.contains(id))
    {
        return false;
    }

    QObject* obj = metaPage.newInstance();

    if (obj == Q_NULLPTR)
    {
        return false;
    }

    if (qobject_cast<GtProcessWizardPage*>(obj) == Q_NULLPTR)
    {
        delete obj;
        return false;
    }

    m_pages.insert(id, metaPage);

    delete obj;

    return true;
}

int
GtCustomProcessWizard::nextId() const
{
    return 0;
}

const QSize&
GtCustomProcessWizard::size() const
{
    return m_size;
}

void
GtCustomProcessWizard::setSize(const QSize& size)
{
    m_size = size;
}

void
GtCustomProcessWizard::setSize(int w, int h)
{
    setSize(QSize(w, h));
}

int
GtCustomProcessWizard::currentId() const
{
    return 0;
}

int
GtCustomProcessWizard::pageCount()
{
    return m_pages.count();
}

QList<QMetaObject>
GtCustomProcessWizard::pages()
{
    return m_pages.values();
}

QList<int>
GtCustomProcessWizard::pageIds() const
{
    return m_pages.keys();
}
