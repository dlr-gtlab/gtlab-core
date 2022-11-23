#ifndef GTPOSTTEMPLATEFACTORY_H
#define GTPOSTTEMPLATEFACTORY_H

#include "gt_abstractobjectfactory.h"
#include "qobject.h"

#define gtPostTemplateFactory (GtPostTemplateFactory::instance())

class GtPostTemplateFactory : public QObject, public GtAbstractObjectFactory
{
public:
    /**
     * @brief instance
     * @return
     */
    static GtPostTemplateFactory* instance();

    /**
     * @brief registerPlot
     * @param metaObject
     */
    void registerPlots(const QList<QMetaObject>& metaObjects);

    /**
     * @brief knownPlots
     * @return
     */
    const QStringList& knownPlots();

private:
    ///
    QStringList m_plotIds;

    /**
     * @brief GtPostTemplateFactory
     */
    explicit GtPostTemplateFactory(QObject* parent = nullptr);
};

#endif // GTPOSTTEMPLATEFACTORY_H
