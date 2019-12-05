#ifndef GT_TABLEAXIS_H
#define GT_TABLEAXIS_H

#include "gt_datamodel_exports.h"

#include "gt_extrapolators.h"
#include "gt_interpfuncs.h"
#include "gt_interpolators.h"
#include "gt_object.h"
#include "gt_unit.h"

#include <QString>
#include <QVector>

using namespace GtNumerics;

class GT_DATAMODEL_EXPORT GtTableAxis : public GtObject
{
    Q_OBJECT

    Q_PROPERTY(QString description READ description WRITE setDescription)
    Q_PROPERTY(QVector<double> ticks READ ticks WRITE setTicks)
    Q_PROPERTY(GtTableAxis::ExtrapMethod loExtrapolation READ loExtMethod WRITE
               setLoExtMethod)
    Q_PROPERTY(GtTableAxis::ExtrapMethod hiExtrapolation READ hiExtMethod WRITE
               setHiExtMethod)
    Q_PROPERTY(GtTableAxis::InterpMethod interpolation READ interMethod WRITE
               setInterMethod)
    Q_PROPERTY(QString unit READ unit WRITE setUnit)
public:
    enum InterpMethod {I_LINEAR, I_LAGRANGE2, I_LAGRANGE3, I_CATMULL,
                       I_LOGARITHMIC};
    enum ExtrapMethod {E_NONE, E_CONST, E_LINEAR, E_LAGRANGE2};
    Q_ENUMS(InterpMethod)
    Q_ENUMS(ExtrapMethod)

    /** Constructor */
    Q_INVOKABLE GtTableAxis();

    /** Deconstructor */
    ~GtTableAxis();

    /**
     * @brief description Getter method for the the axis description.
     * @return Axis description
     */
    QString description() const;

    /**
     * @brief setDescription Setter method for the the axis description.
     * @param description New axis description.
     */
    void setDescription(const QString &description);

    /**
     * @brief ticks Returns
     * @return
     */
    QVector<double> ticks() const;
    void setTicks(const QVector<double> &ticks);

    ExtrapMethod loExtMethod() const;
    void setLoExtMethod(const ExtrapMethod loExtMethod);

    ExtrapMethod hiExtMethod() const;
    void setHiExtMethod(const ExtrapMethod hiExtMethod);

    InterpMethod interMethod() const;
    void setInterMethod(const InterpMethod interMethod);

    QString unit() const;
    void setUnit(const QString &unit);

    /**
     * @brief scale Scales the axis ticks by a constant factor.
     * @param factor Scaling factor.
     */
    void scale(double factor);

    /**
     * @brief size Returns the number of ticks on the axis.
     * @return Number of ticks
     */
    int size();

    GtExtrapolator* loExtrapolator() const;
    GtExtrapolator* hiExtrapolator() const;
    GtNumerics::GtInterpolator* interpolator() const;
private:
    /// Axis unit
    QString m_unit;

    /// Brief description of axis
    QString m_brief;

    /// Array of axis ticks
    QVector<double> m_ticks;

    /// Extrapolation method on lower boundary side
    ExtrapMethod m_loExtMethod;

    /// Pointer to low extrapolator object
    GtExtrapolator* m_loExtrapolator;

    /// Extrapolation method on higher boundary side
    ExtrapMethod m_hiExtMethod;

    /// Pointer to high extrapolator object
    GtExtrapolator* m_hiExtrapolator;

    /// Interpolation method
    InterpMethod m_InterMethod;

    /// Pointer to interpolator object
    GtNumerics::GtInterpolator* m_interpolator;

    /**
     * @brief genExtrap
     * @param em
     * @return
     */
    GtNumerics::GtExtrapolator* genExtrap(ExtrapMethod em);

    /**
     * @brief genInterp
     * @param em
     * @return
     */
    GtNumerics::GtInterpolator* genInterp(InterpMethod em);
};

#endif // GT_TABLEAXIS_H
