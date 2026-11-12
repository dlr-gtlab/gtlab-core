Accessing Project Data
======================

Choose project access by what your code operates on:

* The **GUI-selected project** is the project selected in the active session.
* The **execution project** belongs to the running task or calculator. It can
  differ from the GUI-selected project, and execution may provide only a path.

An active execution context determines project lookup on the current thread.
This distinction is about the operation, not simply GUI versus worker threads.

GUI and application code
------------------------

Use ``gtApp->currentProject()`` for GUI actions, dialogs, wizards, and tools
that intentionally target the currently selected project, outside an execution
context. Handle ``nullptr`` when no project is selected:

.. code-block:: cpp

   void MyAction::trigger()
   {
       auto* project = gtApp->currentProject();
       if (!project) return;
       openEditor(project); // Module helper: open an editor for this project.
   }

A dialog, wizard, or MDI widget bound to a particular project should keep the
project or object supplied when it opens. For example, use a
``QPointer<GtProject>`` and check it before access. Switching the GUI selection
must not silently retarget that widget. A tool intended to follow the selection
can react to ``currentProjectChanged`` instead.

See :doc:`../modules/user_interface` for GUI integration.

Process execution
-----------------

Implement calculator logic in ``GtCalculator::run()``. GTlab's ``exec()``
prepares linked objects and execution state before calling ``run()``; new
calculators normally do not override ``exec()``.

Prefer the smallest dependency your calculation needs:

* Use ``data<T>(property)`` to obtain objects referenced by registered
  ``GtObjectLinkProperty`` or ``GtObjectPathProperty`` properties. These are
  objects available to the execution, rather than a lookup in the selected
  GUI project. Check for a missing object or an incompatible type.
* Use the inherited ``projectPath()`` when you only need the execution's
  project directory. Check for an empty path before constructing relative file
  names. There is no need to fetch a project object just to obtain its path.
* Pass required objects, paths, or values explicitly to reusable helpers.
  Helpers should not discover their inputs through ``gtApp``.

For example, ``m_partLink`` below is a registered ``GtObjectLinkProperty``;
``MyPart`` and ``exportPart()`` belong to the module:

.. code-block:: cpp

   bool ExportPartCalculator::run()
   {
       auto* part = data<MyPart*>(m_partLink);
       const QString path = projectPath();
       if (!part || path.isEmpty()) return false;
       return exportPart(*part, QDir(path).filePath("part.xml"));
   }

The same access rules apply in custom ``GtTask`` hooks such as ``setUp()`` and
``evaluate()``. Process objects may be cloned or detached from the persistent
project hierarchy, so do not use ``findParent<GtProject*>()`` to identify their
execution project. See :doc:`../basics/process_elements` for task hooks and
calculator setup.

``GtProcessComponent::projectPath()`` delegates to the associated runnable and
returns an empty string without one. ``GtRunnable::projectPath()`` returns the
active execution context's path, even if empty. Without a context, it uses a
supplied custom path, then the selected project's path.

If you genuinely need project metadata beyond the available objects or path,
include ``gt_executioncontext.h`` and use ``GtExecutionContext::current()``.
Check both the context and its ``project()`` pointer before reading the project;
a context can contain only a path. Prefer passing the required metadata to
helpers over making them depend on the current context.

During calculator execution, work on the available execution objects and use
the established task/result and merge mechanisms to apply results. Do not
directly mutate the persistent project through ``currentProject()`` or the
context's project pointer.

Background threads and asynchronous work
----------------------------------------

``GtExecutionContext::current()`` is thread-local. Threads created by a module
do not inherit it. ``GtExecutionContextScope`` installs a context on its current
thread and restores the previous context when the scope ends.

The current context and its project pointer are borrowed: they do not own the
project or extend its lifetime. Do not capture them for delayed callbacks or
retain them after execution. A ``QPointer`` can detect deletion but does not
keep a project alive or make its data safe to access from another thread.

Pass the required paths and values, or a suitable copy of the input data,
explicitly to background work. If a synchronous worker needs a project context,
its caller must ensure the context and project stay alive and that access is
safe, and install a scope on that worker thread. A queued callback must not
infer its original project later through a global lookup.

Legacy compatibility
--------------------

``gtApp->currentProject()`` delegates to ``GtCoreDatamodel::currentProject()``
when the application has a data model. Resolution follows these rules:

#. With an active execution context, return its ``project()``. A path-only
   context returns ``nullptr``; it does not fall back to the GUI selection.
#. Without an execution context, return the active session's selected project,
   or ``nullptr`` when none is available.

Existing calculators remain compatible: with an execution context their
``currentProject()`` calls resolve to the execution project. This describes
what the accessor returns; new execution code should use the more specific
APIs above. ``currentProject()`` is not globally deprecated and remains
appropriate for operations targeting the GUI-selected project.

In Developer Mode, accessing ``currentProject()`` while a process component
executes with an active context produces a warning. It identifies the component
and, when known, its module, at most once per module/component class per
application run. It guides module developers toward explicit execution data;
it does not change project resolution or calculation results. Access outside
process-component execution does not produce this warning.

Set ``GTLAB_LEGACY_PROJECT_ACCESS_WARNING`` before starting GTlab to override
the diagnostic (the value is read once):

* ``1``, ``true``, ``on``, ``yes``: enable warnings even outside Developer Mode.
* ``0``, ``false``, ``off``, ``no``: disable warnings, including in Developer Mode.
* Unset or any other value: warn in Developer Mode only.
