/*
 * spectrogram_data.h
 *
 *  Created on: 06.03.2009
 *      Author: rikebs
 */

#ifndef SPECTROGRAM_DATA_H_
#define SPECTROGRAM_DATA_H_

#include "lattice_interface.h"
#include "plotwindow_2d.h"

class SpectrogramData : public QwtRasterData {
    Waveprogram2DPlot::ModelLattice* lattice_;
    int variable_;
    Waveprogram2DPlot* programBase;
public:
    SpectrogramData(Waveprogram2DPlot::ModelLattice* lattice, int variable = 0,
                    Waveprogram2DPlot* programBase = 0) :
        QwtRasterData( QwtDoubleRect( 0., 0., (lattice->sizeX()), (lattice->sizeY()) ) ), lattice_(
            lattice ), variable_( variable ), programBase( programBase )
    {
    }
    ~SpectrogramData()
    {
    }
    ;
    // Seltsamer Fehler in Berechnung. Lasse Fkt aus
    virtual QSize rasterHint(const QwtDoubleRect &) const
    {
        return QSize( lattice_->latticeSizeX(), lattice_->latticeSizeY() );
    }

    virtual QwtRasterData* copy() const
    {
        return new SpectrogramData( lattice_, variable_, programBase );
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
        int ix = qRound( x / lattice_->scaleX() );
        int iy = qRound( y / lattice_->scaleY() );

        //if ( ix >= 0 && ix < 10 && iy >= 0 && iy < 10 )
        //return _values[ix][iy];

        //assert( ix >= 0 );
        //assert( iy - 1 >= 0 );
        //assert( ix < lattice_->latticeSizeX() );
        //assert( iy - 1 < lattice_->latticeSizeY() );

        return lattice_->getComponentAt( variable_, ix, iy - 1 );
        //return lattice_->getAt((unsigned int) floor(x / lattice_->scaleX()), (unsigned int) floor(y / lattice_->scaleY()))[ variable_ ];
    }
};


class SpectrogramDataFft : public QwtRasterData {
    Waveprogram2DPlot::ModelLattice* lattice_;
    int variable_;
    Waveprogram2DPlot* programBase;
public:
    SpectrogramDataFft(Waveprogram2DPlot::ModelLattice* lattice, int variable = 0,
                    Waveprogram2DPlot* programBase = 0) :
        QwtRasterData( QwtDoubleRect( 0., 0., (lattice->sizeX()), (lattice->sizeY()) ) ), lattice_(
            lattice ), variable_( variable ), programBase( programBase )
    {
    }
    ~SpectrogramDataFft()
    {
    }
    ;
    // Seltsamer Fehler in Berechnung. Lasse Fkt aus
    virtual QSize rasterHint(const QwtDoubleRect &) const
    {
        return QSize( lattice_->latticeSizeX(), lattice_->latticeSizeY() );
    }

    virtual QwtRasterData* copy() const
    {
        return new SpectrogramDataFft( lattice_, variable_, programBase );
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
        int ix = qRound( x / lattice_->scaleX() );// % (lattice_->sizeX() / 2);
        int iy = qRound( y / lattice_->scaleY() );


        iy = (iy + lattice_->latticeSizeY() / 2) % lattice_->latticeSizeY();
        iy = (iy < lattice_->latticeSizeY() / 2) ? iy : lattice_->latticeSizeY() - iy;
        ix = (ix + lattice_->latticeSizeX() / 2) % lattice_->latticeSizeX();


        return std::abs(lattice_->getFftComponentAt( variable_, ix, iy )) ;// sqrt( lattice_->latticeSize() );
    }
};


#endif /* SPECTROGRAM_DATA_H_ */
