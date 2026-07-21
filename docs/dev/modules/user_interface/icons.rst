Icons
=====

GTlab provides a shared, theme-aware icon set in ``gt_icons.h``. Reusing these
icons gives familiar actions the same symbol throughout the application and
avoids maintaining copies of Core resources.

.. code-block:: cpp

  #include "gt_icons.h"

  openButton->setIcon(gt::gui::icon::folderOpen());
  removeAction->setIcon(gt::gui::icon::delete_());

Functions in ``gt::gui::icon`` return ``gt::gui::Icon``, which can be passed
where Qt expects a ``QIcon``. In GTlab developer mode, use the icon browser to
inspect the available names and their appearance.

Custom icons
------------

Use ``gt::gui::getIcon(resourcePath)`` for an icon stored in the module's Qt
resources. SVG resources are adapted to the active theme, making them the
preferred format for single-color interface symbols:

.. code-block:: cpp

  auto icon = gt::gui::getIcon(QStringLiteral(":/mymodule/icons/result.svg"));
  resultButton->setIcon(icon);

Use ``gt::gui::colorize`` only when color conveys additional meaning and check
the result in enabled, disabled, light, and dark states. Do not encode essential
state through color alone; pair it with text, shape, or another accessible cue.

Keep action semantics consistent. For example, use existing add, remove, save,
and configuration icons for those operations instead of introducing a
module-specific alternative.
