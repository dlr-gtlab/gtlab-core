#include "gt_dummyresults.h"

#include "gt_datazonetable.h"
#include "gt_datazone0d.h"
#include "gt_datazone.h"

#include <QDebug>
#include <QStringList>

GtDummyResults::GtDummyResults()
{

}

GtDataZoneTable*
GtDummyResults::createDzt(int count_drops, int dim)
{
    // ---------------------------------------
    // START create dummy DataZoneTable data
    // ---------------------------------------

    QStringList x;
    x.append("OP1");
    x.append("OP2");
    x.append("OP3");
    x.append("OP4");
    x.append("OP5");
    QStringList y;
    y.append("Stage1");
    y.append("Stage2");
//    y.append("Stage3");
    QStringList z;
    z.append("Station1");
    z.append("Station2");
    z.append("Station3");
    z.append("Station4");

    QList<GtAbstractDataZone*> data;

    QStringList params;
    QStringList units;



    if (count_drops == 1)
    {
        params.append("HPC.T");
        units.append("[K]");
        params.append("HPC.P");
        units.append("[Pa]");
        params.append("HPC.W");
        units.append("[kg/s]");
        params.append("HPC.In.Cp");
        units.append("[J/(kg/sec)]");
    }
    else if (count_drops == 2)
    {
        params.append("INTAKE.T");
        units.append("[KKKK]");
        params.append("INTAKE.P");
        units.append("[Pa]");
        params.append("INTAKE.W");
        units.append("[kg/s]");
        params.append("INTAKE.In.Cp");
        units.append("[J/(kg/sec)]");
    }
    else if (count_drops == 3)
    {
        params.append("CRTF.T");
        units.append("[K]");
        params.append("CRTF.P");
        units.append("[Pa]");
        params.append("CRTF.W");
        units.append("[kg/s]");
        params.append("CRTF.In.Cp");
        units.append("[J/(kg/sec)]");
    }

    QVector<double> ticks1;

    int endFirst = 1;

    if (dim > 0)
    {
        endFirst = 3;
    }

    for (int j = 0; j < endFirst; j++)
    {
        double t = j / 2.0;
        ticks1.push_back(t);
    }

    qDebug() << "ticks1 = " << ticks1;

    QVector<double> ticks2;

    int endSecond = 1;

    if (dim > 1)
    {
        endSecond = 5;
    }

    for (int j = 0; j < endSecond; j++) // originally 5
    {
        double t = j / 2.0;
        ticks2.push_back(t);
    }

    qDebug() << "ticks2 = " << ticks2;


    for (int m = 0; m < x.size()*y.size()*z.size(); m++)
    {
        GtDataZone* dzND = new GtDataZone();
        GtDataZone0D* dz = new GtDataZone0D();
        QMap< QString, QVector<double> > allVals;

        QVector<double> vals0D;
        for (int i = 0; i < params.size(); i++)
        {
            QVector<double> vals;

            for (int j = 0; j < ticks1.size() * ticks2.size(); j++)
            {
                double ran = rand() % 100;

                vals.push_back(ran * (m + 1) * 10.0 + 3 * i + 2 * j);
                vals0D.push_back(ran * (m + 5) + 3);

            }

            allVals.insert(params.at(i), vals);
        }
//        qDebug() << "vals0D = " << vals0D;

        if (dim == 0)
        {
            dz->setData(params, vals0D, units);
            data.push_back(dz);
        }
        else if (dim == 1)
        {
            dzND->setData1D(params, ticks1, "R", allVals, units);
            data.push_back(dzND);
        }
        else if (dim == 2)
        {
            dzND->setData2D(params, ticks1, ticks2, "R", "PHI", allVals, units);
            data.push_back(dzND);
        }
        else
        {
            gtWarning() << "Higher order than 2 nod yet implemented.";
        }

    }

    GtDataZoneTable* dzt = new GtDataZoneTable(x, y, z, data);

    return dzt;
}
