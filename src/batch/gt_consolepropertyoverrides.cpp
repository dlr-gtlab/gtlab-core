/* GTlab - Gas Turbine laboratory
 *
 * SPDX-License-Identifier: MPL-2.0+
 * SPDX-FileCopyrightText: 2026 German Aerospace Center (DLR)
 */

#include "gt_consolepropertyoverrides.h"

#include "gt_object.h"
#include "gt_abstractproperty.h"
#include "gt_propertystructcontainer.h"
#include "gt_structproperty.h"

#include <algorithm>
#include <utility>

#include <QRegularExpression>

namespace
{

    /**
     * @brief Error message container returned by the internal resolvers
     */
    struct ResolveResult
    {
        QString error;
        GtAbstractProperty* property = nullptr;
        QString readOnlyContainerId;

        //! true if the referenced object/name does not exist at all
        bool notFound = false;

        explicit operator bool() const
        {
            return error.isEmpty();
        }
    };

    struct ObjectResult
    {
        GtObject* object = nullptr;
        ResolveResult result;

        explicit operator bool() const
        {
            return object != nullptr;
        }
    };

    bool isDigitOnly(const QString& s)
    {
        static const QRegularExpression re(QStringLiteral("^[0-9]+$"));
        return s.isEmpty() ? false : re.match(s).hasMatch();
    }

    /**
     * @brief Splits a string at every @p delimiter that is not enclosed by
     * brackets ("[" / "]" or "{" / "}")
     *
     * Reports an error for unbalanced brackets or empty tokens.
     */
    ResolveResult splitOutsideBrackets(const QString& str,
                                       const QChar& delimiter,
                                       QStringList* tokens)
    {
        ResolveResult result;

        if (str.isEmpty())
        {
            result.error = QObject::tr("Path is empty");
            result.notFound = true;
            return result;
        }

        int depth = 0;
        QString current;

        for (const QChar& c : str)
        {
            if (c == QLatin1Char('[') || c == QLatin1Char('{'))
            {
                depth++;
            }
            else if (c == QLatin1Char(']') || c == QLatin1Char('}'))
            {
                depth--;
            }

            if (depth < 0)
            {
                result.error =
                    QObject::tr("Unbalanced brackets in path '%1'").arg(str);
                return result;
            }

            if (c == delimiter && depth == 0)
            {
                tokens->append(current);
                current.clear();
            }
            else
            {
                current.append(c);
            }
        }

        if (depth != 0)
        {
            result.error =
                QObject::tr("Unbalanced brackets in path '%1'").arg(str);
            return result;
        }

        tokens->append(current);

        const bool hasEmptyToken =
            std::any_of(tokens->cbegin(), tokens->cend(),
                        [](const QString& token) { return token.isEmpty(); });
        if (hasEmptyToken)
        {
            result.error =
                QObject::tr("Empty path element in path '%1'").arg(str);
            return result;
        }

        return result;
    }

    /**
     * @brief Checks if a token ends with a "[...]" suffix and splits it into
     * the base part and the (still bracket enclosed) suffix
     */
    bool splitSuffix(const QString& token, QString* base, QString* suffix)
    {
        if (!token.endsWith(QLatin1Char(']')))
        {
            return false;
        }

        const int open = token.indexOf(QLatin1Char('['));

        if (open <= 0)
        {
            // no '[' at all, or the token starts with '[' -> not a valid
            // "base[suffix]" form here
            return false;
        }

        // only the last bracket pair is the suffix, the base must not contain
        // any brackets
        const QString candidateBase = token.left(open);
        const QString candidateSuffix = token.mid(open);

        if (candidateBase.contains(QLatin1Char('[')) ||
            candidateBase.contains(QLatin1Char(']')) ||
            candidateSuffix.indexOf(QLatin1Char(']')) !=
                candidateSuffix.size() - 1)
        {
            return false;
        }

        *base = candidateBase;
        *suffix = candidateSuffix;
        return true;
    }

    ObjectResult resolveUuidSegment(GtObject& parent, const QString& segment)
    {
        ObjectResult retval;
        const QString uuid = segment.mid(1, segment.size() - 2);

        if (uuid.isEmpty())
        {
            retval.result.error =
                QObject::tr("Empty UUID segment '{}' in path");
            return retval;
        }

        GtObject* child = parent.getDirectChildByUuid(segment);
        if (!child)
        {
            child = parent.getDirectChildByUuid(uuid);
        }

        if (!child)
        {
            retval.result.error =
                QObject::tr("No child object with UUID '%1' found").arg(uuid);
            retval.result.notFound = true;
            return retval;
        }

        retval.object = child;
        return retval;
    }

    ObjectResult resolveNamedObjectSegment(GtObject& parent,
                                           const QString& name, bool hasIndex,
                                           int index, const QString& indexStr)
    {
        ObjectResult retval;
        const auto children = parent.findDirectChildren<GtObject*>(name);

        if (children.isEmpty())
        {
            retval.result.error =
                QObject::tr("No child object named '%1' found in object '%2'")
                    .arg(name, parent.objectName());
            retval.result.notFound = true;
            return retval;
        }

        if (hasIndex)
        {
            if (index > children.size())
            {
                retval.result.error =
                    QObject::tr(
                        "Object index [%1] out of range: object '%2' has "
                        "%3 child/children named '%4' (valid: 1 to %5)")
                        .arg(indexStr)
                        .arg(parent.objectName())
                        .arg(children.size())
                        .arg(name)
                        .arg(children.size());
                return retval;
            }

            retval.object = children.at(index - 1);
            return retval;
        }

        if (children.size() > 1)
        {
            retval.result.error =
                QObject::tr("Object name '%1' is ambiguous in object '%2' "
                            "(%3 matches), use '%4[n]' with an explicit index")
                    .arg(name, parent.objectName())
                    .arg(children.size())
                    .arg(name);
            return retval;
        }

        retval.object = children.first();
        return retval;
    }

    /**
     * @brief Resolves a single object path segment relative to @parent
     */
    ObjectResult resolveObjectSegment(GtObject& parent, const QString& segment)
    {
        ObjectResult retval;

        if (segment.startsWith(QLatin1Char('{')) &&
            segment.endsWith(QLatin1Char('}')))
        {
            return resolveUuidSegment(parent, segment);
        }

        QString name = segment;
        QString suffix;
        const bool hasIndex = splitSuffix(segment, &name, &suffix);
        QString indexStr;
        int index = 0;

        if (hasIndex)
        {
            indexStr = suffix.mid(1, suffix.size() - 2);

            bool ok = false;
            index = indexStr.toInt(&ok);

            if (!ok || index < 1)
            {
                // Leave room for a root property-container interpretation.
                retval.result.error =
                    QObject::tr("Invalid object index '%1' in path")
                        .arg(segment);
                return retval;
            }
        }
        else if (segment.contains(QLatin1Char('[')) ||
                 segment.contains(QLatin1Char(']')))
        {
            retval.result.error =
                QObject::tr("Invalid object segment '%1' in path").arg(segment);
            return retval;
        }

        return resolveNamedObjectSegment(parent, name, hasIndex, index,
                                         indexStr);
    }

    /**
     * @brief Resolves an object path ("/" separated segments) relative to @root
     *
     * An empty path resolves to @root itself.
     */
    ObjectResult resolveObjectPath(GtObject& root, const QString& objectPath)
    {
        ObjectResult retval;
        retval.object = &root;

        if (objectPath.isEmpty())
        {
            return retval;
        }

        QStringList segments;
        const ResolveResult splitResult =
            splitOutsideBrackets(objectPath, QLatin1Char('/'), &segments);

        if (!splitResult)
        {
            retval.object = nullptr;
            retval.result = splitResult;
            return retval;
        }

        GtObject* current = &root;

        for (const QString& segment : std::as_const(segments))
        {
            ObjectResult child = resolveObjectSegment(*current, segment);

            if (!child)
            {
                retval.object = nullptr;
                retval.result = child.result;
                return retval;
            }

            current = child.object;
        }

        retval.object = current;
        return retval;
    }

    /**
     * @brief Checks the write access and applies the value to @prop
     */
    QString setPropertyValue(GtAbstractProperty* prop, const QString& path,
                             const QString& value)
    {
        assert(prop);

        if (prop->isMonitoring())
        {
            return QObject::tr("Cannot set '%1': property '%2' is a monitoring "
                               "property")
                .arg(path, prop->ident());
        }

        if (prop->isReadOnly())
        {
            return QObject::tr("Cannot set '%1': property '%2' is read only")
                .arg(path, prop->ident());
        }

        if (!prop->setValueFromVariant(QVariant(value)))
        {
            return QObject::tr("Cannot set '%1': value '%2' could not be "
                               "converted to the property type")
                .arg(path, value);
        }

        return {};
    }

    /**
     * @brief Applies a plain "propertyId" path on @object
     */
    ResolveResult resolvePlainProperty(GtObject& object, const QString& propId,
                                       const QString& path)
    {
        ResolveResult result;

        if (propId.contains(QLatin1Char('[')) ||
            propId.contains(QLatin1Char(']')))
        {
            result.error =
                QObject::tr("Invalid property element '%1' in path '%2': "
                            "container entries need a trailing '.<propertyId>'")
                    .arg(propId, path);
            result.notFound = true;
            return result;
        }

        GtAbstractProperty* prop = object.findProperty(propId);

        if (!prop)
        {
            result.error =
                QObject::tr("No property named '%1' found in object '%2'")
                    .arg(propId, object.objectName());
            result.notFound = true;
            return result;
        }

        result.property = prop;
        return result;
    }

    struct ContainerSelector
    {
        QString value;
        bool byId = false;
        QString error;

        explicit operator bool() const
        {
            return error.isEmpty();
        }
    };

    ContainerSelector parseContainerSelector(const QString& selector,
                                             const QString& path)
    {
        ContainerSelector result;
        const QString content = selector.mid(1, selector.size() - 2);
        result.byId = content.startsWith(QLatin1Char('{')) &&
                      content.endsWith(QLatin1Char('}'));
        result.value =
            result.byId ? content.mid(1, content.size() - 2) : content;

        if (result.byId && result.value.isEmpty())
        {
            result.error =
                QObject::tr("Empty entry id in selector '%1' of path '%2'")
                    .arg(selector, path);
        }
        else if (!result.byId && !isDigitOnly(result.value))
        {
            result.error =
                QObject::tr("Invalid entry selector '%1' in path '%2': "
                            "expected '[index]' or '[{entryId}]'")
                    .arg(selector, path);
        }

        return result;
    }

    struct EntryResult
    {
        GtPropertyStructInstance* entry = nullptr;
        QString error;
        bool notFound = false;

        explicit operator bool() const
        {
            return entry != nullptr;
        }
    };

    EntryResult resolveContainerEntry(GtPropertyStructContainer& container,
                                      const ContainerSelector& selector,
                                      const QString& containerId,
                                      GtObject& object, const QString& path)
    {
        EntryResult result;

        if (selector.byId)
        {
            if (container.type() == GtPropertyStructContainer::Sequential)
            {
                result.error =
                    QObject::tr("Property container '%1' is sequential, "
                                "select an entry with '[<index>]', not "
                                "'[{%2}]'")
                        .arg(containerId, selector.value);
                return result;
            }

            auto entryIt = container.findEntry(selector.value);
            if (entryIt == container.end())
            {
                result.error =
                    QObject::tr("No entry with id '%1' found in property "
                                "container '%2' of object '%3'")
                        .arg(selector.value, containerId, object.objectName());
                result.notFound = true;
                return result;
            }

            result.entry = &(*entryIt);
            return result;
        }

        if (container.type() == GtPropertyStructContainer::Associative)
        {
            result.error =
                QObject::tr("Property container '%1' is associative, "
                            "select an entry with '[{%2}]'")
                    .arg(containerId, selector.value);
            return result;
        }

        bool ok = false;
        const int index = selector.value.toInt(&ok);
        if (!ok || index < 1)
        {
            result.error = QObject::tr("Invalid entry index '%1' in path '%2': "
                                       "indices start at 1")
                               .arg(selector.value, path);
            return result;
        }

        if (static_cast<size_t>(index) > container.size())
        {
            const QString validRange =
                container.size() == 0
                    ? QObject::tr("no valid indices; the container is empty")
                    : QObject::tr("valid indices: 1 to %1")
                          .arg(container.size());
            result.error =
                QObject::tr("Entry index [%1] out of range: property "
                            "container '%2' of object '%3' has %4 entries "
                            "(%5)")
                    .arg(selector.value)
                    .arg(containerId, object.objectName())
                    .arg(container.size())
                    .arg(validRange);
            return result;
        }

        result.entry = &container.at(static_cast<size_t>(index - 1));
        return result;
    }

    /**
     * @brief Resolves a "container[selector].propertyId" path on @object
     */
    ResolveResult resolveContainerProperty(GtObject& object,
                                           const QString& containerToken,
                                           const QString& memberId,
                                           const QString& path)
    {
        ResolveResult result;

        QString containerId;
        QString selector;
        if (!splitSuffix(containerToken, &containerId, &selector))
        {
            result.error =
                QObject::tr("Invalid container element '%1' in path '%2': "
                            "expected '<containerId>[<selector>]'.<propertyId>")
                    .arg(containerToken, path);
            result.notFound = true;
            return result;
        }

        GtPropertyStructContainer* container =
            object.findPropertyContainer(containerId);
        if (!container)
        {
            result.error =
                QObject::tr("No property container named '%1' found in "
                            "object '%2'")
                    .arg(containerId, object.objectName());
            result.notFound = true;
            return result;
        }

        if (container->getFlags() & GtPropertyStructContainer::ReadOnly)
        {
            result.readOnlyContainerId = containerId;
        }

        const ContainerSelector parsedSelector =
            parseContainerSelector(selector, path);
        if (!parsedSelector)
        {
            result.error = parsedSelector.error;
            return result;
        }

        const EntryResult entryResult = resolveContainerEntry(
            *container, parsedSelector, containerId, object, path);
        if (!entryResult)
        {
            result.error = entryResult.error;
            result.notFound = entryResult.notFound;
            return result;
        }

        GtAbstractProperty* prop = entryResult.entry->findProperty(memberId);
        if (!prop)
        {
            result.error =
                QObject::tr("No property named '%1' found in entry of "
                            "property container '%2' of object '%3'")
                    .arg(memberId, containerId, object.objectName());
            result.notFound = true;
            return result;
        }

        result.property = prop;
        return result;
    }

    /**
     * @brief Resolves the property path (token list after the object path)
     *
     * Accepted shapes:
     *   [propId]                         plain property
     *   [container[sel], memberId]       property container entry member
     */
    ResolveResult resolvePropertyPath(GtObject& object,
                                      const QStringList& propTokens,
                                      const QString& path)
    {
        if (propTokens.size() == 1)
        {
            return resolvePlainProperty(object, propTokens.first(), path);
        }

        if (propTokens.size() == 2)
        {
            return resolveContainerProperty(object, propTokens.at(0),
                                            propTokens.at(1), path);
        }

        ResolveResult result;
        result.error = QObject::tr("Invalid property path '%1'").arg(path);
        result.notFound = true;
        return result;
    }

    ResolveResult resolveCandidate(GtObject& root, const QString& objectPath,
                                   const QStringList& propTokens,
                                   const QString& path)
    {
        ObjectResult object = resolveObjectPath(root, objectPath);
        if (!object)
        {
            return object.result;
        }

        return resolvePropertyPath(*object.object, propTokens, path);
    }

    QString applyResolvedProperty(const ResolveResult& result,
                                  const QString& path, const QString& value)
    {
        if (!result)
        {
            return result.error;
        }

        if (!result.readOnlyContainerId.isEmpty())
        {
            return QObject::tr(
                       "Cannot set '%1': property container '%2' is read only")
                .arg(path, result.readOnlyContainerId);
        }

        return setPropertyValue(result.property, path, value);
    }

    QString applyTwoSegmentPath(GtObject& root, const QStringList& tokens,
                                const QString& path, const QString& value)
    {
        // This form may mean either a child object's property or a root
        // property-container entry. Resolve both before modifying either.
        const ResolveResult objectProperty =
            resolveCandidate(root, tokens.at(0), {tokens.at(1)}, path);
        const ResolveResult rootContainerProperty =
            resolvePropertyPath(root, tokens, path);

        if (objectProperty && rootContainerProperty)
        {
            return QObject::tr(
                       "Property path '%1' is ambiguous between a child "
                       "object and a root property container; prefix it with "
                       "'.' to select the property container")
                .arg(path);
        }

        if (objectProperty)
        {
            return applyResolvedProperty(objectProperty, path, value);
        }

        if (rootContainerProperty)
        {
            return applyResolvedProperty(rootContainerProperty, path, value);
        }

        return objectProperty.notFound ? rootContainerProperty.error
                                       : objectProperty.error;
    }

} // namespace

QList<gt::console::PropertyOverride>
gt::console::parsePropertyOverrides(const QStringList& rawArgs,
                                    QStringList* errors)
{
    QList<PropertyOverride> retval;
    bool hadInvalidArgument = false;

    for (const QString& arg : rawArgs)
    {
        const int sep = arg.indexOf(QLatin1Char('='));

        if (sep <= 0)
        {
            hadInvalidArgument = true;
            if (errors)
            {
                errors->append(QObject::tr("Invalid --set argument '%1': "
                                           "expected \"<path>=<value>\"")
                                   .arg(arg));
            }
            continue;
        }

        retval.append(PropertyOverride{arg.left(sep), arg.mid(sep + 1)});
    }

    return hadInvalidArgument ? QList<PropertyOverride>{} : retval;
}

QString
gt::console::applyPropertyOverride(GtObject& root, const QString& path,
                                   const QString& value)
{
    const bool propertyOnly = path.startsWith(QLatin1Char('.'));
    const QString resolverPath = propertyOnly ? path.mid(1) : path;

    QStringList dotTokens;

    const ResolveResult splitResult =
        splitOutsideBrackets(resolverPath, QLatin1Char('.'), &dotTokens);

    if (!splitResult)
    {
        return splitResult.error;
    }

    const int n = dotTokens.size();

    if (propertyOnly)
    {
        return applyResolvedProperty(resolvePropertyPath(root, dotTokens, path),
                                     path, value);
    }

    if (n == 1)
    {
        // property path only, directly on the root object
        return applyResolvedProperty(resolvePropertyPath(root, dotTokens, path),
                                     path, value);
    }

    if (n == 2)
    {
        return applyTwoSegmentPath(root, dotTokens, path, value);
    }

    if (n == 3)
    {
        // "<objectPath>.<containerId>[<selector>].<propertyId>"
        return applyResolvedProperty(
            resolveCandidate(root, dotTokens.at(0),
                             {dotTokens.at(1), dotTokens.at(2)}, path),
            path, value);
    }

    return QObject::tr("Invalid property path '%1'").arg(path);
}
