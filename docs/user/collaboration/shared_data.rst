Shared Resources
================

GTlab can connect to simple web servers that publish reusable resources in a fixed folder layout.
This is the page to read if you want to make your own server available to GTlab users.

The resources are not project data. They are shared, domain-specific assets such as:

- engine maps
- scripts
- blade profiles
- other reusable reference data

If you only want to exchange projects between users, start with :doc:`Projects <../basics/04_projects>`.

What this page explains
-----------------------

This page focuses on the user-facing setup:

- what GTlab expects on the server
- how a server is discovered
- which files belong to one resource
- where GTlab stores downloaded copies locally

It does not describe module implementation details. Those belong to the module developer documentation.

How GTlab sees a shared-resource server
---------------------------------------

GTlab works with a collection interface that exposes a collection ID, an optional settings dialog, and the structure of the resource metadata.

For users, the important result is simple:

1. A module defines a collection ID for a resource family.
2. GTlab looks for access data with the same ID.
3. The configured server is queried for an ``index.dat`` file.
4. That file lists the available resources.
5. Each resource is described by its own ``index.json`` file.

In practice, this means that a single web server can host a whole resource library as long as it follows the required folder structure.

Map collections in practice
---------------------------

A common use case is a map collection server that publishes several engine maps.
In GTlab, the available resources appear in the collection browser on the left.
When you select one entry, GTlab shows its metadata and files on the right.

.. image:: images/Map-Collection.png
   :align: center
   :class: only-light
   :alt: GTlab collection browser showing shared engine maps in the light theme

.. image:: images/Map-Collection-Dark.png
   :align: center
   :class: only-dark
   :alt: GTlab collection browser showing shared engine maps in the dark theme

Server layout
-------------

At the top level, the server must provide an ``index.dat`` file.
The file contains one entry per line.
Each entry points to one resource folder.

Each resource folder must contain an ``index.json`` file.
That JSON file describes the resource and lists the files that belong to it.

The layout therefore looks like this:

.. code-block:: text

   https://your-server.example/
   ├── index.dat
   ├── engine_map_001/
   │   ├── index.json
   │   ├── map.dat
   │   └── map_meta.xml
   ├── blade_profile_a/
   │   ├── index.json
   │   └── profile.xml
   └── script_pack_2026/
       ├── index.json
       ├── prepare_case.py
       └── helper_functions.py

The paths in ``index.dat`` are read as server-relative entries.
GTlab then appends ``index.json`` to each listed resource path.

Required resource metadata
--------------------------

The resource descriptor in ``index.json`` must contain these fields:

.. list-table:: Required fields
   :header-rows: 1

   * - Field
     - Meaning
   * - ``ident``
     - Human-readable name shown in GTlab
   * - ``description``
     - Short description of the resource
   * - ``uuid``
     - Stable identifier used to match local and remote copies
   * - ``version``
     - Numeric version used to detect updates
   * - ``files``
     - List of files that belong to the resource

A minimal example looks like this:

.. code-block:: json

   {
     "ident": "Engine map for test rig A",
     "description": "Baseline map for the 2026 calibration setup",
     "uuid": "f4a4f2b4-5f0b-4f2d-bf25-2f76dce9c5f4",
     "version": 1.0,
     "files": [
       "map.dat",
       "map_meta.xml"
     ]
   }

If a module defines additional resource properties, they are also read from ``index.json``.
Those extra fields come from the module's collection structure and are shown in GTlab where applicable.

How GTlab uses the data
-----------------------

When GTlab reads the server, it compares the remote resources with what is already installed locally:

- same ``uuid`` and same or lower ``version`` → installed
- same ``uuid`` and higher ``version`` → update available
- unknown ``uuid`` → available for installation

Downloaded resources are stored locally under the application collection cache.
The cache path follows this pattern:

.. code-block:: text

   <GTlab application directory>/Collections/<collection-id>/<resource-uuid>/

GTlab downloads every file listed in ``files`` and also stores the corresponding ``index.json`` locally.

Setting up access in GTlab
--------------------------

To connect a server, open the access settings in GTlab and add a host for the relevant collection.
The collection ID must match the resource family provided by the module.

In the current setup, the access entry is just the server location GTlab should query.
If your server is behind a reverse proxy or a custom path, make sure the configured host points to the directory that exposes ``index.dat`` at its root.

Recommended setup checklist
---------------------------

- Host the resources on a plain HTTP or HTTPS server.
- Put one ``index.dat`` file at the server root.
- List one resource folder per line in ``index.dat``.
- Put an ``index.json`` file into every resource folder.
- Make sure the file names in ``files`` really exist on the server.
- Keep ``uuid`` stable across updates.
- Increase ``version`` whenever you publish a newer resource.

Typical mistakes
----------------

- ``index.dat`` points to a folder, but the folder has no ``index.json``.
- ``index.json`` misses one of the required fields.
- A file listed in ``files`` is not actually present on the server.
- The same resource gets a new ``uuid`` after an update, so GTlab treats it as a new item instead of an update.
- The server path is configured incorrectly, so GTlab cannot reach ``index.dat`` at the expected location.

If you need module-specific details
-----------------------------------

The exact metadata fields and the way GTlab displays them are defined by the module that owns the collection.
If you need help for a specific resource family, ask the module maintainers for the matching collection structure.

Related pages
-------------

- :doc:`Projects <../basics/04_projects>`
- :doc:`Preferences <../basics/05_preferences>`
- :doc:`FAQ <../faq>`
