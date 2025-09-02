Function Reference
==================

These are just some examples of how to document functions.

If you like to  cross-ref a function, do it like this :py:func:`footprint` :

Context XXX
-----------

.. py:function:: currentProject()

   Returns the current project

   :returns: The currently opened project. None, if no project is open.
   :rtype: :py:class:`GtProject` or None

.. py:function:: footprint(only_active=True)

   Returns the projects footprint as a dict (module, version)

   :param only_active: If true, only the active modules in the project are included in the dict
   :type only_active: bool
   :return: The dict of the modules. Key = module name, value = module version
   :rtype: dict[str, str]

.. py:method:: findGtTask(name: str) -> GtTask

    Returns an existing GtTask by objectname

    :param name: The object name of the task to search for.
    :type name: str



.. py:class:: GtObject

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

   .. py:method:: findGtProperty(id) -> GtAbstractProperty

    Returns the property matching id. Returns None, if not found.

    :note: The actual id might differ from the displayed property name!

    :param id: The property id to search for.
    :type id: str





.. py:class:: GtProject(GtObject)

   Inherited from :py:class:`GtObject`

   .. py:method:: path() -> str

    Returns the path to project directory on hard drive

   .. 


.. py:class:: GtTask(GtObject)

   An executable task of the hub-spoke workflow engine.

   .. py:method:: run() -> bool

    Executes the task

    :return: True, on success. False otherwise.

   .. py:method:: deleteAllCalculators()

    Deletes all calculator appended to the given task.
