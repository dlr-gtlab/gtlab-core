Viewers and Editors
===================

A GUI item is a widget bound to a single data object that opens in GTlab's central workspace.
It is implemented by :cpp:class:`GtMdiItem`, whose historical name ("MDI item") reflects its origins in a multi-document interface.
The alias ``GtGuiItem`` ("GUI Item") is the preferred term.
A single class covers both **viewers** (read-only representation) and **editors** (which may modify data objects).

.. note::

   Keep the widget separate from the persistent :cpp:class:`GtObject` it displays, so the data class stays usable in batch execution and tests.
   Use the API of the data object to read and modify.

Registering a GUI item
----------------------

Return the metadata of each concrete GUI item class from :cpp:func:`GtMdiInterface::mdiItems` (see :ref:`mdiinterface` for the full interface):

.. code-block:: cpp

   QList<QMetaObject> MyModule::mdiItems()
   {
       return {GT_METADATA(MyObjectViewer)};
   }

The class must be default-invokable (``Q_INVOKABLE`` constructor), because GTlab instantiates it from its metadata.
Set a human-readable ``objectName()`` in the constructor; it is used as the display name and as the "Open with" menu entry (see :ref:`Object UI > Open-With Menu <objectui-open-with-menu>`).

Lifecycle
---------

Every GUI item owns its widget (``widget()``) and usually operates on a data object (``currentObject()``). 
However, the data object may also be null for more general viewers, while items that operate on data can be forbidden from opening without a valid object (see the flag ``BoundToObject``).

GTlab identifies each open item through ``currentId()`` to find, reuse and focus it.
By default an item is unique per data object.
You can override ``customId`` to map several objects onto a single item by returning the same string for each.
The custom ID is name-mangled internally and specific to your GUI item class to avoid colliding with other GUI item classes.

.. note::

   The item's ``widget()`` is a plain container with no layout installed.
   Assign your own layout to it (e.g. ``new QVBoxLayout{widget()}``) and add your child widgets there.

Override these virtual methods to implement behavior:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Method
     - Purpose
   * - ``initialized()``
     - Once, right before the widget is shown; ``currentObject`` is set
   * - ``showEvent()``
     - Once when the item is first shown, before ``setData``
   * - ``setData(GtObject*)``
     - Apply the data object to the view; also called when reopening other objects
   * - ``customId(GtObject*)``
     - Return a shared id to map several objects onto one item instance
   * - ``allowsMultipleInstances()``
     - Allow several open items with the same id (alternative to the ``AllowsMultipleInstances`` flag for historical reasons)
   * - ``icon()``
     - Item-specific tab/window icon (see :doc:`icons`)
   * - ``isPrintable()`` / ``print()``
     - Enable and implement printing to PDF or similar
   * - ``projectChangedEvent(GtProject*)``
     - React to the current project changing
   * - ``objectChangedEvent(GtObject*)``
     - React to the global object selection changing
   * - ``projectSavedEvent(GtProject*)``
     - React to the project being saved
   * - ``onThemeChanged()``
     - React to light/dark theme switches

The methods ``initialized``, ``showEvent`` and ``setData`` run in that order when an item opens.
Project and object change events are queued while the item is not the active tab and delivered once it becomes active again.

A GUI item is deleted when its widget is destroyed, which happens when the user closes the item, or when the `GtMdiItem` is deleted programmatically, which in turn deletes the widget.
If the ``BoundToObject`` flag is set, the item is also deleted once its data object is destroyed.

You can control how GTlab reuses open items with flags set in the constructor via ``setItemFlag``/``setItemFlags``:

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - Flag
     - Effect
   * - ``AllowsMultipleInstances``
     - Open a new item even when one with the same id is already open
   * - ``AllowsReopening``
     - Reuse the open item and call ``setData`` with the new object
   * - ``BoundToObject``
     - Refuse to open without a data object, and delete the item when that object is deleted

.. note::

   Multiple instances are enabled either by the ``AllowsMultipleInstances`` flag or by overriding ``allowsMultipleInstances()``; either is sufficient.
   Prefer the flag, and use the virtual method only when the decision depends on runtime state.

Opening items
-------------

The :cpp:class:`GtGuiLauncher` singleton creates and shows items, and the :cpp:class:`GtGuiFactory` singleton is the registry of known GUI items and dock widgets.
Modules rarely call them directly: double-clicking an object or using its "Open with" menu invokes the launcher, which instantiates the class from the factory, applies the reuse rules, and places the item.

.. code-block:: cpp

   GtGuiLauncher::instance().open(GT_CLASSNAME(MyObjectViewer), object);

The launcher deduplicates by ``currentId()`` unless the item allows multiple instances, and reuses an open item (calling ``setData``) when it allows reopening.

.. note::

    The former ``GtMdiLauncher`` is deprecated and was removed in 2.2; use ``GtGuiLauncher`` and ``GtGuiFactory`` instead.

Toolbar actions, shortcuts, states
----------------------------------

A GUI item can contribute to the main toolbar and to persisted UI state:

- ``addToolbarAction(text, iconUrl)`` / ``addToolbarSeparator()`` add actions shown while the item is active.
- ``registerShortCut(id, key, readOnly)`` and ``getShortCut(id)`` manage item-specific keyboard shortcuts.
- ``initializeState(...)`` creates a global or project-specific ``GtState`` to remember widget specific settings across sessions, like column widths, the collapsed state of tree items or similar.

Viewer and Editors
------------------

A **viewer** only reads the data object; never modify it from a viewer.
An **editor** changes the object, and every change must run through GTlab's undo/redo command mechanism (see :doc:`../user_interface`).

In both cases the data object may be deleted outside the widget.
Prefer the ``BoundToObject`` flag for tightly coupled editors, otherwise guard access with ``QPointer`` and handle the object becoming null.
Test the item with no project open, after switching projects, and after the target object is removed, in both light and dark themes.

Minimal example
---------------

**Header (myobjectviewer.h):**

.. code-block:: cpp

   #include "gt_mdiitem.h"

   class QLabel;

   class MyObjectViewer : public GtMdiItem
   {
       // required
       Q_OBJECT
   public:
        // must be invokable and default-constructible
        Q_INVOKABLE MyObjectViewer();

        void setData(GtObject* obj) override;

   private:
        QLabel* m_label{nullptr};
   };

**Implementation (myobjectviewer.cpp):**

.. code-block:: cpp

   #include "myobjectviewer.h"

   #include <QLabel>
   #include <QVBoxLayout>

   MyObjectViewer::MyObjectViewer()
   {
       setObjectName(tr("My Object Viewer"));
       setItemFlag(BoundToObject, true);

       auto* layout = new QVBoxLayout{widget()};
       m_label = new QLabel{widget()};
       layout->addWidget(m_label);
   }

   void
   MyObjectViewer::setData(GtObject* obj)
   {
       auto* myObject = qobject_cast<MyObject*>(obj);
       if (!myObject) return;

       m_label->setText(QStringLiteral("%1 (%2)")
                            .arg(myObject->objectName(),
                                 myObject->metaObject()->className()));
   }

See also
--------

- :ref:`mdiinterface` -- registering GUI items, dock widgets and UI classes
- :doc:`objectui` -- object icons, context menu and "Open with"
- :cpp:class:`GtMdiItem` -- the GUI item base class
- :cpp:class:`GtGuiLauncher` -- opening and managing GUI items
- :cpp:class:`GtGuiFactory` -- registry of GUI items and dock widgets
