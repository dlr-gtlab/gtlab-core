/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_externalizedobjectfetcher.h
 *
 *  Created on: 20.07.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTEXTERNALIZEDOBJECTFETCHER_H
#define GTEXTERNALIZEDOBJECTFETCHER_H

#include "gt_externalizedobject.h"

#include <QPointer>
#include <memory>

/**
 * @brief The GtExternalizedObjectFetcher class.
 * Helper class to fetch externalized objects for an extended duration
 * (eg. for multiple function calls). Uses RAII.
 * @tparam T_Base Base object class. Must be a subclass of GtExternalizedObject
 * @tparam T_Data Data object class. Must be a subclass of
 * GtExternalizedObjectData.
 */
template<class T_Base, class T_Data = typename T_Base::Data>
class GtExternalizedObjectFetcher
{
public:

    GtExternalizedObjectFetcher() = default;
    explicit GtExternalizedObjectFetcher(T_Base* object);

    /**
     * @brief access object
     * @return base ptr
     */
    T_Base* get() const { return m_objPtr.data(); }

    /**
     * @brief access data object, will return null if data is not fetched
     * @return data ptr
     */
    T_Data* data() const { return m_dataPtr.get(); }

    /**
     * @brief checks if object is not null and the data object is valid
     * @return is valid
     */
    bool isValid() const;

    /**
     * @brief clears the data object if fetched. Keeps base object pointer
     */
    void clear();

    /**
     * @brief fetches a new data object to internalizes the base object.
     */
    void fetch();

    /**
     * @brief sets the object to fetch
     * @param object new base
     */
    void set(T_Base* object);

    /**
     * @brief operator -> to access underlying object
     * @return pointer to object
     */
    T_Base* operator->(void) const { return this->get(); }

    // casting
    operator T_Base*() const { return this->get(); }
    operator bool() const { return isValid(); }

private:
    /// pointer to object
    QPointer<T_Base> m_objPtr;
    /// pointer to data object
    std::unique_ptr<T_Data> m_dataPtr;
};

template<class T_Base, class T_Data>
GtExternalizedObjectFetcher<T_Base, T_Data>::GtExternalizedObjectFetcher(
        T_Base* object) :
    m_objPtr(object)
{
    static_assert (
        std::is_base_of<GtExternalizedObject, T_Base>(),
        "T_Base must be derived of GtExternalizedObject");
    static_assert (
        std::is_base_of<GtExternalizedObjectData, T_Data>(),
        "T_Data must be derived of GtExternalizedObjectData");
    fetch();
}

template<class T_Base, class T_Data>
inline bool GtExternalizedObjectFetcher<T_Base, T_Data>::isValid() const
{
    return m_objPtr && m_dataPtr && m_dataPtr->isValid();
}

template<class T_Base, class T_Data>
inline void GtExternalizedObjectFetcher<T_Base, T_Data>::clear()
{
    m_dataPtr.reset();
}

template<class T_Base, class T_Data>
inline void GtExternalizedObjectFetcher<T_Base, T_Data>::fetch()
{
    m_dataPtr = std::make_unique<T_Data>(m_objPtr);
}

template<class T_Base, class T_Data>
inline void GtExternalizedObjectFetcher<T_Base, T_Data>::set(T_Base* object)
{
    m_objPtr = QPointer<T_Base>{object};
    fetch();
}

#endif // GTEXTERNALIZEDOBJECTFETCHER_H
