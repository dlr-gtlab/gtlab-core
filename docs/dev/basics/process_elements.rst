Process elements
================

GTlab represents an executable workflow as a tree of process elements. A
:cpp:class:`GtTask` is a container and execution boundary; its children are
calculators or nested tasks. A :cpp:class:`GtCalculator` performs one domain
operation. Both derive from :cpp:class:`GtProcessComponent` and therefore have
serializable properties, execution state, progress, and monitoring data.

For a first module, implement calculators and place them below GTlab's standard
task. Implement a custom task only when the workflow needs its own iteration or
coordination semantics.

Calculators
-----------

A calculator derives from :cpp:class:`GtCalculator` and implements ``run()``.
Return ``true`` when the operation completed successfully and ``false`` after
logging a useful error when it could not complete.

The constructor must be public and ``Q_INVOKABLE`` so GTlab can create the
calculator through its factory:

.. code-block:: cpp

  #include "gt_calculator.h"
  #include "gt_doubleproperty.h"

  class SquareCalculator : public GtCalculator
  {
      Q_OBJECT

  public:
      Q_INVOKABLE SquareCalculator();
      bool run() override;

  private:
      GtDoubleProperty m_input;
      GtDoubleProperty m_result;
  };

Register configurable inputs as regular properties and values produced during
execution as monitoring properties:

.. code-block:: cpp

  SquareCalculator::SquareCalculator() :
      m_input("input", tr("Input")),
      m_result("result", tr("Result"))
  {
      setObjectName(tr("Square"));
      registerProperty(m_input);
      registerMonitoringProperty(m_result);
  }

  bool SquareCalculator::run()
  {
      m_result.setVal(m_input * m_input);
      return true;
  }

GTlab calls ``exec()`` on the process component. For a local calculator,
``exec()`` prepares its runtime context, resolves linked data objects, updates
the process state, calls your ``run()`` implementation, transfers monitoring
properties, and handles temporary execution data. Module code normally
overrides ``run()``, not ``exec()``.

Access data-model objects
~~~~~~~~~~~~~~~~~~~~~~~~~

Use :cpp:class:`GtObjectLinkProperty` for a stable UUID-based reference or
:cpp:class:`GtObjectPathProperty` for a name-based path. Before ``run()`` is
called, GTlab resolves these properties against the runnable project's data.
Retrieve the typed object with ``data<T>(property)`` and handle a missing or
incompatible target explicitly:

.. code-block:: cpp

  bool EvaluateCalculator::run()
  {
      auto* component = data<MyComponent*>(m_componentLink);
      if (!component)
      {
          gtError() << tr("Selected component is unavailable");
          return false;
      }

      // perform the calculation
      return true;
  }

The process definition stores the reference, not ownership of the target. The
data object remains part of the project data model.

Register calculators
~~~~~~~~~~~~~~~~~~~~

A compiled calculator is not automatically visible in GTlab. Return a
``GtCalculatorData`` descriptor from the module's
:cpp:func:`GtProcessInterface::calculators` implementation. The descriptor
defines the user-facing ID, version, author, category, description, and
development status. See :ref:`processinterface` for the registration example.

``RELEASE`` calculators are available in normal operation. ``PROTOTYPE`` and
``DEPRECATED`` entries are intended for developer mode and migration work;
do not leave a production-ready calculator at its default prototype status.

Tasks
-----

A task owns an ordered list of child process elements. The standard task
executes those children and forms the top-level object of a process chain. A
custom task derives from :cpp:class:`GtTask` when it needs to implement loops,
convergence checks, optimization steps, or another execution policy.

The default task lifecycle is:

#. ``setUp()`` initializes task-specific iteration state;
#. ``runIteration()`` repeatedly runs the task's direct child elements;
#. ``evaluate()`` classifies each completed iteration; and
#. execution finishes, fails, or stops at the configured iteration limit.

``evaluate()`` returns one of:

``EVAL_OK``
  The iteration succeeded and another iteration may run.

``EVAL_FINISHED``
  The task reached its goal and completes successfully.

``EVAL_FAILED``
  The task cannot continue and fails.

The base implementations are sufficient for ordinary process grouping. A
custom iterative task usually overrides ``setUp()`` and ``evaluate()`` while
reusing ``runIteration()`` and ``runChildElements()``. Override the execution
methods only when their ordering is genuinely unsuitable, because they also
manage interruption requests, child states, property transfer, and monitoring
data.

Like custom calculators, custom task classes require a public ``Q_INVOKABLE``
constructor and registration through :cpp:func:`GtProcessInterface::tasks`.

Execution states and interruption
---------------------------------

GTlab updates process elements through states including ``QUEUED``,
``RUNNING``, ``FINISHED``, ``WARN_FINISHED``, ``FAILED``, ``SKIPPED``, and
``TERMINATED``. A calculator should communicate failure through its return
value and diagnostics instead of assigning normal lifecycle states itself.

Task interruption is cooperative: an interruption request is observed after a
currently running child has finished. Long-running calculator implementations
should therefore be designed with their execution environment and cancellation
requirements in mind rather than assuming immediate task termination.

Design checklist
----------------

* Give each calculator one clear operation and a stable registered identity.
* Keep configurable inputs in properties and results in data-model or
  monitoring properties as appropriate.
* Validate referenced data before using it and log actionable failures.
* Prefer the standard task until custom iteration semantics are required.
* Test both successful execution and missing-input or failed-calculation paths.
