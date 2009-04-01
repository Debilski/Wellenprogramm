/*
 * plot_view.cpp
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#include "plot_view.h"

#include "tiny_double_edit.h"

#include "qwt_plot_picker.h"
class Painter: public QwtPlotPicker {
    public:
        Painter(int xAxis, int yAxis, QwtPlotCanvas *canvas):
        QwtPlotPicker(xAxis, yAxis, canvas)
    {
            setSelectionFlags(QwtPicker::PointSelection | QwtPicker::ClickSelection);

            setTrackerMode(QwtPicker::ActiveOnly);
            setRubberBand(QwtPicker::NoRubberBand);
    }
};



/**
 * \class PlotView
 * \brief Class that holds Spectrogram, ColorMap and Plot for each tab.
 *
 */

PlotView::PlotView( const PlotStack& plotStack, const QString label, QWidget* parent ) :
plotStack_(plotStack), QWidget(parent)
{
    QFont plotFont = QFont( "", 8 );
    QwtText labelIntensity( label );
    labelIntensity.setFont( plotFont );


    QHBoxLayout* horizontalLayout = new QHBoxLayout( this );
    plot_ = new QwtPlot( this );
    plot_->setAxisFont( QwtPlot::yLeft, plotFont );
    plot_->setAxisFont( QwtPlot::xBottom, plotFont );
    horizontalLayout->addWidget( plot_ );

    plotStack_.attach(plot_);

    Painter* painter = new Painter( QwtPlot::xBottom, QwtPlot::yLeft,
            plot_->canvas());
    painter->setTrackerPen(QColor(Qt::white));

    painter->setEnabled(true);

    connect(painter, SIGNAL(selected(const QwtDoublePoint &)), this, SLOT(registerMouseEvent(const QwtDoublePoint&)));

    rightAxis = plot_->axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( labelIntensity );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorMap( plotStack_.plotStack_.first()->spectrogram()->data().range(), plotStack_.plotStack_.first()->spectrogram()->colorMap() );

    plot_->setAxisScale(
        QwtPlot::yRight, plotStack_.plotStack_.first()->spectrogram()->data().range().minValue(),
        plotStack_.plotStack_.first()->spectrogram()->data().range().maxValue() );
    plot_->enableAxis( QwtPlot::yRight );
    plot_->setAxisFont( QwtPlot::yRight, plotFont );
    plot_->plotLayout()->setAlignCanvasToScales( true );

    rightClickMenu.addAction( QString( "Top Value: %1" ).arg(
        plotStack_.plotStack_.first()->spectrogram()->data().range().maxValue() ), this, SLOT(changeTop()) );
    rightClickMenu.addAction( QString( "Bottom Value: %1" ).arg(
        plotStack_.plotStack_.first()->spectrogram()->data().range().minValue() ), this, SLOT(changeBottom()) );

    rightAxis->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( rightAxis, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showMenu( const QPoint& ) ) );
}


void PlotView::registerMouseEvent(const QwtDoublePoint & p)
{
    qDebug() << p;
    emit selected(plotStack_.plotStack_.first()->component, p);
}

void PlotView::registerMouseEvent(const QwtArray< QwtDoublePoint > &pa) {
    qDebug() << "called";
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

        rightAxis->setColorMap( plotStack_.plotStack_.first()->spectrogram()->data().range(), plotStack_.plotStack_.first()->spectrogram()->colorMap() );
        plot_->setAxisScale(
            QwtPlot::yRight, plotStack_.plotStack_.first()->spectrogram()->data().range().minValue(),
            plotStack_.plotStack_.first()->spectrogram()->data().range().maxValue() );

        plot_->replot();
    }
}

void PlotView::setColorMap(const QwtColorMap& colorMap, ColourMapAdaptationModes mode)
{
    plotStack_.plotStack_.first()->setColorMap( colorMap, mode );
    rightAxis->setColorMap( plotStack_.plotStack_.first()->spectrogram()->data().range(), plotStack_.plotStack_.first()->spectrogram()->colorMap() );
    replot();
}

void PlotView::showMenu(const QPoint& p)
{
    qDebug() << p;

    rightClickMenu.popup( rightAxis->mapToGlobal( p ) );
}

void PlotView::changeTop()
{
    TinyDoubleEdit tEdit( this, plotStack_.plotStack_.first()->spectrogram()->data().range().maxValue() );
    if ( tEdit.exec() ) {
        double c = tEdit.value();
        qDebug() << c;
    }
}

void PlotView::changeBottom()
{
    TinyDoubleEdit tEdit( this, plotStack_.plotStack_.first()->spectrogram()->data().range().minValue() );
    if ( tEdit.exec() ) {
        double c = tEdit.value();
        qDebug() << c;
    }
}

