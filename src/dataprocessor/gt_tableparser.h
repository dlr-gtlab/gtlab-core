/* GTlab - Gas Turbine laboratory
 * copyright 2009-2015 by DLR
 *
 * gt_tableparser.h
 *
 *  Created on: 16.04.2015
 *      Author: Richard Becker
 */

#ifndef GT_TABLEPARSER_H
#define GT_TABLEPARSER_H

#include "gt_table.h"

#include <QDomElement>
#include <QVector>

class GT_DATAMODEL_EXPORT GtTableParser
{
public:
    /** Constructor */
    GtTableParser(){}

    /**
     * @brief fromDomElement Creates a table object from a given XML DomElement.
     * @param root Reference to root DomElement.
     * @param success Reference to success flag (output).
     * @return Pointer to newly created DomElement.
     */
    GtTable* fromDomElement(const QDomElement& root, bool& success);

    /**
     * @brief fromDomElement Creates a table object from a given XML DomElement.
     * @param root Reference to root DomElement.
     * @param table which is going to be filled by parsing
     * @param success Reference to success flag (output).
     * @return void
     */
    void fromDomElement(const QDomElement& root, GtTable* table, bool& success);
private:
    /**
     * @brief parseTable Class internal helper method to parse the table.
     * @param root Reference to root DomElement of the table.
     * @param table Pointer to empty table object.
     * @return True if parsing was successful.
     */
    bool parseTable(const QDomElement& root, GtTable* table);

    /**
     * @brief parseAxes Class internal helper method to parse all axes of the
     * table.
     * @param root Reference to root DomElement of the axes list.
     * @param table Pointer to created table object.
     * @return True if parsing was successful.
     */
    bool parseAxes(const QDomElement& root, GtTable* table);

    /**
     * @brief parseAxis Class internal helper method to parse a table axis.
     * @param root Reference to root DomElement of the axis.
     * @param table Pointer to created table object.
     * @return True if parsing was successful.
     */
    bool parseAxis(const QDomElement& root, GtTable* table);

    /**
     * @brief parseValues Class internal helper method to parse the table vals.
     * @param root Reference to root DomElement of the value node.
     * @param table Pointer to created table object.
     * @return True if parsing was successfull.
     */
    bool parseValues(const QDomElement& root, GtTable* table);

    /**
     * @brief parseDoubleVector Internal helper method to parse double arrays.
     * @param root Reference to root DomElement of the array node.
     * @param success Reference to success flag (output).
     * @return Double vector of array elements.
     */
    QVector<double> parseDoubleVector(const QDomElement& root, bool& success);
};

#endif // GT_TABLEPARSER_H

