// SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
//
// SPDX-License-Identifier: MPL-2.0+

/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 10.11.2017
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTEXAMPLESENTRY_H
#define GTEXAMPLESENTRY_H

#include <QPixmap>

/**
 * @brief The GtExamplesEntry class
 * Collects information about an Exmaple entry
 */
class GtExamplesEntry
{
public:
    /**
     * @brief GtExamplesEntry - Constructor
     */
    GtExamplesEntry();

    /**
     *  Destructor
     */
    ~GtExamplesEntry();

    /**
     * @brief dirPath
     * @return
     */
    QString dirPath() const;

    /**
     * @brief setDirPath
     * @param dirPath
     */
    void setDirPath(const QString& dirPath);

    /**
     * @brief pixmap - get the pixmap of the exampleentry
     * @return
     */
    QString pixmapPath() const;

    /**
     * @brief setPixmap - set the pixmap of the exampleentry
     * @param pixmap
     */
    void setPixmapPath(const QString& pixmapPath);

    /**
     * @brief category - getter
     * @return
     */
    QString category() const;

    /**
     * @brief setCategory- setter
     * @param category
     */
    void setCategory(const QString& category);

    /**
     * @brief name - getter
     * @return
     */
    QString name() const;

    /**
     * @brief setName - name setter
     * @param name
     */
    void setName(const QString& name);

    /**
     * @brief description - description getter
     * @return
     */
    QString description() const;

    /**
     * @brief setDescription - description setter
     * @param description
     */
    void setDescription(const QString& description);

private:
    /// path of the example main directory
    QString m_dirPath;

    /// path of the picture for the preview
    QString m_pixmapPath;

    /// category to sort examples
    QString m_category;

    /// name of the example
    QString m_name;

    /// description of the example
    QString m_description;
};

#endif // GTEXAMPLESENTRY_H
