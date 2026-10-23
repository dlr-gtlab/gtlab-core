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

* Calculators record ``run()``; tasks record ``setUp()`` and
  ``runIteration()``.
* Link resolution runs before the executor is called (ADR 0002) and is not
  recorded. ``GtTask::exec()`` leaves ``setUp()`` to the executor, which
  calls it inside the recording right before ``runIteration()``.
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
* Writes: properties report changes through
  ``GtAbstractProperty::notifyChanged()``, which calls the recording hook in
  the thread that caused the change (see Owner notification). A write means
  the property reported a change, not necessarily a new value.
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
* Registered properties no longer use Qt connections to notify their object
  or the recording hook (see Owner notification). This lowers memory and
  write costs below the state before recording (see Performance).
* Property classes that emit ``changed()`` directly instead of calling
  ``notifyChanged()`` neither mark their object as changed nor are recorded.
* ``GtAbstractProperty``, ``GtPropertyStructContainer`` and ``GtObject``
  change behaviour in their registration, accessor and destruction paths;
  modules must be rebuilt for 2.1. Reads are only tracked in code compiled
  against the new headers.
* The test process module registers ``TestRecordingExecutor``; anything that
  loads the test modules sees one more execution mode.

V1 limits
~~~~~~~~~

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
commit before the recording changes. V1 is the first implementation with an
additional recording connection per property; current is the implementation
with owner notification.

.. list-table:: Without any recording session
   :header-rows: 1
   :widths: 40 15 15 15 15

   * - Measurement
     - Baseline
     - V1
     - Current
     - Current vs. baseline
   * - ``getVal()``
     - 1.15 ns
     - 1.38 ns
     - 1.40 ns
     - +0.25 ns
   * - ``valueToVariant()``
     - 8.9 ns
     - 9.9 ns
     - 10.1 ns
     - +13 %
   * - ``setVal()``
     - 32 ns
     - 45 ns
     - 18.5 ns
     - -42 %
   * - Construct 100,000 objects (10 properties)
     - 849 ms
     - 1030 ms
     - 559 ms
     - -34 %
   * - Memory per object
     - 10,609 B
     - 12,276 B
     - 7,001 B
     - -34 %
   * - Restore memento with 20,000 objects
     - 262 ms
     - 304 ms
     - 195 ms
     - -26 %

.. list-table:: With a recording session (recording builds only)
   :header-rows: 1
   :widths: 46 18 18 18

   * - Scenario
     - ``getVal()``
     - ``setVal()`` V1
     - ``setVal()`` current
   * - Session running on another thread
     - 4.0 ns
     - 45 ns
     - 18.5 ns
   * - Session active, object not observed
     - 17 ns
     - 58 ns
     - 28 ns
   * - Session active, object observed
     - 23 ns
     - 64 ns
     - 34 ns

Findings:

* Reads without a session are practically unaffected.
* In V1, the additional ``changed()`` connection per property (about 167 bytes
  each) caused the write, construction, memory and loading overhead, even when
  recording was never used.
* With owner notification, a registered property needs no connection for its
  object or for recording. This saves about 530 bytes per registered
  property compared to V1 (1,114 B to 584 B per ``GtDoubleProperty``,
  measured separately with 20,000 objects). Unregistered properties grow by
  16 bytes for the guarded owner pointer.
* During recording, the locked membership lookup costs about 15 ns per read.
  This only applies while a session runs and is accepted for V1.

Owner notification
~~~~~~~~~~~~~~~~~~

Before, ``GtObject::connectProperty()`` connected every registered property's
``changed()`` to a lambda (auto connection) that set ``HasOwnChanges`` and
emitted ``dataChanged()``. V1 added a second connection for the recording
hook. Each connection costs memory per property and time per write.

``GtAbstractProperty::notifyChanged()`` replaces both connections:

#. If the property has an owner, it calls the recording hook in the thread
   that changed the property.
#. If the property was registered directly, or was a sub-property at
   registration time, it sets ``HasOwnChanges`` on the owner and emits
   ``dataChanged()``. In another thread than the owner's, this is posted to
   the owner's thread, like the former auto connection.
#. It emits ``changed()`` for all other receivers.

Details:

* ``GtProperty<T>`` setters, ``setActive()`` and ``GtColorProperty`` call
  ``notifyChanged()`` instead of emitting ``changed()``.
* The owner is stored as ``QPointer``: a property that outlives its object no
  longer accesses it.
* Behaviour is kept for the paths ``connectProperty()`` did not cover:
  property container entries notify their object through the container
  signals, and sub-properties added after registration do not mark the object
  as changed. Both are recorded.
* The owner is now notified before other ``changed()`` receivers. Before, the
  object's connection was usually the first one as well.

Issue #1600 asked to leave the ``changed()`` handling untouched. The change is
accepted because modules must be rebuilt for 2.1 anyway, and it makes
recording free of memory overhead while also reducing the costs of normal
execution. Property classes in modules that emit ``changed()`` directly must
call ``notifyChanged()`` instead (see :doc:`/howtos/migration_2-1`).

Validation
----------

* Unit tests in ``tests/unittests/datamodel/test_gt_recording*.cpp`` cover the
  session lifecycle and failures, property hooks, object tree changes, worker
  threads and parallel child components.
* ``GtObjectPropertyNotification`` tests in
  ``tests/unittests/datamodel/test_gt_object.cpp`` cover owner notification,
  unregistered properties, owners destroyed before their properties and
  changes from other threads.
* ``SystemTest.Recording`` (``tests/systemtests/recording``) runs a recorded
  task through ``GTlabConsole`` and checks that ``setUp()`` runs once inside
  the recording, the task iteration and a ``local`` child are recorded in the
  task session, and an inheriting child gets a nested recording.
