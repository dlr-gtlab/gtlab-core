/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2022 by DLR
 *
 *  Created on: 11.10.2022
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTCHANGELOGPARSER_H
#define GTCHANGELOGPARSER_H

#include "gt_gui_exports.h"
#include <QString>
#include <QMap>
#include <QDate>
#include "gt_versionnumber.h"

/**
 * @brief The GtChangeLogEntry class
 * Struct to keep the information of a change log entry
 */
struct GT_GUI_EXPORT GtChangeLogEntry
{

    GtChangeLogEntry(GtVersionNumber const& v,
                     QString const& c,
                     QDate const& d = {});

    explicit GtChangeLogEntry(QString const& mdContent);

    /**
    * @brief versionFromMD
    * @param content
    * @return the version number that can be found in the content
    */
   static GtVersionNumber versionFromMD(QString const& content);

   /**
    * @brief versionFromMD
    * @param content
    * @return the version number that can be found in the content
    */
   static QDate dateFromMD(QString const& content);

   /**
    * @brief toMarkDownText
    * @return a markdown string of the version, date and content
    * of the change log entry
    */
   QString toMarkDownText() const;

   const GtVersionNumber& version() const;

   const QString& content() const;

   const QDate& date() const;

private:
   GtVersionNumber m_version;

   QString m_content;

   QDate m_date;
};

/**
 * @brief The GtChangeLogParser class
 * Takes the contentn of a mrkdown file witht a changelog based on the
 * "Keep a Changelog" format.
 * This is read and storred with the versionnumbers and the formmated content
 * of the paragraph of that version number
 *
 * The part of the changelog of "unreleased" is not parsed.
 */
class GT_GUI_EXPORT GtChangeLogParser
{
public:
    /**
     * @brief GtChangeLogParser
     * takes the content of a markdown file and starts the parse function
     * @param fileContent of a markdown file following the convention
     * of "Keep a Changelog".
     */
    explicit GtChangeLogParser(QString const& fileContent);

    /// the mark down formatted content of the changelog file for the latest
    /// release
    GtChangeLogEntry currentReleaseMD() const;

    /// the mark down formatted content of all releases from the changelo
    QList<GtChangeLogEntry> allReleasesDM() const;

    /**
     * @brief contentOfVersion
     * @param v
     * @return the markdown formatted part of a verionnumber v.
     * Returns empty string if the verion is not part of the changelog
     */
    QString contentOfVersion(GtVersionNumber const& v) const;

    /**
     * @brief versions
     * @return list of all versions stored in the changelog parser
     */
    QList<GtVersionNumber> versions() const;

private:
    /**
     * @brief parse
     * reads the file content and sorts it
     *  to the changelog map of versionnumbers and its makrdown content
     * @param fileContent
     */
    void parse(const QString& fileContent);

    /// container to store the information from the file sorted with the
    /// verionsnumber.
    QList<GtChangeLogEntry> m_changeLog;

    /**
     * @brief releaseNumberOccurances
     * @param fileContent - content of the markdown changelog file
     * @return the indices of all occurances of the start of the
     * mark down formatted part of that release (starting with
     * "## [X...]"
     */
    static QList<int> releaseNumberOccurances(const QString& fileContent);
};


#endif // GTCHANGELOGPARSER_H
