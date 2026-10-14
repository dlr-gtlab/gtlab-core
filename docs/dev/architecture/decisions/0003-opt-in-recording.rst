Architecture decision 0003 — Opt-in recording sessions and observer hooks
==========================================================================

:Status: Accepted
:Date: 2026-10-14
:Normative source: GitHub issue `#1600 <https://github.com/dlr-gtlab/gtlab-core/issues/1600>`_
:Related: ADR 0002, `#1596 <https://github.com/dlr-gtlab/gtlab-core/pull/1596>`_
:Developer guide: :doc:`/modules/recording`

Context
-------

Provenance needs to know which data an execution reads and changes. An
earlier prototype (``feature/accesstracking-per-context``) recorded every
execution automatically with a Core-owned recorder, manager and tracker.

Recording must instead be opt-in: a module-provided executor (initially
Provenance) decides when to record and which objects to observe. Core
provides the lifecycle and forwards events; the module processes and stores
them. Normal execution without a recording executor must not change.

Decision
--------

Classes and placement
~~~~~~~~~~~~~~~~~~~~~

* ``GtRecordingSession`` (one recording of one invocation) and
  ``GtRecordingObserver`` (implemented by the module) live in the
  ``GTlabDataProcessor`` library, next to ``GtObject`` and
  ``GtAbstractProperty``. No dependency from the data model to Core and no
  bridge are needed; the recorded component is passed as ``const GtObject&``.
* The executor owns the observer and the session for one call and runs the
  work through ``session.run()``. The session stores no events and does not
  own child sessions.
* Internal hooks live in ``gt_recordinghooks.h`` (``gt::detail``). The
  activation helper ``gt::detail::GtRecordingScope`` is exported because the
  header template ``gt::propagateRecording()`` needs it.

Execution boundary
~~~~~~~~~~~~~~~~~~

* Calculators record ``run()``; tasks record ``runIteration()``.
* Link resolution and a task's ``setUp()`` run before the executor is called
  (ADR 0002), so ``setUp()`` is not recorded.
* ``run()`` takes a callable returning ``bool``. On an exception, the
  recording finishes with ``success = false``, the previous session is
  restored and the exception is rethrown.

Active session
~~~~~~~~~~~~~~

* A thread-local pointer holds the session active on each thread.
  ``start()`` stores the previous value as the parent and activates the
  session; ``finish()`` restores the parent. The parent ID is therefore set
  automatically.
* An exported atomic counter of running sessions is checked inline before any
  thread-local lookup, so the no-session path of the inline property getters
  costs one relaxed load.
* While any observer callback runs, the thread has no active session:
  callbacks never create observations and may read properties.
* Observer exceptions are caught at every dispatch: a failing
  ``recordingStarted()`` runs the work unrecorded (the session stays active so
  nothing leaks into a parent session), a failing event callback stops event
  delivery for the session, a failing ``recordingFinished()`` is logged. The
  execution result never changes.

Observed objects
~~~~~~~~~~~~~~~~

* Each component observes its own ``linkedObjects()`` and their descendants.
  ``GtProcessComponent::linkedObjects()`` is public for this (ADR 0002).
* The observed set is built when ``run()`` starts and kept up to date on
  attach, detach, move and deletion, including in suspended parent sessions.
  Roots always stay observed. A read-write lock protects the set.
* Attach and detach events are filtered by the parent: the new parent for
  attach, the old parent for detach.

Property hooks
~~~~~~~~~~~~~~

* A property stores its owning object in ``GtAbstractProperty::Impl``
  (``ownerObject()``). It is set on ``registerProperty()``, propagated to
  sub-properties (also ones added later) and to property container entries.
  Properties registered with ``registerSilentProperty()`` get no owner and are
  not recorded.
* Reads: ``get()``, ``getVal()``, ``operator()()``, ``valueToVariant()`` and
  the overrides that read the value directly open a ``PropertyReadScope``.
  A per-thread depth counter reports only the outermost read, so accessors
  that call each other produce one event.
* Writes: setting the owner adds one permanent ``Qt::DirectConnection`` from
  ``changed()`` to the recording hook, so writes are reported in the thread
  that caused them. A write means the property reported a change, not
  necessarily a new value.
* ``GtGroupProperty`` rebuilds a summary from all sub-property values on every
  change. This bookkeeping runs in a ``SuppressRecordingScope``; otherwise
  every write to a property container entry also reported reads of all its
  siblings.

Object tree hooks
~~~~~~~~~~~~~~~~~

* ``GtObject::appendChild()`` reports ``objectAttached`` once the parent was
  actually set (Qt refuses cross-thread reparenting).
  ``disconnectFromParent()`` reports ``objectDetached``; a move reports both.
* ``~GtObject`` reports ``objectDetached`` and removes the object from all
  observed sets, so a new object at the same address is not treated as
  observed. Only identity and UUID are valid at that point.
* ``GtObject(parent)`` attaches before the UUID is assigned and before the
  derived class is constructed. Observers keep pointers during the run and
  resolve everything in ``recordingFinished()``.

Nested recordings and threads
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* A child gets its own session only if its effective executor is the
  recording executor; with the default ``parent`` mode this applies to all
  children of a recorded task. Other children run in the parent's session.
* A child session takes over the thread; the suspended parent only receives
  membership updates, no events.
* All recording state is per thread. ``gt::propagateRecording()`` carries the
  session into worker threads. One session accepts concurrent events, which
  also prepares for child components running in parallel; the task's thread
  is assumed to wait for them.

Consequences
------------

* Provenance can record executions without Core storing any events.
* Every property gets one additional connection, also when recording is never
  used (see Performance).
* ``GtAbstractProperty``, ``GtPropertyStructContainer`` and ``GtObject``
  change behaviour in their registration, accessor and destruction paths;
  modules must be rebuilt for 2.1. Reads are only tracked in code compiled
  against the new headers.
* The test process module registers ``TestRecordingExecutor``; anything that
  loads the test modules sees one more execution mode.

V1 limits
~~~~~~~~~

* A task's ``setUp()`` is not recorded.
* A task recording only sees a ``local`` child's accesses on objects the task
  itself links.
* Adding or removing property container entries produces no event.
* No recording in out-of-process runs (process runner) or for children on a
  remote executor.
* Code reading property member values directly is not tracked; mutations
  through mutable references are reported as reads; no ``objectAccessed()``.

Performance
-----------

Measured with the disabled benchmark
``tests/unittests/datamodel/test_gt_recordingbenchmark.cpp`` (Release, MSVC,
Windows 11, best of five runs per value, two rounds each). The baseline is the
commit before the recording changes.

.. list-table:: Without any recording session
   :header-rows: 1
   :widths: 46 18 18 18

   * - Measurement
     - Baseline
     - Recording
     - Difference
   * - ``getVal()``
     - 1.15 ns
     - 1.38 ns
     - +0.2 ns
   * - ``valueToVariant()``
     - 8.9 ns
     - 9.9 ns
     - +11 %
   * - ``setVal()``
     - 32 ns
     - 45 ns
     - +39 %
   * - Construct 100,000 objects (10 properties)
     - 849 ms
     - 1030 ms
     - +21 %
   * - Memory per object
     - 10,609 B
     - 12,276 B
     - +16 %
   * - Restore memento with 20,000 objects
     - 262 ms
     - 304 ms
     - +16 %

.. list-table:: With a recording session (recording build only)
   :header-rows: 1
   :widths: 46 27 27

   * - Scenario
     - ``getVal()``
     - ``setVal()``
   * - Session running on another thread
     - 3.5 ns
     - 45 ns
   * - Session active, object not observed
     - 17 ns
     - 58 ns
   * - Session active, object observed
     - 22 ns
     - 64 ns

Findings:

* Reads without a session are practically unaffected.
* The additional ``changed()`` connection per property (about 167 bytes each)
  causes the write, construction, memory and loading overhead, even when
  recording is never used.
* During recording, the locked membership lookup costs about 15 ns per read.
  This only applies while a session runs and is accepted for V1.

Follow-up: single connection per property
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``GtObject::connectProperty()`` already connects every registered property's
``changed()`` to a lambda that emits ``dataChanged()``. That lambda could call
the recording hook, removing the second connection for all registered
properties:

* The existing connection is an auto connection. A write from a worker thread
  is queued and would run the hook in the object's thread with the wrong
  session.
* Proposal: make it a direct connection that calls the recording hook first,
  then emits ``dataChanged()`` directly on the object's thread, or posts it to
  the object's thread otherwise. Existing receivers see the same behaviour.
* The separate connection then remains only for paths ``connectProperty()``
  does not cover: sub-properties added after registration and property
  container entries.
* Expected effect: memory, construction and loading close to the baseline,
  ``setVal()`` close to 32 ns.

This changes the existing ``changed()`` handling, which issue #1600 asked to
leave untouched, and is therefore not part of V1. Re-measure with the
benchmark when implementing it.

Validation
----------

* Unit tests in ``tests/unittests/datamodel/test_gt_recording*.cpp`` cover the
  session lifecycle and failures, property hooks, object tree changes, worker
  threads and parallel child components.
* ``SystemTest.Recording`` (``tests/systemtests/recording``) runs a recorded
  task through ``GTlabConsole`` and checks that ``setUp()`` runs once outside
  the recording, the task iteration and a ``local`` child are recorded in the
  task session, and an inheriting child gets a nested recording.
