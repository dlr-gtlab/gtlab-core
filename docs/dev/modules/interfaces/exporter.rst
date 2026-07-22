.. _exporterinterface:

Exporter interface
==================

Use :cpp:class:`GtExporterInterface` to expose actions that write GTlab data to
external file formats. An exporter derives from ``GtAbstractExporter`` and
reads from an appropriate target object without changing its domain state.

.. code-block:: cpp

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtExporterInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtExporterInterface)

  public:
      GtVersionNumber version() override;
      QString description() const override;
      QList<QMetaObject> exporter() const override;
  };

Register exporter classes with Qt metadata:

.. code-block:: cpp

  QList<QMetaObject> MyModule::exporter() const
  {
      return {GT_METADATA(MyExporter)};
  }

GTlab uses the exporter's supported target type to offer the action in the
relevant object context. The exporter implementation is responsible for file
selection, overwrite handling, validation, and clear error reporting. Do not
register the data-model interface solely to provide an exporter for object
types owned by another module.
