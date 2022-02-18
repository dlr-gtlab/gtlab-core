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


/**
 * @brief The GtExternalizedObject class
 */
class GT_DATAMODEL_EXPORT GtExternalizedObject : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(bool isFetched READ isFetched WRITE setIsFetched)

    Q_PROPERTY(QString cachedHash READ cachedHash WRITE updateCachedHash)

public:

    /**
     * @brief The ReleaseFlag enum
     */
    enum ReleaseFlag
    {
        /// does not write the data to disk (useful for read only processes)
        Discard,
        /// writes the data to disk if data has changed.
        Externalize,
        /// writes the data to disk.
        ForceExternalize
    };
    Q_ENUM(ReleaseFlag);

    /**
     * @brief The ObjectState enum
     */
    enum ObjectState
    {
        /// indicates whether the data is fetched (independent of ref count)
        Fetched = 1,
        /// indicates whether the object should externalize to a temp file
        Temporary = 2,
        /// indicates that the object was not externalized before and should not
        /// not be cleared. This flag should not be used directly
        Standalone = 4,
    };
    Q_DECLARE_FLAGS(ObjectStates, ObjectState);

    /**
     * @brief refCount returns the number of current accesses. To check if the
     * object is fetched use isFetched.
     * @return ref count
     */
    int refCount() const;

    /**
     * @brief isFetched
     * @return whether the object is fetched.
     */
    bool isFetched() const;

    /**
     * @brief isTemporary
     * @return
     */
    bool isTemporary() const;

    /**
     * @brief setIsTemporary whether this object should externalize to temp files
     * @param enable
     */
    void setIsTemporary(bool enable);

    /**
     * @brief hasChanges checks if the new calculated hash is equal to
     * the cached hash.
     * @return false if hashes are equal
     */
    bool hasChanges();
    bool hasChanges(const QString& otherHash) const;

    /**
     * @brief cachedHash getter for hash property.
     * @return cached hash.
     */
    QString cachedHash() const;

    /**
     * @brief recalculateHash calculates the hash of this object but does not
     * include the hash and ref count property in the calculation
     * @return new hash
     */
    QString recalculateHash();

    /**
     * @brief updateCachedHash setter for hash property.
     * @param hash new hash
     */
    void updateCachedHash(const QString& hash);

    /**
     * @brief objectStates
     * @return
     */
    GtExternalizedObject::ObjectStates objectStates() const;

    /**
     * @brief setObjectState
     * @param states
     */
    void setObjectState(ObjectState state, bool enable);

    /**
     * @brief resetRefCount resets the ref count to zero.
     */
    void resetRefCount();

public slots:

    /**
     * @brief fetchData fetches the data if not fetched yet. Should always be
     * called before accessing any data of this object. Increments the ref count
     * @return whether fetching was successfull.
     */
    bool fetchData();

    /**
     * @brief releaseData releases the data. Should always be called if
     * fetchData was called before. Decrements the ref count. Writes
     * the data depending on the mode specified. Clears the data only if ref
     * count is equal to 0.
     * @return whether externalizing was successfull.
     */
    bool releaseData(ReleaseFlag mode);

protected:

    /**
     * @brief GtExternalizedObject
     */
    Q_INVOKABLE GtExternalizedObject();

    /**
     * @brief doFetch method to implement for fetching the externalized data.
     * @return success
     */
    virtual bool doFetchData() = 0;
    /**
     * @brief doExternalize method to implement for externalizing the data.
     * @return success
     */
    virtual bool doExternalizeData() = 0;
    /**
     * @brief doClearExternalizedData method to implement for clearing the
     * externalized data. Should not emit a signal or set any object flags.
     */
    virtual void doClearExternalizedData() = 0;

private:

    /// keeps track of number of accesses
    int m_refCount;
    /// cache for the hash
    QString m_cachedHash;
    /// object states
    ObjectStates m_states;

    /**
     * @brief setIsFetched setter for is fetched property
     * @param value
     */
    void setIsFetched(bool value);
};



#endif // GTEXTERNALIZEDOBJECT_H
