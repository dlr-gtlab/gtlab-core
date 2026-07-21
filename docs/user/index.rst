GTlab User Documentation
========================

Welcome to the GTlab docs. This site covers installation, everyday workflows, scripting, and headless operation.  
If you’re new, follow the quick setup below; otherwise jump straight to a section via the sidebar.

Get started
-----------

1. **Install GTlab** on your machine.
2. **Open the app** and take a short **UI tour**.
3. Create your first **project** and run a **basic workflow**.
4. Check **Preferences** to tailor GTlab to your environment.

Core concepts
-------------

GTlab is built on three key ideas:

- **Modules** - GTlab is designed to be extensible. Modules allow you to add new tools, editors, or data formats without altering the core application. They transform GTlab from a general framework into a domain-specific solution — for example, a dedicated environment for designing aircraft engines.
- **Project Data** - At the heart of GTlab are your project data, which are defined and structured through data models. The platform enables you to create, edit, and process this data consistently.
- **Workflows** - Complex processes can be expressed as workflows: pipelines where processing steps are connected into repeatable, shareable sequences.

Together these concepts make GTlab flexible for research, automation, and team collaboration.

.. note::
   If you’re new, continue with :doc:`The Basics <basics/02_installation>` after reading this overview.


Quick links
-----------

- :doc:`Install GTlab <basics/02_installation>`
- :doc:`User Interface tour <basics/03_user-interface>`
- :doc:`Projects <basics/04_projects>`
- :doc:`Preferences <basics/05_preferences>`
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
