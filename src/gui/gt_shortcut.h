/* GTlab - Gas Turbine laboratory
 * Source File: gt_shortcut.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 13.08.2021
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTSHORTCUT_H
#define GTSHORTCUT_H

#include <QKeySequence>
#include <QObject>

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
               const QString& cat);

    /**
     * @brief id
     * @return shortcut id
     */
    QString id() const;

    /**
     * @brief setId
     * @param id - identification string
     */
    void setId(const QString &id);

    /**
     * @brief keys
     * @return sequence to use for the shortcut
     */
    QKeySequence key() const;

    /**
     * @brief setKeys
     * @param key - keysequence
     */
    void setKey(const QKeySequence &key);

    /**
     * @brief categoryString
     * @return category as a string
     */
    QString category() const;
private:
    ///
    QString m_id;

    ///
    QKeySequence m_key;

    ///
    QString m_cat;
};

#endif // GTSHORTCUT_H
