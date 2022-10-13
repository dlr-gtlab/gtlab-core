/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 29.07.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTIO_H
#define GTOBJECTIO_H

#include <QObject>
#include <QPointF>
#include <QSet>

#include "gt_objectmemento.h"

class QDomElement;
class QDomDocument;
class GtObject;
class GtAbstractObjectFactory;
class GtObjectMementoDiff;
class GtAbstractProperty;

/**
 * @brief The GtObjectIO class
 */
class GtObjectIO
{
public:
    static const QString S_OBJECT_TAG;
    static const QString S_OBJECTLIST_TAG;
    static const QString S_UUID_TAG;
    static const QString S_CLASS_TAG;
    static const QString S_NAME_TAG;
    static const QString S_ENTRY_NAME_TAG;
    static const QString S_TYPE_TAG;
    static const QString S_ID_TAG;
    static const QString S_VALUE_TAG;
    static const QString S_OPTIONAL_TAG;
    static const QString S_ACTIVE_TAG;
    static const QString S_PROPERTY_TAG;
    static const QString S_PROPERTYLIST_TAG;
    static const QString S_PROPERTYCONT_TAG;
    static const QString S_DIFF_INDEX_TAG;
    static const QString S_DIFF_INDEX_CHANGED_TAG;
    static const QString S_ENTRY_TAG;
    static const QString S_DIFF_OBJ_REMOVE_TAG;
    static const QString S_DIFF_OBJ_ADD_TAG;
    static const QString S_DIFF_PROP_CHANGE_TAG;
    static const QString S_DIFF_PROPLIST_CHANGE_TAG;
    static const QString S_DIFF_PROPCONT_ENTRY_ADDED_TAG;
    static const QString S_DIFF_PROPCONT_ENTRY_REMOVE_TAG;
    static const QString S_DIFF_PROPCONT_ENTRY_CHANGE_TAG;
    static const QString S_DIFF_ATTR_CHANGE_TAG;
    static const QString S_DIFF_ATTR_REMOVE_TAG;
    static const QString S_DIFF_NEWVAL_TAG;
    static const QString S_DIFF_OLDVAL_TAG;
    static const QSet<QString> S_LISTTYPES;

    /**
     * @brief GtObjectIO
     * @param factory
     */
    explicit GtObjectIO(GtAbstractObjectFactory* factory = nullptr);

    /** Sets new object factory.
        @param factory GtdAbstractObjectFactory */
    void setFactory(GtAbstractObjectFactory* factory);

    /** Creates Memento from given GtObject.
        @param o GtObject pointer
        @param clone Wether identiy information should be cloned or not
        @return GtObjectMemento memento */
    GtObjectMemento toMemento(const GtObject* o, bool clone = true);

    /** Creates QDomElement from given GtObjectMemento.
        @param m GtObjectMemento
        @param doc Reference to QDomDocument to generate new QDomElements
        @param skipChildren Wether only the properties of this GtObject should be stored or also all child-GtObjects
        @return QDomElement memento */
    QDomElement toDomElement(const GtObjectMemento& m,
                             QDomDocument& doc,
                             bool skipChildren = false);

    /**
     * Converts property memento data element into a dom element
     * 
     * @param m A memento property
     * @param doc[in/out] A document to create elements from
     */
    QDomElement toDomElement(const GtObjectMemento::PropertyData& m,
                             QDomDocument& doc);

    /** Creates Memento from given QDomElement.
        @param e QDomElement with memento data
        @return GtObjectMemento memento */
    GtObjectMemento toMemento(const QDomElement& e);


    /**
     * @brief applyDiff
     * @param diff
     * @param obj
     * @return
     */
    static bool applyDiff(GtObjectMementoDiff& diff, GtObject* obj);

    /**
     * @brief revertDiff
     * @param diff
     * @param obj
     * @return
     */
    static bool revertDiff(GtObjectMementoDiff& diff, GtObject* obj);

    /**
     *  Converts given QVariant to QString.
     *  @param var QVariant
     *  @return QString
     */
    static inline QString variantToString(const QVariant& var)
    {
        if (var.type() == QVariant::PointF)
        {
            QPointF val = var.toPointF();
            return QString::number(val.x()) + QStringLiteral("_")
                   + QString::number(val.y());
        }
        else
        {
            return var.toString();
        }
    }

    /**
     * @brief propertyListString
     * @param var
     * @return
     */
    static void propertyListStringType(const QVariant& var,
                                       QString& valStr,
                                       QString& typeStr);

    /**
     * @brief detect wether a QVariant data element would
     *        be stored as propertylist instead of property element.
     */
    static inline bool usePropertyList(const QVariant& var)
    {
        return S_LISTTYPES.contains(var.typeName());
    }
private:
    /// Pointer to current object factory
    GtAbstractObjectFactory* m_factory;

    /**
     * @brief writeProperties
     * @param m memento
     * @param obj
     */
    void writeProperties(GtObjectMemento& m,
                         const GtObject* obj);


    GtObjectMemento::PropertyData
    toDynamicPropertyData(const GtPropertyStructContainer& vec) const;

    /**
     * @brief writeProperties
     * @param root
     * @param obj
     * @param m
     */
    void writeProperties(QDomDocument& doc,
                         QDomElement& root,
                         const GtObjectMemento& m);


    QDomElement dynamicSizePropToDomElement(
        const GtObjectMemento::PropertyData& property,
        QDomDocument& doc);

    /**
     * @brief readProperties
     * @param element
     */
    QVector<GtObjectMemento::PropertyData>  readProperties(
                        const QDomElement& element) const;

    /**
     * @brief readStructProperties
     * @param element
     * @return
     */
    QVector<GtObjectMemento::PropertyData> readPropertyContainers(
            const QDomElement& element) const;

    /**
     * @brief writePropertyHelper
     */
    void writePropertyHelper(QVector<GtObjectMemento::PropertyData>& pVec,
            QSet<QString>& stored, const GtAbstractProperty *property) const;


    /** Creates an QDomElement of a given property.
        @param name Name of property
        @param var QVariant of property
        @param dataType Data type specifier
        @return doc Referened to QDomDocument to generate new QDomElements */
    QDomElement propertyToDomElement(const QString& name,
                                     const QVariant& var,
                                     const QString &dataType,
                                     QDomDocument& doc);

    QDomElement enumerationToDomElement(const QString& name,
                                        const QString& typeName,
                                        const QVariant& var,
                                        QDomDocument& doc);

    /** Creates an QDomElement of a given property list.
        @param name Name of property list
        @param var QVariantList with all properties
        @return doc Referened to QDomDocument to generate new QDomElements */
    QDomElement propertyListToDomElement(const QString& name,
                                         const QVariant& var,
                                         QDomDocument& doc);


    /**
     * @brief handlePropertyNodeChange
     * @param target
     * @param change
     * @param revert
     * @return
     */
    static bool handlePropertyNodeChange(GtObject *target,
                                         const QDomElement& change,
                                         const bool list = false,
                                         const bool revert = false);

    /**
     * @brief handlePropertyNodeChange
     * @param target
     * @param change
     * @param revert
     * @return
     */
    static bool handleAttributeNodeChange(GtObject *target,
                                         const QDomElement& change,
                                         const bool revert = false);

    /**
     * @brief handleObjectAdd
     * @param target
     * @param objectToAdd
     * @return
     */
    static bool handleObjectAdd(GtObject* parent,
                                const QDomElement& objectToAdd,
                                const QString& index);

    /**
     * @brief handleObjectRemove
     * @param parent
     * @param objectToAdd
     * @return
     */
    static bool handleObjectRemove(GtObject* parent,
                                   const QDomElement& objectToRemove,
                                   const QString&);

    /**
     * @brief handleIndexChanged
     * @param parent
     * @param object
     * @param oldIndex
     * @param newIndex
     * @return
     */
    static bool handleIndexChange(GtObject* parent,
                                  const QDomElement& object,
                                  const int newIndex);


    /** writes all members of a QList/QVector to a string (with ';' delimiter)
     */
    template<class T>
    static inline QString listToString(const T& t)
    {
        QString str = std::accumulate(std::begin(t), std::end(t), QString(""),
                        [](const QString& str, const typename T::value_type& m) {
            return str + m + ";";
        });
        str.truncate(str.size()-1);

        return str;
    }

};

#endif // GTOBJECTIO_H
