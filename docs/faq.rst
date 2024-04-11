Frequently Asked Questions
==========================

Console Application
-------------------


.. collapse:: Execution of the console application in a CI job (headless) fails ("Could not connect to any X display")

    In the current version of the GTlabConsole application (v2.0), a GUI interface still needs to be created temporarily.
    This causes the application to crash on a Linux computer without a graphical session (headless) with one of the following error messages:

        qt.qpa.screen: QXcbConnection: Could not connect to display
        Could not connect to any X display

    A workaround is to use a virtual display with the help of xvfb (X-Window virtual frame buffer),
    which can be installed under Debian with ``apt-get install xvfb``.

    To run the console application in a virtual display, use the following command:

        xvfb-run -a ./GTlabConsole


Calculators
-----------

.. collapse:: Why can't I see my calculator in GTlab?

    Do you start GTlab in developer mode?
    
    If not, it is possible that the calculator still has the default property that it is registered as PROTOTYPE.
    If this is the case, it cannot be opened without dev mode.

    Start GTlab in dev mode (using the ``--dev`` command line argument) or register the calculator in release status
    (``status = GtAbstractProcessData::RELEASE``).

.. collapse:: Why do I have problems with sub-properties in the wizard?

    It is crucial to register sub-properties **before** registering the parent property:
    
    .. code-block:: cpp

       PropA.registerSubProperty(PropB) ;
       registerProperty(PropA);

.. collapse:: What is the difference between GtObjectLinkProperty and GtObjectPathProperty?

    - The :cpp:class:`GtObjectLinkProperty` links to an Object given its UUID, which is always unique.
    - :cpp:class:`GtObjectPathProperty` saves the path based on ObjectName (e.g. ``PreDesign/HPC/BladeRows/R01`` ).
      The first matching object is returned.

Data model
----------

.. collapse:: What is the difference between GtObject::appendChild and ::insertChild?

    - :cpp:func:`GtObject::appendChild` always inserts the child at the end of the children() list.
    - :cpp:func:`GtObject::insertChild` inserts the child at the corresponding position.
      The index of this position can be defined via the transfer parameter.
      0 is at the very front ``children().size-1`` is at the end.

.. collapse:: How do I delete a child from a GtObject in code?

    If the object is to be deleted never to be seen again, the child can simply be deleted using
    the c++ default statement ``delete obj;``. The child is automatically separated from the parent.

    If the object should be kept but should no longer appear under the parent,
    then use the the statement

    .. code-block:: cpp

        obj->setParent(nullptr);

.. collapse::  Why do I get multiple definition errors of properties with the template argument QString?

    Please add the code 
    
    .. code-block:: cpp

       #include "gt_stringproperty.h"

    to the beginning of your custom property header file.