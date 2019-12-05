#ifndef GTNOZZLEMAPPROPERTY_H
#define GTNOZZLEMAPPROPERTY_H

#include "gt_datamodel_exports.h"

#include <QPointer>

#include "gt_property.h"
#include "gt_object.h"

class GtNozzleMap;

/**
 * @brief The GtNozzleMapProperty class
 */
class GT_DATAMODEL_EXPORT GtNozzleMapProperty : public GtProperty<QString>
{
    Q_OBJECT

public:    /**
     * @brief GtCompressorMapProperty
     * @param name
     * @param brief
     * @param obj
     */
    GtNozzleMapProperty(const QString& name,
                        const QString& brief,
                        const QString& id,
                        GtObject* obj);
    /**
     * @brief turboMap
     * @return
     */
    GtNozzleMap* nozzleMap();

    /**
     * @brief object
     * @return
     */
    GtObject* object();

    /**
     * @brief valueToVariant
     * @return
     */
    virtual QVariant valueToVariant(const QString& unit,
                                    bool* success = 0) const Q_DECL_OVERRIDE;

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    virtual bool setValueFromVariant(const QVariant& val,
                                     const QString& unit,
                                     bool* success = 0) Q_DECL_OVERRIDE;

protected:
    ///
    QPointer<GtObject> m_obj;

};

#endif // GTNOZZLEMAPPROPERTY_H
