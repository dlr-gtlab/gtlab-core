/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GTABSTRACTPOSTWIDGET_H
#define GTABSTRACTPOSTWIDGET_H

#include "gt_gui_exports.h"

#include "gt_object.h"
#include "gt_objectuiaction.h"

#include "gt_version.h"

#include <QList>
#include <QWidget>


class GtPostTemplateItem;
class QPrinter;
class QPainter;
class QRect;
class GtAbstractChartProvider;

/**
 * @brief The GtAbstractPostWidget class
 */
class GT_GUI_EXPORT GtAbstractPostWidget : public QWidget
{
    Q_OBJECT

public:

    using ActionFunction = GtObjectUIAction::ActionMethod;

    /**
     * @brief GtAbstractPostWidget
     */
    explicit GtAbstractPostWidget(QWidget* w = nullptr);

    /**
     * @brief configActions
     * @return
     */
    GtActionList configActions();

    /**
     * @brief data
     * @return
     */
    GtPostTemplateItem* data();

    /**
     * @brief setData
     * @param dm
     */
    void setData(GtPostTemplateItem* dm);

    /**
     * @brief Constructs a config action object and returns it as a reference.
     * Useful for specify its properties after a function call.
     * NOTE: Reference may become invalid if a new action has been added.
     * @param actionText Action name
     * @param actionMethod Action method to invoke. Must be invokable
     * from MOS (Meta Object System).
     * @return Action reference
     */
    GtObjectUIAction& addConfigAction(const QString& actionText,
                                      const QString& actionMethod);

    /**
     * @brief Overload.Accepts a lambda or function pointer.
     * @param actionText Action name
     * @param actionMethod Method/Function/lambda to invoke
     * @return Action reference
     */
    GtObjectUIAction& addConfigAction(const QString& actionText,
                                      ActionFunction actionMethod);

    GT_DEPRECATED_REMOVED_IN(2, 1, "Use dedicated setters instead: "
                 "addConfigAction(<text>, <method>)"
                 "  .setIcon(<icon>)"
                 "  .setVerificationMethod(<method>)")
    bool addConfigAction(const QString& actionText,
                         const QString& actionMethod,
                         const QString& actionIcon,
                         const QString& actionVerification);

    GT_DEPRECATED_REMOVED_IN(2, 1, "Use dedicated setters instead: "
                 "addConfigAction(<text>, <method>)"
                 "  .setIcon(<icon>)"
                 "  .setVerificationMethod(<method>)"
                 "  .setVisibilityMethod(<method>)")
    bool addConfigAction(const QString& actionText,
                         const QString& actionMethod,
                         const QString& actionIcon,
                         const QString& actionVerification,
                         const QString& actionVisibility);

    /**
     * @brief hasConfigMenu
     * @return
     */
    bool hasConfigMenu();

    /**
     * @brief isPrintable
     * @return
     */
    bool isPrintable();

    GT_DEPRECATED_REMOVED_IN(2, 1, "Use icon instead")
    QString iconString();

    GT_DEPRECATED_REMOVED_IN(2, 1, "Use setIcon instead")
    void setIconString(const QString& iconString);

    /**
     * @brief Returns the icon of the post widget
     * @return icon
     */
    QIcon icon();

    /**
     * @brief Sets the icon of the post widget
     * @param icon Icon
     */
    void setIcon(const QIcon& icon);

    /**
     * @brief changePlotName
     */
    virtual void changePlotName(const QString&) {}

    /**
     * @brief plotToPdf
     */
    virtual void plotToPdf() {}

    /**
     * @brief plotToPdf
     */
    virtual void plotToPng() {}

    /**
     * @brief pdfEmbedded
     */
    virtual void printEmbedded(QPrinter* /*printer*/,
                               QPainter* /*painter*/,
                               QRectF /*rect*/) {}

    /**
     * @brief updatePlot
     */
    virtual void updatePlot() {}

    /**
     * @brief resizePlot
     */
    virtual void resizePlot() {}

    /**
     * @brief initShowHideMarkers
     */
    virtual void initShowHideMarkers();

    /**
     * @brief providerName
     * @return
     */
    virtual QString providerName();

protected:

    GtAbstractChartProvider* chartProvider();

private:

    GtPostTemplateItem* m_data;

    bool m_printable;

    // We should use a QIcon member here instead of the QString.
    // However this might break the ABI
    GT_DEPRECATED_REMOVED_IN(2, 1, "Replace with QIcon")
    QString m_iconString;

    /// List of UI actions
    GtActionList m_actionList;

private slots:

    /**
     * @brief updatePlots
     */
    void updatePlots();

    /**
     * @brief hideMarkers
     */
    virtual void hideMarkers(GtObject*) {};

    /**
     * @brief showMarkers
     */
    virtual void showMarkers(GtObject*) {};

    /**
     * @brief canShowMarkers
     * @return
     */
    virtual bool canShowMarkers(GtObject*);

    /**
     * @brief canHideMarkers
     * @return
     */
    virtual bool canHideMarkers(GtObject*);

    /**
     * @brief showConfigurationMenu
     */
    virtual void showConfigurationMenu(GtObject*) = 0;

    /**
     * @brief canShowConfigurationMenu
     * @return
     */
    virtual bool canShowConfigurationMenu(GtObject*) = 0;

    /**
     * @brief showAxisLabels
     */
    virtual void showAxisLabels(GtObject*) = 0;

    /**
     * @brief canShowAxisLabels
     * @return
     */
    virtual bool canShowAxisLabels(GtObject*) = 0;

    /**
     * @brief hideAxisLabels
     */
    virtual void hideAxisLabels(GtObject*) = 0;

    /**
     * @brief canHideAxisLabels
     * @return
     */
    virtual bool canHideAxisLabels(GtObject*) = 0;


public slots:

    /**
     * @brief updatePlot
     */
    virtual void initFillingUI(GtObject* = nullptr) {}

signals:

    void refreshRequest();

    /**
     * @brief providerNameChanged - provider name has changed to "name"
     * @param name
     */
    void providerNameChanged(QString name);

};

#endif // GTABSTRACTPOSTWIDGET_H
