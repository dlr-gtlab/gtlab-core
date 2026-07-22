Team Data
=========

GTlab is built around a shared project data model. That makes collaboration practical, but it also means a team needs a few rules of the road:

- A project is the main unit of exchange.
- Only one project can be open in one GTlab instance at a time.
- Team members should use the same GTlab version and the same relevant modules whenever possible.

If you want to understand the project side first, read :doc:`Projects <../basics/04_projects>` before using this page.

What to share
-------------

The simplest way to share work with another user is to hand over the project directory or a copied project. Use the project handling options in the :doc:`Projects <../basics/04_projects>` page when you want to duplicate, rename, or move a project.

For workflow logic, you do not need to share the whole project every time. Individual tasks and calculators can be exported as ``.xml`` files and imported into another workflow. See :doc:`Workflow Elements <../workflows/hub-spoke/02_tasks_calculators>` for the available actions.

For context and traceability, use:

- project comments
- project footprint information
- a consistent module selection for the project

How teams usually work
----------------------

A practical GTlab team setup usually looks like this:

1. One person creates or updates the project structure.
2. Other team members open the project in their own GTlab instance.
3. Workflows or workflow parts are exchanged as exported ``.xml`` files when needed.
4. Comments and footprint information are used to document what changed and which modules were involved.

Because only one project can be open per instance, side-by-side comparisons should be done with separate GTlab windows or separate instances.

What to keep aligned
--------------------

To avoid confusion when a project moves between users, keep these points aligned:

- GTlab version
- installed modules
- selected project packages
- Python environment, if scripting is involved

If those do not match, a colleague may open the project but still miss data types, workflow elements, or scripts that your setup depends on.

Related pages
-------------

- :doc:`Project handling <../basics/04_projects>`
- :doc:`Project settings and footprint <../basics/04_projects>`
- :doc:`Workflow creation <../workflows/hub_spoke>`
- :doc:`Scripting overview <../scripting/02_types_of_scripting>`
