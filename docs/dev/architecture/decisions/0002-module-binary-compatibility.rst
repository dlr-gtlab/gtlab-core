Architecture decision 0002 — Module binary compatibility
=========================================================

:Status: Accepted
:Date: 2026-10-06
:Normative source: GitHub issue `#1590 <https://github.com/dlr-gtlab/gtlab-core/issues/1590>`_
:Related: `#618 <https://github.com/dlr-gtlab/gtlab-core/issues/618>`_, `#1588 <https://github.com/dlr-gtlab/gtlab-core/issues/1588>`_, and `#1589 <https://github.com/dlr-gtlab/gtlab-core/issues/1589>`_

Context
-------

GTlab loads modules as native Qt plugins. A module built against an
incompatible GTlab Core can fail during loading or execution. The Core product
version also changes for releases that do not break the module ABI, so it does
not by itself identify binary compatibility.

Decision
--------

GTlab assigns modules an explicit ABI generation through
``GTLAB_MODULE_ABI``. This value is the compatibility contract and is
independent of the GTlab Core product version. The initial generation is
``2.1``; later Core releases keep this value while they remain binary
compatible. The Core product version is recorded separately as module build
provenance.

The ``GTLAB_MODULE_ABI`` value has one source of truth in the Core build. The
build uses and exports it for all module integration points:

* the module filename, for example ``BasicTools.gtm.2.1.dll``;
* the Qt plugin metadata embedded by ``add_gtlab_module()``;
* the installed ``GTlabConfig.cmake`` used by external module projects; and
* the module loader's compatibility check.

The loader checks the ABI generation from the filename and plugin metadata
before it instantiates the plugin. It rejects a module when either value is
missing, when the values disagree, or when the generation differs from the
running Core. Discovery considers only libraries with the GTlab module naming
scheme. Qt's existing module interface identifier check remains in place.

Patch releases must not introduce module ABI changes and must keep
``GTLAB_MODULE_ABI`` unchanged. On a non-patch development line, the first
pull request that introduces a binary-incompatible change must bump
``GTLAB_MODULE_ABI`` in that pull request. This makes development builds
reject modules built for the previous ABI as soon as compatibility is broken.
Follow-up ABI-breaking changes in the same ABI generation keep the bumped
value; they do not each require another bump. A later pull request bumps the
value only when it intentionally establishes another compatibility boundary.
The pull request template asks authors to consider the ABI impact. A Core
product version change alone does not bump the module ABI.

Consequences
------------

* A compatible module ABI can span several Core product versions.
* Core version metadata helps identify the version used to build a module but
  does not determine whether the module ABI is compatible.
* Existing modules that do not provide the new filename and metadata must be
  rebuilt with the supported GTlab CMake integration.
* The ABI generation does not replace platform, architecture, compiler, or Qt
  compatibility requirements. Those requirements still apply to native Qt
  plugins.
