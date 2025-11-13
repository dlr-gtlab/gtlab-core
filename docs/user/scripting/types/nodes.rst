Nodes
^^^^^

.. image:: /scripting/images/Nodes-Example.png
  :align: center
  :alt: Example of the usage of a scripting node
  
The scripting capabilities in GTlab can also be used in the graph-based workflow system.
Individual nodes of the graphs can work with Python scripts to modify, generate, analyse or visualise data.
As in the other accesses to the python interface in GTlab, any other python libraries installed in the python environment used can be used.
 
As for all other nodes, the Python nodes can also be added via the context menu of the graphs.
They can be found in the *Scripting* category under the name *Python Scripting Node*.
 
The script that is called when the node is executed can be edited by clicking the Python button on the node to open the editor.

.. image:: /scripting/images/Python_Node_Editor.png
  :align: center
  :alt: Python Node Editor

Input and output variables
""""""""""""""""""""""""""
Corresponding ports can be created to make parameters available to a Python node within the graph system or to pass them on.

.. image:: /scripting/images/Python_Node_Context_Menu.png
  :align: center
  :alt: Python Node Editor


This can be done by opening the context menu of the node with a right-click or by adding new elements in the corresponding tabs in the properties using the plus sign.
The properties of the ports can then be found in the corresponding tabs under properties.

.. image:: /scripting/images/Python_Node_Props.png
  :align: center
  :alt: Python Node Editor

It is important to give the ports a meaningful name here, as this identifier can be used to access the port in the Python script.
The type can be used to specify what type of data can be transferred.

The port is made accessible in the Python script via the name. To access the value of the port, the *.value()* function must be called on this novh.
Please note that error messages may occur for unconnected ports when analysing the Python script.


.. image:: /scripting/images/Python_Node_Props_Usage1.png
  :align: center
  :alt: Python Node Editor					

A simple assignment using an equals sign (=) is sufficient for setting output values.
  
If a data model object is to be accepted and processed, do not use the *value()* function but the *object()* function on the port object in the script. 
This makes the data model object available with all its queries and function calls available in Python, but in a read-only version only for queries, not for manipulations. 
If the object is to be manipulated in order to be passed on in a modified version, a copy can be made at the beginning using the cobined call *object().clone()*. 
The copy created in this way can be manipulated and then used further in the graph process by assigning it to an outport.  
  
Another special case is the handling of the data type "intelli::ByteArrayData". This is possible in its reference and when assigning the output values like the standard parameters via *value()*. The special feature here is the wide range of possible uses.
All Python objects that can be picked can also be exchanged between nodes in this data type.

Plotting with Matplotlib
""""""""""""""""""""""""
The Python interface in the Python nodes allows direct access to a Matplotlib backend. 
If a Matplotlib plot is defined in a Python script of a node, a call to the function *plt.show()* is sufficient to generate the plot and store it in the system. 
If the property *Plot enabled* is true in the properties of the node, the plot is displayed directly on the node.
