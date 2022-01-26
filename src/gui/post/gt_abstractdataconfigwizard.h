#ifndef GTABSTRACTDATACONFIGWIZARD_H
#define GTABSTRACTDATACONFIGWIZARD_H

#include "gt_gui_exports.h"

#include <QWizardPage>
#include <QPointer>

//class GtBarProvider;
class GtAbstractChartProvider;
//class GtXyProvider;
class GtDataZoneTableList;

/**
 * @brief The GtAbstractDataConfigWizard class
 */
class GT_GUI_EXPORT GtAbstractDataConfigWizard : public QWizardPage
{
public:
    /**
     * @brief GtAbstractDataConfigWizard
     * @param provider
     * @param parent
     */
//    GtAbstractDataConfigWizard(GtBarProvider* provider,
//                               QWidget* parent = Q_NULLPTR);

    GtAbstractDataConfigWizard(GtAbstractChartProvider* provider,
                               QWidget* parent = Q_NULLPTR);

    GtAbstractDataConfigWizard();

protected:
    /**
     * @brief isValid
     */
    bool isValid();

    /// datazonetablelist
    GtDataZoneTableList* m_dztList;

};

#endif // GTABSTRACTDATACONFIGWIZARD_H
