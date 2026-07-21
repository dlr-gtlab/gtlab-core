GTlab developer documentation
=============================

GTlab is an extensible application framework for collaborative engineering
workflows. This documentation explains how to extend an installed GTlab with
domain objects, calculations, workflows, and user-interface components.

If you want to operate GTlab rather than develop an extension, start with the
`GTlab user documentation <https://gtlab.readthedocs.io/projects/user/>`_.

Choose your path
----------------

I want to develop a module
~~~~~~~~~~~~~~~~~~~~~~~~~~

Start with :doc:`modules/about` for the architecture, then follow
:doc:`modules/getting_started` to create and load a first module. Use
:doc:`modules/interfaces` to choose only the extension points your feature
needs. The complete minimal project is available in
:doc:`howtos/basic_module`.

I need to understand GTlab concepts
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Read :doc:`basics/data_modelling` before designing persistent domain classes.
Read :doc:`basics/process_elements` before implementing calculators or custom
workflow tasks. These concepts are shared by modules and the Core framework.

I maintain an existing module
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use :doc:`howtos/migration_and_deprecation` when updating a module to a newer
GTlab version. :doc:`faq` collects focused solutions to common integration
problems.

I contribute to GTlab Core
~~~~~~~~~~~~~~~~~~~~~~~~~~

Core contributors can find source-build instructions in
:doc:`basics/building` and API declarations in :doc:`api`. Module developers
normally use an installed GTlab development package and do not need to build
the Core framework.

.. toctree::
   :caption: Start here
   :maxdepth: 2

   modules/about
   modules/getting_started

.. toctree::
   :caption: Module development
   :maxdepth: 2

   howtos/basic_module
   modules/interfaces
   modules/shared_functions
   modules/user_interface
   modules/logging

.. toctree::
   :caption: Concepts
   :maxdepth: 2

   basics/data_modelling
   basics/process_elements

.. toctree::
   :caption: Maintenance and support
   :maxdepth: 2

   howtos/migration_and_deprecation
   faq
   changelog

.. toctree::
   :caption: Core contributors
   :maxdepth: 1

   basics/building
   api
   genindex
