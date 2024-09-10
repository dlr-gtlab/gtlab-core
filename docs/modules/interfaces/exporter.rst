.. _exporterinterface:

Exporter Interface
------------------

The exporter interface is used for including exporters derived of GtAbstractExporter. With these external data can be written into the GTlab datamodel.

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_exporterinterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtExporterInterface
    {
        [...]
    
        Q_INTERFACES(GtDatamodelInterface)
        Q_INTERFACES(GtExporterInterface)
    
        [...]
    
        /**
        * @brief Returns identification string of exporter.
        * @return Identification string of exporter.
        */
        QList<QMetaObject> exporter() const override;
    
        [...]
    }

Documentation on Virtual Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :cpp:func:`QList<QMetaObject> exporter()`

Each GTlab module that implements the Exporter interface must provide a corresponding exporter() function. This function enables the execution of the exporter with a right-click on the target datamodel object.

By using the GT_METADATA macro, the exporter class can be transformed into a QMetaObject, facilitating integration with the Qt framework. If there are no exporters an empty QList can be returned.

.. code-block:: cpp
    
    [...]
    QList<QMetaObject> retval;
    retval << GT_METADATA(YourExporter);
    return retval;
    [...]