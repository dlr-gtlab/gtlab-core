#ifndef GT_REGULAREXPRESSION_H
#define GT_REGULAREXPRESSION_H

#include "gt_datamodel_exports.h"

#include <QRegularExpression>


namespace gt
{
namespace rex
{

/**
 * @brief woUmlauts - accepts all but ä ö ü ß
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& woUmlauts();

/**
 * @brief umlauts - accept äöüßÄÖÜ
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& umlauts();

/**
 * @brief umlautsAnsSpecialChar
 * - accepts äöüßÄÖÜ .:,;#?+~* /&%$§!"{}
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& umlautsAndSpecialChar();

/**
 * @brief onlyLetters - accepts all letter (a-z and A-Z) with a
 * minimal length of 1
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& onlyLetters();
GT_DATAMODEL_EXPORT QString onlyLettersHint();

/**
 * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ with a minimal length of 1
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& onlyLettersAndNumbers();
GT_DATAMODEL_EXPORT QString onlyLettersAndNumbersHint();

/**
 * @brief onlyLettersAndNumbers - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ with a minimal length of 1 and dot
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& onlyLettersAndNumbersAndDot();
GT_DATAMODEL_EXPORT QString onlyLettersAndNumbersAndDotHint();

/**
 * @brief onlyLettersAndNumbersAndSpace - accepts all letter (a-z and A-Z)
 * and numbers (0-9) and -  and _ and space with a minimal length of 1
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& onlyLettersAndNumbersAndSpace();
GT_DATAMODEL_EXPORT QString onlyLettersAndNumbersAndSpaceHint();

/**
 * @brief forUnits - accepts all letters and numbers and some
 * special symbols typical used for units
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& forUnits();

/**
 * @brief forExpressions
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& forExpressions();

/**
 * @brief forStations - accepts a combination
 * of an S and one to three numbers
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& forStations();

/**
 * @brief forDoubles
 * @return
 */
GT_DATAMODEL_EXPORT const QRegularExpression& forDoubles();


/**
 * @brief forDoublesLists
 * @return regexp for double lists
 */
GT_DATAMODEL_EXPORT const QRegularExpression& forDoublesLists();


/**
 * @brief for semantic versioning
 * @return regexp for semantic versioning
 *
 * Example: 2.1.3-alpha+4
 *
 * Capturing Groups:
 *
 *   1: Major version (e.g. 2)
 *   3: Minor version (e.g. 1)
 *   5: Patch version (e.g. 3)
 *   6: Prelease (e.g. alpha)
 *   7: Build (e.g. 4)
 */
GT_DATAMODEL_EXPORT const QRegularExpression& forSemVers();


/**
 * @brief Regular expression for matching file dialog filter strings
 *
 * This regular expression matches file dialog filters in the format:
 *
 *   FilterName (pattern1 pattern2 pattern3 ...)
 *
 * Example Matches:
 *   - "Images (*.png *.jpg *.bmp)"
 *   - "Text_Files (*.txt *.md)"
 *   - "Any-Files (*.*)"
 *
 * Capturing Groups:
 *   1: Filter name (e.g., "Images" or "Text_Files")
 *   2: First file pattern including wildcards (e.g., "*.png")
 *   3: Additional file patterns if present (space-separated)
 */
GT_DATAMODEL_EXPORT const QRegularExpression& forFileDialogFilters();

/**
 * @brief Regular expression for matching hexadecimal color codes
 *
 * This regular expression matches typical hex color codes used
 * in web development, Qt, and design tools.
 *
 * It supports:
 *
 *   - 3-digit short hex codes (e.g., `#abc`)
 *   - 4-digit short hex codes with alpha channel (e.g., `#abcd`)
 *   - 6-digit full hex codes (e.g., `#abcdef`)
 *   - 8-digit full hex codes with alpha channel (e.g., `#abcdef12`)
 *
 * Only valid hexadecimal digits (0-9, a-f, A-F) are accepted.
 * A leading '#' character is required.
 *
 * **Examples of valid matches:**
 *   - `#123`
 *   - `#1a2b`
 *   - `#abcdef`
 *   - `#ABCDEF12`
 *
 * @return A QRegularExpression matching valid hex color codes.
 */
GT_DATAMODEL_EXPORT const QRegularExpression& forHexColorCode();


/**
 * @brief Helper function to replace missing QRegExp::ExactMatch
 */
inline bool
exactMatch(const QRegularExpression& re, const QString& input)
{
    QRegularExpressionMatch match = re.match(input);
    return match.hasMatch() && match.capturedLength() == input.length();
}

} // namespace rex
} // namespace gt

#endif // GT_REGULAREXPRESSION_H
