/*
 * Author: Matthias Schuff
 */

#ifndef GT_COMMANDLINETEMPLATEFINDER_H
#define GT_COMMANDLINETEMPLATEFINDER_H

#include <QObject>
#include <QDir>


class GtCommandlineTemplateFinder
{
public:
    Q_INVOKABLE GtCommandlineTemplateFinder();

    Q_INVOKABLE void loadGtlabSettings();


    Q_INVOKABLE bool hasToolVersion(const QString &toolname, const QString &version);
    Q_INVOKABLE bool hasTemplate(const QString &toolname, const QString &version, const QString &templatename);
    Q_INVOKABLE const QString searchTemplatePath(const QString &toolname, const QString &version, const QString &templatename, int* error=nullptr);

    Q_INVOKABLE const QString osType();
    Q_INVOKABLE const QString templateSearchPath();
    Q_INVOKABLE const QString machine();
    Q_INVOKABLE const QString defaultShell();

    Q_INVOKABLE void setOsType(const QString &ostype);
    Q_INVOKABLE void setTemplateSearchPath(const QString &templatepath);
    Q_INVOKABLE void setMachine(const QString &machine);
    Q_INVOKABLE void setDefaultShell(const QString &shell);


private:
    QString m_osType;
    QString m_templatepath;
    QString m_machine;
    QString m_shell;

    const QString resolveTemplateSearchPath();
    const QString makeExecTemplatePath(const QString &toolname, const QString &version, const QString &exectemplate);
};

#endif // GT_COMMANDLINETEMPLATEFINDER_H
