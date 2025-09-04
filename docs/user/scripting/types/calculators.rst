Calculators
^^^^^^^^^^^

The Python Script Calculator can be integrated into a workflow like any other calculator. 
Unlike other calculators with fixed, predefined behavior (e.g., specific computations or other operations), the Python Script Calculator is flexible in its behavior: it runs a user-defined Python script. 

New calculation methods can thus be integrated into workflows during GTlab runtime.  
This makes the Python Script Calculator particularly useful for prototyping new calculators and quickly testing alternative calculation methods.

hier noch ergänzen, dass man Datenmodell zugriff hat!

The Python Script Calculator provides a configuration dialog with an integrated script editor, where the Python script executed by the calculator can be created, edited, and tested.

.. image:: images/script-calculator.png
   :align: center
   :alt: Python Script Calculator configuration dialog
   :class: only-light

.. image:: images/script-calculator-dark.png
   :align: center
   :alt: Python Script Calculator configuration dialog
   :class: only-dark

auto-completion erwähen
Through this editor, all data model packages of the current project can be accessed by entering their name in the script, allowing reading and modification of data object properties.

To check the script for errors, it can be evaluated using ``Ctrl+E``. Evaluation also registers any newly defined variables, classes, or functions, making them available through the editor's auto-completion.  
Output messages resulting from the evaluation are displayed directly below the editor in the output console.

Note that changes made during evaluation are not immediately applied to the data model. Modifications are only transferred when the Python Script Calculator is executed as part of a workflow.

During execution within a workflow, output messages appear in the :ref:`Python Console <label_python_console>`.
