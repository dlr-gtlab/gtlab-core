<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->


<div class="hideindoc" id="badgesbox">
 <img src="https://raw.githubusercontent.com/dlr-gtlab/gtlab-core/master/src/resources/pixmaps/gt-logo.png" alt="gtlab logo" style="max-width: 100%;">
 
 <a href="https://zenodo.org/doi/10.5281/zenodo.10666586"><img src="https://zenodo.org/badge/727352741.svg" alt="DOI"></a>
 <a href="https://www.gtlab.de/pages/documentation.html"><img src='https://readthedocs.org/projects/gtlab/badge/?version=latest' alt='Documentation' /></a>
 <a href="https://codecov.io/github/dlr-gtlab/gtlab-core" ><img src="https://codecov.io/github/dlr-gtlab/gtlab-core/graph/badge.svg?token=05UZ6982F6"/></a>
 <!-- Add badges here-->
</div>

# GTlab

This repository includes the __GTlab core framework__. GTlab (Gas Turbine Laboratory) is a versatile
software framework designed for multidisciplinary, collaborative research in the field of aircraft propulsion.
It supports the design and simulation of propulsion systems and individual components across various levels
of detail throughout the entire lifecycle. With advanced core functions in data modeling, data management,
workflow management, and other collaborative architectures, GTlab has the potential for applications far beyond the area of propulsion technology.

The key strength of GTlab is its consistently implemented modularity. In addition to powerful core functions and modules, GTlab allows the integration of special plugin-capable extensions to support a wide range of applications. Its high degree of adaptability, extensibility and flexibility is ensured by an object-oriented software concept based on the C++ programming language, coupled with the utilisation of standardised libraries.

GTlab's modularity is also a key factor in promoting teamwork and collaboration. Different teams can work together efficiently and combine their expertise on a common platform. This increases productivity while encouraging the interdisciplinary approach that is essential for solving complex engineering challenges.

## Why GTlab

GTlab is an open and modular research software platform for collaborative engineering workflows.
It combines data modeling, workflow automation, scripting, and extensibility through plugins in one framework.
Originally developed for aircraft propulsion research, GTlab is designed so that methods, modules, and
domain-specific extensions can be reused across teams and application areas.

This repository contains the GTlab core framework. For most users, the recommended way to get started is
to install the complete GTlab distribution via the official installers rather than building the core from source.

## Installation

For normal use, we recommend installing GTlab via the official GTlab Community Installer.

- Download GTlab for Windows or Linux from the [official download page](https://www.gtlab.de/pages/download.html).
- The Community Installer installs GTlab together with the available modules and can also be used to update GTlab.
- For DLR internal users, an internal installer with additional modules is also available through internal DLR distribution channels.
- Building from source is mainly intended for developers and contributors; see [Building GTlab](#building-gtlab).

GTlab is supported on Windows and Linux. macOS should theoretically work, but is currently not officially tested.

## Documentation

GTlab provides separate documentation for users and developers:

- [User Documentation](https://gtlab.readthedocs.io/projects/user)
- [Developer Documentation](https://gtlab.readthedocs.io/en/latest/)

If you want to use GTlab, start with the user documentation.
If you want to extend GTlab, develop modules, or build from source, use the developer documentation.

## Learn More

- [Feature overview](https://www.gtlab.de/pages/features.html)
- [Screenshots](https://www.gtlab.de/pages/screenshots.html)

We welcome the development of extension modules by interested parties.
Please use our [test modules](https://github.com/dlr-gtlab/gtlab-core/tree/master/tests/modules) for a first guideline on how to implement GTlab modules.
If you need help or are interested in collaborating with us, please open an issue on GitHub or contact us at [gtlab-support@dlr.de](mailto:gtlab-support@dlr.de).

## License

The largest portion of the code is licensed under the `Mozilla Public License 2.0` (MPL 2.0).
This license allows in particular
 - The use of GTlab in closed-source environments for commercial applications.
 - Creation and distribution of commercial modules.

Smaller third-party parts of the code base use other permissive licenses, such as the
`BSD` and `MIT` licenses. Please review the directory [LICENSES](https://github.com/dlr-gtlab/gtlab-core/tree/master/LICENSES) and [.reuse](https://github.com/dlr-gtlab/gtlab-core/tree/master/.reuse)
for a full overview of all licenses used.

## Building GTlab

This section is intended for developers who want to build or modify GTlab from source.

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
To cite the framework: Reitenbach S. and Vieweg M. and Becker R. and Hollmann C. and Wolters F. and Schmeink J. and Otten T. and Siggel M.

[Collaborative Aircraft Engine Preliminary Design using a Virtual Engine Platform, Part A: Architecture and Methodology, AIAA Scitech 2020 Forum, 2020](https://arc.aiaa.org/doi/10.2514/6.2020-0867)

```
@inproceedings{reitenbach2020gtlab,
  title = {Collaborative Aircraft Engine Preliminary Design using a Virtual Engine Platform,
           Part A: Architecture and Methodology},
  author = {Reitenbach, Stanislaus and Vieweg, Maximilian and Becker, Richard and Hollmann, Carsten
            and Wolters, Florian and Schmeink, Jens and Otten, Tom and Siggel, Martin},
  booktitle = {AIAA Scitech 2020 Forum},
  year = {2020},
  doi = {10.2514/6.2020-0867},
  issn = {AIAA 2020-0867},
}
```

To cite the data model approach: Reitenbach S. and Hollmann C. and Schmeink J.
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
