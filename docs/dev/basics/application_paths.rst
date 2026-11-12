.. SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
.. SPDX-License-Identifier: MPL-2.0+

Application Paths
=================

GTlab needs directories for settings, user data, caches, state, process
coordination, and temporary files. Use ``GtApplicationPaths::directory`` to
resolve these locations instead of writing under the installation directory.
The API creates a missing directory. It does not guarantee that a later write
will succeed, so handle errors from the file operation itself.

Locations
---------

The examples below assume a typical, non-sandboxed Linux or Windows
installation and the application name ``GTlab 2``. They show typical paths,
not fixed guarantees. Actual paths depend on the operating system, environment
and XDG settings. ``Runtime`` and ``State`` include GTlab-specific
subdirectories; ``Runtime`` also has a fallback location.

``Temp`` is owned by one GTlab process and is removed after normal shutdown.
``Runtime`` stays stable across GTlab processes so they can coordinate. ``State``
stores persistent state for an installation.

.. rst-class:: compact-table

.. list-table:: Standard application locations
   :header-rows: 1
   :widths: 11 27 39 23

   * - Location
     - Purpose & Lifetime
     - Typical Path
     - GTlab example
   * - ``Config``
     - User settings.

       Persistent for this user and application.
     - ``QStandardPaths::AppConfigLocation``

       | **Linux:** ``~/.config/GTlab 2``
       | **Windows:** ``%LOCALAPPDATA%/GTlab 2``
     - Settings written by ``GtSettings``.
   * - ``Data``
     - User-owned files.

       Persistent data, including downloaded and user-created collections.
       Shipped collections remain read-only installation resources.
     - ``QStandardPaths::AppDataLocation``

       | **Linux:** ``~/.local/share/GTlab 2``
       | **Windows:** ``%APPDATA%/GTlab 2``
     - Downloaded or user-created collections under ``Collections``.
   * - ``Cache``
     - Re-creatable data.

       Can be deleted to reclaim space.
     - ``QStandardPaths::CacheLocation``

       | **Linux:** ``~/.cache/GTlab 2``
       | **Windows:** ``%LOCALAPPDATA%/GTlab 2/cache``
     - Cached downloads or derived data.
   * - ``State``
     - Installation state.

       Persistent per installation. GTlab falls back to Config if State is
       unavailable.
     - ``QStandardPaths::StateLocation``

       | **Linux:** ``~/.local/state/GTlab 2/installation-<hash>``
       | **Windows:** ``%LOCALAPPDATA%/GTlab 2/State/installation-<hash>``
     - ``GTlab.ini`` records modules that crashed during loading;
       ``GTlab.ini.module-loading.lock`` coordinates module loading.
   * - ``Runtime``
     - Process coordination files.

       Shared by GTlab processes. If the preferred location is unavailable,
       GTlab uses a stable per-user fallback.
     - **Linux preferred** (``QStandardPaths::RuntimeLocation`` plus
       ``DLR/GTlab 2``)

       | **Linux:** ``/run/user/<UID>/DLR/GTlab 2``

       **Fallback** (``AppLocalDataLocation/runtime``)

       | **Linux:** ``~/.local/share/GTlab 2/runtime``
       | **Windows:** ``%LOCALAPPDATA%/GTlab 2/runtime``
     - Project-open lock files.
   * - ``Temp``
     - Task temporary files.

       Shared within one process. Removed on normal shutdown; usually left
       after abnormal termination.
     - ``QStandardPaths::TempLocation/gtlab-<random>``

       | **Linux:** ``/tmp/gtlab-<random>``
       | **Windows:** ``%TEMP%/gtlab-<random>``
     - Use a task-specific ``QTemporaryDir`` below this location.

Resolve a location and check the result before using it:

.. code-block:: cpp

   #include "gt_applicationpaths.h"

   QString error;
   const auto data = GtApplicationPaths::directory(
       GtApplicationPaths::Location::Data, &error);
   if (!data)
   {
       // Report or handle the unavailable location.
       return;
   }

   const QString collectionRoot = data->filePath(QStringLiteral("Collections"));

Use ``Temp`` as the parent for temporary work owned by one task. ``QTemporaryDir``
creates and removes the task directory independently:

.. code-block:: cpp

   #include <QTemporaryDir>

   const auto tempRoot = GtApplicationPaths::directory(
       GtApplicationPaths::Location::Temp, &error);
   if (!tempRoot)
   {
       return;
   }

   QTemporaryDir taskDir(tempRoot->filePath(QStringLiteral("task-XXXXXX")));
   if (!taskDir.isValid())
   {
       return;
   }

See :cpp:class:`GtApplicationPaths` for the full API contract.
