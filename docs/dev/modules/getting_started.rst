.. _getting-started:

Getting started
===============

The recommended way to begin a module is the `GTlab Module Generator
<https://gitlab.dlr.de/gtlab/public/development-tools/gtlab-module-generator>`_.
It creates the CMake target, plugin entry point, and initial source layout.
Use :ref:`create-basic-module` when you want to understand those pieces or
create a minimal module manually.

A practical first milestone is a module that:

#. configures with the installed GTlab CMake package;
#. builds and installs as a shared library;
#. appears in GTlab's module overview; and
#. registers one domain object, calculator, or user-interface contribution.

By default, GTlab looks for modules in its ``modules`` folder. If you install a
module somewhere else during development, add the extra search path in the
module-directory settings so GTlab can load it.

How modules fit into GTlab
--------------------------

A module is a shared library loaded by GTlab at runtime. Its entry-point class
implements :cpp:class:`GtModuleInterface`, which provides the stable module
identity, version, and description. The same class can implement optional
:doc:`interfaces <interfaces>` to advertise additional capabilities.

GTlab discovers these capabilities; module code does not register itself by
calling internal loader APIs. Registration functions return Qt metadata or
GTlab descriptors for the classes the module provides.

Development workflow
--------------------

#. Generate or create the module skeleton.
#. Keep the module ID stable from the first release onward.
#. Implement only the interfaces required for the first feature.
#. Build and install against the same GTlab and Qt versions used at runtime.
#. Start GTlab, inspect the module overview and output, and exercise the new
   contribution in a small project.
#. Add focused tests in the module repository before extending the next area.

Continue with :doc:`interfaces` to choose extension points. The
:doc:`../basics/data_modelling` and :doc:`../basics/process_elements` pages
explain the object and process concepts used by those interfaces.
