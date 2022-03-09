#ifndef GTTTEMPLATEVIEWER_H
#define GTTTEMPLATEVIEWER_H

#include <QDropEvent>
#include <QPointer>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QMutex>

#include "gt_mdiitem.h"
#include "gt_gui_exports.h"

class GtTemplateViewerWidgetItem;
class GtXYPlotWidget;
class QVBoxLayout;
class GtPostTemplate;
class GtAbstractItemUI;
class GtScrollArea;
class GtPostTemplateItem;
class QPrinter;
class GtPostTemplatePath;
class GtAbstractPostWidget;
class QLabel;
class GtCollectionLoadingWidget;

/**
 * @brief The GtTemplateViewer class
 */
class GT_GUI_EXPORT GtTemplateViewer : public GtMdiItem
{
    Q_OBJECT

public:
    enum MoveDirection
    {
        MOVE_UP = 0,
        MOVE_DOWN = 1
    };

    /**
      * @brief Constructor.
      */
    Q_INVOKABLE GtTemplateViewer();

    /**
     * @brief Desturctor.
     */
    ~GtTemplateViewer() override;

    /**
     * @brief setData
     * @param obj
     */
    void setData(GtObject* obj) override;

private:

    /**
     * @brief readFromXMLmemento
     */
    void readFromXMLmemento(const QString& path);

    /**
     * @brief createPlot
     * @param data
     * @return
     */
    bool createPlot(GtPostTemplateItem* data);

    /**
     * @brief createPlots
     * @return
     */
    bool createPlots();

    /**
     * @brief createSpecialPlot
     * @param plotName
     */
    void createSpecialPlot(const QString& plotName);

    /**
     * @brief showEvent
     */
    void showEvent() override;

    QVBoxLayout* m_layplot;

    GtPostTemplate* m_postTemplateDM;

    QList<GtAbstractItemUI*> m_itemUIList;

    GtScrollArea* m_scrollarea;

    QWidget* m_plotcontainerwidget;

    /// Template identification string
    QString m_templateId;

    QPointer<GtPostTemplatePath> m_templatePath;

    QFileSystemWatcher m_fileWatcher;

    /// Title line
    QLabel* m_titleLineEdit;

    /**
     * @brief setPlotIcon
     * @param actPlot
     * @param str
     */
    void setPlotIcon(QAction* actPlot, const QString& str) const;

    /**
     * @brief shortPlotName
     * @param providerName
     * @return
     */
    QString shortPlotName(const QString& providerName) const;

    QMutex m_mutex;

    QTimer m_timer;

    QList<GtAbstractPostWidget*> m_widgetList;

    /// Fetch list animation
    GtCollectionLoadingWidget* m_fetchMovie;

    /**
     * @brief Event called after project was saved.
     * @param project Savend project.
     */
    void projectSavedEvent(GtProject* project) override;

private slots:

    /**
     * @brief addButtonClicked
     */
    void addButtonClicked();

    /**
     * @brief saveButtonClicked
     */
    void saveButtonClicked();

    /**
     * @brief reloadWidgets
     */
    void reloadWidgets();

    /**
     * @brief resizeButtonClicked
     */
    void resizeButtonClicked();

    /**
     * @brief printButtonClicked
     */
    void printButtonClicked();

    /**
     * @brief pdfButtonClicked
     */
    void pdfButtonClicked();

    /**
     * @brief deleteUI
     * @param ai
     * @return
     */
    bool deleteUI(GtAbstractItemUI* ai);

    /**
     * @brief copyUI
     * @param ti
     */
    void copyUI(GtPostTemplateItem* ti);

    /**
     * @brief printPreview
     * @param printer
     */
    void printPreview(QPrinter* printer);

    /**
     * @brief moveWidget
     * @param d
     */
    void moveWidget(GtTemplateViewer::MoveDirection d);

public slots:

    /**
     * @brief print
     */
    void print() override;

    /**
     * @brief onFileChanged
     * @param path
     */
    void onFileChanged(const QString& path);

    void onRefreshRequest();

};

#endif // GTTTEMPLATEVIEWER_H
