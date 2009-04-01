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

enum ColourMapAdaptationModes {
    defaultColourMapMode, adaptiveColourMapMode, delayedAdaptiveColourMapMode
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

class PlotStackLayer {
public:
    uint component;

    void attach(QwtPlot* plot)
    {
        spectrogram_->attach( plot );
    }
    PlotStackLayer(LatticeController* latticeController) :
        latticeController_( latticeController )
    {
        spectrogram_ = new QwtPlotSpectrogram();
        adaptationMode_ = new DefaultColorMapAdaptationMode();
    }
    ~PlotStackLayer()
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
    void setColorMap(const QwtColorMap& colorMap, ColourMapAdaptationModes mode)
    {
        switch (mode) {
            case adaptiveColourMapMode:
                setAdaptationMode( AdaptiveColorMapAdaptationMode() );
                break;
            case delayedAdaptiveColourMapMode:
                setAdaptationMode( DelayedAdaptiveColorMapAdaptationMode() );
                break;
            case defaultColourMapMode:
            default:
                double min =
                    latticeController_->lattice()->componentInfos[ component ].assumedMin();
                double max =
                    latticeController_->lattice()->componentInfos[ component ].assumedMax();
                setAdaptationMode( DefaultColorMapAdaptationMode( QwtDoubleInterval( min, max ) ) );
        }
        spectrogram()->setColorMap( colorMap );
    }
private:
    LatticeController* latticeController_;
    QwtPlotSpectrogram* spectrogram_;
    ColorMapAdaptationMode* adaptationMode_;
};

class PlotStack {
public:
    QList< PlotStackLayer* > plotStack_;
    void attach(QwtPlot* plot)
    {
        foreach( PlotStackLayer* p, plotStack_ )
            {
                p->attach( plot );
            }
    }
    void append(PlotStackLayer* pL)
    {
        plotStack_.append( pL );
    }
    void adaptRange()
    {
        foreach( PlotStackLayer* p, plotStack_ )
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
    void setColorMap(const QwtColorMap& colorMap, ColourMapAdaptationModes mode);
    void showMenu(const QPoint& p);
    void changeTop();
    void changeBottom();
    void registerMouseEvent(const QwtDoublePoint &pos);
    void registerMouseEvent(const QwtArray< QwtDoublePoint > &pa);
signals:
    void selected(const uint&, const QPointF&);

};

#endif /* PLOT_VIEW_H_ */
