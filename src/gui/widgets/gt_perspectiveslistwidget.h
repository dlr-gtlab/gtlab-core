/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 13.08.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTPERSPECTIVESLISTWIDGET_H
#define GTPERSPECTIVESLISTWIDGET_H

#include <QListWidget>

/**
 * @brief The GtPerspectivesListWidget class
 */
class GtPerspectivesListWidget : public QListWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtPerspectivesListWidget
     */
    GtPerspectivesListWidget(bool initialize = true, QWidget* parent = NULL);

    /**
     * @brief Perspective list initialization
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
    /// Last perspective id before editing
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
    bool addDataItem(const QString id);

    /**
     * @brief duplicateDataItem
     * @param source
     * @param target
     * @return
     */
    bool duplicateDataItem(const QString source, const QString target);

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

#endif // GTPERSPECTIVESLISTWIDGET_H
