Advanced interfaces
===================

Advanced interfaces connect a module to specialized GTlab subsystems. Most
new modules do not need them initially. First make the module load and validate
its basic data-model, process, or GUI contribution; then add the relevant
advanced interface.

.. list-table:: Specialized extension points
   :widths: 30 70
   :header-rows: 1

   * - Interface
     - Use it to
   * - :doc:`advanced/property`
     - register custom property types and their editors
   * - :doc:`advanced/calculatorexec`
     - provide an alternative calculator execution environment
   * - :doc:`advanced/collection`
     - integrate a module-specific collection structure
   * - :doc:`advanced/network`
     - provide module-specific network access

These interfaces use the same inheritance, ``Q_INTERFACES``, linking, and
registration pattern described in :doc:`../interfaces`. Consult the individual
page for the descriptors required by each subsystem.

.. toctree::
   :maxdepth: 1
   :hidden:

   advanced/property
   advanced/calculatorexec
   advanced/collection
   advanced/network
