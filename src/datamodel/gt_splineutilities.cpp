
#include "gt_spline.h"
#include "gt_exceptions.h"
#include "gt_rootfinder.h"

#include "sl_common.h"
#include "sl_point.h"
#include "sl_bsplinecurve.h"

#include "gt_splineutilities.h"


GtSpline*
GtSplineUtilities::insertKnot(GtSpline* spline, double t)
{

    QVector<double> m_knot = spline->KnotVec();
    QList<QPointF> m_Cp = spline->controlPointsConverted();
    int degree = spline->degree();

    //qDebug() << "~~~~~~~ Insert knot at t = " << t;
    //qDebug() << "~~~~~~~ m_knot = " << m_knot;
    //qDebug() << "~~~~~~~ degree = " << degree;

    if (t == 0 || t == 1)
    {
        return spline;
    }

    // Bezier spline can not exist now, after number of Cp is increased by
    // one, because this requires that prior to this mission the number of
    // Cp equaled the degree which is not permitted


    // get span number, which equals the highest index of the control points
    // vector that affects the point of the current t parameter
    // k, k-1, k-2, ..., k-p all affect the point of the current t parameter

    //int k = spline->findSpan(degree, t, m_knot); // does not work for u = 0.2
    //int k = GtBSplineAlgorithm::findSpan(degree, t, m_knot);    ///JS
    int k = SLCommon::findSpan(degree, t, m_knot.toStdVector());

    // Initial multiplicity of t
    int s = multiplicity(t, m_knot);
    //qDebug() << "~~~~~~~ Initial multiplicity s = " << s;

    // Desired multiplicity of new Cp
    int h = degree - s;
    //qDebug() << "~~~~~~~ Inserted h times h = " << h;

    double alpha;

    QPointF Q;

    // Nested Vector, because knot is inserted h times, and points of h-th
    // insertion needs points of h-1-th insertion
    QVector< QVector<QPointF> > Qlist;
    Qlist.resize(h + 1);

    // Final values
    QList<QPointF> Qfinal;



    // Put all first Cp prior to any insertion into the 0-th level of
    // the nested array and
    // Add the first, unaffected points to the Cp-Vector
    for (int i = 0; i < m_Cp.size(); i++)
    {
        Qlist[0].append(m_Cp[i]);
        if (i <= k - degree)
        {
            Qfinal.append(m_Cp[i]);
        }
    }

    // Loop through all insertions (h = m_N - s times)
    for (int r = 1; r <= h; r++)
    {
        // Adjust Vector size of the r insertion and loop through
        // All new points
        Qlist[r].resize(k - s + 1);
        for (int i = k - degree + r; i <= k - s; i++)
        {
            alpha = (t - m_knot[i]) / ( m_knot[i + degree - r + 1] - m_knot[i]);
            Q = (1 - alpha) * Qlist[r-1][i-1] + alpha * Qlist[r-1][i];

            Qlist[r].replace(i, Q);

            // Add all new values of the last insertion (is only 1 value
            // if Cp is to be stored on the BSpline curve
            if (r == h)
            {
                Qfinal.append(Q);
            }
            // Add the first new values of each r-th insertion to the final Vec
            else if (i == k - degree + r)
            {
                Qfinal.append(Q);
            }

        }
    }

    // Add the last new values of each r-th insertion to the final Vec
    for (int i = h - 1; i >= 1; i--)
    {
        Qfinal.append(Qlist[i][k - s]);
    }

    // Add the last, unaffected points to the Cp-Vector
    for (int i = k - s; i < m_Cp.size(); i++)
    {
        Qfinal.append(m_Cp[i]);
    }

    m_Cp = Qfinal;


    for (int i = 0; i < h; i++)
    {
        m_knot.insert(k + 1, t);
    }

    qDebug() << "Final knots:" << m_knot.size();
    qDebug() << "Final ControlPoints:" << m_Cp.size();

    GtSpline* newSpline = new GtSpline(m_Cp, degree, m_knot);
    return newSpline;




    // old code, which adds a Cp on a Leg between Pi-1 and Pi
    // adds the knot only one time, so no multiplicity and the Cp
    // is therefore not positioned on the BSpline Curve but on the Leg
//    for (int i = 0; i < m_Cp.size(); i++)
//    {
//        qDebug() << "k = " << k;
//        if (i <= k - m_N)
//        {
//            Q = m_Cp[i];
//        }
//        else if (i >= k + 1)
//        {
//            Q = m_Cp[i - 1];
////            qDebug() << ", Q = " << Q;
//        }
//        else
//        {
//            qDebug() << "i = " << i  << ", m_Cp.size() = " << m_Cp.size();
//            alpha = (t - m_knot[i]) / ( m_knot[i + m_N] - m_knot[i]);
//            Q = (1- alpha) * m_Cp[i-1] + alpha * m_Cp[i];
//            qDebug() << "t - m_knot[i] = " << t - m_knot[i]  <<
//                        ", m_knot[i + m_N] - m_knot[i] = " << m_knot[i + m_N] - m_knot[i];
//            qDebug() << "alpha = " << alpha  << ", Q = " << Q;
//        }


//        Qlist.append(Q);
////        qDebug() << "i = " << i  << ", Q = " << Q;

//    }
//    Qlist.append(m_Cp.last());
//    m_Cp = Qlist;
    //    m_knot.insert(k + 1, t);


    // TODO: implement
    // python knot insertion von Martin Siggel
//    if self.is_rational:
//        self.__insert_knot_nurbs_single(x)
//        return

//    if x <= self.knots[0] or x >= self.knots[-1]:
//        raise Exception("Knot value is outside valid knot range")

//    pos = find_pos(self.degree, self.knots, x)

//    # count multiplicity of knot
//    k = pos
//    while x - self.knots[k] <= abs(x) * 1e-14:
//        k -= 1

//    multiplicity = pos - k
//    if multiplicity == self.degree:
//        # knot has full multiplicity already, stop
//        return pos

//    self.cp = np.hstack((self.cp[:, 0:pos + 1], self.cp[:, pos:]))

//    for k in range(pos, pos - self.degree, -1):
//        alpha = (x - self.knots[k]) / (self.knots[k + self.degree] - self.knots[k])
//        self.cp[:, k] = alpha * self.cp[:, k] + (1. - alpha) * self.cp[:, k - 1]

//    self.knots = np.hstack((self.knots[0:pos + 1], x, self.knots[pos + 1:]))
//    return pos + 1
}

QList<GtSpline*>
GtSplineUtilities::splitCurve(GtSpline* spline, double t, QVector<double> knot)
{

    QList<GtSpline*> splines;

    if (t == 0 || t == 1)
    {
        qDebug() << "##### splitCurve Function exit, t = " << t;
        splines.append(spline);
        splines.append(spline);
        return splines;
    }

    // Initial multiplicity of t
    int s = multiplicity(t, knot);

    QVector<double> new_knot;
    QList<QPointF> new_Cp;

    int m_N = spline->degree();
    QVector<double> m_knot = spline->KnotVec();
    QList<QPointF> m_Cp = spline->controlPointsConverted();

    //int k = spline->findSpan(m_N, t, knot);
    //int k = GtBSplineAlgorithm::findSpan(m_N, t, knot); /// JS
    int k = SLCommon::findSpan(m_N, t, knot.toStdVector());
    qDebug() << "k = " << k << ", s = " << s << ", m_N = " << m_N;

    for (int i = 0; i < k + m_N; i++)
    {
        double m;
        m = m_knot[i] / m_knot[k + m_N - 1];
        new_knot.append(m);

        qDebug() << "i = " << i << ", m_knot[i] = " << m_knot[i];

        // Ensure that there is always a multiplicity of m_N + 1 = p + 1 at
        // the right end
        if (i == k + m_N - 1)
        {
            new_knot.append(m);
            if (s == 0)
            {
                new_knot.append(m);
            }
        }
    }

    qDebug() << "new_Knot Left = " << new_knot;

    for (int i = 0; i <= k - s; i++)
    {
        new_Cp.append(m_Cp[i]);
    }

    qDebug() << "new_Cp Left = " << new_Cp;

    GtSpline* leftSpline = new GtSpline(new_Cp, m_N, new_knot);



    new_knot.clear();
    new_Cp.clear();



    double m;
    double mFix = m_knot[k + 1];
    for (int i = k + 1; i < m_knot.size(); i++)
    {
        m = m_knot[i];
        // Ensure that there is always a multiplicity of m_N + 1 = p + 1 at
        // the left end
        if (i == k + 1)
        {
            new_knot.append((m - mFix) / (1- mFix));
            qDebug() << "new_knot = " << new_knot;
            if (s != 0)
            {
                new_knot.append((m - mFix) / (1- mFix));
            }
        }
        new_knot.append((m - mFix) / (1- mFix));


    }

    qDebug() << "new_Knot Right = " << new_knot;

    for (int i = k-s; i < m_Cp.size(); i++)
    {
        new_Cp.append(m_Cp[i]);
    }

    qDebug() << "new_Cp Right = " << new_Cp;

    GtSpline* rightSpline = new GtSpline(new_Cp, m_N, new_knot);

    splines.append(leftSpline);
    splines.append(rightSpline);
    return splines;
}

bool
GtSplineUtilities::isAxialFunction(GtSpline* spline, double nLoops, int movedCp)
{
    double dt = 1.0 / nLoops;
    int startLoop = 0;

    //if (movedCp != -1)
    if (false) // works only if if was a function previously
    {
        // perform algorithm between distinct knotVec values
        double t_low = spline->KnotVec().at(movedCp);
        double t_high = spline->KnotVec().at(movedCp + spline->degree() + 1);

        // New number of loops will be lower than the original nLoops
        // + 1 ensures that it is always rounded up
        nLoops = qRound(((t_high - t_low) / dt) + 1);
        startLoop = qRound(((t_low - 0) / dt) - 1);
        if (startLoop < 0)
        {
            startLoop = 0;
        }
    }
    QPointF p;
    QPointF pNext;

    QList<QPointF> controls = spline->controlPointsConverted();

    // convert control points
    std::vector<SLPoint> cps;
    foreach (const QPointF& cp, controls)
    {
        cps.push_back(SLPoint(cp.x(), cp.y()));
    }

    // generate spline
    SLBSplineCurve spline_tmp(cps, spline->KnotVec().toStdVector(),
                              spline->degree());

    for (int i = startLoop; i <= nLoops; i++)
    {
        //p = spline->curvePoint(i * dt);
        //pNext = spline->curvePoint((i + 1) * dt);

        // get points on curve
        SLPoint p_temp = spline_tmp.eval2(i * dt);
        SLPoint p_next_temp = spline_tmp.eval2((i + 1) * dt);

        /// JS
//        p = GtBSplineAlgorithm::curvePoint(i * dt,
//                                           controls,
//                                           spline->KnotVec(),
//                                           spline->degree());
        p.setX(p_temp.X());
        p.setY(p_temp.Y());

//        pNext = GtBSplineAlgorithm::curvePoint(
//                    (i + 1) * dt,
//                    controls,
//                    spline->KnotVec(),
//                    spline->degree());
        pNext.setX(p_next_temp.X());
        pNext.setY(p_next_temp.Y());

        if (pNext.x() < p.x())
        {
            return false;
        }
    }

    return true;

    // old, slow algorithm, two loops
    // iterates over all points once, then for each point it iterates again
    // to check if two points of the second loop enclose this first point
    // this means that the spline is not a function as there are multiple
    // values of y for one x
//    QPointF p;
//    QPointF pInnerFirst;
//    QPointF pInnerSecond;
//    for (int i = 0; i <= nLoops; i++)
//    {

//        qDebug() << "i = " << i;
//        p = spline->curvePoint(i * dt);

//        for (int j = 0; j <= nLoops; j++)
//        {
//            pInnerFirst = spline->curvePoint(j * dt);
//            pInnerSecond = spline->curvePoint((j + 1) * dt);

//            if (pInnerFirst.x() < p.x() && pInnerSecond.x() > p.x())
//            {
//                return false;
//            }
//            if (pInnerFirst.x() > p.x() && pInnerSecond.x() < p.x())
//            {
//                return false;
//            }
//        }
//    }
//    return true;
}

bool
GtSplineUtilities::isVerticalFunction(GtSpline* spline, double nLoops,
                                      int movedCp)
{
    double dt = 1.0 / nLoops;

    int startLoop = 0;

    //if (movedCp != -1)
    if (false) // works only if if was a function previously
    {
        // perform algorithm between distinct knotVec values
        double t_low = spline->KnotVec().at(movedCp);
        double t_high = spline->KnotVec().at(movedCp + spline->degree() + 1);
        // New number of loops will be lower than the original nLoops
        // + 1 ensures that it is always rounded up
        nLoops = qRound(((t_high - t_low) / dt) + 1);
        startLoop = qRound(((t_low - 0) / dt) - 1);
        if (startLoop < 0)
        {
            startLoop = 0;
        }
    }

    QPointF p;
    QPointF pNext;

    QList<QPointF> controls = spline->controlPointsConverted();
    QVector<double> knot = spline->KnotVec();

    // convert control points
    std::vector<SLPoint> cps;
    foreach (const QPointF& cp, controls)
    {
        cps.push_back(SLPoint(cp.x(), cp.y()));
    }

    // generate spline
    SLBSplineCurve spline_tmp(cps, knot.toStdVector(), spline->degree());

    for (int i = startLoop; i <= nLoops; i++)
    {
        //p = spline->curvePoint(i * dt);
        //pNext = spline->curvePoint((i + 1) * dt);

        SLPoint p_temp = spline_tmp.eval2(i * dt);
        SLPoint p_next_temp = spline_tmp.eval2((i + 1) * dt);

        /// JS
//        p = GtBSplineAlgorithm::curvePoint(i * dt,
//                                           controls,
//                                           knot,
//                                           spline->degree());

//        pNext = GtBSplineAlgorithm::curvePoint(
//                    (i + 1) * dt,
//                    controls,
//                    knot,
//                    spline->degree());

        p.setX(p_temp.X());
        p.setY(p_temp.Y());

        pNext.setX(p_next_temp.X());
        pNext.setY(p_next_temp.Y());

        if (pNext.y() < p.y())
        {
            return false;
        }
    }

    return true;
}

QPointF
GtSplineUtilities::getYfromX(GtSpline* spline, double xVal, double tGuess)
{

    if (!isAxialFunction(spline))
    {
        qDebug() << "Spline is not a function in axial direction, cannot "
                    "get a unique value for desired x-Value = " << xVal;
        return QPointF(0, 0);
    }

    QPointF p0;
    QPointF p1;

    QList<QPointF> controls = spline->controlPointsConverted();

    // convert control points
    std::vector<SLPoint> cps;
    foreach (const QPointF& cp, controls)
    {
        cps.push_back(SLPoint(cp.x(), cp.y()));
    }

    // generate spline
    SLBSplineCurve spline_tmp(cps, spline->KnotVec().toStdVector(),
                              spline->degree());

    try
    {
        SLPoint p_temp = spline_tmp.eval2(0);
        p0.setX(p_temp.X());
        p0.setY(p_temp.Y());

//        p0 = GtBSplineAlgorithm::curvePoint(
//                    0,
//                    controls,
//                    spline->KnotVec(),
//                    spline->degree());
    }
    catch (GTlabException& e)
    {
        qDebug() << "Spline point p0 could not be calculated";
        qDebug() << "Reason:" << e.what();
        return QPointF(0, 0);
    }

    try
    {
        SLPoint p_temp = spline_tmp.eval2(1);
        p1.setX(p_temp.X());
        p1.setY(p_temp.Y());

//        p1 = GtBSplineAlgorithm::curvePoint(
//                    1,
//                    controls,
//                    spline->KnotVec(),
//                    spline->degree());
    }
    catch (GTlabException& e)
    {
        qDebug() << "Spline point p0 could not be calculated";
        qDebug() << "Reason:" << e.what();
        return QPointF(0, 0);
    }

    if (p0.x() > xVal)
    {
        qDebug() << "x-Value smaller than smallest x-Value of Spline.";
        qDebug() << "requested:" << xVal << " smallest Spline X:" << p0.x();
        return p0;
    }
    if (p1.x() < xVal)
    {
        qDebug() << "x-Value greater than greatest x-Value of Spline.";
        return p1;
    }

    double t1 = tGuess;
    double t_new;
    double delta = 0.00001;
    double f_t1;
    double df_t1;
    int count;
    double epsMin = 0.001;
    int maxCount = 15;
    double eps;




    // TODO: put Newton-Raphson from GTlab numerics in here
//    t1 = myRootFinder::Newton(funcXvariation(spline, xVal, t1),
//                              funcXvariation(spline, xVal, t1), t1, epsMin, maxCount);
//    qDebug() << "Newton from Performance";
//    qDebug() << "t1 = " << t1;
//    t1 = 0.5;
    count = 0;
    do
    {
        if (t1 < 0)
        {
            t1 = 0;
        }
        f_t1 = funcXvariation(spline, xVal, t1);
        df_t1 = (funcXvariation(spline, xVal, t1 + delta) - f_t1) / delta;

        t_new = t1 - (f_t1 / df_t1);
        eps = qAbs(t1 - t_new);

//        qDebug() << "t1 = " << t1 << ", t_new = " << t_new << "eps = " << eps;

        t1 = t_new;
        count ++;
    }
    while (eps >= epsMin && count < maxCount);

    /// JS
    ///return spline->curvePoint(t1);

    // convert control points
    std::vector<SLPoint> cps_temp;
    foreach (const QPointF& cp, controls)
    {
        cps_temp.push_back(SLPoint(cp.x(), cp.y()));
    }

    // generate spline
    SLBSplineCurve spline_tmp2(cps_temp, spline->KnotVec().toStdVector(),
                               spline->degree());

    try
    {
        SLPoint p_temp = spline_tmp2.eval2(t1);

        return QPointF(p_temp.X(), p_temp.Y());

//        return GtBSplineAlgorithm::curvePoint(
//                    t1,
//                    controls,
//                    spline->KnotVec(),
//                    spline->degree());
    }
    catch (GTlabException& e)
    {
        qDebug() << "Spline point p0 could not be calculated";
        qDebug() << "Reason:" << e.what();
        return QPointF(0, 0);
    }
}

QPointF
GtSplineUtilities::getXfromY(GtSpline* spline, double yVal)
{
    if (!isVerticalFunction(spline))
    {
        qDebug() << "Spline is not a function in vertical direction, cannot "
                    "get a unique value for desired x-Value = " << yVal;
        return QPointF(0,0);
    }
    /// JS
    ///if (spline->curvePoint(0).y() > yVal)
    ///{
    ///    qDebug() << "y-Value smaller than smallest y-Value of Spline.";
    ///    return spline->curvePoint(0);
    ///}
    ///if (spline->curvePoint(1).y() < yVal)
    ///{
    ///    qDebug() << "y-Value greater than greatest y-Value of Spline.";
    ///    return spline->curvePoint(1);
    ///}

    QList<QPointF> controls = spline->controlPointsConverted();

    // convert control points
    std::vector<SLPoint> cps;
    foreach (const QPointF& cp, controls)
    {
        cps.push_back(SLPoint(cp.x(), cp.y()));
    }

    // generate spline
    SLBSplineCurve spline_tmp(cps, spline->KnotVec().toStdVector(),
                              spline->degree());

    SLPoint p0_temp = spline_tmp.eval2(0);
    SLPoint p1_temp = spline_tmp.eval2(1);

    QPointF p0(p0_temp.X(), p0_temp.Y());
    QPointF p1(p1_temp.X(), p1_temp.Y());

//    QPointF p0 = GtBSplineAlgorithm::curvePoint(
//                0,
//                controls,
//                spline->KnotVec(),
//                spline->degree());
//    QPointF p1 = GtBSplineAlgorithm::curvePoint(
//                1,
//                controls,
//                spline->KnotVec(),
//                spline->degree());

    if (p0.y() > yVal)
    {
        qDebug() << "y-Value smaller than smallest y-Value of Spline.";
        return p0;
    }
    if (p1.y() < yVal)
    {
        qDebug() << "y-Value greater than greatest y-Value of Spline.";
        return p1;
    }


    qDebug() << "Newton Iteration to get X val";
    double t1 = 0.5;
    double t_new;
    double delta = 0.00001;
    double f_t1;
    double df_t1;
    int count;
    double epsMin = 0.001;
    int maxCount = 15;
    double eps;



    // TODO: put Newton-Raphson from GTlab numerics in here
    count = 0;
    do
    {
        if (t1 < 0)
        {
            t1 = 0;
        }
        f_t1 = funcYvariation(spline, yVal, t1);
        df_t1 = (funcYvariation(spline, yVal, t1 + delta) - f_t1) / delta;

        t_new = t1 - (f_t1 / df_t1);
        eps = qAbs(t1 - t_new);

        qDebug() << "t1 = " << t1 << ", t_new = " << t_new << "eps = " << eps;

        t1 = t_new;
        count ++;
    }
    while (eps >= epsMin && count < maxCount);

    /// JS
    ///return spline->curvePoint(t1);

    SLPoint pt_temp = spline_tmp.eval2(t1);

//    return GtBSplineAlgorithm::curvePoint(
//                t1,
//                controls,
//                spline->KnotVec(),
//                spline->degree());

    return QPointF(pt_temp.X(), pt_temp.Y());
}

int
GtSplineUtilities::multiplicity(double t, QVector<double> knot)
{
    int s = 0;
    for (int i = 0; i < knot.size(); i ++)
    {
        if (t == knot[i]) //qAbs(t - knot[i]) < 0.000000001)
        {
            s++;
        }
    }
    return s;
}

double
GtSplineUtilities::funcXvariation(GtSpline* spline, double xVal, double t)
{
    QPointF p;

    /// JS
    ///p = spline->curvePoint(t);
    QList<QPointF> controls = spline->controlPointsConverted();

    // convert control points
    std::vector<SLPoint> cps;
    foreach (const QPointF& cp, controls)
    {
        cps.push_back(SLPoint(cp.x(), cp.y()));
    }

    // generate spline
    SLBSplineCurve spline_tmp(cps, spline->KnotVec().toStdVector(),
                              spline->degree());

    SLPoint p_temp = spline_tmp.eval2(t);

//    p = GtBSplineAlgorithm::curvePoint(
//                t,
//                controls,
//                spline->KnotVec(),
//                spline->degree());

    p.setX(p_temp.X());
    p.setY(p_temp.Y());

    return xVal - p.x();
}

double
GtSplineUtilities::funcYvariation(GtSpline* spline, double yVal, double t)
{
    QPointF p;

    /// JS
    ///p = spline->curvePoint(t);
    QList<QPointF> controls = spline->controlPointsConverted();

    // convert control points
    std::vector<SLPoint> cps;
    foreach (const QPointF& cp, controls)
    {
        cps.push_back(SLPoint(cp.x(), cp.y()));
    }

    // generate spline
    SLBSplineCurve spline_tmp(cps, spline->KnotVec().toStdVector(),
                              spline->degree());

    SLPoint p_temp = spline_tmp.eval2(t);

//    p = GtBSplineAlgorithm::curvePoint(
//                t,
//                controls,
//                spline->KnotVec(),
//                spline->degree());

    p.setX(p_temp.X());
    p.setY(p_temp.Y());

    return yVal - p.y();
}

double
GtSplineUtilities::pointsDistance(QPointF p0, QPointF p1)
{
    return qSqrt(pow((p0-p1).x(), 2) + pow((p0-p1).y(), 2));
}

QPointF
GtSplineUtilities::halfWay(QPointF p0, QPointF p1)
{
    return QPointF((p0.x() + p1.x()) / 2.0, (p0.y() + p1.y()) / 2.0);
}
