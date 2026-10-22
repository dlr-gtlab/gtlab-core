Recording executions
====================

GTlab can record which data an execution reads and changes. Recording is
opt-in: a module provides an executor (for example Provenance) that decides
when to record and which objects to observe. Core provides the recording
lifecycle and forwards events; the module processes and stores them.

Without a recording executor, execution is unchanged and no recording
objects are created.

.. note::

   Recording was added in GTlab 2.1 (issue #1600). Modules and executor
   plugins must be rebuilt against the 2.1 headers. Reads are only tracked in
   code compiled against these headers.

Overview
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Class
     - Responsibility
   * - Executor (module)
     - Creates an observer and a ``GtRecordingSession`` per execution and runs
       the work through ``session.run()``.
   * - ``GtRecordingSession``
     - One recording: ID, parent ID, observed roots, event mask, start/finish,
       filtering and dispatch. Stores no events.
   * - ``GtRecordingObserver``
     - Implemented by the module. Receives lifecycle callbacks and events.

All classes live in the ``GTlabDataProcessor`` library
(``gt_recordingsession.h``, ``gt_recordingobserver.h``).

Write a recording executor
--------------------------

An executor is selected through the execution mode of a process component
(see :doc:`/basics/process_elements`). Children in the default ``parent`` mode
inherit it. The executor owns the observer and the session for one call:

.. code-block:: cpp

   bool
   MyRecordingExecutor::exec(GtCalculator* calc)
   {
       MyObserver observer;
       GtRecordingSession session{*calc, observer, calc->linkedObjects()};

       return session.run([&] { return calc->run(); });
   }

   bool
   MyRecordingExecutor::exec(GtTask* task)
   {
       MyObserver observer;
       GtRecordingSession session{*task, observer, task->linkedObjects()};

       return session.run([&] { return task->runIteration(); });
   }

Register the executor with ``GtCalculatorExecInterface`` in your module
interface (``executorId()``, ``executorDescription()``, ``executor()``); see
:doc:`interfaces/advanced/calculatorexec`.

``session.run()`` takes a callable returning ``bool``. It:

#. calls ``recordingStarted()``,
#. activates the session on the calling thread,
#. runs the work,
#. restores the previously active session,
#. calls ``recordingFinished(success)``.

A session is single-use. ``success`` describes the execution, not a later
project merge; failed executions are recorded too.

.. note::

   Link resolution and a task's ``setUp()`` run before the executor is
   called. ``setUp()`` is therefore not recorded.

Implement an observer
---------------------

Override only the callbacks you need:

.. code-block:: cpp

   class MyObserver : public GtRecordingObserver
   {
   public:
       void recordingStarted(const GtRecordingSession& session) override;
       void propertyRead(const GtObject& owner,
                         const GtAbstractProperty& property) override;
       void propertyWritten(const GtObject& owner,
                            const GtAbstractProperty& property) override;
       void objectValueRead(const GtObject& object) override;
       void objectAttached(const GtObject& parent,
                           const GtObject& child) override;
       void objectDetached(const GtObject& parent,
                           const GtObject& child) override;
       void recordingFinished(const GtRecordingSession& session,
                              bool success) override;
   };

Rules for observers:

* **Synchronous delivery.** Events arrive directly in the thread that caused
  them. If the recorded code uses worker threads, the observer must be
  thread-safe, for example with one accumulator per thread that is merged in
  ``recordingFinished()``.
* **No observations in callbacks.** While any callback runs, no recording is
  active on the thread. Callbacks may read properties, e.g. to capture a
  baseline in ``recordingStarted()``.
* **Exceptions are contained.** If ``recordingStarted()`` throws, the work
  runs without recording and ``recordingFinished()`` is not called. If an
  event callback throws, the session stops forwarding events. If
  ``recordingFinished()`` throws, a warning is logged. The execution result
  never changes.
* **Store identities, resolve later.** ``objectAttached()`` can deliver a
  child that is still being constructed: no valid UUID yet, derived class and
  properties not set up, and the derived constructor's ``setVal()`` calls
  produce write events. ``objectDetached()`` is also sent from ``~GtObject``,
  when only identity and UUID remain valid. Keep pointers during the run and
  resolve everything in ``recordingFinished()``.

What is recorded
----------------

The session observes its roots and all their descendants. The observed tree
is captured when ``run()`` starts and kept up to date when objects are
attached, detached, moved or deleted. Roots always stay observed.

.. list-table::
   :header-rows: 1
   :widths: 25 75

   * - Event
     - Reported when
   * - ``propertyRead``
     - A property of an observed object is read through ``get()``,
       ``getVal()``, ``operator()()`` or ``valueToVariant()``. Accessors that
       call each other produce one event. Mutable references from ``get()``
       count as reads.
   * - ``propertyWritten``
     - A property of an observed object emits ``changed()``, even if the
       value did not change.
   * - ``objectValueRead``
     - An accessor of an observed object marked with
       ``gt::ObjectValueReadScope`` runs (see below).
   * - ``objectAttached``
     - A child is attached to an observed parent (``appendChild()``, including
       ``GtObject(parent)``).
   * - ``objectDetached``
     - A child is detached from an observed parent
       (``disconnectFromParent()``, moves, deletion).

Properties are attributed to the object that registered them, including
sub-properties and property container entries. Properties registered with
``registerSilentProperty()`` are not recorded. Restrict the events with the
event mask of the session.

Record own accessors
^^^^^^^^^^^^^^^^^^^^

A ``GtObject`` subclass whose accessors return data that is not held in
registered properties marks them with ``gt::ObjectValueReadScope``
(``gt_recordinghooks.h``):

.. code-block:: cpp

   double
   MyObject::value() const
   {
       gt::ObjectValueReadScope readScope(*this);
       return m_value;
   }

If the object is observed, the observer receives ``objectValueRead(object)``.
The scope shares the nesting with property reads: only the outermost read is
reported, so property reads inside the accessor produce no extra event.
Without a running recording, the scope only checks a counter.

Nested recordings
-----------------

A child component gets its own recording only if its effective executor is
the recording executor. With the default ``parent`` mode this is the case for
all children of a recorded task. The child session:

* takes over the thread while it runs; only its observer gets events,
* has the task's recording as ``parentId()``,
* keeps the suspended task session's observed tree up to date.

A child in ``local`` mode runs in the task's recording. Its events reach the
task observer only for objects the task itself links.

Worker threads
--------------

Recording state is per thread and does not follow new threads. Wrap the
entry point of every worker whose accesses should be recorded:

.. code-block:: cpp

   std::thread worker(gt::propagateRecording([&] {
       calculateSomething();
   }));
   worker.join();

The wrapper runs the callable in the session that was active when the wrapper
was created. Without an active session, the callable runs unchanged. Join all
workers before the recorded work returns.

Limits
------

* A task's ``setUp()`` is not recorded.
* A task recording only sees a ``local`` child's accesses on objects the task
  links.
* Adding or removing property container entries produces no event.
* No recording in out-of-process runs (process runner) or for children on a
  remote executor.
* Code that reads property member values directly bypasses the read hooks.
  Own accessors are recorded only if they use ``gt::ObjectValueReadScope``.
* No ``objectAccessed()`` event; mutations through mutable references are
  reported as reads.

Checklist
---------

* The executor creates one observer and one session per call.
* The observer is thread-safe if the recorded code uses workers.
* Worker entry points are wrapped with ``gt::propagateRecording()``.
* Event callbacks store identities; resolution happens in
  ``recordingFinished()``.
