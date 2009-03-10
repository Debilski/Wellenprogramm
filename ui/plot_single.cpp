/*
 * plot_single.cpp
 *
 *  Created on: 08.03.2009
 *      Author: rikebs
 */

#include "plot_single.h"

PlotSingle::PlotSingle(LatticeInterface* lattice, QWidget* parent) :
    lattice_( lattice ), QDialog( parent ), timeShown_( 10.0 )
{
    setupUi( this );
    setAttribute( Qt::WA_DeleteOnClose );
    setWindowTitle( "Lattice Element" );

    updateIntervallSpinBox->setValue( 0.05 );
    updateIntervallSpinBox->setDecimals( 3 );

    readSettings();

    actionCopy_to_Clipboard->setShortcut( QKeySequence::Copy );

    if ( xSelector->value() >= lattice_->sizeX() )
        xSelector->setValue( lattice_->sizeX() );

    if ( ySelector->value() >= lattice_->sizeY() )
        ySelector->setValue( lattice_->sizeY() );

    xSelector->setMaximum( lattice_->sizeX() );
    ySelector->setMaximum( lattice_->sizeY() );

    connect( clearButton, SIGNAL( clicked() ), this, SLOT( clearData() ) );

    for (uint i = 0; i < lattice_->numberOfVariables(); ++i) {
        QwtPlotCurve* c = new QwtPlotCurve();
        QBrush brush( Qt::blue );
        QPen pen( brush, 2, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin );
        c->setPen( pen );
        QVector< double > d;

        c->setData( timeArray_, d );
        c->attach( singlePlotView );
        data_ << d;
        curves_ << c;
    }
}

PlotSingle::~PlotSingle()
{
    qDebug() << "delete";
    foreach ( QwtPlotCurve* c, curves_ ) {
            delete c;
        }
}

void PlotSingle::closeEvent(QCloseEvent* event)
{
    qDebug() << "close";
    lattice_ = 0;
    writeSettings();
    event->accept();
}

/**
 * Liest neue Werte aus der Lattice und schreibt den Plot neu.
 */
void PlotSingle::update()
{
    // Sollte eigentlich nicht notwendigsein. Als Workaround jedoch noch drin.
    if ( lattice_ == 0 )
        return;

    if ( timeArray_.size() != 0 && lattice_->time() - timeArray_.last()
        <= updateIntervallSpinBox->value() )
        return;

    int x = xSelector->value();
    int y = ySelector->value();

    timeArray_ << lattice_->time();

    for (uint component = 0; component < lattice_->numberOfVariables(); ++component) {
        double val = lattice_->getComponentAt( component, x, y );
        data_[ component ] << val;

        curves_[ component ]->setData( timeArray_, data_[ component ] );
        curves_[ component ]->attach( singlePlotView );
    }
    singlePlotView->replot();
}

void PlotSingle::clearData()
{
    timeArray_.clear();
    for (uint component = 0; component < lattice_->numberOfVariables(); ++component) {
        data_[ component ].clear();
    }
}

/**
 * Schreibt den aktuellen Plot in einem Matlab-lesbaren Format in die Zwischenablage.
 */
void PlotSingle::on_actionCopy_to_Clipboard_triggered()
{
    QClipboard* clipboard = QApplication::clipboard();
    QString text;
    text += QString( "[" );

    for (uint c = 0; c < lattice_->numberOfVariables(); ++c) {
        for (int i = 0; i < data_[ c ].size() - 1; ++i) {
            text += QString( "%1" ).arg( data_[ c ][ i ], 0, 'f', 4 );
            text += QString( ", " );
        }
        text += QString( "%1" ).arg( data_[ c ].last(), 0, 'f', 4 );
        text += QString( "; " );
    }
    for (int i = 0; i < timeArray_.size() - 1; ++i) {
        text += QString( "%1" ).arg( timeArray_[ i ], 0, 'f', 4 );
        text += QString( ", " );
    }
    text += QString( "%1" ).arg( timeArray_.last(), 0, 'f', 4 );

    text += QString( "]" );
    clipboard->setText( text );
}

void PlotSingle::readSettings()
{
    QSettings settings;
    settings.beginGroup( "PlotSingle" );
    restoreGeometry( settings.value( "geometry" ).toByteArray() );
    xSelector->setValue( settings.value( "x", 0 ).toInt() );
    ySelector->setValue( settings.value( "y", 0 ).toInt() );

    settings.endGroup();
}

void PlotSingle::writeSettings()
{
    QSettings settings;
    settings.beginGroup( "PlotSingle" );
    settings.setValue( "geometry", saveGeometry() );
    settings.setValue( "x", xSelector->value() );
    settings.setValue( "y", ySelector->value() );

    settings.endGroup();
}

