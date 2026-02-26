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
    Q_INVOKABLE GtCommandlineTemplateFinder(const bool reinitializeEverytime=true);

    Q_INVOKABLE const void reinitialize();

    Q_INVOKABLE bool hasToolVersion(const QString &toolname, const QString &version="");
    Q_INVOKABLE bool hasExecutionTemplate(const QString &toolname, const QString &version, const QString &exectemplate);

    Q_INVOKABLE const QString getExecTemplatePath(const QString &toolname, const QString &version, const QString &exectemplate, int* error=nullptr);

    Q_INVOKABLE const QString getDefaultShell();
    Q_INVOKABLE const QString getOsType();


private:
    void makeMainPathAbs();

    bool m_reinitializeEverytime;
    QString m_mainpath;
    QString m_machine;

    const QString makeExecTemplatePath(const QString &toolname, const QString &version, const QString &exectemplate);
};

#endif // GT_COMMANDLINETEMPLATEFINDER_H
