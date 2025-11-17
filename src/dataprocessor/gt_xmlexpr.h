/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
 */

#ifndef GT_XMLEXPR_H
#define GT_XMLEXPR_H

namespace gt
{
namespace xml
{

constexpr const char * S_GTLAB_TAG = "GTLAB";
constexpr const char * S_OBJECT_TAG = "object";
constexpr const char * S_OBJECTLIST_TAG = "objectlist";
constexpr const char * S_OBJECTFILE_TAG = "GTLABOBJECTFILE";
constexpr const char * S_OBJECTREF_TAG = "objectref";
constexpr const char * S_ASLINK_TAG = "aslink";
constexpr const char * S_HREF_TAG = "href";
constexpr const char * S_UUID_TAG = "uuid";
constexpr const char * S_CLASS_TAG = "class";
constexpr const char * S_NAME_TAG = "name";
constexpr const char * S_ENTRY_NAME_TAG = "entryName";
constexpr const char * S_TYPE_TAG = "type";
constexpr const char * S_ID_TAG = "id";
constexpr const char * S_VALUE_TAG = "value";
constexpr const char * S_OPTIONAL_TAG = "optional";
constexpr const char * S_ACTIVE_TAG = "active";
constexpr const char * S_PROPERTY_TAG = "property";
constexpr const char * S_PROPERTYLIST_TAG = "propertylist";
constexpr const char * S_PROPERTYCONT_TAG = "property-container";
constexpr const char * S_DIFF_INDEX_TAG = "index";
constexpr const char * S_DIFF_INDEX_CHANGED_TAG = "diff-index-changed";
constexpr const char * S_ENTRY_TAG = "entry";
constexpr const char * S_DIFF_OBJ_REMOVE_TAG = "diff-object-remove";
constexpr const char * S_DIFF_OBJ_ADD_TAG = "diff-object-add";
constexpr const char * S_DIFF_PROP_CHANGE_TAG = "diff-property-change";
constexpr const char * S_DIFF_PROPLIST_CHANGE_TAG = "diff-propertylist-change";
constexpr const char * S_DIFF_PROPCONT_ENTRY_ADDED_TAG =
        "diff-property-container-entry-add";
constexpr const char * S_DIFF_PROPCONT_ENTRY_REMOVE_TAG =
        "diff-property-container-entry-remove";
constexpr const char * S_DIFF_PROPCONT_ENTRY_CHANGE_TAG =
        "diff-property-container-entry-change";
constexpr const char * S_DIFF_ATTR_CHANGE_TAG = "diff-attribute-change";
constexpr const char * S_DIFF_ATTR_REMOVE_TAG = "diff-attribute-remove";
constexpr const char * S_DIFF_NEWVAL_TAG = "newVal";
constexpr const char * S_DIFF_OLDVAL_TAG = "oldVal";

} // namespace xml
} // namespace gt

#endif // GT_XMLEXPR_H
