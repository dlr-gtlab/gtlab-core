Building the documentation
--------------------------

To build the documentation, __Doxygen__ and __Sphinx__ need to be installed.

To install sphinx and its dependencies, simply run

    pip install -r requirements.txt

> **Note:** On Linux/GNU systems you may need to use a virtual environment, use
>
>     python3 -m venv venv
>     source venv/bin/activate

To build the developer documentation, use

    cd dev
    doxygen
    sphinx-build . html

To build the user documentation, use

    cd user
    sphinx-build . html

To start a local web-server for serving the HTML requests, use

    python3 -m http.server -d html