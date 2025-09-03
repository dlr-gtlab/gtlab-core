User Interface
==============

Docks
-----

Explorer
^^^^^^^^

Properties
^^^^^^^^^^

Output
^^^^^^
The *Output* widget primarily holds two tabs. The *Application* tab shows the logging output in GTlab. More on this in the Logging_ section.  
Der Tab *Python Console* emröglicht es mit der Python Instanz zu interagieren um sowohl Nachrichten aus der Python Interaktion mit GTlab zu erhalten, als auch Python Befehle in GTlab zu nutzen.
More on this in the section :ref:`Python Console <label_chapter_python_console>`.


Process/Calculators
^^^^^^^^^^^^^^^^^^^


Viewers and Editors
-------------------


.. _label_section_logging:

Logging
-------
GTlab uses a logging system which is displayed in the Output window.
In the *Application* tab, all logging messages that are issued in the system are collected, both from the main application and from modules.

Each message is displayed with its category and information on the ID and time. The ID indicates which part of the programme sent the message.

The possible categories are explained below; not all of them are relevant for users of the software, but some are aimed at developers.

+------------+---------------------------------+---------------------------------------------------------------------------------------------------------------------+
| Category   | Icon                            | Description                                                                                                         |
+============+=================================+=====================================================================================================================+
| Trace      | |trace_bright| |trace_dark|     | Only for developers: Hint for code development                                                                      |
+------------+---------------------------------+---------------------------------------------------------------------------------------------------------------------+
| Debug      | |debug_bright| |debug_dark|     | Only for developers: Hint for code development                                                                      |
+------------+---------------------------------+---------------------------------------------------------------------------------------------------------------------+
| Info       | |info_bright| |info_dark|       | General information about the process, useful for understanding the system’s normal operation.                      |
+------------+---------------------------------+---------------------------------------------------------------------------------------------------------------------+
| Warning    | |warning_bright| |warning_dark| | Indicates potential issues or situations that might lead to problems if not addressed.                              |
+------------+---------------------------------+---------------------------------------------------------------------------------------------------------------------+
| Error      | |error_bright| |error_dark|     | Indicates critical errors that need attention of the user and may impact or interrupt the workflow’s functionality. |
+------------+---------------------------------+---------------------------------------------------------------------------------------------------------------------+
| Fatal      | |fatal_bright| |fatal_dark|     |  Unrecoverable errors that lead to the termination of the application.                                              |
+------------+---------------------------------+---------------------------------------------------------------------------------------------------------------------+

The categories can be filtered. To do this, select or deselect the buttons in the bottom right-hand corner. A general preselection can be made via the
drop-down menu in the top right-hand corner. It also modifies the generally displayed logging level. In *Default* mode, for example, the logging messages relevant only for
developer relevant logging messages of the categories *Trace* and *Debug* are not displayed.

Further buttons in the bottom right-hand corner allow you to remove all output or export it to an external text file.

Individual messages can be filtered using the search window in the lower part of the window.


.. |trace_bright| image:: images/logging_trace_bright.png
   :align: middle
   :height: 16
   :alt: Trace
   :class: only-light
   
.. |trace_dark| image:: images/logging_trace_dark.png
   :align: middle
   :height: 16
   :alt: Trace
   :class: only-dark

.. |debug_bright| image:: images/logging_debug_bright.png
   :align: middle
   :width: 16
   :alt: Debug
   :class: only-light
   
.. |debug_dark| image:: images/logging_debug_dark.png
   :align: middle
   :height: 16
   :alt: Debug
   :class: only-dark   

.. |info_bright| image:: images/logging_info_bright.png
   :align: middle
   :width: 16
   :alt: Info
   :class: only-light
   
.. |info_dark| image:: images/logging_info_dark.png
   :align: middle
   :width: 16
   :alt: Info
   :class: only-dark   

.. |warning_bright| image:: images/logging_warning_bright.png
   :align: middle
   :width: 16
   :alt: Warning
   :class: only-light
   
.. |warning_dark| image:: images/logging_warning_dark.png
   :align: middle
   :width: 16
   :alt: Warning
   :class: only-dark 
   
.. |error_bright| image:: images/logging_error_bright.png
   :align: middle
   :width: 16
   :alt: Error
   :class: only-light
   
.. |error_dark| image:: images/logging_error_dark.png
   :align: middle
   :width: 16
   :alt: Error
   :class: only-dark 
   
.. |fatal_bright| image:: images/logging_fatal_bright.png
   :align: middle
   :width: 16
   :alt: Fatal
   :class: only-light
   
.. |fatal_dark| image:: images/logging_fatal_dark.png
   :align: middle
   :width: 16
   :alt: Fatal
   :class: only-dark 

Toolbars
--------
GTlabs toolbar uses a adaptive design to offer buttons depending on possible option based on the shown windows done actions.


.. _label_section_perspectives:

Perspectives
------------
