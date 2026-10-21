<!-- SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR) -->
<!-- SPDX-License-Identifier: MPL-2.0+ -->

## summary
### Description
{{summary}}

Closes #{{issue_number}}

### How Has This Been Tested?
See the ProPR run status and linked logs below for execution and validation details.

## review_guidelines
### What should reviewers look at closely?
Focus on non-obvious logic, assumptions, API or ABI impact, and any issues identified during automated review.

### Checklist
- [ ] A test for the new functionality was added (if applicable).
- [ ] All tests run without failure.
- [ ] The changelog has been extended, if this change contains user-visible changes.
- [ ] New interface methods or functions are exported via `EXPORT`; non-interface functions are not exported.
- [ ] The module ABI impact has been considered. The first PR that introduces a binary-incompatible change bumps `GTLAB_MODULE_ABI`; patch releases must not introduce ABI breaks.

## commits

## files_changed

## trailer
