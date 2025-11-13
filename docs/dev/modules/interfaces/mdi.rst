.. _mdiinterface:

MDI Interface
--------------

Multiple Document Interface (MDI) is used for implementing complex widgets into the GTlab environment. These include:

* mdiItems -> widgets opened by a click on a datamodel object 
* dockWidgets -> widgets which can be docked anywhere into the GtApplication
* uiItems -> userInterface items e.g. data model objects which are made known to the ui of GTlab
* postItems ...

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_mdiinterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtMdiInterface
    {
        [...]
    
        Q_INTERFACES(GtDatamodelInterface)
        Q_INTERFACES(GtMdiInterface)
    
        [...]
    
        /**
        * @brief Returns static meta objects of mdi item classes.
        * @return list including meta objects
        */
        QList<QMetaObject> mdiItems() override;

        /**
        * @brief Returns static meta objects of dockwidget classes.
        * @return list including meta objects
        */
        QList<QMetaObject> dockWidgets() override;

        /**
        * @brief uiItems
        * @return
        */
        QMap<const char*, QMetaObject> uiItems() override;

        /**
        * @brief postItems
        * @return
        */
        QList<QMetaObject> postItems() override;
    }

Documentation on Virtual Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Each GTlab module that implements the MDI interface must provide corresponding mdiItems(), dockWidgets(), uiItems(), postItems() functions. When no classes are implemented for these widgets. An empty QList, QMap can be returned.


* :cpp:func:`QList<QMetaObject> mdiItems()`

Each GTlab module that implements the MDI interface must provide a corresponding mdiItems() function. This function enables the execution of widgets in the central dock widgets of GTlab. It can be accessed with a right-click on the target datamodel object.
The mdiItem must be derived from GtAbstractMdiItem and needs a data model object as a target. Essentially an mdiItem is also a widget.
By using the GT_METADATA macro, the mdi class can be transformed into a QMetaObject, facilitating integration with the Qt framework. If there are no mdiItems an empty QList can be returned.

.. code-block:: cpp
    
    [...]
    QList<QMetaObject> retval;
    retval << GT_METADATA(YourMdiItem);
    return retval;
    [...]

* :cpp:func:`QList<QMetaObject> dockWidgets()`

Each GTlab module that implements the MDI interface must provide a corresponding dockWidgets() function. This function enables the docking of additional widgets in GTlab. All dockWidgets are accessible with Window -> DockWidgets in the GtApplication.
By using the GT_METADATA macro, the mdi class can be transformed into a QMetaObject, facilitating integration with the Qt framework. If there are no dockWidgets an empty QList can be returned.

.. code-block:: cpp
    
    [...]
    QList<QMetaObject> retval;
    retval << GT_METADATA(YourDockWidget);
    return retval;
    [...]

* :cpp:func:`QMap<const char*, QMetaObject> uiItems()`

Each GTlab module that implements the MDI interface must provide a corresponding uiItems() function. This function enables the displayal of your own data model classes into GTlab. These can now be added to the object tree in the data model via different access possibilities.
For this each of your data model classes need their own UI class. These are almost always dependent on GtObjectUI/QObjectUI.
By using the GT_METADATA and GT_CLASSNAME macros, the uiItem and class can be transformed, facilitating integration with the Qt framework. If there are no uiItems an empty QMap can be returned.

.. code-block:: cpp
    
    [...]
    QMap<const char*, QMetaObject> retval;
       retval.insert(GT_CLASSNAME(GtdYourClass),
                  GT_METADATA(GtdYourClass));
    return retval;
    [...]

* :cpp:func:`QList<QMetaObject> postItems()`

Each GTlab module that implements the MDI interface must provide a corresponding postItems() function. Add more information here...
By using the GT_METADATA and GT_CLASSNAME macros, the postItems class can be transformed into a QMetaObject, facilitating integration with the Qt framework. If there are no uiItems an empty QList can be returned.

.. code-block:: cpp
    
    [...]
    QList<QMetaObject> postItems() retval;
    retval << GT_METADATA(YourPostItem);
    return retval;
    [...]