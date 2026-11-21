/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_moduleinterface.h"

#ifdef PROJECT_PACKAGE_FIXTURE
  #include "gt_datamodelinterface.h"
  #include "gt_package.h"

class ProjectPackageFixture : public GtPackage
{
    Q_OBJECT
public:
    Q_INVOKABLE ProjectPackageFixture()
    {
        setObjectName(QStringLiteral("Test Package"));
    }

    bool saveData(QDomElement& root, QDomDocument& doc) override
    {
        return objectName() != QStringLiteral("Fail Save") &&
               GtPackage::saveData(root, doc);
    }
};

#endif

class FootprintTestModule : public QObject,
                            public GtModuleInterface
#ifdef PROJECT_PACKAGE_FIXTURE
    ,
                            public GtDatamodelInterface
#endif
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID GT_MODULE_ID FILE "metadata.json")
    GT_IMPLEMENT_MODULE_
#ifdef PROJECT_PACKAGE_FIXTURE
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
#ifdef PROJECT_PACKAGE_FIXTURE
    QMetaObject package() override
    {
        return ProjectPackageFixture::staticMetaObject;
    }
    QList<gt::VersionUpgradeRoutine> upgradeRoutines() const override
    {
        return {{{1, 2, 0}, [](QDomElement&, const QString&) { return true; }}};
    }
    bool standAlone() override
    {
        return true;
    }
#endif
};

#include "module.moc"
