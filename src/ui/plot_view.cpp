/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * plot_view.cpp
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#include "plot_view.h"

#include <qmenu.h>

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

class PlotView::PrivateData {
public:
    PlotStack plotStack;
    QwtPlot* plot;
    QwtScaleWidget* colorBarAxis;
    QMenu rightClickMenu;
    QString name;
};

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
    QWidget( parent )
{
    d_data = new PrivateData;
    d_data->plotStack = plotStack;
    d_data->plot = new QwtPlot( this );

    QFont plotFont = QFont( "", 8 );
    QwtText labelIntensity( label );
    labelIntensity.setFont( plotFont );

    QHBoxLayout* horizontalLayout = new QHBoxLayout( this );

    d_data->plot->setAxisFont( QwtPlot::xBottom, plotFont );
    d_data->plot->setAxisFont( QwtPlot::yLeft, plotFont );

    QRectF dimensions = d_data->plotStack.plotStack_.first()->spectrogram()->boundingRect();
    d_data->plot->setAxisScale( QwtPlot::yLeft, dimensions.top(), dimensions.bottom() );
    d_data->plot->setAxisScale( QwtPlot::xBottom, dimensions.left(), dimensions.right() );

    horizontalLayout->addWidget( d_data->plot );

    d_data->plotStack.attach( d_data->plot );

    Painter* painter = new Painter( QwtPlot::xBottom, QwtPlot::yLeft, d_data->plot->canvas() );
    painter->setTrackerPen( QColor( Qt::white ) );

    painter->setEnabled( true );

    connect(
        painter, SIGNAL(selected(const QwtDoublePoint &)), this,
        SLOT(registerMouseEvent(const QwtDoublePoint&)) );

    d_data->colorBarAxis = d_data->plot->axisWidget( QwtPlot::yRight );
    d_data->colorBarAxis->setTitle( labelIntensity );
    d_data->colorBarAxis->setColorBarEnabled( true );
    d_data->colorBarAxis->setColorMap(
        d_data->plotStack.plotStack_.first()->spectrogram()->data().range(),
        d_data->plotStack.plotStack_.first()->spectrogram()->colorMap() );

    d_data->plot->setAxisScale(
        QwtPlot::yRight,
        d_data->plotStack.plotStack_.first()->spectrogram()->data().range().minValue(),
        d_data->plotStack.plotStack_.first()->spectrogram()->data().range().maxValue() );
    d_data->plot->enableAxis( QwtPlot::yRight );
    d_data->plot->setAxisFont( QwtPlot::yRight, plotFont );
    d_data->plot->plotLayout()->setAlignCanvasToScales( true );

    d_data->rightClickMenu.addAction(
        QString( "Top Value: %1" ).arg(
            d_data->plotStack.plotStack_.first()->spectrogram()->data().range().maxValue() ), this,
        SLOT(changeTop()) );
    d_data->rightClickMenu.addAction(
        QString( "Bottom Value: %1" ).arg(
            d_data->plotStack.plotStack_.first()->spectrogram()->data().range().minValue() ), this,
        SLOT(changeBottom()) );

    d_data->colorBarAxis->setContextMenuPolicy( Qt::CustomContextMenu );
    connect(
        d_data->colorBarAxis, SIGNAL( customContextMenuRequested( const QPoint& ) ), this,
        SLOT( showMenu( const QPoint& ) ) );
}

QwtPlot* PlotView::plot()
{
    return d_data->plot;
}

const QString& PlotView::name() const
{
    return d_data->name;
}
void PlotView::setName(const QString& name)
{
    d_data->name = name;
}

void PlotView::registerMouseEvent(const QwtDoublePoint & p)
{
    emit selected( d_data->plotStack.plotStack_.first()->component, p );
}

void PlotView::registerMouseEvent(const QwtArray< QwtDoublePoint > &pa)
{
    qDebug() << pa;
}

PlotView::~PlotView()
{
    //delete d_data->plot;
    delete d_data;
    //delete spectrogram();
}

void PlotView::replot(int)
{
    replot( false );
}

void PlotView::replot()
{
    replot( false );
}

void PlotView::replot(bool force)
{
    if ( isVisible() || force ) {

        d_data->plotStack.adaptRange();

        d_data->colorBarAxis->setColorMap(
            d_data->plotStack.plotStack_.first()->spectrogram()->data().range(),
            d_data->plotStack.plotStack_.first()->spectrogram()->colorMap() );
        d_data->plot->setAxisScale(
            QwtPlot::yRight,
            d_data->plotStack.plotStack_.first()->spectrogram()->data().range().minValue(),
            d_data->plotStack.plotStack_.first()->spectrogram()->data().range().maxValue() );

        d_data->plot->replot();
    }
}

void PlotView::setColorMap(const QwtColorMap& colorMap)
{
    d_data->plotStack.plotStack_.first()->setColorMap( colorMap );
    d_data->colorBarAxis->setColorMap(
        d_data->plotStack.plotStack_.first()->spectrogram()->data().range(),
        d_data->plotStack.plotStack_.first()->spectrogram()->colorMap() );
    replot();
}

void PlotView::setColorMapMode(ColorMapAdaptationModes mode)
{
    d_data->plotStack.plotStack_.first()->setColorMapMode( mode );
    d_data->colorBarAxis->setColorMap(
        d_data->plotStack.plotStack_.first()->spectrogram()->data().range(),
        d_data->plotStack.plotStack_.first()->spectrogram()->colorMap() );
    replot();
}

void PlotView::setColorMap(const QwtColorMap& colorMap, ColorMapAdaptationModes mode)
{
    d_data->plotStack.plotStack_.first()->setColorMap( colorMap, mode );
    d_data->colorBarAxis->setColorMap(
        d_data->plotStack.plotStack_.first()->spectrogram()->data().range(),
        d_data->plotStack.plotStack_.first()->spectrogram()->colorMap() );
    replot();
}

void PlotView::print(QImage& image, bool raw, bool resize)
{
    // Replot, damit das Bild auf aktuelle Farben eingestellt wird.
    replot( true );
    image.fill( Qt::white ); // Qt::transparent ?

    QwtPlotPrintFilter filter;
    int options = 0;//= QwtPlotPrintFilter::PrintAll;
    options &= ~QwtPlotPrintFilter::PrintBackground;
    options &= ~QwtPlotPrintFilter::PrintFrameWithScales;
    options &= ~QwtPlotPrintFilter::PrintMargin;
    options &= ~QwtPlotPrintFilter::PrintTitle;
    options &= ~QwtPlotPrintFilter::PrintLegend;
    options &= ~QwtPlotPrintFilter::PrintGrid;

    filter.setOptions( options );

    bool oldAxes[ QwtPlot::axisCnt ];
    int canvasLineWidth = d_data->plot->canvasLineWidth();
    int margin = d_data->plot->margin();
    for (int i = 0; i < QwtPlot::axisCnt; ++i) {
        oldAxes[ i ] = d_data->plot->axisEnabled( i );
        if ( raw ) {
            d_data->plot->enableAxis( i, false );
        }
    }
    if ( raw ) {
        d_data->plot->setCanvasLineWidth( 0 );
        d_data->plot->setMargin( 0 );
//        d_data->plot->plotLayout()->setAlignCanvasToScales( true );
        d_data->plot->plotLayout()->setMargin( 0 );
        d_data->plot->plotLayout()->setSpacing( 0 );

        d_data->plot->plotLayout()->setCanvasMargin(0,QwtPlot::xBottom);
        d_data->plot->plotLayout()->setCanvasMargin(0,QwtPlot::xTop);
        d_data->plot->plotLayout()->setCanvasMargin(0,QwtPlot::yLeft);
        d_data->plot->plotLayout()->setCanvasMargin(0,QwtPlot::yRight);
        d_data->plot->updateLayout();
    }
    d_data->plot->print( image, filter );

    // Back to normal
    for (int i = 0; i < QwtPlot::axisCnt; ++i) {
        d_data->plot->enableAxis( i, oldAxes[ i ] );
    }
    d_data->plot->setCanvasLineWidth( canvasLineWidth );
    d_data->plot->setMargin( margin );
}

void PlotView::showMenu(const QPoint& p)
{
    d_data->rightClickMenu.popup( d_data->colorBarAxis->mapToGlobal( p ) );
}

void PlotView::changeTop()
{
    TinyDoubleEdit tEdit(
        this, d_data->plotStack.plotStack_.first()->spectrogram()->data().range().maxValue(),
        TinyDoubleEdit::NoException );
    if ( tEdit.exec() ) {
        double c = tEdit.value();
        d_data->plotStack.plotStack_.first()->adaptionMode().hintMax( c );
        replot();
    }
}

void PlotView::changeBottom()
{
    TinyDoubleEdit tEdit(
        this, d_data->plotStack.plotStack_.first()->spectrogram()->data().range().minValue(),
        TinyDoubleEdit::NoException );
    if ( tEdit.exec() ) {
        double c = tEdit.value();
        d_data->plotStack.plotStack_.first()->adaptionMode().hintMin( c );
        replot();
    }
}

void PlotView::attachItem(QwtPlotItem* item)
{
    item->attach( d_data->plot );
}

void PlotView::attachItems(QList< QwtPlotItem* > itemList)
{
    foreach( QwtPlotItem* item, itemList )
        {
            attachItem( item );
        }
}

void PlotView::removeItem(QwtPlotItem* item)
{
    if ( d_data->plot->itemList().contains( item ) ) {
        item->detach();
    }
}

void PlotView::removeItems(QList< QwtPlotItem* > itemList)
{
    foreach( QwtPlotItem* item, itemList )
        {
            removeItem( item );
        }
}

QwtPlotSpectrogram* PlotView::firstSpectrogram()
{
    return d_data->plotStack.plotStack_.first()->spectrogram();
}
