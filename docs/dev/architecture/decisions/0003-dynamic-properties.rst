Architecture decision 0003 — Dynamic properties
===============================================

:Status: Proposed
:Date: 2026-10-20
:Related: `PR #1626 <https://github.com/dlr-gtlab/gtlab-core/pull/1626>`_

Context
-------

Static properties are created by the ``GtObject`` constructor. Dynamic
properties are added later, so restoring their values alone is not enough.
Project files and Undo/Redo snapshots must also describe how to recreate them.
See :doc:`../../basics/data_modelling` for the API.

Decision
--------

* ``GtObject`` owns its dynamic properties. Their IDs share the object's
  property namespace, including sub-properties.
* Property constructors are registered on ``GtAbstractObjectFactory`` under
  stable, namespaced type IDs. Modules provide them through
  ``GtDatamodelInterface::propertyFactories()``. This lets Memento restore
  module properties without knowing their C++ classes.

Configurations
~~~~~~~~~~~~~~

* ``gt::PropertyConfiguration`` stores type-specific construction settings
  (e.g. bounds, initial value, unit category) as a JSON-like ``QVariantMap``.
  Current values remain in ``PropertyData``. Configuration is separate because
  the current value does not describe how the property was constructed.
* ``GtObjectMemento::DynamicPropertyData`` stores type ID, property metadata,
  configuration and values. Project XML and Undo/Redo use this same definition;
  XML preserves configuration value types. Restoration validates the saved
  definitions and prepares replacements before changing the object. Unchanged
  definitions keep existing property instances and their connections.
* An unavailable type or unsupported configuration becomes a read-only
  ``GtDummyProperty``. It retains the complete saved record so loading and
  saving without the module does not discard property data.

Consequences
------------

Type IDs and configuration keys are part of the project-file contract.
Module changes must keep old configurations readable or leave their data
intact as dummy properties. Mementos must capture definition changes as well
as value changes for Undo/Redo.
