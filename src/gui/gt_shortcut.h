/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 * Source File: gt_shortcut.h
 *
 *  Created on: 13.08.2021
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTSHORTCUT_H
#define GTSHORTCUT_H

#include <QKeySequence>
#include <QObject>

#include "gt_settings.h"
#include "gt_gui_exports.h"

/**
 * @brief The GtShortCut class
 * Class to organize a short cut for GTlab
 */
class GT_GUI_EXPORT GtShortCut : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief GtShortCut
     * @param id - identification string
     * @param key - keysequence
     * Currently only keysequences with CTRL as modifier works,
     * shift and alt not
     * @param cat - category to use to sort shortcut
     */
    GtShortCut(const QString& id, const QKeySequence& key,
               const QString& cat, const bool isReadOnly = false);

    /**
     * @brief GtShortCut
     * @param data Shortcut data
     */
    explicit GtShortCut(GtShortCutSettingsData data);

    /**
     * @brief Getter for id
     * @return shortcut id
     */
    QString id() const;

    /**
     * @brief Setter for id
     * @param id - identification string
     */
    void setId(const QString &id);

    /**
     * @brief Getter for keysequence
     * @return sequence to use for the shortcut
     */
    QKeySequence key() const;

    /**
     * @brief Setter for keysequence
     * @param key - keysequence
     */
    void setKey(const QKeySequence &key);

    /**
     * @brief Getter for category
     * @return category as a string
     */
    QString category() const;

    /**
     * @brief isReadOnly
     * @return true if the short cut is read only and canoot be modified
     */
    bool isReadOnly() const;

    /**
     * @brief Compares the id to the shortcuts id (case insensitive)
     * @param id Id to compare
     * @return equal
     */
    bool compare(const QString& id) const;

    /**
     * @brief Compares the id and category to the shortcuts data
     * (case insensitive)
     * @param id Id to compare
     * @param cat Category to compare
     * @return equal
     */
    bool compare(const QString& id, const QString& cat) const;

private:

    /// shortcut data
    GtShortCutSettingsData m_data;
};

#endif // GTSHORTCUT_H
