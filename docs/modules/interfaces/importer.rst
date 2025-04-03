.. _importerinterface:

Importer Interface
------------------

The importer interface is used for including importers derived of GtAbstractImporter. With these external data can be written into the  GTlab datamodel.

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_importerinterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtImporterInterface
    {
        [...]
    
        Q_INTERFACES(GtImporterInterface)
    
        [...]
    
        /**
        * @brief Returns identification string of importer.
        * @return Identification string of importer.
        */
        QList<QMetaObject> importer() const override;
    
        [...]
    }

Documentation on Virtual Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :cpp:func:`QList<QMetaObject> importer()`

Each GTlab module that implements the Importer interface must provide a corresponding importer() function. This function enables the execution of the importer with a right-click on the target datamodel object.

By using the GT_METADATA macro, the importer class can be transformed into a QMetaObject, facilitating integration with the Qt framework. If there are no importers an empty QList can be returned.

.. code-block:: cpp
    
    [...]
    QList<QMetaObject> retval;
    retval << GT_METADATA(YourImporter);
    return retval;
    [...]