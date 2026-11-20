.. _calcexecinterface:

Process execution interface
===========================

Use :cpp:class:`GtProcessExecInterface` (``gt_processexecinterface.h``) to provide an execution mode other
than the built-in local calculator execution. Typical examples are a remote
service, scheduler, or isolated runtime. It does not register calculators;
combine it with :ref:`processinterface` only if the same module also provides
calculator classes.

.. code-block:: cpp

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtProcessExecInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtProcessExecInterface)

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
``GtAbstractProcessExecutor`` implementation (``gt_abstractprocessexecutor.h``).
Its constructor must be ``Q_INVOKABLE`` because GTlab creates a fresh executor
through Qt metadata when the mode is used:

.. code-block:: cpp

  QMetaObject MyModule::executor()
  {
      return GT_METADATA(MyRemoteExecutor);
  }

The executor's ``exec(GtCalculator*)`` implementation is responsible for the
complete alternative execution and returns whether it succeeded. Override
``exec(GtTask*)`` as well to run tasks in this mode. Both overloads default to
local execution, so override only the ones your executor handles.
``executorSettings()`` may expose properties needed to configure the execution
environment; leave the default empty list when no global settings are needed.

.. note::

   ``GtProcessExecInterface`` replaces ``GtCalculatorExecInterface`` and
   ``GtAbstractProcessExecutor`` replaces ``GtAbstractCalculatorExecutor``.
   Both old names are deprecated and will be removed in GTlab 2.2.

Test missing credentials, unavailable infrastructure, cancellation, and
transfer failures as well as successful execution. The executor must leave the
calculator in a state that lets GTlab report a clear result.
