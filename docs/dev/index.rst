GTlab developer documentation
=============================

.. container:: landing-hero

   **Build domain-specific tools on a shared engineering platform.**

   Extend an installed GTlab with persistent data models, calculations,
   workflows, file formats, and user-interface components.

.. note:: Looking for instructions on using the application?

   Start with the `GTlab user documentation
   <https://gtlab.readthedocs.io/projects/user/>`_. This site is for module
   developers, module maintainers, and GTlab Core contributors.

Choose your path
----------------

.. list-table::
   :class: landing-paths
   :widths: 50 50

   * - **Develop a module**

       **Recommended starting point**

       Understand the plugin architecture, create a first module, and select
       the extension interfaces your feature needs.

       :doc:`Start module development → <modules/about>`
     - **Understand GTlab concepts**

       Learn how persistent objects, properties, calculators, and workflow
       tasks fit together before designing your domain model.

       :doc:`Explore the concepts → <basics/data_modelling>`
   * - **Maintain an existing module**

       Update build integration and deprecated APIs, or find focused answers
       to common module-development problems.

       :doc:`Open the migration guide → <howtos/migration_and_deprecation>`
     - **Contribute to GTlab Core**

       Build the framework from source and use the generated API reference.
       Module developers normally do not need this path.

       :doc:`View Core build instructions → <basics/building>`

Your first module
-----------------

.. container:: landing-workflow

   **1 · Understand** — learn how modules fit into GTlab

   **2 · Create** — build and load a minimal module

   **3 · Extend** — add only the interfaces your feature needs

.. container:: landing-actions

   :doc:`About modules <modules/about>` ·
   :doc:`Getting started <modules/getting_started>` ·
   :doc:`Create a module from scratch <howtos/basic_module>` ·
   :doc:`Choose interfaces <modules/interfaces>`

.. toctree::
   :caption: Start here
   :maxdepth: 2
   :hidden:

   modules/about
   modules/getting_started

.. toctree::
   :caption: Module development
   :maxdepth: 2
   :hidden:

   howtos/basic_module
   modules/interfaces
   modules/shared_functions
   modules/user_interface
   modules/logging

.. toctree::
   :caption: Concepts
   :maxdepth: 2
   :hidden:

   basics/data_modelling
   basics/process_elements

.. toctree::
   :caption: Maintenance and support
   :maxdepth: 2
   :hidden:

   howtos/migration_and_deprecation
   faq
   changelog

.. toctree::
   :caption: Core contributors
   :maxdepth: 1
   :hidden:

   basics/building
   api
   genindex
