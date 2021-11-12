#include <QVector>
#include <QDebug>

#include "gt_objectmemento.h"
#include "gt_objectfactory.h"
#include "gt_exceptions.h"
#include "gt_logging.h"
#include "gt_table.h"
#include "gt_tableaxis.h"
#include "gt_tablevalues.h"

#include <QList>

GtTable::GtTable()
{
    setFactory(gtObjectFactory);
    setDefault(true);
    setUserHidden(true);

    m_type = DEFAULT;
}

bool
GtTable::addAxis(const QString& id, const QString desc, const QString& unit,
                 GtTableAxis::ExtrapMethod loExtMethod,
                 GtTableAxis::InterpMethod intMethod,
                 GtTableAxis::ExtrapMethod hiExtMethod,
                 const QVector<double>& ticks)
{
    // perform some preliminary checks

    if (!isAscending(ticks) && m_type == DEFAULT)
    {
        gtError() << "GtTable::addAxis(): "
                  << "Ticks of axis '" + id + "' are not in ascending order.";

        return false;
    }

    if (!checkAxisName(id))
    {
        gtError() << "Table::addAxis(): "
                  << "Duplicate axis name " << id;

        return false;
    }

    GtTableAxis* tableAxis = new GtTableAxis;
    tableAxis->setObjectName(id);
    tableAxis->setDescription(desc);
    tableAxis->setUnit(unit);
    tableAxis->setLoExtMethod(loExtMethod);
    tableAxis->setHiExtMethod(hiExtMethod);
    tableAxis->setInterMethod(intMethod);

    int tickLen = ticks.size();

    if (tickLen < tableAxis->interpolator()->getMinPoints())
    {
        gtError() << "Table::addAxis(): "
                  << "Number of Ticks on axis '" + id + "' of table '"
                  + objectName() + "' too low for given interpolation"
                  " method.";

        delete tableAxis;

        return false;
    }

    tableAxis->setTicks(ticks);

    appendChild(tableAxis);

    return true;
}

bool
GtTable::addAxis(GtTableAxis* ax)
{
    appendChild(ax);

    return true;
}

bool
GtTable::checkAxisName(const QString& axisName)
{
    return !getAxesNames().contains(axisName);
}

bool
GtTable::addValues(const QString& id, const QString& desc,
                   const QString& unit, const QVector<double>& vals)
{
    int expSize = 1;
    QList<GtTableAxis*> axList = getAxesList();

    for (int i = 0; i < axList.size(); ++i)
    {
        expSize *= axList.at(i)->size();
    }

    if (vals.size() != expSize && m_type == DEFAULT)
    {
        gtError() << "Table::addValues()"
                  << "Size of given value vector for table '"
                  << objectName()
                  << "' does not match table dimensions ("
                  << vals.size() << " != " << expSize << ").";

        return false;
    }

    GtTableValues* tabVals = findDirectChild<GtTableValues*>(id);
    if (tabVals == Q_NULLPTR)
    {
        tabVals = new GtTableValues;
        appendChild(tabVals);
    }

    tabVals->setObjectName(id);
    tabVals->setDescription(desc);
    tabVals->setUnit(unit);
    tabVals->setValues(vals);
    m_tabVals.insert(id, tabVals);

    return true;
}

bool
GtTable::addValues(GtTableValues* tv)
{
    if (tv == Q_NULLPTR)
    {
        return false;
    }

    int expSize = 1;
    QList<GtTableAxis*> axList = getAxesList();

    for (int i = 0; i < axList.size(); ++i)
    {
        expSize *= axList.at(i)->size();
    }

    if (tv->values().size() != expSize && m_type == DEFAULT)
    {
        gtError() << "Table::addValues()"
                  << "Size of given value vector for table '"
                  << objectName()
                  << "' does not match table dimensions ("
                  << tv->values().size() << " != " << expSize << ").";

        return false;
    }

    appendChild(tv);

    m_tabVals[tv->objectName()] = tv;

    return true;
}


double
GtTable::approxDimension(const QString& valsId, const int axisIndex,
                         QVector<int>& indices,
                         const QVector<double>& coords) const
{
    double x = coords[axisIndex];
    int n = getAxisSize(axisIndex);
    QList<GtTableAxis*> axList = getAxesList();

    GtTableAxis* currentAxis = axList.at(axisIndex);

    if (currentAxis == Q_NULLPTR)
    {
        throw GTlabException("Table::approxDimension()",
                             "currentAxis is Q_NULLPTR.");
    }

    GtNumerics::GtInterpolator* curInterp = currentAxis->interpolator();

    if (curInterp == Q_NULLPTR)
    {
        /// Reinitialize the method helped to solve an error
        currentAxis->setInterMethod(currentAxis->interMethod());

        curInterp = currentAxis->interpolator();
    }

    if (curInterp->getMinPoints() > getAxisTicks(axisIndex).size())
    {
        gtError() << "invalid number of points for the selected interpolator";
        gtError() << "The table has " << getAxisTicks(axisIndex).size()
                  << "points but the interpolation method requires "
                  << curInterp->getMinPoints();
    }

    if (curInterp == Q_NULLPTR)
    {
        throw GTlabException("Table::approxDimension()",
                             "curInterp is Q_NULLPTR.");
    }

    if (axisIndex > 0)
    {
        // for a non-zero axis index we need to perform an dimension recursion

        GtNumerics::darray yy(n, 0.0);

        const GtNumerics::darray& xx = getAxisTicks(axisIndex);

        if (currentAxis == Q_NULLPTR)
        {
            throw GTlabException("Table::approxDimension()",
                                 "Axis is Q_NULLPTR.");
        }

        int sliceSize = curInterp->getSliceSize();

        int j = GtNumerics::locate(x, xx);
        int start = GtNumerics::subArrayIndex(j, n, sliceSize);
        int end = start + sliceSize;

        for (int i = start; i < end; ++i)
        {
            indices[axisIndex] = i;

            yy[i] = approxDimension(valsId, axisIndex - 1, indices, coords);
        }

        if (xx.size() < 2)
        {
            return yy[0];
        }

        if (x < xx[0])
        {
            if (currentAxis->loExtrapolator() == NULL)
            {
                throw GTlabException("Table::approxDimension()",
                                     "loExtrapolation forbidden.");
            }

            return currentAxis->loExtrapolator()->calc(x, 0, curInterp,
                    xx, yy);
        }
        else if (x > xx[n - 1])
        {
            if (currentAxis->hiExtrapolator() == NULL)
            {
                throw GTlabException("Table::approxDimension()",
                                     "hiExtrapolation forbidden.");
            }

            return currentAxis->hiExtrapolator()->calc(x, n - 1,
                    curInterp, xx,
                    yy);
        }
        else
        {
            return curInterp->calc(x, xx, yy, j, start);
        }
    }
    else
    {
        // with a zero axis index we are able to compute the function value
        // directly

        const GtNumerics::darray& yy = getValSlice(valsId, 0, indices);
        const GtNumerics::darray& xx = getAxisTicks(0);

        if (axList.at(0) == Q_NULLPTR)
        {
            throw GTlabException("Table::approxDimension()",
                                 "Axis at 0 is Q_NULLPTR.");
        }

        int sliceSize = curInterp->getSliceSize();

        int i = GtNumerics::locate(x, xx);
        int j = GtNumerics::subArrayIndex(i, n, sliceSize);

        if (xx.size() < 2)
        {
            return yy[0];
        }

        if (x < xx[0])
        {
            return axList.at(0)->loExtrapolator()->calc(x, 0, curInterp, xx,
                    yy);
        }
        else if (x > xx[n - 1])
        {
            return axList.at(0)->hiExtrapolator()->calc(x, n - 1, curInterp, xx,
                    yy);
        }
        else
        {
            return curInterp->calc(x, xx, yy, i, j);
        }
    }
}


int
GtTable::calcStart(const int axisIndex, const QVector<int>& indices) const
{
    int start = 0;

    for (int i = 0; i < indices.size(); ++ i)
    {
        if (i == axisIndex)
        {
            continue;
        }

        int mult = 1;

        for (int j = i; j > 0; --j)
        {
            mult *= getAxisSize(j - 1);
        }

        start += indices[i] * mult;
    }

    return start;
}


int
GtTable::calcStride(const int axisIndex) const
{
    int stride = 1;

    for (int i = 0; i < axisIndex; ++i)
    {
        stride *= getAxisSize(axisIndex);
    }

    return stride;
}


GtNumerics::darray
GtTable::getAxisTicks(int axisIndex) const
{
    QList<GtTableAxis*> axList = getAxesList();

    if (axisIndex > axList.size() - 1)
    {
        throw GTlabException("Table::getAxis()",
                             "Unkown axis index.");
    }

    if (axList.at(axisIndex) == Q_NULLPTR)
    {
        throw GTlabException("Table::getAxisTicks()",
                             "Axis at index is Q_NULLPTR.");
    }

    return axList.at(axisIndex)->ticks();
}


void
GtTable::getAxisTicks(const QString& name, QVector<double>& dvecOut)
{
    if (!getAxesNames().contains(name))
    {
        throw GTlabException("Table::getAxis()", "Could not find axis '" +
                             name + "' in table '"
                             + objectName() + "'.");
    }

    int index = axisIndex(name);
    int n = getAxisSize(index);
    GtNumerics::darray xx = getAxisTicks(index);

    dvecOut.clear();

    for (int i = 0; i < n; ++i)
    {
        dvecOut.push_back(xx[i]);
    }
}


QList<GtTableAxis*>
GtTable::getAxesList() const
{
    return findDirectChildren<GtTableAxis*>();
}


QStringList
GtTable::getAxesNames() const
{
    QStringList names;

    foreach (GtTableAxis* axis, getAxesList())
    {
        names.append(axis->objectName());
    }

    return names;
}


QString
GtTable::getDescription() const
{
    return m_description;
}


GtNumerics::darray
GtTable::getSlice(const QString& valsId, int axisIndex,
                  const QVector<int>& indices)
{
    if (axisIndex >= nDims())
    {
        throw GTlabException("Table::getSlice()",
                             "Axis index out of bounds.");
    }

    if (indices.size() != nDims())
    {
        throw GTlabException("Table::getSlice()",
                             "Size of indices vector must be equal to number "
                             "of dimensions.");
    }

    int start  = calcStart(axisIndex, indices);
    int size   = getAxisSize(axisIndex);
    int stride = calcStride(axisIndex);

    return slice(valsId, start, size, stride);
}


GtNumerics::darray
GtTable::getSlice(const QString& axisName, const QVector<int>& indices)
{
    QList<QString> keys = m_tabVals.keys();
    return getSlice(keys.at(0), axisIndex(axisName), indices);
}


int
GtTable::getTableSize() const
{
    return m_tabVals.size();
}


QString
GtTable::getUnit() const
{
    return m_unit;
}

GtTableValues*
GtTable::getVals(const QString id)
{
    if (!m_tabVals.contains(id))
    {
        throw GTlabException("Table::getVals()",
                             "Unkown Value id.");
    }

    return m_tabVals[id];
}


double
GtTable::getValue(const QString& valsId, const QVector<double>& coords)
{
    if (coords.size() != nDims() && m_type == DEFAULT)
    {
        throw GTlabException("Table::getValue()",
                             "Size of coordinate vector must be equal to "
                             "number of dimensions.");
    }

    QVector<int> indices(nDims());
    return approxDimension(valsId, nDims() - 1, indices, coords);
}


double
GtTable::getValue1D(const QString& valsId, double x0)
{
    QVector<double> coords(1);

    coords[0] = x0;

    return getValue(valsId, coords);
}

double
GtTable::getValue2D(const QString& valsId, double x0, double x1) const
{
    if (m_type == GRIDLESS)
    {
        return getValue2Dgridless(valsId, x0, x1);
    }

    if (nDims() != 2)
    {
        throw GTlabException("Table::getValue2D()",
                             "Size of coordinate vector must be equal to "
                             "number of dimensions.");
    }

    QList<GtTableAxis*> axList = getAxesList();

    if (axList.at(0) == Q_NULLPTR || axList.at(1) == Q_NULLPTR)
    {
        throw GTlabException("Table::getValue2D()",
                             "Axis is Q_NULLPTR.");
    }

    int i0 = axList.at(0)->ticks().indexOf(x0);
    int i1 = axList.at(1)->ticks().indexOf(x1);

    if (i0 != -1 && i1 != -1)
    {
        return getValue2DatIndices(valsId, i0, i1);
    }

    GtNumerics::GtInterpolator* Interp0 = axList.at(0)->interpolator();
    GtNumerics::GtInterpolator* Interp1 = axList.at(1)->interpolator();


    if (Interp0 == Q_NULLPTR || Interp1 == Q_NULLPTR)
    {
        axList.at(0)->setInterMethod(axList.at(0)->interMethod());
        axList.at(1)->setInterMethod(axList.at(1)->interMethod());

        Interp0 = axList.at(0)->interpolator();
        Interp1 = axList.at(1)->interpolator();
    }

    if (Interp0 == Q_NULLPTR || Interp1 == Q_NULLPTR)
    {
        throw GTlabException("Table::getValue2D()",
                             "Interpolator is Q_NULLPTR.");
    }

    const GtNumerics::darray& xx0 = getAxisTicks(0);
    const GtNumerics::darray& xx1 = getAxisTicks(1);

    int n0 = xx0.size();
    int n1 = xx1.size();

    int loc0 = GtNumerics::locate(x0, xx0);
    int loc1 = GtNumerics::locate(x1, xx1);

    int sliceSize0 = Interp0->getSliceSize();
    int sliceSize1 = Interp1->getSliceSize();

    int sub0 = GtNumerics::subArrayIndex(loc0, n0, sliceSize0);
    int sub1 = GtNumerics::subArrayIndex(loc1, n1, sliceSize1);

    int end1 = sub1 + sliceSize1;

    const int axb0  = axList[0]->size();

    GtNumerics::darray yy1(n1);

    for (int j = sub1; j < end1; ++j)
    {
        int start = j * axb0;
        GtNumerics::darray yy0 = slice(valsId, start, axb0, 1);

        if (x0 < xx0[0])
        {
            if (axList.at(0)->loExtrapolator() == NULL)
            {
                throw GTlabException("Table::approxDimension()",
                                     "loExtrapolation forbidden.");
            }

            yy1[j] = axList.at(0)->loExtrapolator()->calc(x0, 0, Interp0, xx0,
                     yy0);
        }
        else if (x0 > xx0[n0 - 1])
        {
            if (axList.at(0)->hiExtrapolator() == NULL)
            {
                throw GTlabException("Table::approxDimension()",
                                     "hiExtrapolation forbidden.");
            }

            yy1[j] = axList.at(0)->hiExtrapolator()->calc(x0, n0 - 1, Interp0,
                     xx0, yy0);
        }
        else
        {
            yy1[j] = Interp0->calc(x0, xx0, yy0, loc0, sub0);
        }
    }

    if (x1 < xx1[0])
    {
        if (axList.at(1)->loExtrapolator() == NULL)
        {
            throw GTlabException("Table::approxDimension()",
                                 "loExtrapolation forbidden.");
        }

        return axList.at(1)->loExtrapolator()->calc(x1, 0, Interp1, xx1, yy1);
    }
    else if (x1 > xx1[n1 - 1])
    {
        if (axList.at(1)->hiExtrapolator() == NULL)
        {
            throw GTlabException("Table::approxDimension()",
                                 "hiExtrapolation forbidden.");
        }

        return axList.at(1)->hiExtrapolator()->calc(x1, n1 - 1, Interp1, xx1,
                yy1);
    }
    else
    {
        return Interp1->calc(x1, xx1, yy1, loc1, sub1);
    }
}

double
GtTable::getValue2DatIndices(const QString &valsId, int axisIndex1,
                             int axisIndex2, bool* check) const
{
    GtTableValues* tableVals = findDirectChild<GtTableValues*>(valsId);
    if (tableVals == Q_NULLPTR)
    {
        if (check != Q_NULLPTR)
        {
            *check = false;
        }

        return 0.0;
    }

    int index = 0;
    if (axisIndex2 == 0)
    {
        index = axisIndex1;
    }
    else if (axisIndex1 == 0)
    {
        index = getAxisSize(0) * axisIndex2;
    }
    else
    {
        index = axisIndex2 * getAxisSize(0) + axisIndex1;
    }

    if (index > tableVals->values().size()-1)
    {
        if (check != Q_NULLPTR)
        {
            *check = false;
        }

        return 0.0;
    }

    return tableVals->values().at(index);
}

double
GtTable::getValue2Dgridless(const QString& valsId, double x0, double x1) const
{
    double phi = x0;
    double xn = x1;

    // axis ticks for first axis
    const GtNumerics::darray& xx0 = getAxisTicks(0);
    // axis ticks for second axis
    const GtNumerics::darray& xx1 = getAxisTicks(1);

    // get xn values (low and up) close to xn
    double xnlow = 0.0;
    double xnup = 0.0;
    GtNumerics::locateVicinityIndices(xn, xx0, xnlow, xnup);

    bool xninterp = true;

    if (xnlow == xnup)
    {
        // no interpolation over XN necessary
        xninterp = false;
    }

    // interpolate on phi axis of current xnlow data row
    double valXnlowMean = phiInterpolatedVal(valsId, xnlow, phi, xx0, xx1);

    if (!xninterp)
    {
        // no interpolation over xn necessary (xnlow = xnup)
        return valXnlowMean;
    }

    // interpolate on phi axis of current xnup data row
    double valXnupMean = phiInterpolatedVal(valsId, xnup, phi, xx0, xx1);

    double valXnMean = valXnlowMean + (valXnupMean - valXnlowMean) /
                       (xnup - xnlow) * (xn - xnlow);

    return valXnMean;
}

double
GtTable::phiInterpolatedVal(const QString& valsId,
                            double xn,
                            double phi,
                            const darray& xx0,
                            const darray& xx1) const
{
    // index in xx0 where xn was found for the first time
    int indxfirst = 0;

    // get all phi ticks for given xn from valarray xx0
    const GtNumerics::darray& xx1slice = getSuitedSlice(xn, xx0, xx1, indxfirst);

    double philow = 0.0;
    double phiup = 0.0;

    // find values in vicinity of phi at xn
    GtNumerics::locateVicinityIndices(phi, xx1slice, philow, phiup);

    // get indices for philow and phiup
    int loc0 = GtNumerics::locateC(philow, xx1slice) + indxfirst;
    int loc1 = GtNumerics::locateC(phiup, xx1slice) + indxfirst;

    // get table values at the found indices
    double vallow = tabVals(valsId).at(loc0);
    double valup = tabVals(valsId).at(loc1);

    double valXnlowMean;

    if (loc0 == loc1)
    {
        // indices where philow and phiup were found are the same!
        // thus philow = phiup

        // requested phi value does not lie in interpolation regime
        // extrapolate

        // left side xn value
        double xnLowerTest = xx0[loc0 - 1];

        if (xnLowerTest < xn)
        {
            // extrapolation to lower bound necessary

            philow = xx1[loc0];
            phiup = xx1[loc0 + 1];

            vallow = tabVals(valsId).at(loc0);
            valup = tabVals(valsId).at(loc0 + 1);

            valXnlowMean = vallow + (valup - vallow) /
                           (phiup - philow) * (phi - philow);
        }
        else
        {
            // extrapolation to upper bound necessary

            philow = xx1[loc0 - 1];
            phiup = xx1[loc0];

            vallow = tabVals(valsId).at(loc0 - 1);
            valup = tabVals(valsId).at(loc0);

            valXnlowMean = vallow + (valup - vallow) /
                           (phiup - philow) * (phi - philow);
        }
    }
    else
    {
        valXnlowMean = vallow + (valup - vallow) /
                       (phiup - philow) * (phi - philow);
    }

    return valXnlowMean;
}

GtNumerics::darray
GtTable::getSuitedSlice(double xnlow,
                        const GtNumerics::darray& xx0,
                        const GtNumerics::darray& vals,
                        int& indxfirst) const
{
    double xnArr[] = {xnlow};

    auto last = std::find_end(std::begin(xx0), std::end(xx0),
                              std::begin(xnArr), std::end(xnArr));
    auto first = std::find_first_of(std::begin(xx0), std::end(xx0),
                                    std::begin(xnArr), std::end(xnArr));

    int indxend = 0;

    if (last != std::end(xx0))
    {
        indxend = (last - std::begin(xx0));
    }
    else
    {
        gtError() << "Error, not found!" << '\n';
    }

    if (first != std::end(xx0))
    {
        indxfirst = (first - std::begin(xx0));
    }
    else
    {
        gtError() << "Error, not found!" << '\n';
    }

    int size = indxend - indxfirst + 1;
    int stride = 1;

    GtNumerics::darray out(size);

    for (int i = 0; i < size; ++i)
    {
        out[i] = vals.at(indxfirst + i * stride);
    }

    return out;
}

QString
GtTable::interExtraPolationString(GtTableAxis::InterpMethod methode)
{
    switch (methode)
    {
        case GtTableAxis::InterpMethod::I_LINEAR:
            return "linear";

        case GtTableAxis::InterpMethod::I_LAGRANGE2:
            return "lagrange2";

        case GtTableAxis::InterpMethod::I_LAGRANGE3:
            return "lagrange3";

        case GtTableAxis::InterpMethod::I_CATMULL:
            return "catmull";

        case GtTableAxis::InterpMethod::I_LOGARITHMIC:
            return "logarithmic";

        default:
            gtError() << "GtTable::getInterExtraPolationString(): "
                      << "Unknown interpolation method" << methode <<
                      " -> set to default: \"Linear\"";
            return "linear";
    }

    return "linear";
}

QString
GtTable::interExtraPolationString(GtTableAxis::ExtrapMethod methode)
{
    switch (methode)
    {
        case GtTableAxis::ExtrapMethod::E_NONE:
            return "none";

        case GtTableAxis::ExtrapMethod::E_CONST:
            return "const";

        case GtTableAxis::ExtrapMethod::E_LINEAR:
            return "linear";

        case GtTableAxis::ExtrapMethod::E_LAGRANGE2:
            return "lagrange2";

        default:
            gtError() << "GtTable::getInterExtraPolationString(): "
                      << "Unknown extrapolation method" << methode <<
                      " -> set to default: \"Linear\"";
            return "linear";
    }

    return "linear";
}

double
GtTable::getValue3D(const QString& valsId, double x0, double x1,
                    double x2) const
{
    if (nDims() != 3)
    {
        throw GTlabException("Table::getValue3D()",
                             "Size of coordinate vector must be equal to "
                             "number of dimensions.");
    }

    QList<GtTableAxis*> axList = getAxesList();

    for (int i = 0; i < 3; i++)
    {
        if (axList.at(i) == Q_NULLPTR)
        {
            QString where = QString("Axis ") + i + QString("is Q_NULLPTR.");

            throw GTlabException("Table::getValue3D()",
                                 where);
        }
    }

    GtNumerics::GtInterpolator* Interp0 = axList.at(0)->interpolator();;
    GtNumerics::GtInterpolator* Interp1 = axList.at(1)->interpolator();;
    GtNumerics::GtInterpolator* Interp2 = axList.at(2)->interpolator();;

    if (Interp0 == Q_NULLPTR || Interp1 == Q_NULLPTR || Interp2 == Q_NULLPTR)
    {
        throw GTlabException("Table::getValue3D()",
                             "Interpolator is Q_NULLPTR.");
    }

    const GtNumerics::darray& xx0 = getAxisTicks(0);
    const GtNumerics::darray& xx1 = getAxisTicks(1);
    const GtNumerics::darray& xx2 = getAxisTicks(2);

    int n0 = xx0.size();
    int n1 = xx1.size();
    int n2 = xx2.size();

    int loc0 = GtNumerics::locate(x0, xx0);
    int loc1 = GtNumerics::locate(x1, xx1);
    int loc2 = GtNumerics::locate(x2, xx2);

    int sliceSize0 = Interp0->getSliceSize();
    int sliceSize1 = Interp1->getSliceSize();
    int sliceSize2 = Interp2->getSliceSize();

    int sub0 = GtNumerics::subArrayIndex(loc0, n0, sliceSize0);
    int sub1 = GtNumerics::subArrayIndex(loc1, n1, sliceSize1);
    int sub2 = GtNumerics::subArrayIndex(loc2, n2, sliceSize2);

    int end1 = sub1 + sliceSize1;
    int end2 = sub2 + sliceSize2;

    const int axb0  = axList[0]->size();
    const int axb01 = axList[0]->size() * axList[1]->size();

    GtNumerics::darray yy1(n1);
    GtNumerics::darray yy2(n2);

    for (int k = sub2; k < end2; ++k)
    {
        for (int j = sub1; j < end1; ++j)
        {
            int start = j * axb0 + k * axb01;
            GtNumerics::darray yy0 = slice(valsId, start, axb0, 1);

            if (x0 < xx0[0])
            {
                yy1[j] = axList.at(0)->loExtrapolator()->calc(x0, 0, Interp0,
                         xx0, yy0);
            }
            else if (x0 > xx0[n0 - 1])
            {
                yy1[j] = axList.at(0)->hiExtrapolator()->calc(x0, n0 - 1,
                         Interp0, xx0,
                         yy0);
            }
            else
            {
                yy1[j] = Interp0->calc(x0, xx0, yy0, loc0, sub0);
            }
        }

        if (x1 < xx1[0])
        {
            yy2[k] = axList.at(1)->loExtrapolator()->calc(x1, 0, Interp1, xx1,
                     yy1);
        }
        else if (x1 > xx1[n1 - 1])
        {
            yy2[k] = axList.at(1)->hiExtrapolator()->calc(x1, n1 - 1, Interp1,
                     xx1, yy1);
        }
        else
        {
            yy2[k] = Interp1->calc(x1, xx1, yy1, loc1, sub1);
        }
    }

    if (x2 < xx2[0])
    {
        return axList.at(2)->loExtrapolator()->calc(x2, 0, Interp2, xx2, yy2);
    }
    else if (x2 > xx2[n2 - 1])
    {
        return axList.at(2)->hiExtrapolator()->calc(x2, n2 - 1, Interp2, xx2,
                yy2);
    }
    else
    {
        return Interp2->calc(x2, xx2, yy2, loc2, sub2);
    }

}


double
GtTable::getValue4D(const QString& valsId, double x0, double x1, double x2,
                    double x3) const
{
    if (nDims() != 4)
    {
        throw GTlabException("Table::getValue4D()",
                             "Size of coordinate vector must be equal to "
                             "number of dimensions.");
    }

    QList<GtTableAxis*> axList = getAxesList();

    for (int i = 0; i < 4; i++)
    {
        if (axList.at(i) == Q_NULLPTR)
        {
            QString where = QString("Axis ") + i + QString("is Q_NULLPTR.");
            throw GTlabException("Table::getValue4D()",
                                 where);
        }
    }

    GtNumerics::GtInterpolator* Interp0 = axList.at(0)->interpolator();
    GtNumerics::GtInterpolator* Interp1 = axList.at(1)->interpolator();
    GtNumerics::GtInterpolator* Interp2 = axList.at(2)->interpolator();
    GtNumerics::GtInterpolator* Interp3 = axList.at(3)->interpolator();

    if (Interp0 == Q_NULLPTR || Interp1 == Q_NULLPTR
            || Interp2 == Q_NULLPTR || Interp3 == Q_NULLPTR)
    {
        throw GTlabException("Table::getValue4D()",
                             "Interpolator is Q_NULLPTR.");
    }

    QVector<int> indices(nDims());

    const GtNumerics::darray& xx0 = getAxisTicks(0);
    const GtNumerics::darray& xx1 = getAxisTicks(1);
    const GtNumerics::darray& xx2 = getAxisTicks(2);
    const GtNumerics::darray& xx3 = getAxisTicks(3);

    int n0 = xx0.size();
    int n1 = xx1.size();
    int n2 = xx2.size();
    int n3 = xx3.size();

    int loc0 = GtNumerics::locate(x0, xx0);
    int loc1 = GtNumerics::locate(x1, xx1);
    int loc2 = GtNumerics::locate(x2, xx2);
    int loc3 = GtNumerics::locate(x3, xx3);

    int sliceSize0 = Interp0->getSliceSize();
    int sliceSize1 = Interp1->getSliceSize();
    int sliceSize2 = Interp2->getSliceSize();
    int sliceSize3 = Interp3->getSliceSize();

    int sub0 = GtNumerics::subArrayIndex(loc0, n0, sliceSize0);
    int sub1 = GtNumerics::subArrayIndex(loc1, n1, sliceSize1);
    int sub2 = GtNumerics::subArrayIndex(loc2, n2, sliceSize2);
    int sub3 = GtNumerics::subArrayIndex(loc3, n3, sliceSize3);

    int end1 = sub1 + sliceSize1;
    int end2 = sub2 + sliceSize2;
    int end3 = sub3 + sliceSize3;

    GtNumerics::darray yy1(n1, 0.0);
    GtNumerics::darray yy2(n2, 0.0);
    GtNumerics::darray yy3(n3, 0.0);

    for (int l = sub3; l < end3; ++l)
    {
        indices[3] = l;

        for (int k = sub2; k < end2; ++k)
        {
            indices[2] = k;

            for (int j = sub1; j < end1; ++j)
            {
                indices[1] = j;

                const GtNumerics::darray& yy0 = getValSlice(valsId, 0, indices);

                if (x0 < xx0[0])
                {
                    yy1[j] = axList.at(0)->loExtrapolator()->calc(x0, 0,
                             Interp0, xx0,
                             yy0);
                }
                else if (x0 > xx0[n0 - 1])
                {
                    yy1[j] = axList.at(0)->hiExtrapolator()->calc(x0, n0 - 1,
                             Interp0, xx0,
                             yy0);
                }
                else
                {
                    yy1[j] = Interp0->calc(x0, xx0, yy0, loc0, sub0);
                }
            }

            if (x1 < xx1[0])
            {
                yy2[k] = axList.at(1)->loExtrapolator()->calc(x1, 0, Interp1,
                         xx1, yy1);
            }
            else if (x1 > xx1[n1 - 1])
            {
                yy2[k] = axList.at(1)->hiExtrapolator()->calc(x1, n1 - 1,
                         Interp1, xx1,
                         yy1);
            }
            else
            {
                yy2[k] = Interp1->calc(x1, xx1, yy1, loc1, sub1);
            }
        }

        if (x2 < xx2[0])
        {
            yy3[l] = axList.at(2)->loExtrapolator()->calc(x2, 0, Interp2, xx2,
                     yy2);
        }
        else if (x2 > xx2[n2 - 1])
        {
            yy3[l] = axList.at(2)->hiExtrapolator()->calc(x2, n2 - 1, Interp2,
                     xx2, yy2);
        }
        else
        {
            yy3[l] = Interp2->calc(x2, xx2, yy2, loc2, sub2);
        }
    }

    if (x3 < xx3[0])
    {
        return axList.at(3)->loExtrapolator()->calc(x3, 0, Interp3, xx3, yy3);
    }
    else if (x3 > xx3[n3 - 1])
    {
        return axList.at(3)->hiExtrapolator()->calc(x3, n3 - 1, Interp3,
                xx3, yy3);
    }
    else
    {
        return Interp3->calc(x3, xx3, yy3, loc3, sub3);
    }
}

double
GtTable::getValue5D(const QString& valsId, double x0, double x1, double x2,
                    double x3, double x4) const
{
    Q_UNUSED(valsId);
    Q_UNUSED(x0);
    Q_UNUSED(x1);
    Q_UNUSED(x2);
    Q_UNUSED(x3);
    Q_UNUSED(x4);

    return 0;
}


bool
GtTable::isAscending(const QVector<double>& ticks)
{
    int n = ticks.size();

    if (n > 0)
    {
        double last = ticks[0];

        for (int i = 1; i < n; ++i)
        {
            if (ticks[i] <= last)
            {
                return false;
            }

            last = ticks[i];
        }
    }

    return true;
}


void
GtTable::scaleAxis(const QString& axisName, const double scalingFactor)
{
    QStringList nameList = getAxesNames();

    if (!nameList.contains(axisName))
    {
        throw GTlabException("Table::scaleAxis()",
                             "No such Axis: " + axisName);
    }

    int axisIndex = nameList.indexOf(axisName);
    QList<GtTableAxis*> axList = getAxesList();

    axList.at(axisIndex)->scale(scalingFactor);
}


void
GtTable::scaleValues(const QString& valsId, const double scalingFactor)
{
    if (!m_tabVals.contains(valsId))
    {
        throw GTlabException("Table::scaleValues()",
                             "Unkown Value id.");
    }

    m_tabVals[valsId]->scale(scalingFactor);
}


void
GtTable::setDescription(const QString& desc)
{
    m_description = desc;
}


void
GtTable::setName(const QString& name)
{
    setObjectName(name);
}


void
GtTable::setUnit(const QString& unit)
{
    m_unit = unit;
}

QVector<int>
GtTable:: dimensions() const
{
    QVector<int> dims(nDims());
    QList<GtTableAxis*> axis(getAxesList());

    for (int i = 0; i < nDims(); ++i)
    {
        dims[i] = axis.at(i)->ticks().length();
    }

    return dims;
}


GtNumerics::darray
GtTable::tabVals(const QString& valsId) const
{
    if (!m_tabVals.contains(valsId))
    {
        throw GTlabException("Table::tabVals()",
                             "Unkown Value id.");
    }

    return m_tabVals[valsId]->values();
}


QString
GtTable::valDescription(const QString& valsId) const
{
    if (!m_tabVals.contains(valsId))
    {
        throw GTlabException("Table::valDescription()",
                             "Unkown Value id.");
    }

    return m_tabVals[valsId]->description();
}


QString
GtTable::valUnit(const QString& valsId) const
{
    if (!m_tabVals.contains(valsId))
    {
        throw GTlabException("Table::valUnit()",
                             "Unkown Value id.");
    }

    return m_tabVals[valsId]->unit();
}

QList<QString>
GtTable::tabValsKeys()
{
    return m_tabVals.keys();
}

void
GtTable::clear()
{
    foreach (GtTableAxis* ax, getAxesList())
    {
        delete ax;
        ax = Q_NULLPTR;
    }

    foreach (QString key, tabValsKeys())
    {
        if (getVals(key))
        {
            delete getVals(key);
        }
    }

    m_tabVals.clear();
    m_description.clear();
    m_unit.clear();
}

GtTable::GridType
GtTable::type() const
{
    return m_type;
}

void
GtTable::setType(const GridType& type)
{
    m_type = type;
}

double
GtTable::getMax(QString id, bool* check)
{
    GtTableValues* vals = getVals(id);

    if (vals == Q_NULLPTR)
    {
        if (check != Q_NULLPTR)
        {
            *check = false;
        }

        return 0.0;
    }

    double max = vals->values().first();

    foreach (double val, vals->values())
    {
        if (val > max)
        {
            max = val;
        }
    }

    if (check != Q_NULLPTR)
    {
        *check = true;
    }

    return max;
}

double
GtTable::getMin(QString id, bool* check)
{
    GtTableValues* vals = getVals(id);

    if (vals == Q_NULLPTR)
    {
        if (check != Q_NULLPTR)
        {
            *check = false;
        }

        return 0.0;
    }

    double min = vals->values().first();

    foreach (double val, vals->values())
    {
        if (val < min)
        {
            min = val;
        }
    }

    if (check != Q_NULLPTR)
    {
        *check = true;
    }

    return min;
}

QStringList
GtTable::valueNames()
{
    return m_tabVals.keys();
}

void
GtTable::setTabValsKeysSuffix(QString suffix)
{
    //    if (suffix.size() != m_tabVals.size())
    //    {
    //        throw GTlabException("Table::setTabValsKeys()",
    //                             "new keys are not the same size as old ones.");
    //    }

    //    QMap<QString, GtTableValues*>::iterator i;

    //    int count = 0;
    //    for (i = m_tabVals.begin(); i != m_tabVals.end(); ++i)
    //    {
    //        QString origKey = i.key();
    //        retval.insert(suffix.at(count), i.value());
    //    }

    QList<GtTableValues*> vlist = findDirectChildren<GtTableValues*>();

    m_tabVals.clear();

    foreach (GtTableValues* v, vlist)
    {
        m_tabVals.insert(suffix + v->objectName(), v);
    }
}

QDomElement
GtTable::toDomElement(QDomDocument& doc, QString valId)
{
    QString id = objectName();

    if (valId != "")
    {
        // use specified value id to get data
        id = valId;
    }

    QDomElement eTable = doc.createElement("Table");
    eTable.setAttribute("name", id);

    QDomElement eUnit = doc.createElement("Unit");
    QDomText unitTxt = doc.createTextNode(m_unit);
    eUnit.appendChild(unitTxt);

    QDomElement eDesc = doc.createElement("Description");
    QDomText descTxt = doc.createTextNode(m_description);
    eDesc.appendChild(descTxt);

    eTable.appendChild(eUnit);
    eTable.appendChild(eDesc);

    int axis0Size = 0;

    QList<GtTableAxis*> axesList = getAxesList();

    for (int i = 0; i < getAxesNames().size(); ++i)
    {
        GtTableAxis* currAxe = axesList[i];

        QString des = "N/A";
        QString intMethode = "linear";
        QString lowExtMethod = "linear";
        QString highExtraMethod = "linear";

        if (currAxe != nullptr)
        {
            des = currAxe->description();

            intMethode = interExtraPolationString(currAxe->interMethod());
            lowExtMethod = interExtraPolationString(currAxe->loExtMethod());
            highExtraMethod =
                interExtraPolationString(currAxe->hiExtMethod());
        }

        QDomElement eAxis = doc.createElement("Axis");
        eAxis.setAttribute("id", QString::number(i));

        QDomElement eName = doc.createElement("Name");
        QDomText nameTxt = doc.createTextNode(getAxesNames().at(i));
        eName.appendChild(nameTxt);

        QDomElement eUnit = doc.createElement("Unit");
        QString axisUnit = getAxesList().at(i)->unit();
        QDomText unitTxt = doc.createTextNode(
                               axisUnit);
        eUnit.appendChild(unitTxt);

        QDomElement eDesc = doc.createElement("Description");
        QDomText descTxt = doc.createTextNode(des);
        eDesc.appendChild(descTxt);

        QDomElement eInterp = doc.createElement("Interpolation");
        QDomText interpTxt = doc.createTextNode(intMethode);
        eInterp.appendChild(interpTxt);

        QDomElement eLoExt = doc.createElement("LowExtrapolation");
        QDomText loextTxt = doc.createTextNode(lowExtMethod);
        eLoExt.appendChild(loextTxt);

        QDomElement eHiExt = doc.createElement("HighExtrapolation");
        QDomText hiextTxt = doc.createTextNode(highExtraMethod);
        eHiExt.appendChild(hiextTxt);

        QVector< double > ticks;
        const QString name = getAxesNames().at(i);
        getAxisTicks(name, ticks);
        double tick = 0;
        QString ticksStr;

        foreach (tick, ticks)
        {
            ticksStr.append(QString::number(tick, 'f', 13) + ", ");
        }

        ticksStr.chop(2);
        QDomElement eTicks = doc.createElement("Ticks");
        QDomText ticksTxt = doc.createTextNode(ticksStr);
        eTicks.appendChild(ticksTxt);

        if (i == 0)
        {
            axis0Size = ticks.size();
        }


        eTable.appendChild(eAxis);
        eAxis.appendChild(eName);
        eAxis.appendChild(eUnit);
        eAxis.appendChild(eDesc);
        eAxis.appendChild(eInterp);
        eAxis.appendChild(eLoExt);
        eAxis.appendChild(eHiExt);
        eAxis.appendChild(eTicks);
    }

    QDomElement eVals = doc.createElement("Values");
    QString valsStr;

    if (!m_tabVals.contains(id))
    {
        throw GTlabException("Table::toDomElement()",
                             "Unkown Value id: " + id);
    }

    GtTableValues* tv = m_tabVals.value(id);
    QVector<double> vals = tv->values();
    int m = 1;

    valsStr.append("\n");

    for (int i = 0; i < vals.size(); i++)
    {
        QString str;
        str = ", ";

        if (i == m * axis0Size)
        {
            str = ",\n";
            m++;
        }

        valsStr.append(QString::number(vals.at(i), 'f', 13) + str);
    }

    valsStr.chop(2);
    valsStr.append("\n");
    QDomText valsTxt = doc.createTextNode(valsStr);

    eVals.appendChild(valsTxt);
    eTable.appendChild(eVals);

    return eTable;
}


QVector<double>
GtTable::getValSlice(const QString& valsId, int axisIndex,
                     const QVector<int>& indices) const
{
    if (axisIndex >= nDims())
    {
        throw GTlabException("Table::getSlice()",
                             "Axis index out of bounds.");
    }

    if (indices.size() != nDims())
    {
        throw GTlabException("Table::getSlice()",
                             "Size of indices vector must be equal to number "
                             "of dimensions.");
    }

    int start = calcStart(axisIndex, indices);
    int size = getAxisSize(axisIndex);
    int stride = calcStride(axisIndex);

    return slice(valsId, start, size, stride);
}


QVector<double>
GtTable::slice(const QString& valsId, int start, int size, int stride) const
{
    if (!m_tabVals.contains(valsId))
    {
        throw GTlabException("Table::slice()",
                             "Unkown Value id.");
    }

    QVector<double> out(size);

    for (int i = 0; i < size; i++)
    {
        out[i] = m_tabVals[valsId]->values().at(start + i * stride);
    }

    return out;
}


int
GtTable::axisIndex(const QString& axisName)
{
    return getAxesNames().indexOf(axisName);
}


void
GtTable::onObjectDataMerged()
{
    m_tabVals.clear();

    QList<GtTableValues*> vlist = findDirectChildren<GtTableValues*>();

    foreach (GtTableValues* v, vlist)
    {
        m_tabVals.insert(v->objectName(), v);
    }
}

void
GtTable::deleteTabVals()
{
    m_tabVals.clear();
}
