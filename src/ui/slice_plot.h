/*****************************************************************************
 * RDS Lattice Library / ‘Wellenprogramm’
 * Copyright (C) 2008–2009  Rike-Benjamin Schuppner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License, Version 2.0
 *****************************************************************************/
/*
 * slice_plot.h
 *
 *  Created on: 06.04.2009
 *      Author: rikebs
 */

#ifndef SLICE_PLOT_H_
#define SLICE_PLOT_H_

class SlicePlot {
    void replot() {

        /*
         if ( slicePlot->isVisible() ) {
         QVector< double > sliceData[ latticeController_->lattice()->numberOfVariables() ];
         QVector< double > sliceLatticePoints;

         for (int i = 0; i < latticeController_->lattice()->latticeSizeX(); ++i) {
         for (uint component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component)
         {
         sliceData[ component ].push_back( latticeController_->lattice()->getComponentAt(
         component, i, latticeController_->lattice()->latticeSizeY() / 2 ) );
         }
         sliceLatticePoints.push_back( i * latticeController_->lattice()->scaleX() );
         }
         if ( slice.size() == latticeController_->lattice()->numberOfVariables() )
         for (uint component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component)
         {
         slice[ component ]->setData( sliceLatticePoints, sliceData[ component ] );
         }
         slicePlot->replot();
         }

         */
    }
void setUpSlices() {

    /*
     for (uint i = 0; i < slice.size(); ++i)
     slice[ i ] ->detach();
     slice.resize( latticeController_->lattice()->numberOfVariables() );
     for (uint component = 0; component < latticeController_->lattice()->numberOfVariables(); ++component)
     {
     slice[ component ] = new QwtPlotCurve( QString( "slice %1" ).arg( QString::fromStdString(
     latticeController_->lattice()->componentInfos[ component ].name() ) ) );

     slice[ component ]->attach( slicePlot );

     }
     */
}
};



#endif /* SLICE_PLOT_H_ */
