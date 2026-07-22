.. _importerinterface:

Importer interface
==================

Use :cpp:class:`GtImporterInterface` to expose import actions for external
file formats. An importer derives from ``GtAbstractImporter`` and writes the
parsed data into an appropriate target object in the GTlab data model.

.. code-block:: cpp

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtImporterInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtImporterInterface)

  public:
      GtVersionNumber version() override;
      QString description() const override;
      QList<QMetaObject> importer() const override;
  };

Register importer classes with Qt metadata:

.. code-block:: cpp

  QList<QMetaObject> MyModule::importer() const
  {
      return {GT_METADATA(MyImporter)};
  }

GTlab uses the importer's supported target type to offer the action in the
relevant object context. Keep file selection, format validation, and error
messages in the importer implementation. A failed or cancelled import should
leave the target data in a consistent state.
