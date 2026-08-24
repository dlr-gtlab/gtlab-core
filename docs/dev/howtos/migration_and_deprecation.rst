Migration & Deprecation Policy
==============================

GTlab regularly evolves its API to improve usability, performance, and design.
As part of this evolution, functions and types are phased out in a controlled
way so module developers and library users can migrate smoothly.

This guide covers:

- GTlab's deprecation lifecycle,
- Migration to GTlab 2.1,
- Compiler and tooling support for migration.

Deprecation Lifecycle
---------------------

GTlab adheres to a two-step deprecation process:

1. An API is first marked as deprecated and remains available for a couple of
   releases, where it emits warnings.
2. In the removal release (for example 2.1 or 3.0), the API is removed.

This gives module developers multiple release cycles to update their code
before breaking changes occur.

Deprecation Warnings
--------------------

When GTlab deprecates an API, users see warnings like:

    warning: This deprecated function will be removed in GTlab version 2.1: Use newFunction instead

If your compiler or build settings suppress warnings by default, enable warnings
so deprecation messages are visible early (see *Compiler & Tooling Tips*).

Compiler & Tooling Tips
-----------------------

Compilers often provide advanced features that may aid in the migration to a new Qt/GTlab version.

Enforcing Deprecation Warnings
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Some compilers or build setups suppress warnings by default. To ensure warnings
are visible:

- **GCC / Clang / AppleClang**
  Enable warnings with:

      -Wall -Wextra -Wdeprecated-declarations

- **MSVC**
  Ensure warning level is not too low (e.g. ``/W4``), and enable:

      /w14996

If your build treats warnings as errors (``-Werror`` or similar), enabling this
during migration can catch issues early.

Strict Mode
~~~~~~~~~~~

During migration releases, you may want to treat deprecation warnings as errors.
Add this to your CMake target:

.. code-block:: cmake

  target_compile_definitions(my_target PRIVATE GT_STRICT_DEPRECATIONS=1)

This makes deprecated API usage a compile-time error, even during migration
releases.

----

Migration Plans
---------------

.. toctree::
   :maxdepth: 1

   migration_2-1
   migration_2-2
