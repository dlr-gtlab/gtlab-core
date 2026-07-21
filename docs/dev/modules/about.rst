About modules
=============

GTlab has a plugin-based architecture. A *GTlab module* is a shared library
that adds domain functionality without changing the Core application. Modules
can contribute data models, editors and viewers, process elements, file-format
support, and other integrations.

Each module has one entry point and implements one or more well-defined
interfaces. The entry point supplies its identity and metadata; optional
interfaces describe the functionality GTlab should register.

.. figure:: ../images/modules/interfaces/module_interface.png
  :width: 400

Modules use Qt's `low-level plugin API
<https://doc.qt.io/qt-6/plugins-howto.html#the-low-level-api-extending-qt-applications>`_.
GTlab wraps the recurring build, installation, discovery, and registration
steps so module developers can concentrate on their domain types and tools.

Start with :doc:`getting_started`, then use :doc:`interfaces` to select the
extension points your module needs.
