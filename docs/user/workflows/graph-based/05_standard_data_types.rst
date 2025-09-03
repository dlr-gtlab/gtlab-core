Standard Node Datatypes
------------------------

Each module may define their own node datatypes. Furthermore, some node datatypes can automatically be converted by the IntelliGraph-system into other compatible datatypes.

This page gives an overview over the node datatypes and conversions defined by the core modules.

IntelliGraph Module
~~~~~~~~~~~~~~~~~~~

Datatypes
"""""""""

The following node datatypes are defined by the `IntelliGraph`-Module.

Boolean Data (``intelli::BoolData``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

ByteArray Data (``intelli::ByteArrayData``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Double Data (``intelli::DoubleData``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

File Data (``intelli::FileData``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Int Data (``intelli::IntData``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Invalid Data (``intelli::InvalidData``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is a special node datatype, that is not intended for regular use.
It has no value or data associated and soely indicates that the given data is invalid.

All node datatypes can be converted to ``intelli::InvalidData`` and vice versa. However, a node port that is connected to ``intelli::InvalidData`` will never recieve a valid input.

String Data (``intelli::StringData``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

StringList Data (``intelli::StringListData``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Object Data (``intelli::ObjectData``)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Compatibilities and Conversions
"""""""""""""""""""""""""""""""

The following conversions are implemented by the `IntelliGraph`-Module possible:

- ``intelli::IntData`` <-> ``intelli::DoubleData``
- ``intelli::StringData`` <-> ``intelli::ByteArrayData``
- ``intelli::InvalidData`` <-> ``*``
	Any datatype can be converted to ``intelli::InvalidData`` and vice versa. However, a node port that is connected to ``intelli::InvalidData`` will never recieve a valid input.