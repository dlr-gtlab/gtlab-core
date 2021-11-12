#ifndef GT_TABLE_H
#define GT_TABLE_H

#include <QMap>
#include <QPair>
#include <QString>
#include <QStringList>

#include "gt_exceptions.h"
#include "gt_extrapolators.h"
#include "gt_interpfuncs.h"
#include "gt_interpolators.h"
#include "gt_tableaxis.h"
#include "gt_tablescalers.h"
#include "gt_tablevalues.h"
#include "gt_datamodel_exports.h"
#include "gt_object.h"
#include "gt_logging.h"

// temp TODO remove
#include <QDomElement>

class GT_DATAMODEL_EXPORT GtTable : public GtObject
{
    Q_OBJECT
    Q_ENUMS(GridType)

    Q_PROPERTY(QString unit READ getUnit WRITE setUnit)
    Q_PROPERTY(QString description READ getDescription WRITE setDescription)
    Q_PROPERTY(GridType type READ type WRITE setType)

public:
    /** Contructor */
    Q_INVOKABLE GtTable();

    /**
     * @brief The Type enum: gridless enables non-ascending increase of
     * GRIDLESS is necessary for stage maps in multi stage compressor
     */
    enum GridType
    {
        DEFAULT = 0,
        GRIDLESS = 1
    };

    /** Adds an axis represented by a vector containing its ticks in ascending
        order to the table.
        @param id Axis name
        @param desc Axis description
        @param unit Axis unit
        @param loExtMethod Extrapolation method at lower end
        @param intMethod Interpolation method
        @param hiExtMethod Extrapolation method at higher end
        @param ticks Reference to vector of ticks */
    bool addAxis(const QString& id,
                 const QString desc,
                 const QString& unit,
                 GtTableAxis::ExtrapMethod loExtMethod,
                 GtTableAxis::InterpMethod intMethod,
                 GtTableAxis::ExtrapMethod hiExtMethod,
                 const QVector<double>& ticks);

    /**
     * @brief addAxis add axis to table
     * @param ax gttableaxis
     * @return success
     */
    bool addAxis(GtTableAxis* ax);

    /**
     * @brief Adds values to the n-dimensional table. The size of the vals vector
     *  must be equal to the product of all axis sizes. Otherwise an
     *  exception is thrown.
     * @param vals Reference to vector of values
     * @return success
     */
    bool addValues(const QString& id, const QString& desc,
                   const QString& unit, const QVector<double>& vals);

    /**
     * @brief Adds values to the n-dimensional table. The size of the vals
     *  vector must be equal to the product of all axis sizes. Otherwise false
     *  will be returned
     * @param tv gttablevalues
     * @return success
     */
    bool addValues(GtTableValues* tv);

    /** Returns an axis adressed by its index (0 for 1st dimension)
        @param axisIndex Dimension index starting with 0 for 1st dimension */
    GtNumerics::darray getAxisTicks(int axisIndex) const;

    /** Writes axis values to output vector.
        @param name Name of axis
        @param dvecOut Reference to vector<double> output vector */
    void getAxisTicks(const QString& name, QVector<double>& dvecOut);

    /**
     * @brief axesList Returns a list of all axes.
     * @return List of all axes.
     */
    QList<GtTableAxis*> getAxesList() const;

    /** Returns the names of the provided axes
        @return List of axes names. */
    QStringList getAxesNames() const;

    /** Computes the size of an axis
        @param axisIndex Index of axis
        @return Size of axis */
    int getAxisSize(int axisIndex) const;

    /** Returns the description of the table.
        @return table description */
    QString getDescription() const;

    /** Computes the size of a table.
        @return size of table */
    int getTableSize() const;

    /** Returns the unit of the table.
        @return table unit */
    QString getUnit() const;

    /**
     * @brief getVals
     * @param id
     * @return
     */
    GtTableValues* getVals(const QString id);

    /** Returns an inter/extrapolated table value. If extrapolation is not
        permitted and requested data point is outside regular grid an
        Exception is thrown.
        @param coords Vector of size n equal to number of table dimensions
        containing the coordinates of the data point to be interpolated
        @return Table value*/
    double getValue(const QString& valsId, const QVector<double>& coords);

    /** Returns an inter/extrapolated value from a 1D-table.
        @param x0 Value on axis 0
        @return Table value */
    double getValue1D(const QString& valsId, double x0);

    /** Returns an inter/extrapolated value from a 2D-table.
        @param valsId Values identification string
        @param x0 value on axis 0
        @param x1 value on axis 1
        @return Table value */
    double getValue2D(const QString& valsId, double x0, double x1) const;

    /** Returns a value from a 2D-table at specific axis indices
     * @brief getValue2D
     * @param valsId
     * @param axisIndex1
     * @param axisIndex2
     * @return
     */
    double getValue2DatIndices(const QString& valsId, int axisIndex1,
                               int axisIndex2, bool* check = Q_NULLPTR) const;

    /**
     * @brief getValue2Dgridless: xn and phi axis interpolation
     * Necessary for stage maps in multistage compressor
     * @param valsId
     * @param x0
     * @param x1
     * @return
     */
    double getValue2Dgridless(const QString& valsId,
                              double x0, double x1) const;

    /** Returns an inter/extrapolated value from a 3D-table.
        @param x0 value on axis 0
        @param x1 value on axis 1
        @param x2 value on axis 2
        @return Table value */
    double getValue3D(const QString& valsId, double x0, double x1,
                      double x2) const;

    /** Returns an inter/extrapolated value from a 4D-table.
        @param x0 value on axis 0
        @param x1 value on axis 1
        @param x2 value on axis 2
        @param x3 value on axis 3
        @return Table value */
    double getValue4D(const QString& valsId, double x0, double x1, double x2,
                      double x3) const;

    /**
     * @brief getValue5D returns an inter/extrapolated value from a 5D-table
     * @param valsId value identifier (in case of several values)
     * @param x0 value on axis 0
     * @param x1 value on axis 1
     * @param x2 value on axis 2
     * @param x3 value on axis 3
     * @param x4 value on axis 4
     * @return Table value
     */
    double getValue5D(const QString& valsId, double x0, double x1, double x2,
                      double x3, double x4) const;

    /** Scales the values of an axis by a given factor.
        @param axisName Name of axis to be scaled
        @param scalingFactor Scaling factor */
    void scaleAxis(const QString& axisName, const double scalingFactor);

    /** Scales the table values by given factor.
        @param scalingFactor Scaling factor */
    void scaleValues(const QString& valsId, const double scalingFactor);

    template <class F>
    void scaleValuesByFunction(const QString& valsId, F& functionObject);

    /** Sets the table description.
        @param desc Table description */
    void setDescription(const QString& desc);

    /** Sets the table name.
        @param name Table name */
    void setName(const QString& name);

    /** Sets the table unit.
        @param unit Table unit */
    void setUnit(const QString& unit);

    /**
     * @brief nDims
     * @return
     */
    int nDims() const;

    /**
     * @brief dimensions returns a vector where the length is equal to nDims and
     * each entry contains the number of ticks for the axis
     * @return
     */
    QVector<int> dimensions() const;

    /**
     * @brief tabVals
     * @param valsId
     * @return
     */
    GtNumerics::darray tabVals(const QString& valsId) const;

    /**
     * @brief valDescription
     * @param valsId
     * @return
     */
    QString valDescription(const QString& valsId) const;

    /**
     * @brief valUnit
     * @param valsId
     * @return
     */
    QString valUnit(const QString& valsId) const;

    /** Returns all keys of the tabVal Map
     * @brief tabValsKeys
     * @return
     */
    QList<QString> tabValsKeys();

    /**
     * @brief setTabValsKeysSuffix
     * @param keys
     */
    void setTabValsKeysSuffix(QString keys);

    /**
     * @brief toDomElement
     * @param doc
     * @param valId: optional
     * @return
     */
    QDomElement toDomElement(QDomDocument& doc, QString valId = QString());

    /**
     * @brief clear
     *  Added by JS and not tested yet
     * @return
     */
    void clear();

    /**
     * @brief type
     * @return
     */
    GridType type() const;

    /**
     * @brief setType
     * @param type
     */
    void setType(const GridType& type);

    /**
     * @brief getMax
     * @param id
     * @return
     */
    double getMax(QString id, bool* check = Q_NULLPTR);

    /**
     * @brief getMin
     * @param id
     * @return
     */
    double getMin(QString id, bool* check = Q_NULLPTR);

    /**
     * @brief valueNames
     * @return
     */
    QStringList valueNames();

    /**
     * @brief create a String from InterpolationMethode (Enum GtTableAxis)
     * @return String for xml output
     */
    QString interExtraPolationString(GtTableAxis::InterpMethod methode);

    /**
     * @brief create a String from ExtrapolationMethode (Enum GtTableAxis)
     * @return String for xml output
     */
    QString interExtraPolationString(GtTableAxis::ExtrapMethod methode);

private:
    /// type of table
    GridType m_type;

    /// Name of table
    QString m_unit;

    /// Table description
    QString m_description;

    /// Map containing tabulated function values
    QMap<QString, GtTableValues*> m_tabVals;

    /** Adds the axis name and index to the internal data structure.
        @param axisName Name of axis
        @param axisIndex Index of axis */
    bool checkAxisName(const QString& axisName);

    /** Approximates the function in one dimension which given by a set of
        discrete table values at a certain point.
        @param axisIndex Index of axis direction
        @param indices Indices of slice location
        @param coords Point coordinates
        @return Value of approximated function */
    double approxDimension(const QString& valsId, const int axisIndex,
                           QVector<int>& indices,
                           const QVector<double>& coords) const;

    /** Calculates the start index for a 1d-slice.
        @param axisIndex Index of axis direction
        @param indices Indices of slice location
        @return Start index */
    int calcStart(const int axisIndex, const QVector<int>& indices) const;

    /** Calculates the stride for a 1d-slice.
        @param axisIndex index of axis direction */
    int calcStride(const int axisIndex) const;

    /** Creates a 1-dimensional valarray slice in direction of specified axis.
        @param axisIndex Index of axis direction
        @param indices Indices of slice location
        @return 1d slice */
    GtNumerics::darray getSlice(const QString& valsId, int axisIndex,
                                const QVector<int>& indices);

    /** Creates a 1-dimensional valarray slice in direction of specified axis.
        @param axisName Name of axis direction
        @param indices Indices of slice location
        @return 1d slice */
    GtNumerics::darray getSlice(const QString& axisName,
                                const QVector<int>& indices);

    /** returns a 1-dimensional slice in direction of specified axis.
        @param axisIndex Index of axis direction
        @param indices Indices of slice location
        @return 1d slice */
    QVector<double> getValSlice(const QString& valsId, int axisIndex,
                                const QVector<int>& indices) const;

    /** Grows an array by n elements. Already existing elements are
        preserved.
        @param v Reference to array
        @param n Number of elements to be additionally allocated */
    void growArrayByN(QVector<double>& v, int n);

    /** Checks wether tick values in double vector are in ascending order
        @param ticks Vector of tick values
        @return true Is ticks are in ascending order */
    bool isAscending(const QVector<double>& ticks);

    /**
     * @brief slice Generates a slice or subset of a linear array,
     * @param in Reference to input array to be sliced.
     * @param start Position of the first element.
     * @param size Number of elements in the slice.
     * @param stride Number of positions between successive elements in the
     * slice.
     * @return Generated slice
     */
    QVector<double> slice(const QString& valsId, int start, int size,
                          int stride) const;

    /**
     * @brief axisIndex
     * @param axisName
     * @return
     */
    int axisIndex(const QString& axisName);

    /**
     * @brief onObjectDataMerged
     */
    virtual void onObjectDataMerged();

    /**
     * @brief phiInterpolatedVal: Necessary for stage maps in multistage compr
     * @param xn
     * @param phi
     * @param xx0
     * @param xx1
     * @return
     */
    double phiInterpolatedVal(const QString& valsId,
                              double xn,
                              double phi,
                              const GtNumerics::darray& xx0,
                              const GtNumerics::darray& xx1) const;

    /**
     * @brief getSuitedSlice: Necessary for stage maps in multistage compr
     * @param xnlow
     * @param xx0
     * @param vals
     * @param indxfirst
     * @return
     */
    GtNumerics::darray getSuitedSlice(double xnlow,
                                      const GtNumerics::darray& xx0,
                                      const GtNumerics::darray& vals,
                                      int& indxfirst) const;

protected:

    // 02.08.2016 needed to clear table GtControlSchedule
    /**
     * @brief delete table values
     */
    void deleteTabVals();
};


inline int
GtTable::nDims() const
{
    return getAxesList().size();
}


inline int GtTable::getAxisSize(int axisIndex) const
{
    QList<GtTableAxis*> axList = getAxesList();

    if (axList.size() < axisIndex + 1)
    {
        gtError() << tr("Error: Requested axis index: ") << axisIndex
                  << tr(" exceeds number of axis entries: ") << axList.size();
        return 0;
    }

    return axList.at(axisIndex)->size();
}


template <class F> void
GtTable::scaleValuesByFunction(const QString& valsId, F& functionObject)
{
    m_tabVals[valsId]->scaleValuesByFunction(functionObject);
}


#endif // GT_TABLE_H
