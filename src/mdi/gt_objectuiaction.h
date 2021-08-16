/* GTlab - Gas Turbine laboratory
 * Source File:
 * copyright 2009-2015 by DLR
 *
 *  Created on: 24.11.2015
 *  Author: Stanislaus Reitenbach (AT-TW)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTOBJECTUIACTION_H
#define GTOBJECTUIACTION_H

#include "gt_mdi_exports.h"

#include <QString>
#include <QKeySequence>

/**
 * @brief The GtObjectUIAction class
 */
class GT_MDI_EXPORT GtObjectUIAction
{
public:
    /**
     * @brief GtObjectUIAction
     */
    GtObjectUIAction();

    /**
     * @brief GtObjectUIAction
     */
    GtObjectUIAction(const QString& txt, const QString& mthd,
                     const QString& icn = QString(),
                     const QString& ver = QString(),
                     const QString& visible = QString(),
                     const QKeySequence& shortCut = QKeySequence());

    /**
     * @brief text
     * @return
     */
    const QString& text() const;

    /**
     * @brief icon
     * @return
     */
    const QString& icon() const;

    /**
     * @brief method
     * @return
     */
    const QString& method() const;

    /**
     * @brief verificationMethod
     * @return
     */
    const QString& verificationMethod() const;

    /**
     * @brief enabledMethod
     * @return
     */
    const QString& visibilityMethod() const;

    /**
     * @brief shortCut
     * @return Short cut connected to the action
     */
    const QKeySequence& shortCut() const;

private:
    /// Action text
    QString m_txt;

    /// Action icon
    QString m_icon;

    /// Invokable method
    QString m_method;

    /// Verification method
    QString m_ver;

    /// Visibility identificator method
    QString m_visible;

    QKeySequence m_shortCut;
};

typedef QList<GtObjectUIAction> GtActionList;

#endif // GTOBJECTUIACTION_H
