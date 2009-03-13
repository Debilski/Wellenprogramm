/*
 * spectrogram_data.h
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#ifndef SPECTROGRAM_DATA_H_
#define SPECTROGRAM_DATA_H_

#include "lattice_controller.h"
#include "plotwindow_2d.h"

class SpectrogramData : public QwtRasterData {
    LatticeController* latticeController_;
    int variable_;
    Waveprogram2DPlot* programBase;
public:
    SpectrogramData(LatticeController* latticeController, int variable = 0,
                    Waveprogram2DPlot* programBase = 0) :
        QwtRasterData( QwtDoubleRect( 0., 0., (latticeController->sizeX()), (latticeController->sizeY()) ) ), latticeController_(
            latticeController ), variable_( variable ), programBase( programBase )
    {
    }
    ~SpectrogramData()
    {
    }
    ;
    // Seltsamer Fehler in Berechnung. Lasse Fkt aus
    virtual QSize rasterHint(const QwtDoubleRect &) const
    {
        return QSize( latticeController_->latticeSizeX(), latticeController_->latticeSizeY() );
    }

    virtual QwtRasterData* copy() const
    {
        return new SpectrogramData( latticeController_, variable_, programBase );
    }

    virtual QwtDoubleInterval range() const
    {

        /*double min = lattice_->componentInfos.at( variable_ ).assumedMin;
         double max = lattice_->componentInfos.at( variable_ ).assumedMax;
         max = lattice_->getMax( variable_ );
         //min = lattice_->getMin( variable_ );*/

        return programBase->plotRanges_real[ variable_ ];
        //return QwtDoubleInterval( min, max );
    }

    virtual double value(double x, double y) const
    { //std::cout << "(" << x << ";" <<  y << ") ";
        // if (y==0) return 1;
        // return 0;
        //return x / 16 * 4 - 2;

        //return _lattice->getU((unsigned int) floor(x * (_lattice->latticeSizeX() ) / _lattice->sizeX() ),
        //                       (unsigned int) floor(y * (_lattice->latticeSizeY() ) / _lattice->sizeY() ));
        int ix = qRound( x / latticeController_->lattice()->scaleX() );
        int iy = qRound( y / latticeController_->lattice()->scaleY() );

        //if ( ix >= 0 && ix < 10 && iy >= 0 && iy < 10 )
        //return _values[ix][iy];

        //assert( ix >= 0 );
        //assert( iy - 1 >= 0 );
        //assert( ix < lattice_->latticeSizeX() );
        //assert( iy - 1 < lattice_->latticeSizeY() );

        return latticeController_->lattice()->getComponentAt( variable_, ix, iy - 1 );
        //return lattice_->getAt((unsigned int) floor(x / lattice_->scaleX()), (unsigned int) floor(y / lattice_->scaleY()))[ variable_ ];
    }
};


class SpectrogramDataFft : public QwtRasterData {
    LatticeController* latticeController_;
    int variable_;
    Waveprogram2DPlot* programBase;
public:
    SpectrogramDataFft(LatticeController* latticeController, int variable = 0,
                    Waveprogram2DPlot* programBase = 0) :
        QwtRasterData( QwtDoubleRect( 0., 0., (latticeController->sizeX()), (latticeController->sizeY()) ) ), latticeController_(
            latticeController ), variable_( variable ), programBase( programBase )
    {
    }
    ~SpectrogramDataFft()
    {
    }
    ;
    // Seltsamer Fehler in Berechnung. Lasse Fkt aus
    virtual QSize rasterHint(const QwtDoubleRect &) const
    {
        return QSize( latticeController_->latticeSizeX(), latticeController_->latticeSizeY() );
    }

    virtual QwtRasterData* copy() const
    {
        return new SpectrogramDataFft( latticeController_, variable_, programBase );
    }

    virtual QwtDoubleInterval range() const
    {

        /*double min = lattice_->componentInfos.at( variable_ ).assumedMin;
         double max = lattice_->componentInfos.at( variable_ ).assumedMax;
         max = lattice_->getMax( variable_ );
         //min = lattice_->getMin( variable_ );*/
//todo: ranges für fft
        return programBase->plotRanges_real[ variable_ ];
        //return QwtDoubleInterval( min, max );
    }

    virtual double value(double x, double y) const
    {
        int ix = qRound( x / latticeController_->lattice()->scaleX() );// % (lattice_->sizeX() / 2);
        int iy = qRound( y / latticeController_->lattice()->scaleY() );


        iy = (iy + latticeController_->latticeSizeY() / 2) % latticeController_->latticeSizeY();
        iy = (iy < latticeController_->latticeSizeY() / 2) ? iy : latticeController_->latticeSizeY() - iy;
        ix = (ix + latticeController_->latticeSizeX() / 2) % latticeController_->latticeSizeX();


        return std::abs(latticeController_->lattice()->getFftComponentAt( variable_, ix, iy )) ;// sqrt( lattice_->latticeSize() );
    }
};


#endif /* SPECTROGRAM_DATA_H_ */
