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

/**
 * Class that holds Spectrogram, ColorMap and Plot for each tab.
 */
class PlotView : public QWidget {
Q_OBJECT
public:
    QwtPlotSpectrogram* spectrogram_;
    QwtLinearColorMap* colorMap_;
    QwtPlot* plot_;
    QwtScaleWidget* rightAxis;

    PlotView(const QwtRasterData& spectrogrammData, const QwtColorMap& colorMap, uint component,
             const QString label, QWidget* parent, bool isFft = false);
    QMenu rightClickMenu;
    ~PlotView();signals:
    void adaptComponent(uint& component, bool& isFft);

public slots:
    void replot(int);
    void replot();
    void setColorMap(const QwtColorMap& colorMap);
    void showMenu(const QPoint& p);
    void changeTop();
    void changeBottom();
    void registerMouseEvent(const QwtDoublePoint &pos);
    void registerMouseEvent(const QwtArray< QwtDoublePoint > &pa);
    signals:
    void selected(const uint&, const QPointF& );
private:
    unsigned int component_;
    bool isFft_;

};

#endif /* PLOT_VIEW_H_ */
