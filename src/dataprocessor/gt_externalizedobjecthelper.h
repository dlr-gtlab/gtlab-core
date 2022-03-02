/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizedobjecthelper.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 20.07.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTEXTERNALIZEDOBJECTHELPER_H
#define GTEXTERNALIZEDOBJECTHELPER_H

#include "gt_externalizedobject.h"

/**
 * @brief The GtExternalizedObjectHelper class
 */
template<class ExternalizedObject = GtExternalizedObject>
class GtExternalizedObjectHelper        
{
public:

    // pointer to object
    ExternalizedObject* ptr;
    // mode of externalization
    GtExternalizedObject::ReleaseFlag mode;

    /**
     * @brief GtExternalizedObjectHelper constructor fetches the data if object
     * is valid
     * @param object externalized object to automatically load and externalize
     * @param mode whether to discard or save the data
     */
    GtExternalizedObjectHelper(GtObject* object,
                               GtExternalizedObject::ReleaseFlag mode);

    /**
     * @brief ~GtExternalizedObjectHelper destructor externalizes the data if
     * object is valid
     */
    ~GtExternalizedObjectHelper();

    /**
     * @brief isValid
     * @return whether the pointer is valid (not null)
     */
    bool isValid() const { return ptr && ptr->isFetched(); }

    /**
     * @brief operator -> to access member functions
     * @return pointer to object
     */
    ExternalizedObject* operator->(void) { return ptr; }

    // casting
    operator ExternalizedObject*() { return ptr; }
    operator bool() { return isValid(); }
};



template<class ExternalizedObject>
GtExternalizedObjectHelper<ExternalizedObject>::GtExternalizedObjectHelper(
        GtObject* object, GtExternalizedObject::ReleaseFlag mode) :
    ptr(qobject_cast<ExternalizedObject*>(object)),
    mode(mode)
{
    // check if class is derived from GtExternalizedObject
    static_assert (
        std::is_base_of<GtExternalizedObject, ExternalizedObject>(),
        "Object must be derived from GtExternalizedObject");

    if (ptr)
    {
        ptr->fetchData();
    }
}

template<class ExternalizedObject>
GtExternalizedObjectHelper<ExternalizedObject>::~GtExternalizedObjectHelper()
{
    if (isValid())
    {
        ptr->releaseData(mode);
    }
}

#endif // GTEXTERNALIZEDOBJECTHELPER_H
