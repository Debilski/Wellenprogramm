/*
 * plot_view.cpp
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#include "plot_view.h"

#include <qwt_plot.h>
#include <qwt_plot_layout.h>
#include <qwt_plot_spectrogram.h>
#include <qwt_color_map.h>
#include <qwt_scale_widget.h>
#include <qwt_polygon.h>
#include <qwt_plot_picker.h>

#include "tiny_double_edit.h"

#include "lattice_controller.h"
#include "plot_layer.h"

void PlotStack::attach(QwtPlot* plot)
{
    foreach( PlotLayer* p, plotStack_ )
        {
            p->attach( plot );
        }
}

void PlotStack::append(PlotLayer* pL)
{
    plotStack_.append( pL );
}

void PlotStack::adaptRange()
{
    foreach( PlotLayer* p, plotStack_ )
        {
            p->adaptRange();
        }
}

QwtDoubleInterval PlotStack::range() const
{
    return plotStack_.first()->range();
}

class Painter : public QwtPlotPicker {
public:
    Painter(int xAxis, int yAxis, QwtPlotCanvas *canvas) :
        QwtPlotPicker( xAxis, yAxis, canvas )
    {
        setSelectionFlags( QwtPicker::PointSelection | QwtPicker::ClickSelection );

        setTrackerMode( QwtPicker::ActiveOnly );
        setRubberBand( QwtPicker::NoRubberBand );
    }
};

/**
 * \class PlotView
 * \brief Class that holds Spectrogram, ColorMap and Plot for each tab.
 *
 */

PlotView::PlotView(const PlotStack& plotStack, const QString label, QWidget* parent) :
    plotStack_( plotStack ), QWidget( parent )
{
    QFont plotFont = QFont( "", 8 );
    QwtText labelIntensity( label );
    labelIntensity.setFont( plotFont );

    QHBoxLayout* horizontalLayout = new QHBoxLayout( this );
    plot_ = new QwtPlot( this );

    plot_->setAxisFont( QwtPlot::xBottom, plotFont );
    plot_->setAxisFont( QwtPlot::yLeft, plotFont );

    QRectF dimensions = plotStack_.plotStack_.first()->spectrogram()->boundingRect();
    plot_->setAxisScale( QwtPlot::yLeft, dimensions.top(), dimensions.bottom() );
    plot_->setAxisScale( QwtPlot::xBottom, dimensions.left(), dimensions.right() );

    horizontalLayout->addWidget( plot_ );

    plotStack_.attach( plot_ );

    Painter* painter = new Painter( QwtPlot::xBottom, QwtPlot::yLeft, plot_->canvas() );
    painter->setTrackerPen( QColor( Qt::white ) );

    painter->setEnabled( true );

    connect(
        painter, SIGNAL(selected(const QwtDoublePoint &)), this,
        SLOT(registerMouseEvent(const QwtDoublePoint&)) );

    rightAxis = plot_->axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( labelIntensity );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorMap(
        plotStack_.plotStack_.first()->spectrogram()->data().range(),
        plotStack_.plotStack_.first()->spectrogram()->colorMap() );

    plot_->setAxisScale(
        QwtPlot::yRight, plotStack_.plotStack_.first()->spectrogram()->data().range().minValue(),
        plotStack_.plotStack_.first()->spectrogram()->data().range().maxValue() );
    plot_->enableAxis( QwtPlot::yRight );
    plot_->setAxisFont( QwtPlot::yRight, plotFont );
    plot_->plotLayout()->setAlignCanvasToScales( true );

    rightClickMenu.addAction(
        QString( "Top Value: %1" ).arg(
            plotStack_.plotStack_.first()->spectrogram()->data().range().maxValue() ), this,
        SLOT(changeTop()) );
    rightClickMenu.addAction(
        QString( "Bottom Value: %1" ).arg(
            plotStack_.plotStack_.first()->spectrogram()->data().range().minValue() ), this,
        SLOT(changeBottom()) );

    rightAxis->setContextMenuPolicy( Qt::CustomContextMenu );
    connect(
        rightAxis, SIGNAL( customContextMenuRequested( const QPoint& ) ), this,
        SLOT( showMenu( const QPoint& ) ) );
}

void PlotView::registerMouseEvent(const QwtDoublePoint & p)
{
    emit selected( plotStack_.plotStack_.first()->component, p );
}

void PlotView::registerMouseEvent(const QwtArray< QwtDoublePoint > &pa)
{
    qDebug() << pa;
}

PlotView::~PlotView()
{
    //delete spectrogram();
}

void PlotView::replot(int)
{
    replot();
}

void PlotView::replot()
{
    if ( isVisible() ) {

        plotStack_.adaptRange();

        rightAxis->setColorMap(
            plotStack_.plotStack_.first()->spectrogram()->data().range(),
            plotStack_.plotStack_.first()->spectrogram()->colorMap() );
        plot_->setAxisScale(
            QwtPlot::yRight,
            plotStack_.plotStack_.first()->spectrogram()->data().range().minValue(),
            plotStack_.plotStack_.first()->spectrogram()->data().range().maxValue() );

        plot_->replot();
    }
}

void PlotView::setColorMap(const QwtColorMap& colorMap)
{
    plotStack_.plotStack_.first()->setColorMap( colorMap );
    rightAxis->setColorMap(
        plotStack_.plotStack_.first()->spectrogram()->data().range(),
        plotStack_.plotStack_.first()->spectrogram()->colorMap() );
    replot();
}

void PlotView::setColorMapMode(ColorMapAdaptationModes mode)
{
    plotStack_.plotStack_.first()->setColorMapMode( mode );
    rightAxis->setColorMap(
        plotStack_.plotStack_.first()->spectrogram()->data().range(),
        plotStack_.plotStack_.first()->spectrogram()->colorMap() );
    replot();
}

void PlotView::setColorMap(const QwtColorMap& colorMap, ColorMapAdaptationModes mode)
{
    plotStack_.plotStack_.first()->setColorMap( colorMap, mode );
    rightAxis->setColorMap(
        plotStack_.plotStack_.first()->spectrogram()->data().range(),
        plotStack_.plotStack_.first()->spectrogram()->colorMap() );
    replot();
}

void PlotView::showMenu(const QPoint& p)
{
    rightClickMenu.popup( rightAxis->mapToGlobal( p ) );
}

void PlotView::changeTop()
{
    TinyDoubleEdit tEdit(
        this, plotStack_.plotStack_.first()->spectrogram()->data().range().maxValue() );
    if ( tEdit.exec() ) {
        double c = tEdit.value();
        qDebug() << c;
    }
}

void PlotView::changeBottom()
{
    TinyDoubleEdit tEdit(
        this, plotStack_.plotStack_.first()->spectrogram()->data().range().minValue() );
    if ( tEdit.exec() ) {
        double c = tEdit.value();
        qDebug() << c;
    }
}

void PlotView::attachItem(QwtPlotItem* item)
{
    item->attach( plot_ );
}

void PlotView::attachItems(QList<QwtPlotItem*> itemList)
{
    foreach( QwtPlotItem* item, itemList ) {
        attachItem(item);
    }
}

void PlotView::removeItem(QwtPlotItem* item)
{
    if ( plot_->itemList().contains( item ) ) {
        item->detach();
    }
}

void PlotView::removeItems(QList<QwtPlotItem*> itemList)
{
    foreach( QwtPlotItem* item, itemList ) {
            removeItem(item);
        }
}


QwtPlotSpectrogram* PlotView::firstSpectrogram()
{
    return plotStack_.plotStack_.first()->spectrogram();
}
