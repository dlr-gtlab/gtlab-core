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

.. list-table:: Standard application locations
   :header-rows: 1
   :widths: 12 30 24 34

   * - Location
     - Use
     - Lifetime
     - GTlab example
   * - ``Config``
     - User preferences and application settings.
     - Persistent; shared by GTlab installations with the same application
       identity.
     - Settings written by ``GtSettings``.
   * - ``Data``
     - User-owned application data that GTlab must retain.
     - Persistent; separate from installed resources.
     - Downloaded or user-created collections belong here. Collections shipped
       with GTlab remain read-only resources in the installation directory.
   * - ``Cache``
     - Data that GTlab can recreate.
     - May be deleted to reclaim space.
     - Cached downloads or derived data.
   * - ``State``
     - Persistent state needed across application runs.
     - Persistent and scoped to the GTlab installation for crash markers and
       the module-loading lock.
     - ``GTlab.ini`` stores the list of modules that crashed during loading.
   * - ``Runtime``
     - Files used to coordinate running GTlab processes.
     - Shared by cooperating processes. The operating system may remove the
       preferred runtime location; GTlab uses a stable per-user temporary
       fallback when needed.
     - Project-open and module-loading lock files.
   * - ``Temp``
     - Temporary files shared by tasks in the same GTlab process.
     - Unique per process and removed on normal shutdown. It usually remains
       after abnormal termination; GTlab does not clean up such old directories
       on a later launch.
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
