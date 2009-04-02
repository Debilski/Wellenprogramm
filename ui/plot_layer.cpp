/*
 * plot_layer.cpp
 *
 *  Created on: 02.04.2009
 *      Author: rikebs
 */

#include "plot_layer.h"

void PlotLayer::setColorMap(const QwtColorMap& colorMap, ColorMapAdaptationModes mode)
{
    setColorMapMode( mode );
    setColorMap( colorMap );
}

void PlotLayer::setColorMapMode(ColorMapAdaptationModes mode)
{
    switch (mode) {
        case adaptiveColorMapMode:
            setAdaptationMode( AdaptiveColorMapAdaptationMode() );
            break;
        case delayedAdaptiveColorMapMode:
            setAdaptationMode( DelayedAdaptiveColorMapAdaptationMode() );
            break;
        case defaultColorMapMode:
        default:
            double min = latticeController_->lattice()->componentInfos[ component ].assumedMin();
            double max = latticeController_->lattice()->componentInfos[ component ].assumedMax();
            setAdaptationMode( DefaultColorMapAdaptationMode( QwtDoubleInterval( min, max ) ) );
    }
}

void PlotLayer::setColorMap(const QwtColorMap& colorMap)
{
    spectrogram()->setColorMap( colorMap );
}
