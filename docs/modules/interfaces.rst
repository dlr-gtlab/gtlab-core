Interfaces
==========

Module Interface
----------------

GTlab modules are strictly structured using the `Qt <https://doc.qt.io/qt-6/plugins-howto.html#the-low-level-api-extending-qt-applications>`_ low-level API for extending Qt applications.

.. note::
    For instance, if you're looking to create a custom module, you can utilize the `Module Generator <https://gitlab.dlr.de/gtlab/public/development-tools/gtlab-module-generator>`_ to automatically produce fully functional modules.
    This tool generates all the necessary source code structures needed to develop a new GTlab module, streamlining the development process and saving you valuable time.

.. figure:: ../images/modules/interfaces/module_interface.png
  :width: 400

In order for the module to be recognized by GTlab, a module main class must be declared that inherits from QObject and GtModuleInterface. A main() method is not required.
The Q_OBJECT macro must appear in the private section of a class definition to use services provided by Qt's meta-object system and able to be identified by the GTlab Framework.
Additionally, the GtModuleInterface must be registered using the Q_INTERFACES macro. Optionally, meta information can be defined by an external JSON file and registered via the Q_PLUGIN_METADATA macro.
The three most important functions that are mandatory to implement are ident(), version(), and description(). All pure virtual functions of `GtModuleInterface` must be overwritten in the interface.

In coded formate it looks like this. More details on the mandatory functions are listed below.


.. code-block:: cpp

    #include "gt_moduleinterface.h"
 
    class MyModule: public QObject, public GtModuleInterface
    {
        Q_OBJECT

        GT_MODULE("MyModule.json")

        Q_INTERFACES(GtModuleInterface)
    
    public:
        /**
        * @brief Returns current version number of module
        * @return version number
        */
        GtVersionNumber version() override;
    
        /**
        * @brief Returns module identification string.
        * @return identification string
        */
        QString ident() const override;
    
        /**
        * @brief Returns module description
        * @return description
        */
        QString description() const override;
    }

Additional GTlab interfaces can be implemented by stacking the interface classes within the inheritance line.
It must always be ensured that the functions required by the interface, which are always purely virtual, are also implemented.

.. code-block:: cpp

    #include "gt_moduleinterface.h"
    #include "gt_datamodelinterface.h"
    
    class MyModule: public QObject, public GtModuleInterface, GtDatamodelInterface
    {
        […]
    
        Q_INTERFACES(GtModuleInterface)
        Q_INTERFACES(GtDatamodelInterface)
    
        […]
    }

Documentation on Virtual Member Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* :cpp:func:`GtModuleInterface::description`
    Returns description of the module.

    A module description is optional and is intended only to inform the user within the GTlab framework. Even if there is no description of the module it is still mandatory to return an empty string.

    .. code-block:: cpp

        QString
        GtExampleModule::description() const
        {
            return QString("GTlab-Example Module");
        }

* :cpp:func:`GtModuleInterface::version`
    Returns version number of the module.

    The version number is relevant for identifying the module within the GTlab framework. When modifying the module source code, it is advisable to count up the version number.
    Versions are made up of 3 digits. GTlab uses `semantic versioning https://semver.org/`_ to identify between major, minor and patch versions.

        .. code-block:: cpp

            GtVersionNumber
            GtExampleModule::version()
            {
                return GtVersionNumber(0, 0, 1);
            }

* :cpp:func:`GtModuleInterface::ident`

Returns identification string of the module.

The entire footprint that the module leaves behind within the GTlab framework is assigned to this identification string. Changes to the return value at a later date can lead to undesired behavior within the framework.