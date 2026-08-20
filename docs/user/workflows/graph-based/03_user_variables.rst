User Variables
--------------

For some workflows user-defined global variables may be required.
The IntelliGraph module allows you to define such variables as key-value pairs, called **user variables**.

User variables are defined per Graph and are persistently in the project.
All Nodes, including nodes in subgraphs, have access to the same user variables of the root graph.


Editing User Variables
""""""""""""""""""""""

To edit the user variables of a graph, right-click the object of the root graph in the Explorer and select *Edit User Variables*.
   
.. image:: ../images/workflows_user_variables_context_menu_light.png
   :align: center
   :alt: Opening the graph user variables dialog on the root graph
   :class: only-light
   
.. image:: ../images/workflows_user_variables_context_menu_dark.png
   :align: center
   :alt: Opening the graph user variables dialog on the root graph
   :class: only-dark
   
A dialog is opened, in which user variables may be added or edited.
For each variable a unique key must be defined.
Under this key the user variable may be accessed by other nodes.

Each variable requires both a type and a value.
Supported types include:

- Boolean (true, false)
- Integer
- Floating point values
- Strings

Unchecking a user variable and saving will cause the user variable to be deleted.

.. image:: ../images/workflows_user_variables_dialog_light.png
   :align: center
   :alt: The graph user variables dialog
   :class: only-light
   
.. image:: ../images/workflows_user_variables_dialog_dark.png
   :align: center
   :alt: The graph user variables dialog
   :class: only-dark

Using User Variables
""""""""""""""""""""

User variables are usually accessed in two ways: using the dedicated node that exposes the user variables, or using a scripting node.

User Constants Node
^^^^^^^^^^^^^^^^^^^

Right-click the scene and add a **User Constants** node (see :ref:`Creating Graphs → Adding Nodes and Connections <label_section_intelli_graph_add_nodes>` on how to add nodes).

The user variables are automatically exposed as output ports of the node.
Editing the user variables causes the node to automatically update.

.. image:: ../images/workflows_user_variables_node_light.png
   :align: center
   :alt: Accessing the user variables using the User Constants node
   :alt: Stlying a comment
   :class: only-light
   
.. image:: ../images/workflows_user_variables_node_dark.png
   :align: center
   :alt: Accessing the user variables using the User Constants node
   :class: only-dark
   
Python Scripting Node
^^^^^^^^^^^^^^^^^^^^^
  
The user variables can be accessed in a **Python Node** (see :ref:`Scripting of GTlab → Nodes <label_scripting_nodes>`) using the `user_vars` dictionary like a normal Python key-value dictionary.
  
.. image:: ../images/workflows_user_variables_python_node_light.png
   :align: center
   :alt: Accessing the user variables using the Python scripting node
   :class: only-light
   
.. image:: ../images/workflows_user_variables_python_node_dark.png
   :align: center
   :alt: Accessing the user variables using the Python scripting node
   :class: only-dark
