/*
 * plot_view.cpp
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#include "plot_view.h"

#include "tiny_double_edit.h"

/**
 * \class PlotView
 * \brief Class that holds Spectrogram, ColorMap and Plot for each tab.
 *
 */

PlotView::
PlotView(const QwtRasterData& spectrogrammData, const QwtColorMap& colorMap, uint component,
    const QString label, QWidget* parent, bool isFft /*= false*/) :
QWidget( parent ), component_( component ), isFft_( isFft )
{
    QFont plotFont = QFont( "", 8 );
    QwtText labelIntensity( label );
    labelIntensity.setFont( plotFont );

    QHBoxLayout* horizontalLayout = new QHBoxLayout( this );
    plot_ = new QwtPlot( this );
    plot_->setAxisFont( QwtPlot::yLeft, plotFont );
    plot_->setAxisFont( QwtPlot::xBottom, plotFont );
    horizontalLayout->addWidget( plot_ );

    spectrogram_ = new QwtPlotSpectrogram();
    spectrogram_->setData( spectrogrammData );

    spectrogram_->setColorMap( colorMap );
    spectrogram_->attach( plot_ );

    rightAxis = plot_->axisWidget( QwtPlot::yRight );
    rightAxis->setTitle( labelIntensity );
    rightAxis->setColorBarEnabled( true );
    rightAxis->setColorMap( spectrogram_->data().range(), spectrogram_->colorMap() );

    plot_->setAxisScale(
        QwtPlot::yRight, spectrogram_->data().range().minValue(),
        spectrogram_->data().range().maxValue() );
    plot_->enableAxis( QwtPlot::yRight );
    plot_->setAxisFont( QwtPlot::yRight, plotFont );
    plot_->plotLayout()->setAlignCanvasToScales( true );

    rightClickMenu.addAction( QString( "Top Value: %1" ).arg(
            spectrogram_->data().range().maxValue() ), this, SLOT(changeTop()) );
    rightClickMenu.addAction( QString( "Bottom Value: %1" ).arg(
            spectrogram_->data().range().minValue() ), this, SLOT(changeBottom()) );

    rightAxis->setContextMenuPolicy( Qt::CustomContextMenu );
    connect( rightAxis, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showMenu( const QPoint& ) ) );
}

PlotView::~PlotView()
{
    //delete spectrogram_;
}

void PlotView::replot(int)
{
    replot();
}

void PlotView::replot()
{
    if ( isVisible() ) {
        emit adaptComponent( component_, isFft_ );
        rightAxis->setColorMap( spectrogram_->data().range(), spectrogram_->colorMap() );
        plot_->setAxisScale(
            QwtPlot::yRight, spectrogram_->data().range().minValue(),
            spectrogram_->data().range().maxValue() );

        plot_->replot();
    }
}

void PlotView::setColorMap(const QwtColorMap& colorMap)
{
    spectrogram_->setColorMap( colorMap );
    rightAxis->setColorMap( spectrogram_->data().range(), spectrogram_->colorMap() );
    replot();
}

void PlotView::showMenu(const QPoint& p)
{
    qDebug() << p;

    rightClickMenu.popup( rightAxis->mapToGlobal( p ) );
}

void PlotView::changeTop()
{
    TinyDoubleEdit tEdit( this, spectrogram_->data().range().maxValue() );
    if ( tEdit.exec() ) {
        double c = tEdit.value();
        qDebug() << c;
    }
}

void PlotView::changeBottom()
{
    TinyDoubleEdit tEdit( this, spectrogram_->data().range().minValue() );
    if ( tEdit.exec() ) {
        double c = tEdit.value();
        qDebug() << c;
    }
}

