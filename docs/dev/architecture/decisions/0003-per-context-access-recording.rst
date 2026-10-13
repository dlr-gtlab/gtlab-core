Architecture decision 0003 — Per-context access recording
=========================================================

:Status: Accepted
:Date: 2026-10-13
:Normative source: GitHub issue `#1600 <https://github.com/dlr-gtlab/gtlab-core/issues/1600>`_
:Related: `#1601 <https://github.com/dlr-gtlab/gtlab-core/issues/1601>`_

Context
-------

Access recording captures, for each process component execution, the identity of the
executed component, its start and finish timestamps (UTC), its final process state, and
the set of object identifiers that were read while it ran. Recordings are grouped into a
tree through a ``parentId`` link.

Two kinds of recording exist. *Per-component* recordings are created by
``GtProcessComponent::execute()`` for each component as it runs; each nests under the
recording of its parent component. *Context* (execution-level) recordings are created by
``GtCoreProcessExecutor`` for the whole task execution; they carry the identity of the
executed task and the working objects of the execution (the mementized source children
transferred to the runnable).

Originally the context recording was *passive*: it had no access tracker and only
carried its prefilled working objects, while the per-component recordings were the ones
with live trackers. The two kinds formed unlinked, parallel hierarchies. They shared the
same ``componentUuid`` (the executed task's uuid, which object cloning preserves) and
were told apart only by the contents of their ``accessedObjects`` sets. In addition, two
pieces of access-tracking API were dead code: ``GtAbstractRunnable::accessTracker()``
(a lazily created per-execution tracker) and a ``std::shared_ptr``-based
``GtAccessTrackerScope`` constructor. Neither had any call site.

Decision
--------

The context recording becomes the *active* root of the per-execution hierarchy.

Active context recording
~~~~~~~~~~~~~~~~~~~~~~~~

``GtCoreProcessExecutor`` now gives the context recording its own ``GtAccessTracker``
and starts it, in addition to prefilling the working objects. The recording is handed to
the ``GtRunnable`` for the execution through ``GtRunnable::setContextRecording``; the
runnable borrows the pointer (the executor keeps ownership) and activates the recording
on the worker thread in ``run()`` through the existing ``GtRecordingScope`` machinery.
Property accesses made on the worker thread outside any per-component scope (for example
while transferring the input mementos before the first component runs) are routed into
the context recording's tracker.

Linked hierarchy
~~~~~~~~~~~~~~~~

Because the context recording is active on the worker thread, the per-component
recordings that ``GtProcessComponent::execute()`` opens inside it now nest under it:
their ``parentId`` is the context recording's id. The per-execution hierarchy is a
single linked tree rooted at the context recording:

* the context (execution-level) recording is the only top-level recording of the run;
* the per-component recordings for the task and its children are its children, nested
  under each other as before.

The ``parentId`` link is the authoritative way to tell the context recording apart from
the executed task's own per-component recording; the two share the task's
``componentUuid``.

Dead API removed
~~~~~~~~~~~~~~~~

``GtAbstractRunnable::accessTracker()`` and its backing ``m_accessTracker`` member, as
well as the ``std::shared_ptr``-based ``GtAccessTrackerScope`` constructor, are removed.
The raw-pointer scope constructor remains the only way to activate a tracker on a
thread.

Threading
~~~~~~~~~

A recording instance is not thread-safe. The supported lifecycle is: create, prefill and
start the recording on the owning thread (the executor); activate it on the worker
thread, where the scope only reads the recording and routes property accesses into its
tracker; and finalize and deliver it on the owning thread after the worker has finished.
The thread-local activation slots keep threads isolated but do not make the recording
thread-safe.

Consequences
------------

* The context recording now carries live access data (the working objects plus any
  objects touched outside per-component scopes on the worker thread), not only the
  prefilled working objects.
* The per-execution hierarchy is one linked tree rooted at the context recording.
  Consumers identify the context recording through ``parentId`` rather than through the
  contents of ``accessedObjects``.
* The context recording and the executed task's per-component recording share the task's
  ``componentUuid`` (object cloning preserves the uuid); the parent link is the
  disambiguator.
* ``GtCoreProcessExecutor`` owns and delivers the context recording; the ``GtRunnable``
  borrows a pointer to it and outlives nothing.
* The access-tracking API is smaller: no per-runnable tracker, no ``shared_ptr`` scope
  constructor. Both removed members were dead code, so no call sites remain.

Validation
----------

The unit tests in ``tests/unittests/core/test_gt_accessrecording.cpp`` cover the
executor-driven hierarchy (the task-level recording nests under the context recording,
and recordings stay separate across sequential executions) and the scope-level
activation semantics (an active context recording captures exactly the accesses made
outside the nested per-component scopes).
