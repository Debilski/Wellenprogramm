/*
 * plot_layer.cpp
 *
 *  Created on: 02.04.2009
 *      Author: rikebs
 */

#include "plot_layer.h"

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>

#include "lattice_controller.h"
#include "lattice_interface.h"

class PlotLayer::PrivateData {
public:
    PrivateData()
    {
        spectrogram = new QwtPlotSpectrogram();
        adaptationMode = new DefaultColorMapAdaptationMode();
    }
    ~PrivateData()
    {
        delete adaptationMode;
        delete spectrogram;
    }
    LatticeController* latticeController;
    QwtPlotSpectrogram* spectrogram;
    ColorMapAdaptationMode* adaptationMode;
};

PlotLayer::PlotLayer(LatticeController* latticeController)
{
    d_data = new PrivateData();
    d_data->latticeController = latticeController;
}

PlotLayer::~PlotLayer()
{
    delete d_data;
}

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
            double min =
                d_data->latticeController->lattice()->componentInfos[ component ].assumedMin();
            double max =
                d_data->latticeController->lattice()->componentInfos[ component ].assumedMax();
            setAdaptationMode( DefaultColorMapAdaptationMode( QwtDoubleInterval( min, max ) ) );
    }
}

void PlotLayer::setColorMap(const QwtColorMap& colorMap)
{
    spectrogram()->setColorMap( colorMap );
}

void PlotLayer::attach(QwtPlot* plot)
{
    d_data->spectrogram->attach( plot );
}

void PlotLayer::adaptRange()
{
    d_data->adaptationMode->adaptRange(
        d_data->latticeController->lattice()->getMin( component ),
        d_data->latticeController->lattice()->getMax( component ) );
}

void PlotLayer::setAdaptationMode(const ColorMapAdaptationMode& mode)
{
    delete d_data->adaptationMode;
    d_data->adaptationMode = mode.copy();
}

ColorMapAdaptationMode& PlotLayer::adaptionMode()
{
    return *(d_data->adaptationMode);
}

QwtDoubleInterval PlotLayer::range() const
{
    return d_data->adaptationMode->range();
}

QwtPlotSpectrogram* PlotLayer::spectrogram() const
{
    return d_data->spectrogram;
}
