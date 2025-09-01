Function Reference
==================

These are just some examples of how to document functions.

If you like to  cross-ref a function, do it like this :py:func:`footprint` :

Context XXX
-----------

.. py:function:: currentProject()

   Returns the current project

   :returns: The currently opened project. None, if no project is open.
   :rtype: GtProject or None

.. py:function:: footprint(only_active=True)

   Returns the projects footprint as a dict (module, version)

   :param only_active: If true, only the active modules in the project are included in the dict
   :type only_active: bool
   :return: The dict of the modules. Key = module name, value = module version
   :rtype: dict[str, str]
   