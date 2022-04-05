#include "gt_tableaxis.h"
#include "gt_logging.h"

GtTableAxis::~GtTableAxis() = default;

QString
GtTableAxis::description() const
{
    return m_description;
}


void
GtTableAxis::setDescription(const QString& description)
{
    m_description = description;
}


QVector<double>
GtTableAxis::ticks() const
{
    return m_ticks;
}


void
GtTableAxis::setTicks(const QVector<double> &ticks)
{
    m_ticks = ticks;
}


GtTableAxis::ExtrapMethod GtTableAxis::loExtMethod() const
{
    return m_loExtMethod;
}


void
GtTableAxis::setLoExtMethod(ExtrapMethod loExtMethod)
{
    m_loExtMethod = loExtMethod;
    m_loExtrapolator.reset(genExtrap(m_loExtMethod));
}


GtTableAxis::ExtrapMethod
GtTableAxis::hiExtMethod() const
{
    return m_hiExtMethod;
}


void
GtTableAxis::setHiExtMethod(const ExtrapMethod hiExtMethod)
{
    m_hiExtMethod = hiExtMethod;
    m_hiExtrapolator.reset(genExtrap(m_hiExtMethod));
}


GtTableAxis::InterpMethod GtTableAxis::interMethod() const
{
    return m_InterMethod;
}


void
GtTableAxis::setInterMethod(const InterpMethod interMethod)
{
    m_InterMethod = interMethod;
    m_interpolator.reset(genInterp(m_InterMethod));
}


QString
GtTableAxis::unit() const
{
    return m_unit;
}


void
GtTableAxis::setUnit(const QString &unit)
{
    m_unit = unit;
}


int
GtTableAxis::size() const
{
    return m_ticks.size();
}


GtExtrapolator*
GtTableAxis::loExtrapolator() const
{
    return m_loExtrapolator.get();
}


GtExtrapolator*
GtTableAxis::hiExtrapolator() const
{
    return m_hiExtrapolator.get();
}


GtNumerics::GtInterpolator*
GtTableAxis::interpolator() const
{
    return m_interpolator.get();
}


void
GtTableAxis::scale(const double &factor)
{
    for (int i = 0; i < m_ticks.size(); ++i)
    {
        m_ticks[i] *= factor;
    }
}


GtNumerics::GtExtrapolator*
GtTableAxis::genExtrap(ExtrapMethod em)
{
    GtNumerics::GtExtrapolator* extrap = nullptr;

    switch (em)
    {
        case ExtrapMethod::E_CONST:
            extrap = new GtNumerics::GtConstExtrapolator();
            break;
        case ExtrapMethod::E_LINEAR:
            extrap = new GtNumerics::GtLinearExtrapolator();
            break;
        case ExtrapMethod::E_LAGRANGE2:
            extrap = new GtNumerics::GtLagrange2Extrapolator();
            break;
        case ExtrapMethod::E_NONE:
            gtError() << "GtTable::genExtrap(): "
                      << "Extrapolation is forbidden and not implemented.";
            break;
        default:
            gtError() << "GtTable::genExtrap(): "
                      << "Unknown extrapolation method.";
    }

    return extrap;
}


GtNumerics::GtInterpolator*
GtTableAxis::genInterp(InterpMethod im)
{
    GtNumerics::GtInterpolator* interp = nullptr;

    switch (im)
    {
        case InterpMethod::I_LINEAR:
            interp = new GtNumerics::GtLinearInterpolator();
            break;
        case InterpMethod::I_LAGRANGE2:
            interp = new GtNumerics::GtLagrange2Interpolator();
            break;
        case InterpMethod::I_LAGRANGE3:
            interp = new GtNumerics::GtLagrange3Interpolator();
            break;
        case InterpMethod::I_CATMULL:
            interp = new GtNumerics::GtCatmullInterpolator();
            break;
        case I_LOGARITHMIC:
            interp = new GtNumerics::GtLogarithmicInterpolator();
            break;
        default:
            gtError() << "GtTable::genInterp(): "
                      << "Unknown interpolation method";
    }

    return interp;
}





