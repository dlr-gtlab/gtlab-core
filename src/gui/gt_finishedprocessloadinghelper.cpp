
#include <QFile>
#include <QDir>

#include "gt_objectmementodiff.h"
#include "gt_application.h"
#include "gt_finishedprocessloadinghelper.h"

GtFinishedProcessLoadingHelper::GtFinishedProcessLoadingHelper(
        QList<GtObjectMemento>* data,
        QPointer<GtObject> source,
        const QString& taskName) :
    m_data(data),
    m_sumDiff(new GtObjectMementoDiff),
    m_source(source),
    m_taskName(taskName)
{

}

GtFinishedProcessLoadingHelper::~GtFinishedProcessLoadingHelper()
{
    delete m_sumDiff;
}

void
GtFinishedProcessLoadingHelper::run()
{
    for (int i = 0; i < m_data->size(); ++i)
    {
        GtObjectMemento memento = m_data->at(i);

        GtObject* target = m_source->getObjectByUuid(memento.uuid());

        if (target)
        {
            GtObjectMemento old = target->toMemento(true);
            GtObjectMementoDiff diff(old, memento);

            m_sumDiff->operator <<(diff);
        }
    }
}

GtObjectMementoDiff*
GtFinishedProcessLoadingHelper::sumDiff() const
{
    return m_sumDiff;
}
