/* GTlab - Gas Turbine laboratory
 * Source File: gt_accessdata.h
 * copyright 2009-2016 by DLR
 *
 *  Created on: 17.03.2016
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTACCESSDATA_H
#define GTACCESSDATA_H

#include "gt_core_exports.h"

#include <QString>

class QUrl;

/**
 * @brief The GtAccessData class
 */
class GT_CORE_EXPORT GtAccessData
{
public:
    /**
     * @brief Constructor.
     * @param Host name.
     * @param Port number.
     * @param Username.
     * @param Password.
     */
    GtAccessData(const QString& host,
                 int port,
                 const QString& user,
                 const QString& pw = QString());

    /**
     * @brief Constructor.
     * @param Host name.
     * @param Port number.
     */
    GtAccessData(const QString& host, int port);

    /**
     * @brief Constructor.
     * @param Host name.
     * @param Username.
     * @param Password.
     */
    GtAccessData(const QString& host,
                 const QString& user,
                 const QString& pw = QString());

    /**
     * @brief Constructor.
     * @param Host name.
     */
    explicit GtAccessData(const QString& host);

    /**
    * @brief Returns host name.
    * @return Host name.
    */
   const QString& host() const;

   /**
    * @brief Returns port number.
    * @return Port number.
    */
   int port() const;

   /**
    * @brief Returns Username.
    * @return Username.
    */
   const QString& user() const;

   /**
    * @brief Returns password.
    * @return Password.
    */
   const QString& password() const;

   /**
    * @brief Returns data in QUrl format based on internal data.
    * @return Data in QUrl format.
    */
   QUrl data() const;

   /**
    * @brief Returns true if host name is empty. Otherwise false is returned.
    * @return Whether host name is empty or not.
    */
   bool isEmpty();

private:
   /// Host name
   QString m_host;

   /// Port
   int m_port;

   /// Username
   QString m_user;

   /// Password
   QString m_pw;

};

#endif // GTACCESSDATA_H
