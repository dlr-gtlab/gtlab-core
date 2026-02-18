# AGENTS.md

## Purpose
Persistent project guide for work in `gtlab-core` so a new session can start immediately.

## Repository Orientation
- Main source tree: `src/`
- Main developer docs root: `docs/dev/`
- Build directory (pre-configured): `build/`
- PR template: `.github/pull_request_template.md`

## Working Environments
### Build environment
- Use the prepared build directory at repo root: `build/`.
- Detect the configured CMake generator from `build/CMakeCache.txt` (`CMAKE_GENERATOR`).
- Unit test workflow:
  - Configure unit tests: `cmake -S . -B build -DBUILD_UNITTESTS=ON`
  - Configure test modules: `cmake -S . -B build -DBUILD_TESTMODULES=ON`
  - Configure both: `cmake -S . -B build -DBUILD_UNITTESTS=ON -DBUILD_TESTMODULES=ON`
  - Build from `build/` with the matching parallel command:
    - `Unix Makefiles` -> `make -j$(nproc)`
    - `Ninja` -> `ninja -j$(nproc)`
  - Run test runner directly: `./build/GTlabUnitTest`
- Prefer incremental builds in the existing `build/` directory unless a clean reconfigure is explicitly needed.

### Documentation environment
- Activate docs environment from repo root:
  - `source docs/docenv/bin/activate`
- Build docs:
  - `cd docs/dev && make`
- Rebuild after every documentation change to catch warnings/errors early.

## Change Guidelines
### General
- Keep edits minimal and targeted.
- Avoid changing unrelated content.
- Preserve existing style and architecture unless a change is intentional and justified.
- When behavior changes, keep code, docs, and deprecation annotations aligned.

### Code changes
- Prefer backward-compatible changes unless a breaking change is explicitly intended.
- Keep patch branches (for example `2.0.X`) ABI-stable.
- In general, avoid ABI changes whenever possible.
- Keep public API changes explicit and documented.
- Add or update focused tests where practical.

### Documentation changes
- Prefer concise, actionable wording.
- Keep a logical reading flow: overview -> required changes -> details -> examples/checklists.
- Use consistent terminology and version notation (`2.0.12`, `2.1`, `Qt6`, etc.).
- Keep tables readable (short headers, concise cells).

## CMake/Qt Migration Guidance To Preserve
- Migration/deprecation guide: `docs/dev/howtos/migration_and_deprecation.rst`
- Do not document copying a local `GTlab.cmake` into module projects.
- Use installed GTlab CMake integration via `find_package(GTlab REQUIRED)`.
- Use `require_qt(...)` for Qt detection instead of direct `find_package(Qt5/Qt6 ...)` in migration guidance.
- Use version-agnostic Qt targets in examples, e.g. `Qt${QT_VERSION_MAJOR}::Core`.

## Validation Checklist Before Commit
- Build succeeds from `build/` with the configured generator.
- If code changed, run relevant tests/checks for touched components.
- If docs changed, docs build succeeds: `cd docs/dev && make`.
- If migration/deprecation docs changed, ensure TOC/headings/references render correctly.
- PR description is written using `.github/pull_request_template.md` as base.

## Commit/PR Standards
- Use clear, scoped commit messages (separate code/docs/refactor where practical).
- In PR description, state:
  - what changed,
  - why it changed,
  - how it was validated (build/tests/docs checks as applicable).
