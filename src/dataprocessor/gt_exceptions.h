/* GTlab - Gas Turbine laboratory
 * Source File: gt_exceptions.h
 * copyright 2009-2015 by DLR
 *
 *  Created on: 28.10.2015
 *  Author: Richard Becker (AT-TWK)
 *  Tel.: +49 2203 601 3493
 */

#ifndef GT_EXCEPTIONS_H
#define GT_EXCEPTIONS_H

#include <QString>
#include "gt_datamodel_exports.h"

/**
 * @brief The GTNumericException class
 */
class GTlabException : public std::exception
{
public:
    /**
     * @brief GTNumericException - constructor
     * @param where Description about where the exception occured
     * @param what  Description of what whent wrong
     */
    GT_DATAMODEL_EXPORT GTlabException(const QString& where,
                                       const QString& what);

    /** @brief Returns the where string.
     *  @return Where string
     */
    GT_DATAMODEL_EXPORT QString where() const noexcept;

    /**
     * @brief Returns the error string
     */
    GT_DATAMODEL_EXPORT const char* what() const noexcept override;

    /**
     * @brief Returns an error string set during construction of the exception
     * object.
     * @return Error string
     */
    GT_DATAMODEL_EXPORT QString errMsg() const;

    /**
     * @brief Pushes additional information the where string.
     * @param function Function or method in which exception occured
     * @param name Name of object in which exception occured.
     */
    GT_DATAMODEL_EXPORT void pushWhere(
            QString const& function, QString const& name = "");

    /**
     * @brief Sets the where string.
     * @param where Where string
     */
    GT_DATAMODEL_EXPORT void setWhere(const QString& where) noexcept;

private:
    /// String designating the the routine or code section the exception was
    /// raised in
    std::string m_where;

    /// Description of the circumstances that lead to the exception
    std::string m_what;

};

#endif // GT_EXCEPTIONS_H

