.. _datamodelinterface:

Datamodel Interface
-------------------
The datamodel interface is one of the most important interfaces of the GTlab framework. It provides classes and functions for general data management (serialization, diff, etc.).

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_datamodelinterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtDatamodelInterface
    {
        [...]
    
        Q_INTERFACES(GtModuleInterface)
        Q_INTERFACES(GtDatamodelInterface)
    
        [...]
    
        /**
        * @brief Returns static meta objects of datamodel package.
        * @return package meta object
        */
        QMetaObject package();
    
        /**
        * @brief Returns static meta objects of datamodel classes.
        * @return list including meta objects
        */
        QList<QMetaObject> data();
    
        /**
        * @brief Returns true if module is a stand alone module with own
        * data model structure. Otherwise module only extends the overall
        * application with additional functionalities like classes, calculators
        * or graphical user interfaces.
        * @return Stand alone indicator.
        */
        bool standAlone();
    
        [...]
    }

Documentation on Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :cpp:func:`QMetaObject GtDatamodelInterface::package()`

Each GTlab module that implements the DataModel interface must provide a corresponding data package class. This data package enables the creation of a custom data tree within the GTlab project (see :cpp:func:`GtModuleInterface::standAlone()`). Additionally, it offers functions to modify the serialization and deserialization behavior of module-specific data during project save and open procedures.

By using the GT_METADATA macro, the data package class can be transformed into a QMetaObject, facilitating integration with the Qt framework.

.. code-block:: cpp

    [...]
    return GT_METADATA(MyModulePackage);;
    [...]

* :cpp:func:`QList<QMetaObject> GtModuleInterface::data()`

Returns list of meta objects corresponding to data model classes provided by the module.

Using the GT_METADATA macro, new class structures can be transformed into a QMetaObject.

* :cpp:func:`bool GtModuleInterface::standAlone()`

.. code-block:: cpp
    
    [...]
    QList<QMetaObject> metaData;
 
    metaData << GT_METADATA(MyClassA);
    metaData << GT_METADATA(MyClassB);
 
    return metaData;
    [...]

Returns true if the module has an independent data structure or false if the module only extends certain data structures.

Linked XML Serialization (SaveAsOwnFile)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The main reason to use linked object files is version control workflow.
Instead of storing everything in one large monolithic XML file, object data is
split across multiple files. This reduces diff size, lowers merge conflicts,
and makes individual changes easier to track and review.

Module data classes can mark objects for linked-file serialization via the
``GtObject::SaveAsOwnFile`` flag:

- Query: :cpp:func:`GtObject::saveAsOwnFile()`
- Set: :cpp:func:`GtObject::setSaveAsOwnFile(bool)`

If enabled for an object and linked XML project save is active, GTlab writes
this object as a linked entry instead of embedding the full object XML in the
parent document.
In XML, this is represented via the ``aslink`` attribute and an ``<objectref>``
to a separate ``*.gtobj.xml`` file.

Unresolved linked references (for example, missing linked files during restore)
are converted to dummy objects. This is intentional behavior to preserve object
identity and serialized data, so unresolved content is not silently lost on
subsequent saves.

When such unresolved objects are written again, GTlab keeps them as
reference-only links (``aslink="refonly"``) and does not overwrite/create the
missing linked file automatically.
