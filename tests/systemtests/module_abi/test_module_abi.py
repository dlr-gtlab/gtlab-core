# SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

"""End-to-end checks for module naming and pre-load ABI validation."""

import json
import os
import shutil
import subprocess
from pathlib import Path
from typing import Dict, List, Optional

import pytest


def _required_path(name: str) -> Path:
    value = os.environ.get(name)
    assert value, f"{name} must be set by CTest"
    path = Path(value)
    assert path.is_file(), f"Required test binary does not exist: {path}"
    return path


@pytest.fixture
def binaries() -> Dict[str, Path]:
    return {
        "console": _required_path("GTLAB_CONSOLE"),
        "compatible": _required_path("GTLAB_ABI_TEST_MODULE"),
        "adjacent": _required_path("GTLAB_ABI_TEST_ADJACENT_LIBRARY"),
        "incompatible": _required_path("GTLAB_ABI_TEST_INCOMPATIBLE_MODULE"),
        "missing": _required_path("GTLAB_ABI_TEST_MISSING_MODULE"),
        "inspector": _required_path("GTLAB_ABI_METADATA_INSPECTOR"),
    }


@pytest.fixture
def console_environment(tmp_path: Path) -> Dict[str, str]:
    environment = os.environ.copy()
    environment["QT_QPA_PLATFORM"] = "offscreen"
    environment["HOME"] = str(tmp_path / "home")
    environment["XDG_CONFIG_HOME"] = str(tmp_path / "config")
    environment["XDG_DATA_HOME"] = str(tmp_path / "data")
    environment["XDG_CACHE_HOME"] = str(tmp_path / "cache")
    environment.pop("GTLAB_MODULE_DIRS", None)
    return environment


def _copy_module(source: Path, directory: Path, filename: str) -> Path:
    directory.mkdir(parents=True, exist_ok=True)
    destination = directory / filename
    shutil.copy2(source, destination)
    return destination


def _copy_adjacent_dependency(binaries: Dict[str, Path], directory: Path) -> None:
    shutil.copy2(binaries["adjacent"], directory / binaries["adjacent"].name)


def _run_process(
    command: List[str],
    cwd: Path,
    environment: Optional[Dict[str, str]] = None,
):
    return subprocess.run(
        command,
        cwd=cwd,
        env=environment,
        text=True,
        capture_output=True,
        check=False,
    )


def _load_module(console: Path, module: Path, environment: Dict[str, str]):
    module_environment = dict(environment)
    module_environment["PATH"] = os.pathsep.join(
        [str(module.parent), module_environment.get("PATH", "")]
    )
    return _run_process(
        [str(console), "load_module", str(module)], module.parent, module_environment
    )


def _discover_modules(
    console: Path,
    module_directory: Path,
    cwd: Path,
    environment: Dict[str, str],
):
    module_environment = dict(environment)
    module_environment["GTLAB_MODULE_DIRS"] = str(module_directory)
    module_environment["PATH"] = os.pathsep.join(
        [str(module_directory), module_environment.get("PATH", "")]
    )
    return _run_process([str(console), "--help"], cwd, module_environment)


def _metadata(inspector: Path, module: Path) -> dict:
    result = _run_process([str(inspector), str(module)], module.parent)
    assert result.returncode == 0, result.stderr
    return json.loads(result.stdout)


def _plugin_values(metadata: dict) -> dict:
    values = {**metadata, **metadata.get("MetaData", {})}
    return {
        key: value[0] if isinstance(value, list) and len(value) == 1 else value
        for key, value in values.items()
    }


def test_module_build_embeds_core_and_abi_metadata(binaries: Dict[str, Path]):
    """Built modules carry the Core version and module ABI in Qt metadata."""
    module = binaries["compatible"]
    assert module.name.endswith(
        f".gtmod.{os.environ['GTLAB_MODULE_ABI']}{module.suffix}"
    )

    values = _plugin_values(_metadata(binaries["inspector"], module))
    assert values["gtlab_core_version"] == os.environ["GTLAB_CORE_VERSION"]
    assert values["gtlab_module_abi"] == os.environ["GTLAB_MODULE_ABI"]


def test_compatible_module_is_loaded(binaries, console_environment, tmp_path):
    """A module with matching filename and metadata ABI executes successfully."""
    marker = tmp_path / "compatible-loaded"
    environment = console_environment
    environment["GTLAB_ABI_TEST_MARKER"] = str(marker)
    module = _copy_module(
        binaries["compatible"], tmp_path / "modules", binaries["compatible"].name
    )
    _copy_adjacent_dependency(binaries, module.parent)

    result = _load_module(binaries["console"], module, environment)

    assert result.returncode == 0, result.stdout + result.stderr
    assert marker.is_file(), "compatible plugin code was not executed"


@pytest.mark.parametrize("variant", ["incompatible", "missing"])
def test_incompatible_or_missing_abi_is_rejected_before_plugin_code(
    variant, binaries, console_environment, tmp_path
):
    """Direct loading rejects incompatible or missing ABI metadata pre-load."""
    marker = tmp_path / f"{variant}-loaded"
    environment = console_environment
    environment["GTLAB_ABI_TEST_MARKER"] = str(marker)
    source = binaries[variant]
    module = _copy_module(source, tmp_path / "modules", source.name)
    _copy_adjacent_dependency(binaries, module.parent)

    result = _load_module(binaries["console"], module, environment)

    assert result.returncode != 0, result.stdout + result.stderr
    assert not marker.exists(), "incompatible plugin code executed before rejection"
    assert "module ABI" in result.stdout + result.stderr


@pytest.mark.parametrize("variant", ["incompatible", "missing"])
def test_directory_discovery_rejects_module_before_plugin_code(
    variant, binaries, console_environment, tmp_path
):
    """Directory discovery rejects incompatible or missing ABI before loading."""
    marker = tmp_path / f"discovered-{variant}-loaded"
    environment = console_environment
    environment["GTLAB_ABI_TEST_MARKER"] = str(marker)
    source = binaries[variant]
    module_directory = tmp_path / "modules"
    _copy_module(source, module_directory, source.name)
    _copy_adjacent_dependency(binaries, module_directory)
    result = _discover_modules(
        binaries["console"], module_directory, tmp_path, environment
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert not marker.exists(), "discovered incompatible plugin code executed"
    assert "module ABI" in result.stdout + result.stderr


def test_renaming_incompatible_module_does_not_change_compatibility(
    binaries, console_environment, tmp_path
):
    """An ABI-incompatible plugin stays rejected after its filename is changed."""
    marker = tmp_path / "renamed-loaded"
    environment = console_environment
    environment["GTLAB_ABI_TEST_MARKER"] = str(marker)
    module = _copy_module(
        binaries["incompatible"],
        tmp_path / "modules",
        f"Renamed.gtmod.{os.environ['GTLAB_MODULE_ABI']}{binaries['incompatible'].suffix}",
    )
    _copy_adjacent_dependency(binaries, module.parent)

    result = _load_module(binaries["console"], module, environment)

    assert result.returncode != 0, result.stdout + result.stderr
    assert not marker.exists(), "renamed incompatible plugin code executed"
    assert "filename ABI" in result.stdout + result.stderr


def test_module_directory_ignores_native_dependencies(
    binaries, console_environment, tmp_path
):
    """Discovery ignores adjacent shared libraries and loads the real module."""
    module_directory = tmp_path / "modules"
    module_directory.mkdir()
    module = _copy_module(
        binaries["compatible"], module_directory, binaries["compatible"].name
    )
    _copy_adjacent_dependency(binaries, module_directory)
    marker = tmp_path / "compatible-discovered"
    environment = console_environment
    environment["GTLAB_ABI_TEST_MARKER"] = str(marker)
    result = _discover_modules(
        binaries["console"], module_directory, tmp_path, environment
    )

    assert result.returncode == 0, result.stdout + result.stderr
    assert "AbiAdjacentDependency" not in result.stdout + result.stderr
    assert marker.is_file(), "compatible module was not discovered and loaded"
