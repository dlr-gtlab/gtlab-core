/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_moduleinterface.h"

#if defined(PROJECT_PACKAGE_FIXTURE) || defined(AUXILIARY_PACKAGE_FIXTURE)
  #include "gt_datamodelinterface.h"
  #include "gt_externalizedobject.h"
  #include "gt_package.h"
  #include "gt_objectlinkproperty.h"

  #include <QDir>

class PackageLinkObject : public GtObject
{
    Q_OBJECT
public:
    Q_INVOKABLE PackageLinkObject()
    {
        registerProperty(m_link);
    }

private:
    GtObjectLinkProperty m_link{QStringLiteral("link"),
                                QStringLiteral("Link"),
                                QString{},
                                this,
                                {QStringLiteral("GtObject")}};
};

class PackageExternalizedObject : public GtExternalizedObject
{
    Q_OBJECT
public:
    Q_INVOKABLE PackageExternalizedObject()
    { }

protected:
    bool doFetchData(QVariant&, bool) override
    {
        return false;
    }
    bool doExternalizeData(QVariant&) override
    {
        return false;
    }
    void doClearExternalizedData() override
    { }
};

class ProjectPackageFixture : public GtPackage
{
    Q_OBJECT
public:
    Q_INVOKABLE ProjectPackageFixture()
    {
        setObjectName(QStringLiteral("Test Package"));
    }

    bool readMiscData(const QDir& directory) override
    {
        if (directory.exists(QStringLiteral(".package-sidecar")))
        {
            setObjectName(QStringLiteral("With Auxiliary Data"));
        }
        return GtPackage::readMiscData(directory);
    }

    bool saveData(QDomElement& root, QDomDocument& doc) override
    {
        return objectName() != QStringLiteral("Fail Save") &&
               GtPackage::saveData(root, doc);
    }
};

class AuxiliaryPackageFixture : public GtPackage
{
    Q_OBJECT
public:
    Q_INVOKABLE AuxiliaryPackageFixture()
    { }
    bool readMiscData(const QDir&) override
    {
        return true;
    }
};
#endif

class FootprintTestModule : public QObject,
                            public GtModuleInterface
#if defined(PROJECT_PACKAGE_FIXTURE) || defined(AUXILIARY_PACKAGE_FIXTURE)
    ,
                            public GtDatamodelInterface
#endif
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID GT_MODULE_ID FILE "metadata.json")
    GT_IMPLEMENT_MODULE_
#if defined(PROJECT_PACKAGE_FIXTURE) || defined(AUXILIARY_PACKAGE_FIXTURE)
    Q_INTERFACES(GtDatamodelInterface)
#endif

public:
    GtVersionNumber version() override
    {
        return {1, 2, 3};
    }
    QString description() const override
    {
        return QStringLiteral("Test module");
    }
#if defined(PROJECT_PACKAGE_FIXTURE)
    QMetaObject package() override
    {
        return ProjectPackageFixture::staticMetaObject;
    }
    QList<QMetaObject> data() override
    {
        return {PackageLinkObject::staticMetaObject,
                PackageExternalizedObject::staticMetaObject};
    }
    bool standAlone() override
    {
        return true;
    }
#elif defined(AUXILIARY_PACKAGE_FIXTURE)
    QMetaObject package() override
    {
        return AuxiliaryPackageFixture::staticMetaObject;
    }
    bool standAlone() override
    {
        return true;
    }
#endif
};

#include "module.moc"
