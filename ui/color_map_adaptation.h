/*
 * color_map_adaptation.h
 *
 *  Created on: 02.04.2009
 *      Author: rikebs
 */

#ifndef COLOUR_MAP_ADAPTATION_H_
#define COLOUR_MAP_ADAPTATION_H_


enum ColorMapAdaptationModes {
    defaultColorMapMode, adaptiveColorMapMode, delayedAdaptiveColorMapMode
};

class ColorMapAdaptationMode {
public:
    virtual ~ColorMapAdaptationMode()
    {
    }
    ;
    virtual void adaptRange(double min, double max) = 0;
    QwtDoubleInterval range()
    {
        return interval_;
    }
    virtual ColorMapAdaptationMode* copy() const = 0;
protected:
    QwtDoubleInterval interval_;
};

class DefaultColorMapAdaptationMode : public ColorMapAdaptationMode {
public:
    DefaultColorMapAdaptationMode()
    {
        setInterval( QwtDoubleInterval( -2.2, 2.5 ) );
    }
    DefaultColorMapAdaptationMode(QwtDoubleInterval interval)
    {
        setInterval( interval );
    }
    DefaultColorMapAdaptationMode* copy() const
    {
        return new DefaultColorMapAdaptationMode( interval_ );
    }
    void setInterval(QwtDoubleInterval interval)
    {
        interval_ = interval;
        if ( !interval_.isValid() || interval_.isNull() ) {
            interval_ = QwtDoubleInterval( -2.2, 2.5 );
        }
    }
    void adaptRange(double /*min*/, double /*max*/)
    {
    }
};

class AdaptiveColorMapAdaptationMode : public ColorMapAdaptationMode {
    AdaptiveColorMapAdaptationMode* copy() const
    {
        return new AdaptiveColorMapAdaptationMode();
    }
    void adaptRange(double min, double max)
    {
        max = std::floor( max * 5.0 + 1 ) / 5.0;
        min = std::ceil( min * 5.0 - 1 ) / 5.0;
        interval_ = QwtDoubleInterval( min, max );
    }
};

class DelayedAdaptiveColorMapAdaptationMode : public ColorMapAdaptationMode {
    DelayedAdaptiveColorMapAdaptationMode* copy() const
    {
        return new DelayedAdaptiveColorMapAdaptationMode();
    }
    void adaptRange(double min, double max)
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
};



#endif /* COLOUR_MAP_ADAPTATION_H_ */
