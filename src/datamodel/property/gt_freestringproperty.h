/* GTlab - Gas Turbine laboratory
 * Source File: gt_freestringproperty.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 20.11.2018
 *  Author: Björn Schneider (AT-TWK)
 *  Tel.: +49 2203 601 2642
 */

#ifndef GT_FREESTRINGPROPERTY_H
#define GT_FREESTRINGPROPERTY_H

#include "gt_abstractproperty.h"

/**
 * @brief The GtFreeStringProperty class:: a string property, which allows
 * no umlauts
 */
class GT_DATAMODEL_EXPORT GtFreeStringProperty:public GtAbstractProperty
{
    Q_OBJECT

    Q_PROPERTY(QString get READ getVal WRITE setVal)

public:

    /**
     * @brief GtFreeStringProperty
     * @param ident
     * @param name
     */
    Q_INVOKABLE GtFreeStringProperty(const QString& ident,
                                     const QString& name);

    /**
     * @brief GtFreeStringProperty
     * @param ident
     * @param name
     * @param brief
     * @param value
     */
    GtFreeStringProperty(const QString& ident,
                         const QString& name,
                         const QString& brief,
                         const QString& value = QString());

    /**
     * @brief valueToVariant
     * @param unit
     * @param success
     * @return value as a QVariant
     */
    virtual QVariant valueToVariant(const QString& unit,
                                    bool* success = 0) const Q_DECL_OVERRIDE;

    /**
     * @brief setValueFromVariant
     * @param value
     * @param unit
     * @param success
     * @return bool
     */
    virtual bool setValueFromVariant(const QVariant& val,
                                     const QString& unit,
                                     bool* success = 0) Q_DECL_OVERRIDE;

    /// destructor
    ~GtFreeStringProperty();

    /**
     * @brief get value (QString)
     * @return value (QString)
     */
    QString getVal() const;

    /**
     * @brief set value (QString)
     * @param value (QString)
     */
    void setVal(const QString& val);

private:
    /// Value
    QString m_value;

    /// Initial value
    QString m_initValue;
};

#endif // GT_FREESTRINGPROPERTY_H
