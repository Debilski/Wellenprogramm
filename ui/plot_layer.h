/*
 * plot_layer.h
 *
 *  Created on: 02.04.2009
 *      Author: rikebs
 */

#ifndef PLOT_LAYER_H_
#define PLOT_LAYER_H_

#include <QtCore>

#include "color_map_adaptation.h"

class QwtPlot;
class QwtPlotSpectrogram;
class QwtColorMap;

class LatticeController;

class PlotLayer : public QObject {
Q_OBJECT
public:
    uint component;

    PlotLayer(LatticeController* latticeController);
    ~PlotLayer();

    void attach(QwtPlot* plot);

    void adaptRange();

    void setAdaptationMode(const ColorMapAdaptationMode& mode);
    ColorMapAdaptationMode& adaptionMode();

    QwtDoubleInterval range() const;

    QwtPlotSpectrogram* spectrogram() const;
public slots:
    void setColorMap(const QwtColorMap& colorMap, ColorMapAdaptationModes mode);
    void setColorMapMode(ColorMapAdaptationModes mode);
    void setColorMap(const QwtColorMap& colorMap);

private:
    class PrivateData;
    PrivateData* d_data;
};

#endif /* PLOT_LAYER_H_ */
