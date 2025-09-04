Function Reference
==================

These are just some examples of how to document functions.

If you like to  cross-ref a function, do it like this :py:func:`footprint` :

Context XXX
-----------

.. py:function:: currentProject() -> GtProject

   Returns the current project

   :returns: The currently opened project. None, if no project is open.

.. py:function:: footprint(only_active=True) -> dict[str, str]

   Returns the projects footprint as a dict (module, version)

   :param only_active: If true, only the active modules in the project are included in the dict
   :type only_active: bool
   :return: The dict of the modules. Key = module name, value = module version

.. py:method:: findGtTask(name: str) -> GtTask

    Returns an existing GtTask by objectname

    :param name: The object name of the task to search for.
    :type name: str



.. py:class:: GtObject

   .. py:method:: className() -> str

        Returns the class name (type name) of the object.

   .. py:method:: calcHash() -> str

        Returns the hash value of the current object.

        The hash value is a fingerprint of the object state,
        including all property values and child objects.
        Thus, if an object changes, the hash will change too.

   .. py:method:: clone() -> GtObject

        Creates a clone of the object. Note, that the new object
        is a free object and thus does not belong to any parent.

   .. py:method:: findGtChild(childName) -> GtObject

        Returns the first child element matching the object name of childName.
        If no element was found, it returns None. 

        :param childName: The object name of the the child to search for.
        :type childrenName: str

   .. py:method:: findGtChildren(childrenName="", objectClassName="") -> list[GtObject]

        Returns the direct children matching child_name and objectClassName.
        An empty string matches everything.

        :param childrenName: The object name of the the children to search for.
        :type childrenName: str
        :param objectClassName: If not empty, only children matching the class name are returned.
        :type objectClassName: str

   .. py:method:: findGtChildrenByClass(objectClassName="") -> list[GtObject]

        Returns the direct children matching their class name defined by ``objectClassName``.
        An empty string matches everything.

        :param objectClassName: If not empty, only children matching the class name are returned.
        :type objectClassName: str

   .. py:method:: findGtParent() -> GtObject

         Returns the parent object of the object if it exists. 
         Otherwise, ``None`` is returned.

   .. py:method:: findGtProperty(id: str) -> GtAbstractProperty

        Returns the property matching id. Returns ``None``, if not found.

        :note: The actual id might differ from the displayed property name!

        :param id: The property id to search for.
        :type id: str

   .. py:method:: propertyValue(id: str)

        Returns the value of the property with given ``id``.

        :raises: ``RuntimeError``, if the property does not exist.

        :param id: The property id to search for.
        :type id: str

   .. py:method:: setPropertyValue(id: str, value)

        Sets the value of the property with given ``id``.

        :raises: ``RuntimeError``, if the property does not exist.

        :param id: The property id to search for.
        :type id: str
        :param value: The value to set.

   .. py:method:: uuid(): str

        Returns the UUID of the object.

   .. py:method:: objectByUUID(uuid: str) -> GtObject

        Searches a child object, whose UUID is ``uuid``.
        
        :returns: The object matching the UUID. ``None``, if not found.






.. py:class:: GtProject(GtObject)

   Inherited from :py:class:`GtObject`

   .. py:method:: path() -> str

    Returns the path to project directory on hard drive

   .. 


.. py:class:: GtTask(GtObject)

   An executable task of the hub-spoke workflow engine.

   Inherited from :py:class:`GtObject`

   .. py:method:: run() -> bool

    Executes the task

    :return: True, on success. False otherwise.

   .. py:method:: deleteAllCalculators()

    Deletes all calculator appended to the given task.

   .. py:method:: hasWarnings() -> bool

    Returns state of warning flag.


.. py:class:: GtPyTask(GtTask)

   This class represents a python task.
   As such, additional settings like input and output 
   args can be defined via methods.

   Inherited from :py:class:`GtTask`

   .. py:method:: inputArgs() -> dict

      Returns all input arguments of the python task as a dict.

   .. py:method:: inputArg(argName: str)

      Returns the value of the input argument given by ``argName``.

      :raises: ``RuntimeError``, if the argument does not exist

   .. py:method:: setInputArg(argName: str, value)

      Sets the value of the input argument given by ``argName``.

      :raises: ``RuntimeError``, if the argument does not exist

   .. py:method:: outputArgs() -> dict

      Returns all output arguments of the python task as a dict.

   .. py:method:: outputArg(argName: str)

      Returns the value of the output argument given by ``argName``.

      :raises: ``RuntimeError``, if the argument does not exist

   
