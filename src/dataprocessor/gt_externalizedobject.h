/* GTlab - Gas Turbine laboratory
 * Source File: gt_externalizedobject.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 20.07.2021
 *  Author: Marius Bröcker (AT-TWK)
 *  Tel.: +49 2203 601 5199
 */

#ifndef GTEXTERNALIZEDOBJECT_H
#define GTEXTERNALIZEDOBJECT_H

#include "gt_datamodel_exports.h"
#include "gt_object.h"

#include <QPointer>

class QVariant;
class GtExternalizedObject;
class GtExternalizedObjectData;

namespace gt
{

/**
 * @brief Will fetch a helper object for accessing an externalized object.
 * @tparam T_Base Base object class. Must be a subclass of GtExternalizedObject
 * @tparam T_Data Data object class. Must be a subclass of
 * GtExternalizedObjectData.
 * @param obj Object to fetch a helper object for.
 * @return helper object
 */
template<typename T_Base, typename T_Data = typename T_Base::Data>
T_Data fetchExternalizedData(T_Base* obj)
{
    static_assert (
        std::is_base_of<GtExternalizedObject, T_Base>(),
        "T_Base must be derived of GtExternalizedObject");
    static_assert (
        std::is_base_of<GtExternalizedObjectData, T_Data>(),
        "T_Data must be derived of GtExternalizedObjectData");

    return T_Data{obj};
}

} // namespace gt


/// Marco for declaring a data class as friend and adding a dedicated
/// fetchData method impl. Muste be placd inside a private block of the class.
#define GT_DECL_DATACLASS(DataClass) \
    friend class DataClass; \
    public: using Data = DataClass; \
    template <typename T = Data> T fetchData() { \
        return gt::fetchExternalizedData(this); \
    } private:

/// Marco for adding a dedicated base method impl for accessing the base class.
/// Muste be placed inside a private block of the class.
#define GT_DECL_BASECLASS(BaseClass) \
    private: using Base = BaseClass; \
    template <typename T = Base> T* base() const { \
        return static_cast<T*>(m_base.data()); \
    }


/**
 * @brief The GtExternalizedObjectData class
 * Helper object that should be used to access any of the externalized data
 * of an externalized data object. Will automatically fetch and release the
 * object using the RAII principle.
 */
class GT_DATAMODEL_EXPORT GtExternalizedObjectData
{
    GT_DECL_BASECLASS(GtExternalizedObject)

public:

    explicit GtExternalizedObjectData(GtExternalizedObject* base);
    virtual ~GtExternalizedObjectData();

    GtExternalizedObjectData(const GtExternalizedObjectData&) = default;
    GtExternalizedObjectData(GtExternalizedObjectData&&) = default;
    GtExternalizedObjectData& operator=(const GtExternalizedObjectData&) = default;
    GtExternalizedObjectData& operator=(GtExternalizedObjectData&&) = default;

    /**
     * @brief isValid. Will delegate to virtual isDataValid method of base class
     * @return whether base pointer is valid and object is fetched
     */
    bool isValid() const;

protected:

    /// pointer to base class
    QPointer<Base> m_base{};
};

/**
 * @brief The GtExternalizedObject class.
 * Represents a data object that may externalize (some of) its data at runtime
 * to reduce overall resource usage.
 */
class GT_DATAMODEL_EXPORT GtExternalizedObject : public GtObject
{
    Q_OBJECT

    GT_DECL_DATACLASS(GtExternalizedObjectData)

    friend class GtExternalizationManager;
    friend class TestExternalizedObject;

public:

    ~GtExternalizedObject();

    /**
     * @brief Returns the current number of accesses. To check if the object is
     * fetched use the method isFetched.
     * @return ref count
     */
    int refCount() const;

    /**
     * @brief Externalize. Will externalize the object only if its fetched and
     * the object has changed. Call this function with care, as the previous
     * version may be overwritten.
     * @return success
     */
    bool externalize();

    /**
     * @brief Will internalize (fetch) and keep the data internalized
     * for the lifetime of this object or until externalize was called
     * explicitly.
     * @return success
     */
    Q_INVOKABLE bool internalize();

    /**
     * @brief isFetched
     * @return whether the object is fetched.
     */
    Q_INVOKABLE bool isFetched() const;

protected:

    /**
     * @brief GtExternalizedObject
     */
    Q_INVOKABLE GtExternalizedObject();

    /**
     * @brief Returns whether the object data is valid.
     * This is a delegating method and should not be called directly.
     * @return is valid
     */
    virtual bool isDataValid() const;

    /**
     * @brief Returns whether the object can be externalized.
     * @return can externalize
     */
    virtual bool canExternalize() const;

    /**
     * @brief Method for fetching the externalized data
     * @param metaData Data that may need to be saved in between sessions to
     * help fetch/externalize the dataset.
     * @param fetchInitialVersion Whether to fetch the intial version of the
     * data (i.e. the original data that has not changed)
     * @return success
     */
    virtual bool doFetchData(QVariant& metaData, bool fetchInitialVersion) = 0;

    /**
     * @brief Method to implement for externalizing the data. Will only be
     * called if canExternalize returns true.
     * @param metaData Data that may need to be saved in between sessions to
     * help fetch/externalize the dataset.
     * @return success
     */
    virtual bool doExternalizeData(QVariant& metaData) = 0;

    /**
     * @brief doClearExternalizedData method to implement for clearing the
     * externalized data.
     */
    virtual void doClearExternalizedData() = 0;

    /**
     * @brief Checks whether object changed and data should be internalized
     */
    void onObjectDiffMerged() override;

private:

    struct Impl;
    std::unique_ptr<Impl> pimpl;

    /**
     * @brief Increments ref count and fetches the data if not fetched yet.
     * @return whether fetching was successfull.
     */
    bool fetch();

    /**
     * @brief Decrements ref count. Will flag this object for externalization
     * if data has changed.
     * @return whether externalizing was successfull.
     */
    bool release();

    /**
     * @brief Helper method for fetching. Should not be called directly.
     * @return success
     */
    bool fetchHelper();

    /**
     * @brief Setter for the fetchInitialVersion property
     * @param value Whether the initial version should be fetched
     */
    void setFetchInitialVersion(bool value) const;

    /**
     * @brief hasModifiedData
     * @return whether the object changed compared to the externalized version
     */
    bool hasModifiedData();

    /**
     * @brief hasModifiedData
     * @param otherHash hash to compare with
     * @return whether the object changed compared to the externalized version
     */
    bool hasModifiedData(const QString& otherHash) const;

    /**
     * @brief Calculates a special hash of this object used to check for changes
     * in the externalized data. Member variables of this class do not count
     * towards a different hash.
     * @return new hash
     */
    QString calcExtHash();
};

#endif // GTEXTERNALIZEDOBJECT_H
