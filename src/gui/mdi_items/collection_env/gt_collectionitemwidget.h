/* GTlab - Gas Turbine laboratory
 * Source File: gt_collectionitemwidget.h
 * copyright 2009-2017 by DLR
 *
 *  Created on: 10.02.2017
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GT_COLLECTIONITEMWIDGET_H
#define GT_COLLECTIONITEMWIDGET_H

#include <QWidget>

class QFrame;
class GtCollectionItem;

/**
 * @brief The GtCollectionItemWidget class
 */
class GtCollectionItemWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtCollectionItemWidget
     * @param collectionItem
     * @param parent
     */
    GtCollectionItemWidget(const GtCollectionItem& collectionItem,
                           QWidget* parent = nullptr);

    /**
     * @brief ~GtCollectionItemWidget
     */
    virtual ~GtCollectionItemWidget();

private:
    /// Filter string.
    QString m_filter;

    /**
     * @brief filterString
     * @param str
     * @return
     */
    QString filterString(const QString& str);

    /**
     * @brief newFrame
     * @return
     */
    QFrame* newFrame();

    /**
     * @brief newBox
     * @param title
     * @param val
     * @return
     */
    QFrame* newBox(const QString& title, const QString& content);

    /**
     * @brief newFileBox
     * @param fileNames
     * @return
     */
    QFrame* newFileBox(const QStringList& fileNames);

    /**
     * @brief newFileLayout
     * @param filename
     * @return
     */
    QLayout* newFileLayout(const QString& filename);

};

#endif // GT_COLLECTIONITEMWIDGET_H
