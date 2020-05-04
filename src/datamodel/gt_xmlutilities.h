/* GTlab - Gas Turbine laboratory
 * Source File: gt_xmlutilities.h
 * copyright 2009-2020 by DLR
 *
 *  Created on: 04.05.2020
 *  Author: Stanislaus Reitenbach (AT-TWK)
 *  Tel.: +49 2203 601 2907
 */

#ifndef GTXMLUTILITIES_H
#define GTXMLUTILITIES_H

#include "gt_datamodel_exports.h"

class QDomDocument;
class QDomElement;
class QXmlStreamWriter;
class QString;

namespace GtXmlUtilities
{

/**
 * @brief Writes given QDomElement object to given XML stream. All XML
 * attributes are ordered case sensitive.
 * @param element XML DOM element
 * @param writer XML stream writer
 * @return Returns true if entire DOM element was written successfully.
 * Otherwise, false is returned.
 */
bool GT_DATAMODEL_EXPORT writeDomElementOrderedAttribute(
        const QDomElement& element,
        QXmlStreamWriter& writer);

/**
 * @brief Writes given QDomDocument object to given file path. The XML
 * attributes can be saved both unsorted and sorted. Default is sorted.
 * @param filePath Target file path (e.g. /home/files/my_file.xml)
 * @param doc Given DOM document
 * @param attrOrdered if true, the attributes are written in case sensitive
 * order. Otherwise, the order of the attributes is random (faster writing)
 * @return Returns true if entire DOM document was written successfully.
 * Otherwise, false is returned.
 */
bool GT_DATAMODEL_EXPORT writeDomDocumentToFile(const QString& filePath,
                                                const QDomDocument& doc,
                                                bool attrOrdered = true);

}

#endif // GTXMLUTILITIES_H
