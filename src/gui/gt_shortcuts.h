/* GTlab - Gas Turbine laboratory
 * Source File: gt_shortcuts.h
 * copyright 2009-2021 by DLR
 *
 *  Created on: 13.08.2021
 *  Author: Jens Schmeink (AT-TWK)
 *  Tel.: +49 2203 601 2191
 */
#ifndef GTSHORTCUTS_H
#define GTSHORTCUTS_H

#include "gt_gui_exports.h"

#include "gt_settings.h"
#include <QObject>

class GtShortCut;

/**
 * @brief The GtShortCuts class
 * ShortCut collection object
 */
class GT_GUI_EXPORT GtShortCuts : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief GtShortCuts
     * @param parent - object to set as parent
     */
    explicit GtShortCuts(QObject* parent = nullptr);

    /**
     * @brief getKey
     * @param id - id of the shortcut element
     * @param category - uses to identify the short cut aswell
     * @return key sequence for this entry
     */
    QKeySequence getKey(const QString& id,
                        const QString& category = "Core") const;

    /**
     * @brief initialize - initialize short cuts based on the information
     * from the map
     * @param tab - map using the short cut ids as identifier and
     * with a string list containing the QKeySequence and the category as the
     * two elements of the list.
     */
    [[deprecated("use QList<GtShortCutSettingsData> instead")]]
    void initialize(const QMap<QString, QStringList>& tab);

    /**
     * @brief initialize - initializes the short cuts using the shortcut data
     * @param list
     */
    void initialize(const QList<GtShortCutSettingsData>& list);

    /**
     * @brief initialize - initializes the short cut using the shortcut data
     * @param data
     */
    void initialize(const GtShortCutSettingsData& data);

    /**
     * @brief isEmpty
     * @return true if the collection is empty
     */
    bool isEmpty() const;

    /**
     * @brief shortCuts
     * @return list of all short cuts
     */
    QList<GtShortCut*> shortCuts() const;

    /**
     * @brief findShortCut - find short cut by id and category
     * @param id to use for searching
     * @param category  to use for searching
     * @return pointer to short cut or nullptr if it cannot be found
     */
    GtShortCut* findShortCut(const QString& id, const QString& category) const;

    [[deprecated("use emit x->changed() instead")]]
    void emitChange();

signals:
    void changed();
};

#endif // GTSHORTCUTS_H
