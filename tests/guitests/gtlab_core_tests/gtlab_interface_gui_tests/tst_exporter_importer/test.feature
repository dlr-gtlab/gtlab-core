
Feature: Test the exporter and importer interface

    Tests the export and import of mementos using the 'Test Exporter Interface'
    and 'Test Importer Interface' modules. Both modules do not own a data model,
    they act on the classes of the 'Test Datamodel Interface' module.


    Scenario: Setup

        Given GTlab is running
          And only the modules 'Test Datamodel Interface, Test Exporter Interface, Test Importer Interface' are included at startup


    Scenario: The exporters and importers are listed in the explorer context menu

        # enable the required plugins in case they caused a crash
        Given the plugins 'ExporterInterface, ImporterInterface' are marked as reenabled at startup
        Given GTlab is running
          And the dock widgets 'Explorer, Properties, Output' are visible
          And the project 'gui_test_export_import' is removed from the session
         # create a project using the test data model
         When the action 'File > New Project' in the main menu bar is activated
         Then the 'New Project' wizard is enabled
         When the wizard proceeds
          And the input mask is filled out accordingly:
              | label_name | widget_type | input                                         |
              | Name:      | QLineEdit   | gui_test_export_import                        |
              | Directory: | QLineEdit   | $AUT/../tests/guitests/gui_test_export_import |
          And the wizard proceeds
         Then the wizard page title reads 'Project Modules'
          And the wizard should contain a view of type 'QListView'
          And the entry 'Test Datamodel Interface' exists
          And the entry is checked
         When the wizard proceeds
          And the wizard finishes
          And wait 5 seconds
         Then the project 'gui_test_export_import' exists
          And the project entry 'Test DMI Package > Internal_Data' exists
         # the exporter is registered for the data class -> export submenu exists
         When the project entry 'Test DMI Package > Internal_Data' is selected
          And the context menu of the entry is opened
         Then the action 'Export' exists
          And the action 'Memento Exporter...' in the submenu exists
         # the restore importer is registered for the data class
         When every visible menu is closed
          And the context menu of the entry is opened
         Then the action 'Import' exists
          And the action 'Restore Memento...' in the submenu exists
         # the append importer only targets the package
         When every visible menu is closed
          And the context menu of the entry is opened
         Then the action 'Import' exists
          And the action 'Append Memento...' in the submenu does not exist
         # the append importer is registered for the package
         When every visible menu is closed
          And the project entry 'Test DMI Package' is selected
          And the context menu of the entry is opened
         Then the action 'Import' exists
          And the action 'Append Memento...' in the submenu exists
         # the restore importer does not target the package
         When every visible menu is closed
          And the context menu of the entry is opened
         Then the action 'Import' exists
          And the action 'Restore Memento...' in the submenu does not exist
         When every visible menu is closed


    Scenario: An exported object can be restored from its memento

        Given GTlab is running
          And the dock widgets 'Explorer, Properties, Output' are visible
         When the project 'gui_test_export_import' is opened
          And the project entry 'Test DMI Package > Internal_Data' is selected
          And the output is cleared
          And the value of the property 'Value' is set to '5'
         Then the property 'Value' has the value '5.0'
         # export the object into the project directory
         When the action 'Export > Memento Exporter...' in the context menu of the entry is activated
         Then the 'Export Wizard' wizard is enabled
          And the wizard page title reads 'Memento Export Options'
         When the wizard proceeds
          And the input mask is filled out accordingly:
              | label_name | widget_type | input                                                              |
              | File Path  | QLineEdit   | $AUT/../tests/guitests/gui_test_export_import/data_memento.xml |
          And the wizard finishes
         Then the last output line reads 'Memento export finished'
         # modify the object
         When the output is cleared
          And the value of the property 'Value' is set to '7'
         Then the property 'Value' has the value '7.0'
          And the project has unsaved changes
         # restore the exported state of the object
         When the action 'Import > Restore Memento...' in the context menu of the entry is activated
         Then the 'Import Wizard' wizard is enabled
          And the wizard page title reads 'Restore Memento Options'
         When the wizard proceeds
          And the input mask is filled out accordingly:
              | label_name | widget_type | input                                                              |
              | File Path  | QLineEdit   | $AUT/../tests/guitests/gui_test_export_import/data_memento.xml |
          And the wizard finishes
         Then the property 'Value' has the value '5.0'


    Scenario: An exported object can be imported as new child of the package

        Given GTlab is running
          And the dock widgets 'Explorer, Properties, Output' are visible
         When the project 'gui_test_export_import' is opened
          And the project entry 'Test DMI Package' is selected
         Then the subentry 'Internal_Data_appended' does not exist
         # append the exported object as new child of the package
         When the action 'Import > Append Memento...' in the context menu of the entry is activated
         Then the 'Import Wizard' wizard is enabled
          And the wizard page title reads 'Append Memento Options'
         When the input mask is filled out accordingly:
              | label_name | widget_type | input     |
              | Suffix     | QLineEdit   | _appended |
          And the wizard proceeds
          And the input mask is filled out accordingly:
              | label_name | widget_type | input                                                              |
              | File Path  | QLineEdit   | $AUT/../tests/guitests/gui_test_export_import/data_memento.xml |
          And the wizard finishes
         Then the subentry 'Internal_Data_appended' exists
          And the project has unsaved changes
         # the appended object carries the data of the exported memento
         When the project entry 'Test DMI Package > Internal_Data_appended' is selected
         Then the property 'Value' has the value '5.0'


    Scenario: Remove the corresponding project from the active session

        Given GTlab is running
          And the dock widget 'Explorer' is visible
         When the project 'gui_test_export_import' is closed
          And the project is deleted permanently
         Then the project does not exist
