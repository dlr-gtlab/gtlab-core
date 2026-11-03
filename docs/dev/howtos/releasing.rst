Releasing GTlab Core
====================

GTlab Core uses ``bump2version`` (version 1.0.1) to update the version files.
By default, it creates a version commit and a local Git tag using the SemVer
format (for example, ``v2.1.0`` or ``v2.1.0-alpha.1``). Older tags such as
``2-0-14`` remain unchanged.

Run release commands from a clean working tree. Use ``--dry-run --verbose``
to inspect a bump before executing it. The CMake project version contains
only the numeric major, minor and patch components.


Stable releases (major, minor, patch)
-------------------------------------

When the current version is already **stable**, choose the required increment:

.. code-block:: console

   # Starting from 2.1.0, choose one:
   bump2version patch   # 2.1.0 -> 2.1.1
   bump2version minor   # 2.1.0 -> 2.2.0
   bump2version major   # 2.1.0 -> 3.0.0

When the working version already targets the intended release (for example
``2.1.1-dev`` or ``2.1.0-rc.1.dev``), **do not increment the numeric version
again**. Instead, explicitly select the final version:

.. code-block:: console

   bump2version --new-version 2.1.1 stage  # 2.1.1-dev -> 2.1.1
   bump2version --new-version 2.1.0 stage  # 2.1.0-rc.1.dev -> 2.1.0

Before creating a stable release, finalize the corresponding changelog section
(for example, ``[2.1.0] - YYYY-MM-DD`` or ``[2.1.1] - YYYY-MM-DD``) and commit
it. ``bump2version`` does not update ``CHANGELOG.md`` automatically.

After the release bump:

1. Push the release commit and tag, for example
   ``git push origin master v2.1.1``.
2. Check the tag build and tests, then publish a GitHub Release for the tag.
3. Move ``master`` to a development version as described below.


Development versions
--------------------

After every release, move ``master`` to a distinguishable **development**
version. Always use ``--no-tag``: the version change should create a commit,
but it must **not** create a release tag.

After a numbered prerelease, append ``.dev`` to the released version:

.. code-block:: console

   bump2version --no-tag development  # 2.1.0-alpha.1 -> 2.1.0-alpha.1.dev

After a stable release, choose the version currently being developed.
Examples starting from ``2.1.0``:

.. code-block:: console

   # Choose the next planned development version:
   bump2version --no-tag --new-version 2.1.1-dev patch  # patch development
   bump2version --no-tag --new-version 2.2.0-dev minor  # minor development
   bump2version --no-tag --new-version 3.0.0-dev major  # major development

Push the development commit with ``git push origin master``.

.. important::

   From ``2.1.1-dev``, a plain ``bump2version patch`` would produce
   ``2.1.2`` and skip the intended patch release. Use
   ``bump2version --new-version 2.1.1 stage`` to release ``2.1.1``.

   SemVer sorts ``2.1.0-alpha.1.dev`` after ``2.1.0-alpha.1`` and before
   ``2.1.0-alpha.2``. Do not use ``alpha.2.dev`` immediately after
   publishing ``alpha.1``.


Prereleases (alpha, beta, rc)
-----------------------------

Prereleases use the same versioning tool and receive normal release tags.
For example, starting from ``2.1.0-dev``:

.. code-block:: console

   bump2version --new-version 2.1.0-alpha.1 stage

To release further prereleases, choose the appropriate command:

.. code-block:: console

   bump2version serial                           # alpha.1.dev -> alpha.2
   bump2version --new-version 2.1.0-beta.1 stage # alpha.N.dev -> beta.1
   bump2version --new-version 2.1.0-rc.1 stage   # beta.N.dev -> rc.1

The same pattern can be used for a patch prerelease, for example
``bump2version --new-version 2.1.1-alpha.1 patch``.

After **each** prerelease, push the tagged release commit, publish it as a
GitHub *prerelease* after the tag build passes, and then create the untagged
``.dev`` commit using the development procedure above.

Do not create individual changelog sections for alpha, beta or rc releases.
Keep changes under ``[Unreleased 2.1]`` throughout the 2.1 prerelease cycle.
When publishing the final ``2.1.0`` release, consolidate those entries as
described in the stable-release section.
