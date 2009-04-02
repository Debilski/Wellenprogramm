/*
 * plot_layer.h
 *
 *  Created on: 02.04.2009
 *      Author: rikebs
 */

#ifndef PLOT_LAYER_H_
#define PLOT_LAYER_H_

#include <QtCore>

#include <qwt_plot.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>

#include "lattice_controller.h"

#include "color_map_adaptation.h"

class PlotLayer : public QObject{
    Q_OBJECT
public:
    uint component;

    void attach(QwtPlot* plot)
    {
        spectrogram_->attach( plot );
    }
    PlotLayer(LatticeController* latticeController) :
        latticeController_( latticeController )
    {
        spectrogram_ = new QwtPlotSpectrogram();
        adaptationMode_ = new DefaultColorMapAdaptationMode();
    }
    ~PlotLayer()
    {
        delete adaptationMode_;
        delete spectrogram_;
    }
    void adaptRange()
    {
        adaptationMode_->adaptRange(
            latticeController_->lattice()->getMin( component ),
            latticeController_->lattice()->getMax( component ) );
    }

    void setAdaptationMode(const ColorMapAdaptationMode& mode)
    {
        delete adaptationMode_;
        adaptationMode_ = mode.copy();
    }
    QwtDoubleInterval range() const
    {
        return adaptationMode_->range();
    }

    QwtPlotSpectrogram* spectrogram() const
    {
        return spectrogram_;
    }
public slots:
    void setColorMap(const QwtColorMap& colorMap, ColorMapAdaptationModes mode);
    void setColorMapMode(ColorMapAdaptationModes mode);
    void setColorMap(const QwtColorMap& colorMap);

private:
    LatticeController* latticeController_;
    QwtPlotSpectrogram* spectrogram_;
    ColorMapAdaptationMode* adaptationMode_;
};


#endif /* PLOT_LAYER_H_ */
