/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 07.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTSESSIONLISTWIDGET_H
#define GTSESSIONLISTWIDGET_H

#include <QListWidget>

/**
 * @brief The GtSessionListWidget class
 */
class GtSessionListWidget : public QListWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtSessionListWidget
     * @param initialize
     * @param parent
     */
    GtSessionListWidget(bool initialize = true, QWidget* parent = nullptr);

    /**
     * @brief Session list initialization
     */
    void init();

public slots:
    /**
     * @brief renameItem
     */
    void renameItem();

    /**
     * @brief newItem
     */
    void newItem();

    /**
     * @brief duplicateItem
     */
    void duplicateItem();

    /**
     * @brief deleteItem
     */
    void deleteItem();

private:
    /// Last session id before editing
    QString m_lastId;

    /**
     * @brief Iterates through list of names to find duplicates.
        If duplicate is found, additional number is added to the name.
     * @param name
     * @param names
     * @param initName
     * @param iteration
     * @return
     */
    QString generateId(const QString& name, const QStringList& names,
                       QString initName = QString(), int iteration = 0);

    /**
     * @brief Adds a new session item t
     * @param id
     * @return
     */
    bool addSessionItem(const QString id);

    /**
     * @brief duplicateSessionItem
     * @param source
     * @param target
     * @return
     */
    bool duplicateSessionItem(const QString source, const QString target);

private slots:
    /**
     * @brief onLstItemsCommitData
     * @param pLineEdit
     */
    void onLstItemsCommitData(QWidget* pLineEdit);

    /**
     * @brief lastIdChanged
     * @param val
     */
    void lastIdChanged(const QString& val);

};

#endif // GTSESSIONLISTWIDGET_H
