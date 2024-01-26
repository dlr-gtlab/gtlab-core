# Configuration file for the Sphinx documentation builder.
#
# This file only contains a selection of the most common options. For a full
# list see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Path setup --------------------------------------------------------------

# If extensions (or modules to document with autodoc) are in another directory,
# add these directories to sys.path here. If the directory is relative to the
# documentation root, use os.path.abspath to make it absolute, like shown here.
#
# import os
# import sys
# sys.path.insert(0, os.path.abspath('.'))


# -- Project information -----------------------------------------------------

project = 'gtlab'
copyright = '2024, German Aerospace Center'
author = 'Martin Siggel, DLR AT'

# The full version, including alpha/beta/rc tags
release = "2.0.6"


# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = [
    "breathe" ,                # Parses doxygen xml
    "myst_parser",             # Parses markdown files
    "sphinx_toolbox.collapse", # Collapsing text blocks
    'sphinx_copybutton'        # Copy buttom for code blocks
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ["Readme.md"]


# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'furo'

cpp_index_common_prefix = ['Gt']

html_theme_options = {
    "sidebar_hide_name": True,
}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = []

html_logo = 'images/gt-logo.png'

# Breathe Configuration
breathe_projects = {"gtlab": "doxygen/xml"}
breathe_default_project = "gtlab"

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}