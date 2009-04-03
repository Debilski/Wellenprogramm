/*
 * color_map_adaptation.cpp
 *
 *  Created on: 03.04.2009
 *      Author: rikebs
 */

#include "color_map_adaptation.h"

#include <cmath>

ColorMapAdaptationMode::~ColorMapAdaptationMode()
{
}

QwtDoubleInterval ColorMapAdaptationMode::range()
{
    return interval_;
}

DefaultColorMapAdaptationMode::DefaultColorMapAdaptationMode()
{
    setInterval( QwtDoubleInterval( -2.2, 2.5 ) );
}

DefaultColorMapAdaptationMode::DefaultColorMapAdaptationMode(QwtDoubleInterval interval)
{
    setInterval( interval );
}

DefaultColorMapAdaptationMode* DefaultColorMapAdaptationMode::copy() const
{
    return new DefaultColorMapAdaptationMode( interval_ );
}

void DefaultColorMapAdaptationMode::setInterval(QwtDoubleInterval interval)
{
    interval_ = interval;
    if ( !interval_.isValid() || interval_.isNull() ) {
        interval_ = QwtDoubleInterval( -2.2, 2.5 );
    }
}

void DefaultColorMapAdaptationMode::adaptRange(double /*min*/, double /*max*/)
{
}

AdaptiveColorMapAdaptationMode* AdaptiveColorMapAdaptationMode::copy() const
{
    return new AdaptiveColorMapAdaptationMode();
}

void AdaptiveColorMapAdaptationMode::adaptRange(double min, double max)
{
    max = std::floor( max * 5.0 + 1 ) / 5.0;
    min = std::ceil( min * 5.0 - 1 ) / 5.0;
    interval_ = QwtDoubleInterval( min, max );
}

DelayedAdaptiveColorMapAdaptationMode* DelayedAdaptiveColorMapAdaptationMode::copy() const
{
    return new DelayedAdaptiveColorMapAdaptationMode();
}

void DelayedAdaptiveColorMapAdaptationMode::adaptRange(double min, double max)
{
    double oldMax = interval_.maxValue();
    double oldMin = interval_.minValue();

    // Neuen Wert abschätzen
    // Wenn Abweichung nicht zu größer 1: alten Wert behalten, sonst auf 0.2 runden
    if ( max < oldMax && max > oldMax - 1 ) {
        max = oldMax;
    } else {
        max = std::floor( max * 5.0 + 1 ) / 5.0;
    }
    if ( min > oldMin && min < oldMin + 1 ) {
        min = oldMin;
    } else {
        min = std::ceil( min * 5.0 - 1 ) / 5.0;
    }
    interval_ = QwtDoubleInterval( min, max );
}