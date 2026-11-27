Accessing Project Data
======================

Use ``gtApp->currentProject()`` for operations targeting the project selected
in the GUI. Calculators and tasks may run for a different project and should
use their execution data instead.

GUI actions and widgets
-----------------------

Menu actions, dialogs, and wizards may use ``gtApp->currentProject()`` when
they act on the current GUI selection. Handle ``nullptr``:

.. code-block:: cpp

   auto* project = gtApp->currentProject();
   if (!project) return;

An MDI widget tied to a specific project should retain the project or object
it was opened with (e.g. via ``QPointer``). Only widgets meant to follow the
GUI selection should query ``currentProject()`` again.

Calculators and Tasks
---------------------

Linked objects (recommended)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Prefer ``GtObjectLinkProperty`` or ``GtObjectPathProperty`` to specify
which data your calculator or task needs. During normal process execution,
GTlab provides copies of the linked input objects, rather than direct access
to the persistent project data.

Retrieve these objects through ``data<T>()``:

.. code-block:: cpp

   bool MyCalculator::run()
   {
       auto* part = data<MyPart*>(m_partLink);
       if (!part) return false;

       return processPart(*part);
   }

Here, ``m_partLink`` is a registered object-link property. Changes to
execution objects are handled by GTlab's result/merge mechanism; do not
modify the persistent project directly.

Accessing data beyond object links
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you only need the project's directory, use ``projectPath()``.

If you really need the execution project itself, access it through
``GtExecutionContext`` (include ``gt_executioncontext.h``):

.. code-block:: cpp

   const auto* context = GtExecutionContext::current();
   if (!context) return false;

   const QString path = context->projectPath();
   const GtProject* project = context->project();
   if (!project) return false;

``project()`` can be null even when ``projectPath()`` is available.
Unlike linked execution objects, this is a **borrowed pointer to the original
project, not a copy**. Treat it as read-only, and do not assume accessing its
data is thread-safe. Prefer passing the required data into the execution
through object links instead.

The same recommendations apply in custom task hooks. Do not rely on
``findParent<GtProject*>()``: process objects may be cloned and detached
from the persistent project hierarchy.

Threads and callbacks
---------------------

The execution context is thread-local and is not automatically passed to
new threads. Pass the required data explicitly, and do not retain borrowed
project or context pointers beyond the execution.

Legacy ``currentProject()`` compatibility
-------------------------------------------

Existing calculators using ``gtApp->currentProject()`` still work:

* With an active execution context, it returns that context's project
  (``nullptr`` for a path-only context).
* Otherwise, it returns the GUI-selected project, or ``nullptr``.

This behavior preserves old code but is not recommended for new calculators
or tasks. ``currentProject()`` is **not deprecated** for GUI-selection use.

In Developer Mode, legacy access during process execution produces a warning,
once per module/component class, without affecting the calculation.
Set ``GTLAB_LEGACY_PROJECT_ACCESS_WARNING=1`` to enable it outside Developer
Mode, or ``=0`` to disable it. The override is read once on first use.
