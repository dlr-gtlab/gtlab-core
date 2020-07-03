/* GTlab - Gas Turbine laboratory
 * Source File: gt_contour.cpp
 * copyright 2009-2014 by DLR
 *
 *  Created on: 28.03.2014
 *      Author: Carsten Hollmann (AT-TW)
 *		  Tel.: +49 2203 601 2859
 */

#include "gt_contour.h"

#include <QPolygonF>
#include <QColor>
#include <QThreadPool>

#include "gt_contourrunnable.h"
#include "gt_objectfactory.h"

GtContour::GtContour() :
     m_polygonCalculation(false)
{
}

QHash<QString, QList<QPolygonF> >
GtContour::calcContour(GtTable* table, QString param,
                       QStringList isoValues, bool calcPolygons,
                       bool returnPolygonsOnly, bool* check)
{
    m_isoValues.clear();

    QHash<QString, QList<QPolygonF> > contourLines;

    if (!setData(table, param))
    {
        if (check != Q_NULLPTR)
        {
            *check = false;
            return contourLines;
        }
    }

    if (isoValues.isEmpty())
    {
        return contourLines;
    }

    m_isoValues = isoValues;

    GtObjectMemento tableMemento(&m_table);

    QThreadPool* tp = QThreadPool::globalInstance();

    QList<GtContourRunnable*> runnables;

    for(int i=0; i<isoValues.size(); i++)
    {
        GtContourRunnable* cr = Q_NULLPTR;


        if (i != isoValues.size()-1)
        {
            cr = new GtContourRunnable(tableMemento,
                                       isoValues.at(i),
                                       calcPolygons,
                                       true,
                                       isoValues.at(i+1));
        }
        else
        {
            cr = new GtContourRunnable(tableMemento,
                                       isoValues.at(i),
                                       calcPolygons);

        }

        runnables.append(cr);
        tp->start(cr);
    }

    tp->waitForDone();

    foreach(GtContourRunnable* runnable, runnables)
    {
        QList<QPolygonF> polys;
        if (!calcPolygons)
        {
            polys.append(runnable->isoLines());
        }
        else if (calcPolygons && returnPolygonsOnly)
        {
            polys.append(runnable->isoPolygons());
        }
        else
        {
            polys.append(runnable->isoLines());
            polys.append(runnable->isoPolygons());
        }

        contourLines.insert(runnable->isoValue(), polys);

        delete runnable;
    }

    return contourLines;
}

QList<QPolygonF>
GtContour::calcContour(GtTable* table, QString param, QString isoValue,
                       bool calcPolygons, bool returnPolygonsOnly, bool* check)
{
    QStringList isoValues;
    isoValues.append(isoValue);

    return calcContour(table, param, isoValues, calcPolygons,
                       returnPolygonsOnly, check).value(isoValue);
}

QHash<QString, QList<QPolygonF> >
GtContour::calcContour(GtTable* table, QString param, int nIsoValues,
                       bool calcPolygons, bool returnPolygonsOnly, bool* check)
{
    if (table == Q_NULLPTR)
    {
        return QHash<QString, QList<QPolygonF> >();
    }

    double max = table->getMax(param, check);

    if (check != Q_NULLPTR)
    {
        if (!check)
        {
            return QHash<QString, QList<QPolygonF> >();
        }
    }

    double min = table->getMin(param, check);
    if (check != Q_NULLPTR)
    {
        if (!check)
        {
            return QHash<QString, QList<QPolygonF> >();
        }
    }

    if (qAbs(max-min) < 1e-10)
    {
        if (check != Q_NULLPTR)
        {
            *check = false;
        }

        nIsoValues = 1;
    }

    QStringList isoValues;
    if (nIsoValues == 1)
    {
        isoValues.append(QString::number(min, 'g', 16));
    }
    else
    {
        for (int i = 0; i < nIsoValues; i++)
        {
            double isoVal = min + (max - min) / (nIsoValues - 1) * i;
            isoValues.append(QString::number(isoVal, 'g', 16));
        }

    }

    return calcContour(table, param, isoValues, calcPolygons,
                       returnPolygonsOnly, check);
}

bool
GtContour::setData(GtTable* table, QString paramName, int iDim, int jDim)
{
    // ABORT IF GIVEN TABLE IS NOT VALID
    if (!isValid(table, paramName))
    {
        return false;
    }

    // CREATE HELP TABLE
    m_table.clear();

    GtNumerics::darray axis0 = table->getAxisTicks(0);
    double axis0Min = axis0.first();
    double axis0Max = axis0.last();

    GtNumerics::darray axis1 = table->getAxisTicks(1);
    double axis1Min = axis1.first();
    double axis1Max = axis1.last();

    if (iDim == 0)
    {
        iDim = axis0.size();
    }

    if (jDim == 0)
    {
        jDim = axis1.size();
    }

    //DEFINE AXIS VALUES
    QVector<double> phiValues;
    QVector<double> xiValues;

    for (int i = 0; i < iDim; i++)
    {
        xiValues << 0 +  double(i) *(1 / (double(iDim) - 1));
    }

    for (int i = 0; i < jDim; i++)
    {
        phiValues << 0 + double(i) *(1 / (double(jDim) - 1));
    }

    GtNumerics::darray newXvalues;
    GtNumerics::darray newYvalues;
    GtNumerics::darray newZvalues;

    int counter = 0;
    for (int i = 0; i < iDim; i++)
    {
        for (int j = 0; j < jDim; j++)
        {
            newXvalues.append(axis0Min + i * (axis0Max - axis0Min) /
                                  (iDim - 1));

            newYvalues.append(axis1Min + j * (axis1Max - axis1Min)
                                  / (jDim - 1));

            double res = 0.0;

            try
            {
                res = table->getValue2D(paramName,
                                        newXvalues.at(counter),
                                        newYvalues.at(counter));
            }
            catch (GTlabException& e)
            {
                gtError() << "Exception" << e.what() << "in " << e.where();
                return false;
            }

            newZvalues.append(res);

            counter++;
        }
    }

    //INITIALISE HELP TABLE BASED ON INPUT (X, Y, Z)
    m_table.addAxis("phi", "dimensionless phi axis", "[-]",
                     GtTableAxis::E_LINEAR, GtTableAxis::I_LINEAR,
                     GtTableAxis::E_LINEAR, phiValues);

    m_table.addAxis("xi", "dimensionless xi axis", "[-]",
                     GtTableAxis::E_LINEAR, GtTableAxis::I_LINEAR,
                     GtTableAxis::E_LINEAR, xiValues);

    m_table.addValues("x", "x-coordinate", "[-]", newXvalues);
    m_table.addValues("y", "y-coordinate", "[-]", newYvalues);
    m_table.addValues("z", "x-coordinate", "[-]", newZvalues);

    return true;
}

QStringList
GtContour::isoValues()
{
    return m_isoValues;
}

bool
GtContour::isValid(GtTable* t, QString paramName)
{
    if (t->getAxesList().size() != 2)
    {
        gtError() << QObject::tr("Number of Axes does not match 2, cannot "
                                 "perform any contour calculation.");

        return false;
    }

    if (t->tabValsKeys().size() < 1)
    {
        gtError() << QObject::tr("There are no values in this table, cannot "
                                 "perform any contour calculation.");

        return false;
    }

    if (!t->valueNames().contains(paramName))
    {
        gtError() << QObject::tr("No values found in this table for the "
                                 "given parameter name, cannot perform any "
                                 "contour calculation");

        return false;
    }

    return true;
}

