Logging
=======

In GTlab, a comprehensive logging framework is provided to assist module developers in effectively
capturing and managing log messages from various components of their applications.

This section explores the logging capabilities offered by GTlab, including configuration options,
logging levels, custom logging IDs, support for custom types, and integration with external libraries
such as Qt and the C++ Standard Template Library (STL).

Basic Logging
-------------

Preparation
^^^^^^^^^^^

When using GTlab's logging library, module developer need to define the preprocessor variable
``GT_MODULE_ID`` accordingly. Settings this variable up is part of the module building instructions
(see :ref:`Getting Started <getting-started>`). 

Logging Levels
^^^^^^^^^^^^^^

In GTlab, logging messages are associated with different logging levels, representing their importance in the context of debugging and monitoring.
For example, an error has a higher importance than a information message.
Consistent logging practices across all components of GTlab including the core framework, first-party, and third-party modules, is essential for effective debugging and monitoring.

To maintain consistency in logging practices, the following table serves as a guideline for determining when each logging level should be used:

.. list-table::
   :widths: 20 80
   :header-rows: 1

   * - Logging Level
     - Description

   * - Fatal
     - Unrecoverable errors that lead to the termination of the application.

   * - Error
     - Indicates critical errors that need attention of the user and may impact the workflow's functionality.

   * - Warning
     - Indicates potential issues or situations that might lead to problems if not addressed.

   * - Info
     - General information about the process, useful for understanding the system's normal operation.

   * - Debug
     - Detailed information, typically of interest only during development or debugging phases.

   * - Trace
     - Very detailed information, more granular than Debug, used for tracing execution flow.


Usage Guidelines:

- *Trace*: Provides very detailed information, more granular than Debug, typically used for tracing the execution flow within the system. This level of logging can be extremely verbose and is typically reserved for troubleshooting complex issues.

- *Debug*: Use for detailed information relevant during development or debugging phases. This level of logging is typically disabled in production environments due to its verbosity.

- *Info*: Utilize for general information about your module events or functions to provide a high-level overview of what's happening. This level of logging is often enabled in production environments to monitor system behavior.
  Use sparingly to avoid flooding the log.

- *Warning*: Indicate potential issues or situations that may require attention by the user to avoid follow-up problems.

- *Error*: Notify of errors or unexpected behaviors that may impact or interrupt your modules functionality, such as failed operations or invalid inputs.

- *Fatal*: Reserved for severe errors or critical conditions that require immediate attention, such as unrecoverable failures or resource exhaustion.



Logging Functions
^^^^^^^^^^^^^^^^^

GTlab provides four primary functions that module developers should utilize
to log messages effectively. These functions, namely ``gtFatal``, ``gtError``, ``gtWarning``,
and ``gtInfo``, are used as follows:

.. code-block:: cpp

    gtInfo() << "The residuum of the solver is r = " << residuum;
    // INFO  [11-12-42] The residuum of the solver is r = 0.1
    gtWarning() << "The required tolerance could not be achieved!";
    // WARN  [11-12-42] The required tolerance could not be achieved!
    gtError() << "Missing object 'Engine'";
    // ERROR [11-12-42] Missing object 'Engine'
    gtFatal() << "The CPU is malfunctioning. Aborting.";
    // FATAL [11-12-42] The CPU is malfunctioning. Aborting.

Each log message is accompanied by a timestamp and the moduleID in GTlab's output widget.

By employing these logging functions appropriately, developers can ensure comprehensive logging coverage within their modules, aiding in debugging and system monitoring.

Debugging
^^^^^^^^^

In GTlab, developers have the option to log additional information for debugging purposes,
aiding in the identification and resolution of problems within the framework.
By default, GTlab does not log debugging messages.
However, developers can enable the display of debug messages by adjusting the settings within GTlab.

For debugging purposes, GTlab provides the two functions ``gtDebug`` and ``gtTrace``, which can be used as follows:

.. code-block:: cpp

    gtDebug() << "Iteration count = " << count;
    // DEBUG [11-12-42] Iteration count = 101
    gtTrace() << "Begin MyTask::run";
    // TRACE [11-12-42] My fancy trace message

.. note::

    Enabling debugging messages can provide valuable insights during development
    but may result in increased log output and resource usage.
    Hence, use these statements sparingly in productive code.
    Use ``gtDebug``, to print values of variables to help understanding, what the program is doing.
    Use ``gtTrace`` only, to add information on the control flow of your program.
    If unsure, use ``gtDebug``.

**Line Numbers**

The logging system in GTlab offers the option to include the file and line number of the logging statement in the output.
This can be enabled by defining the preprocessor macro ``GT_LOG_LINE_NUMBERS``.

.. code-block:: cpp

    gtDebug() << "My fancy message";
    // DEBUG [11-12-42] my_file.cpp@123: My fancy message

Note, that line number should only be enabled in debug mode!

Advanced Usage
--------------

Custom Logging IDs
^^^^^^^^^^^^^^^^^^

When using the logging functions in GTlab, the module ID is employed as a logging ID.
To enhance the ability to filter specific messages from subcomponents within a module,
custom logging IDs can be specified using the logging functions ``gtFatalId``, ``gtErrorId``,
``gtWarningId``, ``gtInfoId``, ``gtDebugId``, and ``gtTraceId``.

For example

.. code-block:: cpp

    gtWarningId("SolverComponent") << "The solver has not converged";

This usage assigns the custom logging ID "SolverComponent" to the warning message,
enabling more granular filtering and identification of log messages originating
from specific subcomponents within your module.

Add Qt Support
^^^^^^^^^^^^^^

By default, the logging system in GTlab cannot be directly used to log Qt classes such as QString, QObject, etc.
However, developers can enable Qt support within their modules by defining the preprocessor macro ``GT_LOG_USE_QT_BINDINGS``.


.. note::

    If you are using CMake as your build system, ``GT_LOG_USE_QT_BINDINGS`` is already defined
    by the GTlab::Core target. You don't have to do anything.

Extended STL Support
^^^^^^^^^^^^^^^^^^^^

By default, the logging library does not provide logging operators for most STL classes (such as vector, list, tuple, etc.)
to maintain a lean logging interface. However, similar to enabling Qt Support, developers can enable extended
STL support by setting a global define ``GT_LOG_USE_EXTENDED_STL_BINDINGS``.


Alternatively, developers can include specific header files to enable extended STL support for individual STL classes. For example:

- Including `gt_logging/stl_bindings.h` enables extended STL support for all available STL classes.
- Including specific header files like `gt_logging/vector.h` enables extended STL support specifically for `std::vector`.


.. note::

    If Qt support is enabled either globally or by including Qt bindings, all available STL bindings will be included automatically

Logging of Custom Types
^^^^^^^^^^^^^^^^^^^^^^^

To implement logging for a custom type, a custom ``operator<<`` must be defined for this type.
It must be defined in the global namespace:

.. code-block:: cpp

    gt::log::Stream& operator<<(gt::log::Stream& stream, MyType const& type)
    {
        return s << type.value;
    }

Verbosity Levels
^^^^^^^^^^^^^^^^

The logging system also allows for tagging messages to different verbosities.
Conceptually, also the logging levels define a type of verbosity, in which `DEBUG`
should be more verbose than `INFO` etc.

In some rare cases, this is not enough. For example, when e.g. known warning
should be silenced in a specific situations.

In this case, the warning can be made more verbose via

.. code-block:: cpp

    gtInfo().verbose() << "This warning only appears, if the verbosity level is set to 'Everything'";

**Important:** Verbosity levels should be mainly used, when module developers want to differentiate between GTlab's
GUI and the batch mode or other stand-alone commands!

.. warning:: 

    **Prefer logging level over verbosity levels!**

    Only use verbosity levels in exceptional cases when you want to
    make the messages visible in certain cases and in others not.

**Switching the verbosity of the application**

The verbosity of the application can be changed, which has an impact on verbose messages via e.g.

.. code-block:: cpp

    gt::log::Logger::instance().setVerbosity(gt::log::Everything);
