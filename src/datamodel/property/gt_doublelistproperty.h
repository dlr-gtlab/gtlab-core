/* GTlab - Gas Turbine laboratory
 * Source File: gt_doublelistproperty.h
 * copyright 2009-2018 by DLR
 *
 *  Created on: 23.01.2018
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_DOUBLELISTPROPERTY_H
#define GT_DOUBLELISTPROPERTY_H

#include "gt_datamodel_exports.h"

#include "gt_property.h"

/**
 * @brief The GtDoubleListProperty class
 */
class GT_DATAMODEL_EXPORT GtDoubleListProperty :
    public GtProperty<QVector<double> >
{
    Q_OBJECT

public:
    /**
     * @brief Constructor.
     * @param ident Property identification string.
     * @param name Property name visible within property browser.
     */
    GtDoubleListProperty(const QString& ident,
                         const QString& name);

    /**
     * @brief valueToVariant
     * @return
     */
    QVariant valueToVariant(const QString& unit,
                            bool* success = 0) const Q_DECL_OVERRIDE;

    /**
     * @brief setValueFromVariant
     * @param val
     * @return
     */
    bool setValueFromVariant(const QVariant& val,
                             const QString& unit,
                             bool* success = 0) Q_DECL_OVERRIDE;

    /**
     * @brief Returns property values in form of a string.
     * @return Value string
     */
    QString valuesToString() const;

    /**
     * @brief Sets property based on given string.
     * @param val New value in form of a string.
     * @return True if values could be set from given string. Otherwise false
     * is returned.
     */
    bool setValueFromString(const QString& val);

};

#endif // GT_DOUBLELISTPROPERTY_H
