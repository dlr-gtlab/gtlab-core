Object UI
=========

``GtObjectUI`` customizes the appearance and context menu of data objects in
GTlab views such as the explorer dock widget.
Subclass it to define icons, styling, and context menu actions for objects of
the desired data classes.
Register the Object UI for each data class via :cpp:func:`GtMdiInterface::uiItems`
as outlined in :ref:`User-interface contributions<map-objects-to-ui-classes>`.

Overview
--------

Each :cpp:class:`GtObject` can have an associated ``GtObjectUI`` instance that
controls several aspects of how the object is presented to the user.
The ``GtObject* obj`` parameter passed to virtual methods is the actual data
object instance the UI is being queried for.
This lets a single UI class handle multiple object types or adapt its response
based on runtime object state.

The following can be controlled:

- **Icon** -- shown in tree views and other object browsers
- **Context Menu Actions** -- right-click menus in the explorer and similar views
- **Styling** -- foreground/background colors, fonts and similar via data roles
- **Open-with actions** -- register views or editors the object may be opened in
- **Double-click behavior** -- defines what happens when the user double-clicks the object
- **Name validation** -- regular expression for allowed object names when renaming

Changing the Icon
"""""""""""""""""

Override :cpp:func:`GtObjectUI::icon` to return a custom icon.
The ``object`` parameter is the object instance the icon is being requested for.
Use it to return different icons depending on the object type or state.

.. code-block:: cpp

   #include "gt_objectui.h"
   #include "gt_icons.h"

   class MyObjectUI : public GtObjectUI
   {
       Q_OBJECT
   public:
   
       Q_INVOKABLE MyObjectUI() = default;

       QIcon icon(GtObject* object) const override
       {
           if (qobject_cast<MyObject*>(object)) 
           {
               return gt::gui::icon::folder();
           }
           return GtObjectUI::icon(object);
       }
   };

Adding Context Menu Actions
"""""""""""""""""""""""""""

Populate actions in the constructor using :cpp:func:`GtObjectUI::addSingleAction`.
Use static member functions as the action method, as they convert implicitly to the correct Functor type.
Lambdas may also be used for simple actions.

**Header (myobjectui.h):**

.. code-block:: cpp

   #include "gt_objectui.h"

   class MyObjectUI : public GtObjectUI
   {
       Q_OBJECT
   public:
   
       Q_INVOKABLE MyObjectUI();

       static void myAction(GtObject* object);
   };

**Implementation (myobjectui.cpp):**

.. code-block:: cpp

   #include "myobjectui.h"
   #include "gt_icons.h"

   MyObjectUI::MyObjectUI()
   {
       addSingleAction(tr("My Action"), myAction);
   }

   void
   MyObjectUI::myAction(GtObject* object)
   {
       auto* myObject = qobject_cast<MyObject*>(object);
       if (!myObject) return;

       // perform some action on the object
   }

Lambdas work as well and may be used if desired:

.. code-block:: cpp

   MyObjectUI::MyObjectUI()
   {
       addSingleAction(tr("Some other action"), [](GtObject* object){
           // perform operation
       });
   }

Customizing Context Menu Actions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

One can further customize the appearance of the actions using a builder pattern and by chaining the following methods:

- Use ``setIcon`` to give the action a custom icon.
- Use ``setVisibilityMethod`` to control whether the action is shown.
- Use ``setVerificationMethod`` to control whether the action is enabled.
- Use ``setShortCut`` or ``registerShortCut`` to set a keyboard shortcut. When the shortcut is used, the action is triggered regardless of the verification or visibility method.
- Use ``setOrderPriority`` to control the position in the menu. See `Ordering of Context Menu Entries` for details on the order priorities.

The functions ``setVisibilityMethod`` and ``setVerificationMethod`` may accept any function/functor whose call operator accepts a ``GtObject*`` and whose return value is implicitly convertible to a boolean.

**Header (myobjectui.h):**

.. code-block:: cpp

   #include "gt_objectui.h"

   class MyObjectUI : public GtObjectUI
   {
       Q_OBJECT
   public:
   
       Q_INVOKABLE MyObjectUI();
       
       static MyObject* toMyObject(GtObject* object);

       static void showDetails(GtObject* object);
   };

**Implementation (myobjectui.cpp):**

.. code-block:: cpp

    MyObjectUI::MyObjectUI()
    {
        addSingleAction(tr("Show Details"), showDetails)
        .setIcon(gt::gui::icon::info())
        .setVisibilityMethod(toMyObject)
        .setVerificationMethod([](GtObject* object){
            MyObject* myObject = toMyObject(object);
            return myObject && myObject->isMainObject();
        })
        .setShortCut(QKeySequence{ Qt::Key_F1 })
        .setOrderPriority(gt::gui::OrderPriority::Default);
    }
    
    MyObject*
    MyObjectUI::toMyObject(GtObject* object)
    {
        return qobject_cast<MyObject*>(object); 
    }
    
    void
    MyObjectUI::showDetails(GtObject* object)
    {
        MyObject* myObject = toMyObject(object);
        if (!myObject) return;
        
        MyFancyDialog dialog{myObject};
        dialog.exec();
    }
   
Adding Context Menu Group Actions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use ``addActionGroup`` to create a submenu with grouped actions:

.. code-block:: cpp

   addActionGroup(tr("More Actions"))
       .setIcon(gt::gui::icon::data())
       << makeSingleAction(tr("Action A"), [](GtObject*) { ... })
           .setIcon(gt::gui::icon::letter::a())
       << makeSingleAction(tr("Action B"), [](GtObject*) { ... })
           .setIcon(gt::gui::icon::letter::b());

Ordering of Context Menu Entries
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Actions in the context menu are sorted by *order priority*.
Lower values appear earlier in the menu.
GTlab provides named constants in :cpp:struct:`gt::gui::OrderPriority` to
support relative positioning.

- ``First`` and ``Last`` guarantee placement at the beginning or end of the menu. In the case
  that two or more actions both claim ``First`` or ``Last``, the
  last action to use the constant appears first or last among that group.
- ``BeforeXAction`` and ``AfterXAction`` constants allow inserting actions in the same
  section before or after the built-in action(s) corresponding to ``X``.
- ``BeforeXSection`` and ``AfterXSection`` constants allow inserting actions in a new section
  before or after the separators of the built-in action(s) corresponding to ``X``.

.. list-table::
   :header-rows: 1
   :widths: 40 60

   * - Constant prefix
     - Purpose
   * - ``First``
     - Place action first in the menu
   * - ``BeforeXSection``
     - Insert action before the built-in ``X`` actions and their separators
   * - ``BeforeXAction``
     - Insert action before the built-in ``X`` actions in the same section
   * - ``XAction``
     - Order priorities of the built-in ``X`` actions
   * - ``AfterXAction``
     - Insert action after the built-in ``X`` actions in the same section
   * - ``AfterXSection``
     - Insert action after the built-in ``X`` actions and their separators
   * - ``Last``
     - Place action last in the menu

Available built-in anchors include ``OpenWith``, ``Import``,
``Export``, ``Rename``, and ``Delete``.
For example, ``BeforeOpenWithAction`` places an action just before any
open-with entries in the same section, while ``AfterOpenWithSection`` places 
the action after the separator corresponding to the Open-With actions.

Set the priority on a separator to create visual groupings:

.. code-block:: cpp

   addSeparator(gt::gui::OrderPriority::BeforeOpenWithSection);
   
   addSingleAction(tr("Quick Open"), [](GtObject*) { ... })
        .setOrderPriority(gt::gui::OrderPriority::BeforeOpenWithSection);

Removing Context Menu Actions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Removing actions after they have been added is currently not supported.
Instead, you may omit unwanted actions by guarding their registration with a
flag in the constructor of the base Object UI:

.. code-block:: cpp

   struct BaseUI : public GtObjectUI
   {
       enum Option : int { NoOption, NoEditAction };
       using Options = QFlags<Option>;

       BaseUI(Options options = NoOption)
       {
           if (!options.testFlag(NoEditAction))
           {
               addSingleAction(tr("Edit Object..."), myEditFunction);
           }
       }

       static void myEditFunction(GtObject* object);
   };

   struct DerivedUI : public BaseUI
   {
       Q_OBJECT
   public:

       Q_INVOKABLE DerivedUI() : BaseUI(NoEditAction)
       {
           // ...
       }
   };

Multiple options can be combined with the bit-wise operator ``|``. Because the action is never added
for ``DerivedUI``, it does not appear in its context menu.

Styling via Data Roles
""""""""""""""""""""""

Override :cpp:func:`GtObjectUI::specificData` to define custom styling using
Qt's standard roles such as foreground color, background color, or font.
The best practice is to call the base implementation for unhandled roles to preserve default
styling.

.. code-block:: cpp

   QVariant
   MyObjectUI::specificData(GtObject* object, int role, int column) const
   {
        if (column == 0 && role == Qt::ForegroundRole)
        {
            if (qobject_cast<MyObject*>(object)) 
            {
                return gt::gui::color::warningText();
            }
        }
        return GtObjectUI::specificData(object, role, column);
   }

Open-With Menu
""""""""""""""

Override :cpp:func:`GtObjectUI::openWith` to return a list of viewer or editor
class names the object can be opened with.
These entries appear in the context menu under "Open with".
The first entry in this list is the default open action and may be triggered when
double-clicking the object.

.. code-block:: cpp

   QStringList
   MyObjectUI::openWith(GtObject* object)
   {
        if (!qobject_cast<MyObject*>(object)) return GtObjectUI::openWith(object);
       return { GT_CLASSNAME(MyObjectEditor) };
   }

Double-click Behavior
"""""""""""""""""""""

Override :cpp:func:`GtObjectUI::doubleClicked` to implement custom behavior when double clicking the object.
By default the first open-with entry is opened.

.. code-block:: cpp

   void
   MyObjectUI::doubleClicked(GtObject* object)
   {
       if (!object) return;
       gtInfo() << "Double-click handled by custom implementation";
   }

Name Validation
"""""""""""""""

Override :cpp:func:`GtObjectUI::hasValidationRegExp` to return ``true`` and
:cpp:func:`GtObjectUI::validatorRegExp` to return a regular expression that
limits which names are acceptable when renaming an object.
Use ``GtObjectUI::setRegExpHint`` to provide a user-facing explanation shown when the
entered name does not match the expression.

.. code-block:: cpp

   bool
   MyObjectUI::hasValidationRegExp(GtObject* /*object*/)
   {
       return true;
   }

   QRegExp
   MyObjectUI::validatorRegExp(GtObject* /*object*/)
   {
       return gt::re::onlyLettersAndNumbersAndSpace();
   }

See also
--------

- :cpp:class:`GtObjectUIAction` -- action with icon, visibility, and order priority
- :cpp:class:`GtObjectUIActionGroup` -- grouped actions forming a submenu
- :cpp:func:`gt::gui::addToMenu` -- add actions to a ``QMenu``
- :cpp:func:`gt::gui::makeObjectContextMenu` -- populates a ``QMenu`` with the
  default context menu as seen in the explorer dock widget
- :cpp:func:`gt::gui::handleObjectDoubleClick` -- executes the double-click action of an object
