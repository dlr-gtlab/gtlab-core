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
installation and the application name ``GTlab 2``. They illustrate the
underlying ``QStandardPaths`` locations; they are not fixed guarantees. Actual
paths depend on the operating system, Qt version, environment and XDG settings.
``Runtime`` and ``State`` also include GTlab-specific subdirectories and
fallback behavior.

.. list-table:: Standard application locations
   :header-rows: 1
   :widths: 11 27 39 23

   * - Location
     - Purpose & Lifetime
     - Typical Path
     - GTlab example
   * - ``Config``
     - User preferences and application settings. Persistent for the user and
       application.
     - ``QStandardPaths::AppConfigLocation``: Linux
       ``~/.config/GTlab 2``; Windows
       ``%LOCALAPPDATA%/GTlab 2``.
     - Settings written by ``GtSettings``.
   * - ``Data``
     - Persistent user-owned data that GTlab must retain. Downloaded and
       user-created collections belong here; shipped collections remain
       read-only installation resources.
     - ``QStandardPaths::AppDataLocation``: Linux
       ``~/.local/share/GTlab 2``; Windows
       ``%APPDATA%/GTlab 2``.
     - Downloaded or user-created collections under ``Collections``.
   * - ``Cache``
     - Re-creatable data; may be deleted to reclaim space.
     - ``QStandardPaths::CacheLocation``: Linux ``~/.cache/GTlab 2``;
       Windows ``%LOCALAPPDATA%/GTlab 2/cache``.
     - Cached downloads or derived data.
   * - ``State``
     - Persistent state scoped to the GTlab installation. The crash-marker
       file and module-loading lock are stored together here. If State is
       unavailable, GTlab falls back to Config.
     - Qt 6.7+: ``QStandardPaths::StateLocation``; Linux
       ``~/.local/state/GTlab 2/installation-<hash>``; Windows
       ``%LOCALAPPDATA%/GTlab 2/State/installation-<hash>``. With Qt before
       6.7, GTlab uses ``QStandardPaths::AppLocalDataLocation/state`` instead
       (Linux ``~/.local/share/GTlab 2/state/installation-<hash>``; Windows
       ``%LOCALAPPDATA%/GTlab 2/state/installation-<hash>``).
     - ``GTlab.ini`` records modules that crashed during loading;
       ``GTlab.ini.module-loading.lock`` coordinates module loading.
   * - ``Runtime``
     - Stable files shared by cooperating GTlab processes, such as project
       coordination locks. The OS may remove the preferred runtime location;
       GTlab then uses per-user ``AppLocalDataLocation/runtime``.
     - Preferred ``QStandardPaths::RuntimeLocation`` plus
       ``DLR/GTlab 2``: Linux ``/run/user/<UID>/DLR/GTlab 2``; Windows
       ``%USERPROFILE%/DLR/GTlab 2``. Fallback: Linux
       ``~/.local/share/GTlab 2/runtime``; Windows
       ``%LOCALAPPDATA%/GTlab 2/runtime``.
     - Project-open lock files.
   * - ``Temp``
     - Temporary files shared by tasks in one GTlab process. Unique per
       process and removed on normal shutdown. Usually remains after abnormal
       termination; old directories are not cleaned up on later launches.
     - ``QStandardPaths::TempLocation/gtlab-<random>``: Linux
       ``/tmp/gtlab-<random>``; Windows ``%TEMP%/gtlab-<random>``.
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
