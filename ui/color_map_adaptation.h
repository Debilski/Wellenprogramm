/*
 * color_map_adaptation.h
 *
 *  Created on: 02.04.2009
 *      Author: rikebs
 */

#ifndef COLOUR_MAP_ADAPTATION_H_
#define COLOUR_MAP_ADAPTATION_H_

#include "qwt_double_interval.h"

enum ColorMapAdaptationModes {
    defaultColorMapMode, adaptiveColorMapMode, delayedAdaptiveColorMapMode
};

class ColorMapAdaptationMode {
public:
    virtual ~ColorMapAdaptationMode() = 0
    ;
    virtual void adaptRange(double min, double max) = 0;
    QwtDoubleInterval range();
    virtual ColorMapAdaptationMode* copy() const = 0;
protected:
    QwtDoubleInterval interval_;
};

class DefaultColorMapAdaptationMode : public ColorMapAdaptationMode {
public:
    DefaultColorMapAdaptationMode();

    DefaultColorMapAdaptationMode(QwtDoubleInterval interval);

    DefaultColorMapAdaptationMode* copy() const;

    void setInterval(QwtDoubleInterval interval);
    void adaptRange(double /*min*/, double /*max*/);
};

class AdaptiveColorMapAdaptationMode : public ColorMapAdaptationMode {
    AdaptiveColorMapAdaptationMode* copy() const;

    void adaptRange(double min, double max);
};

class DelayedAdaptiveColorMapAdaptationMode : public ColorMapAdaptationMode {
    DelayedAdaptiveColorMapAdaptationMode* copy() const;

    void adaptRange(double min, double max);
};



#endif /* COLOUR_MAP_ADAPTATION_H_ */
