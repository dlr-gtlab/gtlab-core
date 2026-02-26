/*
 * Author: Matthias Schuff
 */

#ifndef GT_PREFERENCESCOMMANDLINETEMPLATES_H
#define GT_PREFERENCESCOMMANDLINETEMPLATES_H

#include "gt_preferencespage.h"
#include <QWidget>
#include <QMap>

namespace Ui
{
    class GtPreferencesCommandlineTemplates;
}





class GtPreferencesCommandlineTemplates : public GtPreferencesPage
{
    Q_OBJECT

public:
    explicit GtPreferencesCommandlineTemplates(QWidget* parent = nullptr);
    ~GtPreferencesCommandlineTemplates();
    void saveSettings(GtSettings&) const override;
    void loadSettings(const GtSettings&) override;

private:
    Ui::GtPreferencesCommandlineTemplates* ui;

private slots:
    void onClickOpenInExplorer();
    void onClickRestoreDefaults();
    void onClickCheckTemplateExists();
};

#endif // GT_PREFERENCESCOMMANDLINETEMPLATES_H
