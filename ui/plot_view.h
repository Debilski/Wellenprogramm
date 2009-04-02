/*
 * plot_view.h
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#ifndef PLOT_VIEW_H_
#define PLOT_VIEW_H_

#include <QtCore>
#include <QtGui>

#include <qwt_plot.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_polygon.h>

#include "lattice_controller.h"
#include "plot_layer.h"
#include "color_map_adaptation.h"

class PlotStack {
public:
    QList< PlotLayer* > plotStack_;
    void attach(QwtPlot* plot)
    {
        foreach( PlotLayer* p, plotStack_ )
            {
                p->attach( plot );
            }
    }
    void append(PlotLayer* pL)
    {
        plotStack_.append( pL );
    }
    void adaptRange()
    {
        foreach( PlotLayer* p, plotStack_ )
            {
                p->adaptRange();
            }
    }
    QwtDoubleInterval range() const
    {
        return plotStack_.first()->range();
    }
};

/**
 * Class that will basically hold the spectrogram together.
 */
class PlotView : public QWidget {
Q_OBJECT
public:
    PlotStack plotStack_;

    //QwtLinearColorMap* colorMap_;
    //QwtAlphaColorMap* colorMapAlpha_;
    QwtPlot* plot_;
    QwtScaleWidget* rightAxis;

    PlotView(const PlotStack& plotStack, const QString label, QWidget* parent);

    QMenu rightClickMenu;
    ~PlotView();
    QwtPlotSpectrogram* firstSpectrogram()
    {
        return plotStack_.plotStack_.first()->spectrogram();
    }

public slots:
    void replot(int);
    void replot();
    void setColorMap(const QwtColorMap& colorMap, ColorMapAdaptationModes mode);
    void setColorMap(const QwtColorMap& colorMap);
    void setColorMapMode(ColorMapAdaptationModes mode);
    void showMenu(const QPoint& p);
    void changeTop();
    void changeBottom();
    void registerMouseEvent(const QwtDoublePoint &pos);
    void registerMouseEvent(const QwtArray< QwtDoublePoint > &pa);
signals:
    void selected(const uint&, const QPointF&);

};

#endif /* PLOT_VIEW_H_ */
