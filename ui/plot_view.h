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


#include <qwt_array.h>
typedef QPointF QwtDoublePoint;


#include "color_map_adaptation.h"

class PlotLayer;

class QwtPlot;
class QwtScaleWidget;
class QwtPlotSpectrogram;
class QwtColorMap;
class QwtDoubleInterval;

class PlotStack {
public:
    QList< PlotLayer* > plotStack_;
    void attach(QwtPlot* plot);
    void append(PlotLayer* pL);
    void adaptRange();
    QwtDoubleInterval range() const;
};

/**
 * Class that will basically hold the spectrogram together.
 */
class PlotView : public QWidget {
Q_OBJECT
public:
    PlotView(const PlotStack& plotStack, const QString label, QWidget* parent);
    ~PlotView();

    PlotStack plotStack_;

    QwtPlot* plot_;
    QwtScaleWidget* rightAxis;

    QMenu rightClickMenu;

    QwtPlotSpectrogram* firstSpectrogram();

public slots:
    void replot(int);
    void replot();
    void setColorMap(const QwtColorMap& colorMap, ColorMapAdaptationModes mode);
    void setColorMap(const QwtColorMap& colorMap);
    void setColorMapMode(ColorMapAdaptationModes mode);
    void showMenu(const QPoint& p);
    void changeTop();
    void changeBottom();
    void registerMouseEvent(const QwtDoublePoint& pos);
    void registerMouseEvent(const QwtArray< QwtDoublePoint >& pa);
signals:
    void selected(const uint&, const QPointF&);

};

#endif /* PLOT_VIEW_H_ */
