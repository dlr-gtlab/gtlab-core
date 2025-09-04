Hub-Spoke Workflows
===================

The principle behind the hub-spoke process architecture is that a central instance coordinates and manages the data flow of the process chain.
Starting from this central instance, individual sub-processes are supplied with the latest data relevant to them and then pass their results back to the central instance.

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
   
   
   
The central building blocks of the processes that follow this architecture are :ref:`Calculators <label_section_calculators>` and :ref:`Tasks <label_section_tasks>`.

Calculators represent individual process steps, while tasks are intended for process coordination,
A simple example of this is a loop task, which coordinates the repeated execution of individual processes.
Like the data model, the processes are generally arranged hierarchically.
This means that the process elements (calculators) are arranged below the coordinating task.

Data communication takes place throughout the process on a temporary copy of the data model, which acts as the central information point of the process and is modified by the individual process elements.
process and is modified by the individual process elements.
If individual process-related parameters are also to be passed on, this can be done using the :ref:`Connection Editor <label_section_connection_editor>`.
As an example, reference can again be made here to the loop in which, for example, the respective iteration step can be communicated to the processes via the editor.
   
The following is intended to illustrate :ref:`how to build a process <label_section_how_to_buid_process>`.