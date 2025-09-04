Hub-Spoke Workflows
===================

The principle behind the hub-spoke workflow architecture is that a central instance coordinates and manages the data flow of the process chain.
Starting from this central instance, individual sub-workflows are supplied with the latest data relevant to them and then pass their results back to the central instance.

This gives rise to the image of a wheel with a hub and spokes.

.. image:: images/Workflows_Hub_Spoke_bright.png
   :align: center
   :alt: Hub Spoke principle
   :class: only-light
   
.. image:: images/Workflows_Hub_Spoke_dark.png
   :align: center
   :alt: Hub Spoke principle
   :class: only-dark

.. toctree::
   :hidden:

   hub-spoke/01_how_to_buid_a_process
   hub-spoke/02_tasks_calculators
   hub-spoke/03_connection_editor
   hub-spoke/04_process_queu
      
In this architecture, the individual :ref:`workflow elements <label_chapter_workflow_elements>` are arranged hierarchically, 
whereby the higher-level elements are called :ref:`Tasks <label_section_tasks>` and take over the process coordination and the lower-level elements 
are :ref:`Calculators <label_section_calculators>` that take over the actual simulations, modifications, etc. 

Data communication takes place throughout the workflow on a temporary copy of the data model, which acts as the central information point of the workflow and is modified by the individual process elements.
workflow and is modified by the individual process elements.
If individual workflow-related parameters are also to be passed on, this can be done using the :ref:`Connection Editor <label_section_connection_editor>`.
As an example, reference can again be made here to the loop in which, for example, the respective iteration step can be communicated to the workflows via the editor.
   
The following is intended to illustrate :ref:`how to build a workflow <label_section_how_to_buid_process>`.