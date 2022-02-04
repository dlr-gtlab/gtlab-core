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
class GtDynamicPropertyContainer;
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
    static const QString S_TYPE_TAG;
    static const QString S_ID_TAG;
    static const QString S_VALUE_TAG;
    static const QString S_OPTIONAL_TAG;
    static const QString S_ACTIVE_TAG;
    static const QString S_PROPERTY_TAG;
    static const QString S_PROPERTYLIST_TAG;
    static const QString S_DIFF_INDEX_TAG;
    static const QString S_DIFF_INDEX_CHANGED_TAG;
    static const QString S_DYNAMICPROPERTY_TAG;
    static const QString S_DYNAMICPROPERTIES_TAG;
    static const QString S_ENTRY_TAG;
    static const QString S_DIFF_OBJ_REMOVE_TAG;
    static const QString S_DIFF_OBJ_ADD_TAG;
    static const QString S_DIFF_PROP_CHANGE_TAG;
    static const QString S_DIFF_PROPLIST_CHANGE_TAG;
    static const QString S_DIFF_DYNPROP_ADD_TAG;
    static const QString S_DIFF_DYNPROP_REM_TAG;
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
    GtObjectMemento::MementoData toMemento(const GtObject* o,
                                           bool clone = true);

    /** Creates QDomElement from given GtObjectMemento.
        @param m GtObjectMemento
        @param doc Reference to QDomDocument to generate new QDomElements
        @param skipChildren Wether only the properties of this GtObject should be stored or also all child-GtObjects
        @return QDomElement memento */
    QDomElement toDomElement(const GtObjectMemento& m,
                             QDomDocument& doc,
                             bool skipChildren = false);

    /** Creates Memento from given QDomElement.
        @param e QDomElement with memento data
        @return GtObjectMemento memento */
    GtObjectMemento::MementoData toMemento(const QDomElement& e);

    /** Creates new GtObjects from given Memento.
        @param m GtObjectMemento memento
        @return New GtObject based on GtObjectMemento memento */
    GtObject* toObject(const GtObjectMemento& m,
                       GtObject* handleObjectAdd = NULL);

    /** Merges GtObjects information from given QDomElement.
        @param element QDomElement memento
        @param obj GtObject */
    void mergeObject(const QDomElement& element,
                     GtObject* obj);

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
     * @brief writeDynamicPropertyHelper
     * @param doc
     * @param root
     * @param property
     */
    void writeDynamicPropertyHelper(QDomDocument& doc,
                                    QDomElement& root,
                                    const GtObjectMemento::MementoData::PropertyData& property);

    /** Converts given QVariant to QString.
        @param var QVariant
        @return QString */
    static inline QString variantToString(const QVariant& var)
    {
        if (var.type() == QMetaType::QPointF)
        {
            QPointF val = var.toPointF();
            return QString::number(val.x()) + QStringLiteral("_") + QString::number(val.y());
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
     * @brief detect wether a QVariant data element would be stored as propertylist instead of property element.
     */
    static inline bool usePropertyList(const QVariant& var)
    {
//        QSet<QVariant::Type> listTypes;
//        if (listTypes.isEmpty())
//        {
//            listTypes.insert(QVariant::nameToType("QList<int>"));
//            listTypes.insert(QVariant::nameToType("QList<bool>"));
//            listTypes.insert(QVariant::nameToType("QList<QPointF>"));
//            listTypes.insert(QVariant::nameToType("QVector<double>"));
//            listTypes.insert(QVariant::nameToType("QStringList"));
//        }

        return S_LISTTYPES.contains(var.typeName());
    }
private:
    /// Pointer to current object factory
    GtAbstractObjectFactory* m_factory;

    /** Creates new GtObjects from given QDomElement.
        @param element QDomElement memento
        @return New GtObject based on QDomElement memento */
    GtObject* toObjectHelper(const QDomElement& element,
                       GtObject* handleObjectAdd = nullptr);

    /** Merges all properties found in QDomElement with properties found
        in GtdObject.
        @param element QDomElement memento
        @param obj GtdObject */
    void mergeObjectProperties(const QDomElement& element,
                               GtObject* obj);

    /**
     * @brief mergeDummyProperies
     * @param element
     * @param obj
     */
    void mergeDummyProperies(const QDomElement& element,
                             GtObject* obj);

    /**
     * @brief mergeDynamicProperties
     * @param element
     * @param obj
     */
    void mergeDynamicProperties(const QDomElement& element,
                                GtObject* obj);

    /**
     * @brief writeProperties
     * @param m memento
     * @param obj
     */
    void writeProperties(GtObjectMemento::MementoData& data,
                         const GtObject* obj);
    /**
     * @brief writeProperties
     * @param root
     * @param obj
     * @param m
     */
    void writeProperties(QDomDocument& doc,
                         QDomElement& root,
                         const GtObjectMemento::MementoData& data);
    /**
     * @brief readProperties
     * @param m memento
     * @param element
     */
    void readProperties(GtObjectMemento::MementoData& data,
                        const QDomElement& element);

    /**
     * @brief readDynamicProperties
     * @param propData memento property data
     * @param element
     */
    void readDynamicProperties(
            GtObjectMemento::MementoData::PropertyData& propData,
            const QDomElement& element);

    /**
     * @brief writeDynamicProperties
     * @param doc
     * @param root
     * @param stored
     * @param property
     */
    void writeDynamicProperties(
            QDomDocument& doc, QDomElement& root,
            const GtObjectMemento::MementoData::PropertyData& property);

    /**
     * @brief writePropertyHelper
     */
    void writePropertyHelper(
            QVector<GtObjectMemento::MementoData::PropertyData>& pVec,
            QSet<QString>& stored, GtAbstractProperty* property);

    /**
     * @brief writePropertyHelper
     */
    void writePropertyHelper(
            QDomDocument& doc, QDomElement& root,
            const GtObjectMemento::MementoData::PropertyData& prop);

    /**
     * @brief readPropertyHelper
     */
    void readPropertyHelper(
            GtObjectMemento::MementoData::PropertyData& propData,
            const QDomElement& element);

    /**
     * @brief readDynamicPropertyHelper
     */
    void readDynamicPropertyHelper(
            GtObjectMemento::MementoData::PropertyData& propData,
            const QDomElement& element);


    /** Creates an QDomElement of a given property.
        @param name Name of property
        @param var QVariant of property
        @return doc Referened to QDomDocument to generate new QDomElements */
    QDomElement propertyToDomElement(const QString& name,
                                     const QVariant& var,
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

    /** Converts given property to QVariant.
        @param value Property value
        @param type Property type
        @return QVariant */
    static QVariant propertyToVariant(const QString& value, const QString&);

    /** Converts given property list to QVariant.
        @param value Property list value
        @param type Property type
        @return QVariant */
    static QVariant propertyListToVariant(const QString& value,
                                          const QString& type);

    /**
     * @brief Returns the element of the dynamic properties branch. If no
     * brunch exists a new created is returned.
     * @param doc
     * @param root
     * @return
     */
    QDomElement dynamicPropertyElement(QDomDocument& doc,
                                       QDomElement& root,
                                       const QString& id);

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
     * @brief handleDynamicPropertyAdd
     * @param target
     * @param toAdd
     * @param index
     * @return
     */
    static bool handleDynamicPropertyAdd(GtObject* target,
                                         const QDomElement& objectToAdd,
                                         const QString& index);

    /**
     * @brief handleDynamicPropertyRemove
     * @param target
     * @param toRemove
     * @param index
     * @return
     */
    static bool handleDynamicPropertyRemove(GtObject* target,
                                            const QDomElement& objectToAdd,
                                            const QString& index);

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

    /**
     * @brief structProperties
     * @return
     */
    QList<GtDynamicPropertyContainer*> structProperties(GtObject* obj);

    /**
     * @brief structPropertyHelper
     * @param prop
     * @return
     */
    QList<GtDynamicPropertyContainer*> structPropertyHelper(
        GtAbstractProperty* prop);

    /** Converts all member of a QList/QVector (non pointer) to QVariantList.
        @param t QList object
        @return QVariantList of converted QList member */
    template<class T>
    static QVariantList convertToVariantList(const T& t)
    {
        QVariantList list;

        foreach (auto m, t)
        {
            list.append(QVariant(m));
        }

        return list;
    }

    /** writes all members of a QList/QVector to a string (with ';' delimiter)
     */
    template<class T>
    static inline QString listToString(const T& t)
    {
        QString str;

        foreach (const auto& m, t)
        {
            str += m + ';';
        }
        str.truncate(str.size()-1);

        return str;
    }

};

#endif // GTOBJECTIO_H
