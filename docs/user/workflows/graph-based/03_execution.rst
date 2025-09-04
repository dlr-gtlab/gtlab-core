Graph Execution
---------------

Bei der Ausführung der Graphen lässt GTlab viele Freiheiten. 
Grundsätzlich können einzelne Elemente oder ganze Graphen ausgeführt werden, genauso wie einzelne Elemente ausgelassen werden können.

Zudem kann ein Graph nur auf Aufforderung hin ausgeführt werden, oder auch automatisch bei jeder Änderung des Workflows.

Manual Execution of Nodes
^^^^^^^^^^^^^^^^^^^^^^^^^
Nodes can be started manually via their context menu.
Clicking on *Execute Once* starts the execution of the node.

.. image:: ../images/Workflows_Graph_Exec_1_bright.png
   :align: center
   :alt: Application settings in preferences
   :class: only-light

.. image:: ../images/Workflows_Graph_Exec_1_dark.png
   :align: center
   :alt: Application settings in preferences
   :class: only-dark

All upstream nodes required to execute this node are also evaluated once. However, no subsequent nodes are executed.

Automatic Execution of Nodes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
It is also possible to set the execution of the graphs to react automatically to changes.
This means that if the initial value of a node changes, all subsequent nodes are also re-evaluated.
This is made possible via a button in the menu bar of the graph in the top left-hand corner of the editor.

Clicking on the arrow symbol activates automated execution. If this is selected, it can be deactivated again by clicking on the stop symbol.

.. image:: ../images/Workflows_Graph_Exec_2_bright.png
   :align: center
   :alt: Automatic Execution
   :class: only-light

.. image:: ../images/Workflows_Graph_Exec_2_dark.png
   :align: center
   :alt: Automatic Execution
   :class: only-dark
   
Selecting the option for the first time allows the graph to be evaluated once in its entirety.  

Pausing Nodes
"""""""""""""
Individual nodes can be paused. These are then not executed even if automated execution is switched on,
as well as all subsequent connected nodes.

Pausing can be switched on and off via the context menu of the node via the entry *Set Inactive* or *Set Active* or in the properties via the value of the entry *Is Node active*.


Evaluation States of Nodes
^^^^^^^^^^^^^^^^^^^^^^^^^^

The nodes are indicated by a coloured dot in the top left-hand corner of the node if it has been evaluated successfully.

.. image:: ../images/Workflows_Graph_Exec_States_bright.png
   :align: center
   :alt: Automatic Execution
   :class: only-light

.. image:: ../images/Workflows_Graph_Exec_States_dark.png
   :align: center
   :alt: Automatic Execution
   :class: only-dark

A distinction is made between three states.

+------------------+---------+----------------------------------------------------------------------------------------------------------+
| Category         | Color   | Description                                                                                              |
+==================+=========+==========================================================================================================+
| Invalid/Error    | red     | An error has occurred during the evaluation of the node, e.g. the incoming data for the node is invalid. |
+------------------+---------+----------------------------------------------------------------------------------------------------------+
| Outdated         | yellow  | This node has not yet been evaluated with the currently available data.                                  |
+------------------+---------+----------------------------------------------------------------------------------------------------------+
| Valid/Evaluated  | green   | The node was successfully analysed with the currently available data.                                    |
+------------------+---------+----------------------------------------------------------------------------------------------------------+

