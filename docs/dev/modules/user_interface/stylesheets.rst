Styles and colors
=================

Standard Qt widgets already follow the active GTlab palette. Prefer their
default appearance and layouts. When a widget needs one of GTlab's established
special styles, use the helpers in ``gt_stylesheets.h``:

.. code-block:: cpp

  #include "gt_stylesheets.h"

  addButton->setStyleSheet(gt::gui::stylesheet::button());
  messageLabel->setStyleSheet(gt::gui::stylesheet::warningLabel());

Available helpers cover common buttons, labels, line edits, combo boxes,
spin boxes, tooltips, background frames, and process-run button states. They
return a ``QString`` suitable for ``QWidget::setStyleSheet``.

Theme-aware colors
------------------

For custom painting, use colors from ``gt_colors.h`` rather than hard-coded
RGB values:

.. code-block:: cpp

  #include "gt_colors.h"

  painter.setPen(gt::gui::color::text());
  painter.setBrush(gt::gui::color::highlight());

The ``gt::gui::color`` namespace provides palette colors such as ``base()``,
``text()``, ``disabled()``, ``highlight()``, and frame colors, plus semantic
colors used elsewhere in GTlab. ``lighten`` and ``darken`` account for the
current theme direction.

Avoid large widget-specific stylesheets when layouts, palettes, or a small
paint implementation are sufficient. Broad selectors can unexpectedly alter
child widgets and are difficult to keep correct across Qt and GTlab versions.
Always verify contrast, disabled states, selection, focus, and both supported
themes.
