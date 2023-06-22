<!--
SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)

SPDX-License-Identifier: MPL-2.0+
-->

<!--- Provide a general summary of your changes in the Title above -->

## Description
<!--- Describe your changes in detail -->
<!--- Why is this change required? What problem does it solve? -->
<!--- If it fixes an open issue, please link to the issue here. -->

## How Has This Been Tested?
<!--- Please describe in detail how you tested your changes. -->


## Checklist:
<!--- Go over all the following points, and put an `x` in all the boxes that apply. -->
<!--- If you're unsure about any of these, don't hesitate to ask. -->
- [ ] A test for the new functionality was added (if applicable).
- [ ] All tests run without failure.
- [ ] The changelog has been extended, if this MR contains important changes from the users's point of view.
- [ ] The new code complies with the GTlab's style guide.
- [ ] New interface methods / functions are exported via `EXPORT`. Non-interface functions are NOT exported.
- [ ] Free functions are placed inside a namespace (below namespace `gt`).
- [ ] Types (classes) start with `Gt` or are placed inside a namespace.
- [ ] Messages (e.g. in logs or UI elements) are registered via `QObject::tr` for translation.
- [ ] The number of code quality warnings is not increasing.
- [ ] The MR is free of clutter unrelated to the purpose of the MR or the corresponding issue.

/unlabel ~Todo ~Review
/label ~InProgress 