/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * color_maps.cpp
 *
 *  Created on: 02.04.2009
 *      Author: rikebs
 */

#include "color_maps.h"

#include "qwt_array.h"
#include "qwt_math.h"
#include "qwt_double_interval.h"
#include "qwt_color_map.h"


class LinearTransparentColorMap::ColorStops
{
public:
    ColorStops()
    {
#if QT_VERSION >= 0x040000
        _stops.reserve(256);
#endif
    }

    void insert(double pos, const QColor& color);
    QRgb rgb(LinearTransparentColorMap::Mode, double pos) const;

    QwtArray<double> stops() const;

private:
    class ColorStop
    {
    public:
        ColorStop() : pos(0.0),
                      rgb(0){};

        ColorStop(double p, const QColor& c) : pos(p),
                                               rgb(c.rgb())
        {
            r = qRed(rgb);
            g = qGreen(rgb);
            b = qBlue(rgb);
        }

        double pos;
        QRgb rgb;
        int r, g, b;
    };

    inline int findUpper(double pos) const;
    QwtArray<ColorStop> _stops;
};

void LinearTransparentColorMap::ColorStops::insert(double pos, const QColor& color)
{
    // Lookups need to be very fast, insertions are not so important.
    // Anyway, a balanced tree is what we need here. TODO ...

    if (pos < 0.0 || pos > 1.0)
        return;

    int index;
    if (_stops.size() == 0) {
        index = 0;
#if QT_VERSION < 0x040000
        _stops.resize(1, QGArray::SpeedOptim);
#else
        _stops.resize(1);
#endif
    } else {
        index = findUpper(pos);
        if (index == (int)_stops.size() || qwtAbs(_stops[index].pos - pos) >= 0.001) {
#if QT_VERSION < 0x040000
            _stops.resize(_stops.size() + 1, QGArray::SpeedOptim);
#else
            _stops.resize(_stops.size() + 1);
#endif
            for (int i = _stops.size() - 1; i > index; i--)
                _stops[i] = _stops[i - 1];
        }
    }

    _stops[index] = ColorStop(pos, color);
}

inline QwtArray<double> LinearTransparentColorMap::ColorStops::stops() const
{
    QwtArray<double> positions(_stops.size());
    for (int i = 0; i < (int)_stops.size(); i++)
        positions[i] = _stops[i].pos;
    return positions;
}

inline int LinearTransparentColorMap::ColorStops::findUpper(double pos) const
{
    int index = 0;
    int n = _stops.size();

    const ColorStop* stops = _stops.data();

    while (n > 0) {
        const int half = n >> 1;
        const int middle = index + half;

        if (stops[middle].pos <= pos) {
            index = middle + 1;
            n -= half + 1;
        } else
            n = half;
    }

    return index;
}

inline QRgb LinearTransparentColorMap::ColorStops::rgb(
    LinearTransparentColorMap::Mode mode, double pos) const
{
    if (pos <= 0.0)
        return _stops[0].rgb;
    if (pos >= 1.0)
        return _stops[(int)(_stops.size() - 1)].rgb;

    const int index = findUpper(pos);
    if (mode == FixedColors) {
        return _stops[index - 1].rgb;
    } else {
        const ColorStop& s1 = _stops[index - 1];
        const ColorStop& s2 = _stops[index];

        const double ratio = (pos - s1.pos) / (s2.pos - s1.pos);

        const int r = s1.r + qRound(ratio * (s2.r - s1.r));
        const int g = s1.g + qRound(ratio * (s2.g - s1.g));
        const int b = s1.b + qRound(ratio * (s2.b - s1.b));

        return qRgb(r, g, b);
    }
}

class LinearTransparentColorMap::PrivateData
{
public:
    ColorStops colorStops;
    LinearTransparentColorMap::Mode mode;
};

/*!
   Build a color map with two stops at 0.0 and 1.0. The color
   at 0.0 is Qt::blue, at 1.0 it is Qt::yellow.

   \param format Preferred format of the color map
*/
LinearTransparentColorMap::LinearTransparentColorMap(QwtColorMap::Format format) : QwtColorMap(format)
{
    d_data = new PrivateData;
    d_data->mode = ScaledColors;

    setColorInterval(Qt::blue, Qt::yellow);
}

//! Copy constructor
LinearTransparentColorMap::LinearTransparentColorMap(const LinearTransparentColorMap& other) : QwtColorMap(other)
{
    d_data = new PrivateData;
    *this = other;
}

/*!
   Build a color map with two stops at 0.0 and 1.0.

   \param color1 Color used for the minimum value of the value interval
   \param color2 Color used for the maximum value of the value interval
   \param format Preferred format of the coor map
*/
LinearTransparentColorMap::LinearTransparentColorMap(const QColor& color1,
    const QColor& color2, QwtColorMap::Format format) : QwtColorMap(format)
{
    d_data = new PrivateData;
    d_data->mode = ScaledColors;
    setColorInterval(color1, color2);
}

//! Destructor
LinearTransparentColorMap::~LinearTransparentColorMap()
{
    delete d_data;
}

//! Assignment operator
LinearTransparentColorMap& LinearTransparentColorMap::operator=(
    const LinearTransparentColorMap& other)
{
    QwtColorMap::operator=(other);
    *d_data = *other.d_data;
    return *this;
}

//! Clone the color map
QwtColorMap* LinearTransparentColorMap::copy() const
{
    LinearTransparentColorMap* map = new LinearTransparentColorMap();
    *map = *this;

    return map;
}

/*!
   \brief Set the mode of the color map

   FixedColors means the color is calculated from the next lower
   color stop. ScaledColors means the color is calculated
   by interpolating the colors of the adjacent stops.

   \sa mode()
*/
void LinearTransparentColorMap::setMode(Mode mode)
{
    d_data->mode = mode;
}

/*!
   \return Mode of the color map
   \sa setMode()
*/
LinearTransparentColorMap::Mode LinearTransparentColorMap::mode() const
{
    return d_data->mode;
}

/*!
   Set the color range

   Add stops at 0.0 and 1.0.

   \param color1 Color used for the minimum value of the value interval
   \param color2 Color used for the maximum value of the value interval

   \sa color1(), color2()
*/
void LinearTransparentColorMap::setColorInterval(
    const QColor& color1, const QColor& color2)
{
    d_data->colorStops = ColorStops();
    d_data->colorStops.insert(0.0, color1);
    d_data->colorStops.insert(1.0, color2);
}

/*!
   Add a color stop

   The value has to be in the range [0.0, 1.0].
   F.e. a stop at position 17.0 for a range [10.0,20.0] must be
   passed as: (17.0 - 10.0) / (20.0 - 10.0)

   \param value Value between [0.0, 1.0]
   \param color Color stop
*/
void LinearTransparentColorMap::addColorStop(double value, const QColor& color)
{
    if (value >= 0.0 && value <= 1.0)
        d_data->colorStops.insert(value, color);
}

/*!
   Return all positions of color stops in increasing order
*/
QwtArray<double> LinearTransparentColorMap::colorStops() const
{
    return d_data->colorStops.stops();
}

/*!
  \return the first color of the color range
  \sa setColorInterval()
*/
QColor LinearTransparentColorMap::color1() const
{
    return QColor(d_data->colorStops.rgb(d_data->mode, 0.0));
}

/*!
  \return the second color of the color range
  \sa setColorInterval()
*/
QColor LinearTransparentColorMap::color2() const
{
    return QColor(d_data->colorStops.rgb(d_data->mode, 1.0));
}

/*!
  Map a value of a given interval into a rgb value

  \param interval Range for all values
  \param value Value to map into a rgb value
*/
QRgb LinearTransparentColorMap::rgb(
    const QwtDoubleInterval& interval, double value) const
{
    const double width = interval.width();

    double ratio = 0.0;
    if (width > 0.0)
        ratio = (value - interval.minValue()) / width;
    if (ratio < 0)
        return qRgba(0, 0, 0, 0);
    return d_data->colorStops.rgb(d_data->mode, ratio);
}

/*!
  Map a value of a given interval into a color index, between 0 and 255

  \param interval Range for all values
  \param value Value to map into a color index
*/
unsigned char LinearTransparentColorMap::colorIndex(
    const QwtDoubleInterval& interval, double value) const
{
    const double width = interval.width();

    if (width <= 0.0 || value <= interval.minValue())
        return 0;

    if (value >= interval.maxValue())
        return (unsigned char)255;

    const double ratio = (value - interval.minValue()) / width;

    unsigned char index;
    if (d_data->mode == FixedColors)
        index = (unsigned char)(ratio * 255);  // always floor
    else
        index = (unsigned char)qRound(ratio * 255);

    return index;
}
