Recording glossary
==================

Terms used for recording executions (see :doc:`recording`).

.. glossary::

   Recording session
      One recording of one execution of a process component.
      *Avoid:* "a recording" as a countable noun; "recording" alone means the
      activity.

   Parent session
      The recording session that was active when another recording session
      started. It is suspended while the nested session runs.
      *Avoid:* outer session.

   Nested session
      A recording session started while another one was active.
      *Avoid:* child session, inner session.

   Observer
      The module-provided receiver of the events and lifecycle callbacks of a
      recording session.

   Recording executor
      An executor that runs a process component inside a recording session.

   Observed root
      An object given to a recording session when it is created. It stays
      observed for the whole session, wherever it moves.

   Observed object
      An observed root or one of its current descendants.
      *Avoid:* observed tree as a term; linked object in the recording sense.

   Event
      A reported access to or change of an observed object, forwarded to the
      observer if the event mask contains its kind.
      *Avoid:* observation, notification.

   Event mask
      The set of event kinds a recording session forwards to its observer.

   Lifecycle callback
      The start or finish of a recording session as reported to its observer.
      Not an event, so the event mask does not apply.

   Write
      A change reported by a property or an object, even if the value stayed
      the same.
      *Avoid:* modification, update.

   Object value
      Data of an object that is not held in a registered property, read and
      changed through the object's own accessors.

   Hook
      A point in the data model that reports an event to the active recording
      session.
      *Avoid:* notification.
