/*
 * Author: Matthias Schuff
 */

#ifndef GT_COMMANDLINETEMPLATERENDERING_H
#define GT_COMMANDLINETEMPLATERENDERING_H


#include <QObject>
#include <QDir>
#include <QMap>

class GtCommandlineTemplateRendering
{
public:
    Q_INVOKABLE GtCommandlineTemplateRendering(const QString shell);

    Q_INVOKABLE bool loadTemplateFromFile(const QString &templatepath);
    Q_INVOKABLE bool loadTemplateFromList(const QStringList &templatelines);
    Q_INVOKABLE bool updateVariables(const QMap<QString, QString> &variables);
    Q_INVOKABLE bool renderTemplateToScript();
    Q_INVOKABLE bool writeScript(const QString &targetfilepath, bool overwriteExisting=false, bool createTargetDirectory=false);

    Q_INVOKABLE bool renderTemplateAndWriteScript(const QString &templatepath, const QMap<QString, QString> &variables, const QString &targetfilepath, bool overwriteExisting=false, bool createTargetDirectory=false);


    Q_INVOKABLE const QStringList scriptLines();
private:
    QString m_shell;
    QStringList m_template;
    QMap<QString, QString> m_variables;


};

#endif // GT_COMMANDLINETEMPLATERENDERING_H
