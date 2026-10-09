Architecture decision 0002 — Calculator executor interface on process components
================================================================================

:Status: Accepted
:Date: 2026-10-09
:Normative source: GitHub issue `#1601 <https://github.com/dlr-gtlab/gtlab-core/issues/1601>`_
:Related: `#1596 <https://github.com/dlr-gtlab/gtlab-core/pull/1596>`_, `#1526 <https://github.com/dlr-gtlab/gtlab-core/issues/1526>`_, `#1531 <https://github.com/dlr-gtlab/gtlab-core/issues/1531>`_, and `#1499 <https://github.com/dlr-gtlab/gtlab-core/issues/1499>`_

Context
-------

Execution plugins (subclasses of ``GtAbstractCalculatorExecutor``) can only
execute ``GtCalculator`` objects, and the execution properties (``execMode``,
``execLabel``) are defined on ``GtCalculator`` alone. ``GtTask`` and other
process components have no execution mode of their own, a process tree cannot
be dispatched to an executor as a group, and there is no way to inherit an
executor choice from a parent component.

The executor list hands out transient executor instances, and the plugin
execution paths of ``GtCalculator`` and ``GtTask`` never released them,
leaking one instance per plugin execution.

Decision
--------

The calculator executor interface is extended and moved to process components:
every process component (``GtCalculator``, ``GtTask``, and module-defined
subclasses of ``GtProcessComponent``) carries the execution properties, and
executors gain a task-based execution entry point.

Property placement
~~~~~~~~~~~~~~~~~~

``execMode`` and ``execLabel`` move from ``GtCalculator`` to
``GtProcessComponent``, the common base of calculators and tasks. The base
constructor registers the properties once, including the plugin modes and
their settings, so every process component exposes the same execution
configuration.

Mode semantics
~~~~~~~~~~~~~~

* ``parent`` is registered as the first, and therefore default, execution
  mode. It selects the execution mode of the parent process component.
* ``execMode()`` returns the effective mode: while the stored mode is
  ``parent``, resolution climbs the parent chain. The root process
  component (``isRootProcessComponent()``: no process component as parent)
  falls back to ``local``.
* Resolution is dynamic. A child keeps its stored ``parent`` selection and
  re-resolves on every query; the effective mode of a parent is never written
  back to children.
* ``local`` runs the component in the owning process.

Task dispatch
~~~~~~~~~~~~~

``GtTask::exec()`` switches between local iteration and plugin execution based
on the effective mode. ``GtAbstractCalculatorExecutor`` gains a virtual
``exec(GtTask*)`` overload; executors override it to run a task, for example
on a remote or HPC backend.

Default executor behavior
~~~~~~~~~~~~~~~~~~~~~~~~~

The base ``exec(GtTask*)`` implementation falls back to local execution of
the task and emits a warning. Every registered executor id remains a valid
task mode, and calculator-only executor plugins keep working.

Executor lifetime
~~~~~~~~~~~~~~~~~

Executors are transient. The dispatching component obtains an instance from
``GtCalculatorExecutorList`` per execution and releases it after the call,
including on failure. This removes the instance leak from calculator and task
plugin execution.

Shared mode registry
~~~~~~~~~~~~~~~~~~~~

Calculators and tasks share one mode registry. ``GtProcessComponent``
registers the mode ids and their settings from ``GtCalculatorExecutorList``
as sub-properties of ``execMode``; the same settings configure the executor
for any process component.

API and ABI impact
~~~~~~~~~~~~~~~~~~

The execution accessors move from ``GtCalculator`` to ``GtProcessComponent``.
Existing call sites keep compiling; ``execMode()`` now returns the effective
mode by value (``QString``) instead of the stored value (``const QString&``).
The new virtuals on ``GtAbstractCalculatorExecutor`` and the changed class
layouts of ``GtProcessComponent`` and ``GtCalculator`` are an ABI change:
modules and executor plugins must be recompiled. The change lands in 2.1; the
2.0.X patch line stays ABI-stable. Old executor plugins keep working and use
the default (local) execution for tasks.

Consequences
------------

* Tasks can be executed through executor plugins. They are first-class
  candidates for remote execution, which is the foundation for the
  ``ProcessTaskOperation`` adapter (#1531) and for process execution in HPC
  jobs (#1499).
* Execution mode/label handling is uniform for all process components, with
  inheritance from the parent process component.
* The ``parent`` default lets the execution mode of a parent task propagate to
  grouped child components, allowing quick executor selection for a whole
  process group.
* Module executor plugins must stay cheap to instantiate; an instance is
  created per execution.
* Calculator-only executor plugins execute tasks locally with a visible
  warning.
* An invalid mode string leaves the resolved mode unchanged.

Validation
----------

The unit tests in ``tests/unittests/core/test_gt_processcomponent.cpp`` cover
mode inheritance, root fallback, dynamic re-resolution, and sibling
independence. ``tests/unittests/core/test_gt_abstractcalculatorexecutor.cpp``
covers task execution dispatch, the default fallback path, and failure
propagation.
