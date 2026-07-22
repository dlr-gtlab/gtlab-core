.. _calcexecinterface:

Calculator execution interface
==============================

Use :cpp:class:`GtCalculatorExecInterface` to provide an execution mode other
than the built-in local calculator execution. Typical examples are a remote
service, scheduler, or isolated runtime. It does not register calculators;
combine it with :ref:`processinterface` only if the same module also provides
calculator classes.

.. code-block:: cpp

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtCalculatorExecInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtCalculatorExecInterface)

  public:
      GtVersionNumber version() override;
      QString description() const override;

      QString executorId() override;
      QString executorDescription() override;
      QMetaObject executor() override;
      QList<GtAbstractProperty*> executorSettings() override;
  };

``executorId()`` is the stable value stored as a calculator's execution mode.
Keep it unchanged after release and make it unique across installed modules.
``executorDescription()`` is the user-facing explanation of that mode.

``executor()`` returns metadata for a concrete
``GtAbstractCalculatorExecutor`` implementation. Its constructor must be
``Q_INVOKABLE`` because GTlab creates a fresh executor through Qt metadata when
the mode is used:

.. code-block:: cpp

  QMetaObject MyModule::executor()
  {
      return GT_METADATA(MyRemoteExecutor);
  }

The executor's ``exec(GtCalculator*)`` implementation is responsible for the
complete alternative execution and returns whether it succeeded.
``executorSettings()`` may expose properties needed to configure the execution
environment; leave the default empty list when no global settings are needed.

Test missing credentials, unavailable infrastructure, cancellation, and
transfer failures as well as successful execution. The executor must leave the
calculator in a state that lets GTlab report a clear result.
