/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_abstractcollectionitem.h
 *
 *  Created on: 02.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONITEM_H
#define GT_COLLECTIONITEM_H

#include "gt_gui_exports.h"

#include <QMap>
#include <QStringList>
#include <QVariant>

class GtCollectionLoader;

/**
 * @brief The GtCollectionItem class
 */
class GT_GUI_EXPORT GtCollectionItem
{
    friend class GtCollectionLoader;

public:
    /**
     * @brief Constructor.
     */
    GtCollectionItem();

    /**
     * @brief ident
     * @return
     */
    QString ident() const;

    /**
     * @brief uuid
     * @return
     */
    QString uuid() const;

    /**
     * @brief version
     * @return
     */
    double version() const;

    /**
     * @brief files
     * @return
     */
    QStringList files() const;

    /**
     * @brief property
     * @param id
     * @return
     */
    QVariant property(const QString& id) const;

    /**
    * @brief description
    * @return
    */
   QString description() const;

   /**
    * @brief isValid
    * @return
    */
   bool isValid() const;

   /**
    * @brief propertyIds
    * @return
    */
   QStringList propertyIds() const;

   /**
    * @brief localPath
    * @return
    */
   QString localPath() const;

private:
   /// Identification string.
    QString m_ident;

    /// Item specific uuid.
    QString m_uuid;

    /// Item specific description.
    QString m_description;

    /// Local path of collection item.
    QString m_localPath;

    /// Version.
    double m_version;

    /// Filename list.
    QStringList m_files;

    /// Property map
    QMap<QString, QVariant> m_properties;

};

#endif // GT_COLLECTIONITEM_H
