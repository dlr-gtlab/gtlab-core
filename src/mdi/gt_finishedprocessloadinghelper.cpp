
#include <QFile>
#include <QDir>

#include "gt_objectmementodiff.h"
#include "gt_application.h"
#include "gt_finishedprocessloadinghelper.h"

GtFinishedProcessLoadingHelper::GtFinishedProcessLoadingHelper(
        QList<GtObjectMemento>* data,
        QPointer<GtObject> source,
        QString taskName) :
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
    QDir tempDir;

    if (gtApp->devMode())
    {
        tempDir = gtApp->applicationTempDir();
    }

    for (int i = 0; i < m_data->size(); ++i)
    {
        GtObjectMemento memento = m_data->at(i);

        GtObject* target = m_source->getObjectByUuid(memento.uuid());

        if (target != Q_NULLPTR)
        {
            GtObjectMemento old = target->toMemento(true);
            GtObjectMementoDiff diff(old, memento);

            //if (!tempDir.absolutePath().isEmpty() && gtApp->devMode())
            //{
            //    QString filename = target->objectName() +
            //                       QStringLiteral(".xml");
            //
            //    QFile file(tempDir.absoluteFilePath(filename));
            //
            //    if (file.open(QFile::WriteOnly))
            //    {
            //        QTextStream TextStream(&file);
            //        TextStream << diff.toByteArray();
            //        TextStream << "######### new";
            //        TextStream << memento.toByteArray();
            //        TextStream << "######### old";
            //        TextStream << old.toByteArray();
            //        file.close();
            //    }
            //}

            m_sumDiff->operator <<(diff);
        }
    }
}

GtObjectMementoDiff*
GtFinishedProcessLoadingHelper::sumDiff() const
{
    return m_sumDiff;
}
