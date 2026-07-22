GTlab User Documentation
========================

Welcome to the GTlab user documentation. This site is written for people who want to use GTlab to work with projects, data models, and workflows, not for core developers.

If you are new, follow the short path below in order: install GTlab, learn the interface, create a project, and then read the workflow and scripting sections only when you need them.

Get started
-----------

1. **Install GTlab** and, if needed, the Python module.
2. **Open the app** and skim the **UI tour** so the main docks make sense.
3. Create your first **project** and read the **Projects** page before changing anything else.
4. Run a **workflow** only after you know where project data lives and how modules are selected.
5. Use **Preferences** to tailor GTlab to your environment once the basics are clear.

Core concepts
-------------

GTlab is built on three key ideas:

- **Modules** - Modules add the domain-specific tools, editors, and data types that turn GTlab into a usable application for a given team or discipline.
- **Project Data** - Projects hold the data model objects you work on. They are the central unit for editing, exchanging, and processing data.
- **Workflows** - Workflows connect processing steps into repeatable sequences. They are the way GTlab automates work on project data.

Together these concepts make GTlab useful for daily engineering work, automation, and team collaboration.

.. note::
   If you are unsure where to continue, start with :doc:`Installation and Updates <basics/02_installation>` and then read :doc:`Projects <basics/04_projects>`.


Quick links
-----------

- :doc:`Install GTlab <basics/02_installation>`
- :doc:`User Interface tour <basics/03_user-interface>`
- :doc:`Projects <basics/04_projects>`
- :doc:`Preferences <basics/05_preferences>`
- :doc:`Team Data <collaboration/shared_data>`
- :doc:`Hub–Spoke workflow <workflows/hub_spoke>`
- :doc:`Graph-based workflow <workflows/graph-based>`
- :doc:`Scripting <scripting/02_types_of_scripting>`
- :doc:`Headless execution <headless/01_overview>`
- :doc:`FAQ <faq>`


Common tasks
------------

- **Run a workflow:** start with :doc:`Hub–Spoke <workflows/hub_spoke>`  
- **Automate with scripts:** see :doc:`Scripting examples <scripting/03_examples>` and the :doc:`reference <scripting/04_scripting-reference>`  
- **Share data with teammates:** :doc:`Collaboration basics <collaboration/shared_data>`  
- **Non-interactive runs (CI/servers):** :doc:`Headless execution <headless/01_overview>`



Release notes & support
-----------------------

- For what’s new, see GTlab's `news page <https://gtlab.de/#news>`_ .
- Need help? Start with the :doc:`FAQ <faq>`. Open an issue or discussion on https://github.com/dlr-gtlab.
- For developer-focused material, check the `Developer Documentation <https://gtlab.readthedocs.io/en/latest/>`_.

.. toctree::
   :maxdepth: 0
   :caption: The Basics
   :hidden:

   basics/02_installation
   basics/03_user-interface
   basics/04_projects
   basics/05_preferences
   basics/06_about-page

.. toctree::
   :caption: Workflows
   :hidden:

   workflows/hub_spoke
   workflows/graph-based

.. toctree::
   :caption: Collaboration
   :hidden:

   collaboration/shared_data

.. toctree::
   :caption: Scripting
   :hidden:

   scripting/01_installation
   scripting/02_types_of_scripting
   scripting/03_examples
   scripting/04_scripting-reference

.. toctree::
   :caption: Headless Execution
   :hidden:

   headless/01_overview

.. toctree::
   :caption: Reference
   :maxdepth: 2
   :hidden:

   Developer Documentation <https://gtlab.readthedocs.io/en/latest/>
   faq
