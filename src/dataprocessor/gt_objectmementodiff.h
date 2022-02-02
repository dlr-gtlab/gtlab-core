/* GTlab - Gas Turbine laboratory
 * Source File: gt_objectmementodiff.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 15.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */
#ifndef GTOBJECTMEMENTODIFF_H
#define GTOBJECTMEMENTODIFF_H

#include "gt_datamodel_exports.h"
#include "gt_objectmemento.h"

#include <QDomDocument>
#include <QHash>
#include <QByteArray>

class QDomElement;
class GtObject;


/**
 * @brief The GtObjectMementoDiff class
 */
class GT_DATAMODEL_EXPORT GtObjectMementoDiff : protected QDomDocument
{
public:
    using QDomDocument::isNull;
    using QDomDocument::documentElement;
    using QDomDocument::toByteArray;
    /**
     * @brief GtObjectMementoDiff
     * @param left
     * @param right
     */
    GtObjectMementoDiff(const GtObjectMemento& left,
                        const GtObjectMemento& right);

    explicit GtObjectMementoDiff(const QByteArray& byteArray);

    /**
     * @brief Creates an empty diff.
     */
    GtObjectMementoDiff();

    /**
     * @brief Returns true if something on the object tree
     * structure (add/delete) has changed.
     * @return
     */
    bool hasObjectTreeChanges() const;

    /**
     * @brief objectAddBranch
     * @param child
     * @param parent
     * @return
     */
    static GtObjectMementoDiff objectAddBranch(GtObject* child,
            GtObject* parent);

    /**
     * @brief objectRemoveBranch
     * @param child
     * @param parent
     * @return
     */
    static GtObjectMementoDiff objectRemoveBranch(GtObject* child,
            GtObject* parent);

    /**
     * @brief appendDiff
     * @param diff
     * @return
     */
    bool appendDiff(const GtObjectMementoDiff& diff);

    /**
     * @brief numberOfDiffSteps
     * @return
     */
    int numberOfDiffSteps();

    /**
     * @brief operator <<
     * @param t
     * @return
     */
    inline GtObjectMementoDiff& operator<< (const GtObjectMementoDiff& t)
    {
        appendDiff(t);
        return *this;
    }

private:
    /**
     * @brief makeDiff
     * @param left
     * @param right
     */
    bool makeDiff(const GtObjectMemento& left, const GtObjectMemento& right, QDomElement& diffRoot);

    /**
     * @brief handleAttributeChange
     */
    void handleAttributeChange(const QString& name,
                               const QString& leftVal,
                               const QString& rightVal,
                               QDomElement& diffRoot);

    /**
     * @brief detectPropertyChanges
     */
    void detectPropertyChanges(const QVector<GtObjectMemento::MementoData::PropertyData>& leftProperties,
                               const QVector<GtObjectMemento::MementoData::PropertyData>& rightProperties,
                               QDomElement& diffRoot, bool isDynamic);

    /**
     * @brief handlePropertyChange
     */
    void handlePropertyChange(const GtObjectMemento::MementoData::PropertyData& leftProp,
                              const GtObjectMemento::MementoData::PropertyData& rightProp,
                              QDomElement& diffRoot);

    /**
     * @brief Proceeds an added object to the needed diff entry
     * @param leftRoot
     * @param changedObj
     */
    void handleObjectAdded(const QDomElement& changedObj,
                           int index,
                           QDomElement& diffRoot);

    /**
     * @brief Proceeds a detected object delete to the needed diff entry
     * @param removedObject
     */
    void handleObjectRemoved(const QDomElement& removedObject,
                             int index,
                             QDomElement& diffRoot);

    /**
     * @brief handleIndexChanged
     * @param changedObject
     * @param oldIndex
     * @param newIndex
     * @param diffRoot
     */
    void handleIndexChanged(const QDomElement& changedObject,
                            int oldIndex,
                            int newIndex,
                            QDomElement& diffRoot);

    /**
     * @brief copies all attributes from a reference QDomElement to a
     *        target QDomElement
     * @param root
     * @param target
     */
    static void copyAttributes(const QDomElement& root, QDomElement& target);
};



#endif // GTOBJECTMEMENTODIFF_H
