#include "gt_stringcontainer.h"

GtStringContainer::GtStringContainer()
{

}

GtStringContainer::GtStringContainer(QString str)
{
    m_entry = str;
}

QString
GtStringContainer::entry() const
{
    return m_entry;
}

void
GtStringContainer::setEntry(const QString& entry)
{
    m_entry = entry;
}
