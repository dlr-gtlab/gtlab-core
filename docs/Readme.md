Building the documentation
--------------------------

To build the documentation, __Doxygen__ and __Sphinx__ need to be installed.

To install sphinx and its dependencies, simply run

    pip install -r requirements.txt


To build the documentation, call

    doxygen
    sphinx-build . html