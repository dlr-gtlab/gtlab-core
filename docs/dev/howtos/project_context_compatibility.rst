Accessing Project Data
======================

Use ``gtApp->currentProject()`` when your code intentionally operates on the
project selected in the GUI. A calculator or task may execute for a different
project, so new process code should use execution-specific data instead.

GUI actions and widgets
-----------------------

Menu actions, dialogs, and wizards may use ``gtApp->currentProject()`` if they
are meant to act on the current GUI selection. Always handle ``nullptr``:

.. code-block:: cpp

   auto* project = gtApp->currentProject();
   if (!project) return;

An MDI widget tied to a particular project should keep the project or object
it was opened with (for example, in a ``QPointer``). Otherwise, switching the
GUI selection could make the widget operate on the wrong project. Only widgets
intentionally following the selection should query ``currentProject()`` again.

Calculators
-----------

Implement calculator logic in ``GtCalculator::run()``, not ``exec()``.
During execution, prefer:

* ``data<T>(property)`` for objects referenced by an object-link or
  object-path property.
* ``projectPath()`` if you only need the execution project's directory.
* Explicit parameters when passing objects or values to helper functions.

For example:

.. code-block:: cpp

   bool MyCalculator::run()
   {
       auto* part = data<MyPart*>(m_partLink);
       if (!part) return false;

       return processPart(*part, projectPath());
   }

Here, ``m_partLink`` is a registered link property and ``processPart()``
is a module function. Check for an empty ``projectPath()`` if a directory is
required.

Tasks
-----

The same rules apply to custom ``GtTask`` code, including ``setUp()`` and
``evaluate()``: use linked execution data, ``projectPath()``, or explicit
parameters. Do not assume that ``findParent<GtProject*>()`` identifies the
execution project; process objects may be cloned.

If a calculator or task really needs the ``GtProject`` itself, use
``GtExecutionContext::current()`` (from ``gt_executioncontext.h``).
Check both the context and ``context->project()``; some executions provide
only a project path. Do not modify the persistent project directly from
calculator code. Return changes through the normal execution/result mechanism.

Threads and callbacks
---------------------

The execution context is thread-local and is not passed automatically to
new threads. Pass the needed data explicitly, and do not keep borrowed project
pointers or context pointers for work that outlives the execution.

Legacy ``currentProject()`` compatibility
-------------------------------------------

Existing calculators using ``gtApp->currentProject()`` still work:

* With an active execution context, it returns that context's project
  (``nullptr`` for a path-only context).
* Otherwise, it returns the GUI-selected project, or ``nullptr``.

This compatibility behavior is not recommended for new calculator or task
code. ``currentProject()`` is **not deprecated** for GUI-selection use.

In Developer Mode, legacy access during process execution produces a warning,
once per module/component class. It does not affect the calculation.
Set ``GTLAB_LEGACY_PROJECT_ACCESS_WARNING=1`` to enable the warning outside
Developer Mode, or ``=0`` to disable it. The override is read once on first
use.
