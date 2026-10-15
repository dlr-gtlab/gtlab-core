Releasing GTlab Core
====================

GTlab Core uses `bump2version` (version 1.0.1) to update version files,
create a release commit, and create a Git tag.

New releases use SemVer tags such as `v2.1.0-alpha.1` or `v2.1.0`.
Historical tags in the `2-0-14` format are unchanged. The numeric CMake
project version remains `2.1.0` throughout the 2.1 prerelease cycle.

Release commands
----------------

Run commands in a clean working tree at the commit to be released. Inspect the
proposed changes first (use `--dry-run --verbose` before an actual bump):

.. code-block:: console

   bump2version --dry-run --verbose --new-version 2.1.0-alpha.1 stage
   bump2version --new-version 2.1.0-alpha.1 stage

To publish another prerelease, or to finish the prerelease sequence:

.. code-block:: console

   bump2version serial                            # 2.1.0-alpha.1 -> alpha.2
   bump2version --new-version 2.1.0-beta.1 stage  # alpha.N -> beta.1
   bump2version --new-version 2.1.0-rc.1 stage    # beta.N -> rc.1
   bump2version stage                             # rc.N -> 2.1.0

For **normal stable patch releases**, use the same command as before:

.. code-block:: console

   bump2version patch                             # 2.1.0 -> 2.1.1

The stage part has `first_value = stable`, so incrementing patch, minor or
major produces a stable version by default, without a `-dev` suffix.
For a patch prerelease instead, select it explicitly:

.. code-block:: console

   bump2version --new-version 2.1.2-alpha.1 patch

The `bump2version` command creates **both a Git commit and a local tag**.
Push the commit and tag after verification, for example:

.. code-block:: console

   git push origin master v2.1.0-alpha.1

Release notes and changelog
---------------------------

Alpha, beta and release candidate releases do **not** create individual
sections in `CHANGELOG.md`. Maintain the `[Unreleased 2.1]` section
throughout the 2.1 development cycle. For the final `2.1.0` release,
rename that section to `[2.1.0] - YYYY-MM-DD`, and open the next
unreleased section as needed.

For a stable patch release `2.1.1`, maintain an unreleased patch section and
finalize it to `[2.1.1] - YYYY-MM-DD` before releasing. Commit the
changelog edit **before** running `bump2version patch` so the release tag
includes it. The tool intentionally does not edit the changelog because it
must remain untouched for prereleases.

Create a GitHub Release for the pushed tag separately, marking alpha/beta/rc
tags as prereleases. Ensure the tag build and tests pass before publishing
release artifacts.
