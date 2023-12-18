<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->


# ![gtlab logo](src/resources/pixmaps/gt-logo.png)

This repository includes the core __GTlab core framework__. GTlab (Gas Turbine Laboratory) is a versatile
software framework designed for collaborative, multidisciplinary research in aircraft propulsion.
It supports the design and simulation of propulsion systems and components across various levels
of detail throughout their lifecycle. With advanced core functions in data modeling, management,
workflow, and collaborative architectures, GTlab extends beyond propulsion technology,
allowing  generic applications across diverse fields.

GTlab allows the addition of specialized, plugin-enabled extension `modules` to support a wide
spectrum of use cases. GTlab's modularity proves vital in promoting teamwork and collaboration,
allowing different teams to efficiently collaborate and pool their expertise on a unified platform.  

We welcome the development of extension module from interested parties.
Please use our [test modules](tests/modules) for a first guideline on how to implement GTlab modules.
Please [contact us](https://www.dlr.de/at/de/desktopdefault.aspx/tabid-1500/mailcontact-29012/),
if you are interested in working with us together.

## Building GTlab

### Prerequisites

Our own libraries required to build GTlab are bundled with GTlab and will be automatically
downloaded during the CMake configuration phase. Whether to use the bundled libraries can be
adjusted using the CMake variable `GTLAB_USE_BUNDLED_XXX=ON`.

GTlab comes with optional support for HDF5. To enable, set `BUILD_WITH_HDF5=ON` during the config step.

| Library       |  Version  | Required | Bundled | Where to get                                 |
| ------------- | --------- | -------- | ------- | -------------------------------------------- |
| Qt            |  5.15.x   | Yes      | No      | https://download.qt.io/official_releases/qt/ |
| GTlab Logging | >= 4.4.0  | Yes      | Yes     | https://github.com/dlr-gtlab/gt-logging      |
| GenH5         | >= 3.3.0  | No       | Yes     | https://github.com/dlr-gtlab/genh5           |
| HDF5          | >= 1.12.0 | No       | No      | https://github.com/HDFGroup/hdf5/releases    |

### Building

GTlab requires a recent `CMake` (>3.15) to build. The configuration and build process is
similar to other CMake builds:

```
cmake -S . -B build -DQt5_DIR=<path/to/cmake/Qt5> -DHDF5_DIR=<path/to/cmake/hdf5> -DCMAKE_INSTALL_PREFIX=<path/to/install>
cmake --build build
cmake --build build --target install
```

In case you own GTlab's development tools, you can simplify CMake's configure step via

```
cmake -S . -B build -DGTLAB_DEVTOOLS_DIR=<path/to/devtools> -DQt5_DIR=<path/to/cmake/Qt5> -DCMAKE_INSTALL_PREFIX=<path/to/install>
```

## Cite us
To scite the framework: Reitenbach S. and Vieweg  M. and Becker R. and Hollmann C. and Wolters F. and Schmeink J. and Otten T. and Siggel M.

[Collaborative Aircraft Engine Preliminary Design using a Virtual Engine Platform, Part A: Architecture and Methodology, AIAA Scitech 2020 Forum, 2020](https://arc.aiaa.org/doi/10.2514/6.2020-0867)

```
@inproceedings{reitenbach2020gtlab,
  title = {Collaborative Aircraft Engine Preliminary Design using a Virtual Engine Platform,
           Part A: Architecture and Methodology},
  author = {Reitenbach, Stanislaus and Vieweg, Maximilian and Becker, Richard and Hollmann, Carsten
            and Wolters, Florian and Schmeink, Jens and Otten, Tom and Siggel, Martin},
  booktitle = {AIAA Scitech 2020 Forum},
  year = {2020},
  doi = {10.2514/6.2020-0867}
  issn = {AIAA 2020-0867},
}
```

To scite the data model: Reitenbach S. and Hollmann C. and Schmeink J.
and Vieweg M. and Otten T. and Haessy J. and Siggel M.

[Parametric datamodel for collaborative preliminary aircraft engine design, AIAA Scitech 2021 Forum, 2021](https://arc.aiaa.org/doi/10.2514/6.2021-1419)

```
@inproceedings{reitenbach2021dm,
  title={Parametric datamodel for collaborative preliminary aircraft engine design},
  author={Reitenbach, Stanislaus and Hollmann, Carsten and Schmeink, Jens and
          Vieweg, Maximilian and Otten, Tom and Haessy, Jannik and Siggel, Martin},
  booktitle={AIAA Scitech 2021 Forum},
  year={2021},
  doi = {10.2514/6.2021-1419},
  issn = {AIAA 2021-1419}
}
```

--------
