.. _processinterface:

Process interface
=================

Use :cpp:class:`GtProcessInterface` when a module provides executable workflow
elements. Calculators perform individual operations; tasks group process
elements into a workflow hierarchy.

.. code-block:: cpp

  #include "gt_moduleinterface.h"
  #include "gt_processinterface.h"

  class MyModule : public QObject,
                   public GtModuleInterface,
                   public GtProcessInterface
  {
      Q_OBJECT
      GT_MODULE()
      Q_INTERFACES(GtProcessInterface)

  public:
      GtVersionNumber version() override;
      QString description() const override;

      QList<GtCalculatorData> calculators() override;
      QList<GtTaskData> tasks() override;
  };

Register calculators
--------------------

Return a descriptor for every calculator users can add to a process. Besides
the C++ type, the descriptor supplies the name and metadata shown in GTlab:

.. code-block:: cpp

  QList<GtCalculatorData> MyModule::calculators()
  {
      auto data = GT_CALC_DATA(MyCalculator);
      data->id = QStringLiteral("Evaluate operating point");
      data->version = GtVersionNumber(1, 0, 0);
      data->status = GtAbstractProcessData::RELEASE;
      data->author = QStringLiteral("My Team");
      data->category = QStringLiteral("Performance");
      data->description = QStringLiteral("Evaluates one configured point.");

      return {data};
  }

Use stable, meaningful IDs and categories: users encounter them in process
editing, and saved workflows depend on the registered calculator type.

Set ``status`` to ``GtAbstractProcessData::RELEASE`` for calculators that
should appear in normal GTlab installations. GTlab shows non-release entries
only in Dev Mode (for example when GTlab is started with ``--dev``), so
leaving a calculator in ``PROTOTYPE`` or ``DEPRECATED`` mode keeps it out of
the regular user-facing lists.

Register tasks
--------------

Override ``tasks()`` only when the module defines custom task classes. Create
each entry with ``GT_TASK_DATA`` and return ``GtTaskData`` descriptors using
the same metadata principles as calculators. The default implementation
returns an empty list, so calculator-only modules do not need to override it.

Registration does not execute a calculator or create a workflow. It makes the
types available to GTlab; users and project loading create instances later.
See :doc:`../../basics/process_elements` for execution and state concepts.
