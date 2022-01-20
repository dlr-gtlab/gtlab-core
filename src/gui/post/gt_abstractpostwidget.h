#ifndef GTABSTRACTPOSTWIDGET_H
#define GTABSTRACTPOSTWIDGET_H

#include "gt_gui_exports.h"

#include "QList"
#include <QWidget>
#include "gt_object.h"


class GtPostTemplateItem;
class QPrinter;
class QPainter;
class QRect;
class GtDataZoneTableList;
class GtAbstractChartProvider;


#include "gt_objectuiaction.h"

/**
 * @brief The GtAbstractPostWidget class
 */
class GT_GUI_EXPORT GtAbstractPostWidget : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief GtAbstractPostWidget
     */
    GtAbstractPostWidget(QWidget* w = Q_NULLPTR);

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
     * @brief addConfigAction
     * @param actionText
     * @param actionIcon
     * @param actionMethod
     * @param actionVerification
     * @return
     */
    bool addConfigAction(const QString& actionText,
                         const QString& actionMethod,
                         const QString& actionIcon,
                         const QString& actionVerification);

    /**
     * @brief addConfigAction
     * @param actionText
     * @param actionMethod
     * @param actionIcon
     * @param actionVerification
     * @param actionVisibility
     * @return
     */
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

    /**
     * @brief iconString
     * @return
     */
    QString iconString();

    /**
     * @brief setIconString
     * @param iconString
     */
    void setIconString(const QString& iconString);

    /**
     * @brief changePlotName
     */
    virtual void changePlotName(QString) {}

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
                             QPainter* /*painter*/, QRectF /*rect*/) {}

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

    void createDztList(QStringList dztUUIDs);

    void deleteDztList();

    GtDataZoneTableList* m_dztList;   

    GtAbstractChartProvider* chartProvider();
private:

    GtPostTemplateItem* m_data;

    bool m_printable;

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
    virtual void hideMarkers(GtObject*);

    /**
     * @brief showMarkers
     */
    virtual void showMarkers(GtObject*);

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
    virtual void initFillingUI(GtObject* = Q_NULLPTR) {}

signals:

    void refreshRequest();

    /**
     * @brief providerNameChanged - provider name has changed to "name"
     * @param name
     */
    void providerNameChanged(QString name);

};

#endif // GTABSTRACTPOSTWIDGET_H
